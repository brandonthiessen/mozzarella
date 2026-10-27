// Magic bitboard search: hill-climbing on collision count, multithreaded.
// Build: g++ -std=c++20 -O3 -march=native -pthread magic_search.cpp -o magic_search
// Run:   ./magic_search [bishop|rook] [threads]
// Bishop results use magic-search-bishop.csv (same format as before); rook uses magic-search-rook.csv.
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using u64 = std::uint64_t;
using Clock = std::chrono::steady_clock;

// ---------- board helpers ----------
static u64 ray_attacks(int sq, u64 occ, bool bishop) {
    static constexpr int R[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    static constexpr int B[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const auto& dirs = bishop ? B : R;
    u64 a = 0;
    for (const auto& d : dirs) {
        for (int r = sq / 8 + d[0], f = sq % 8 + d[1]; r >= 0 && r < 8 && f >= 0 && f < 8;
             r += d[0], f += d[1]) {
            a |= 1ULL << (r * 8 + f);
            if (occ & (1ULL << (r * 8 + f))) break;
        }
    }
    return a;
}

// Relevant occupancy: every ray square except the last (edge) one.
static u64 relevant_mask(int sq, bool bishop) {
    static constexpr int R[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    static constexpr int B[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    const auto& dirs = bishop ? B : R;
    u64 m = 0;
    for (const auto& d : dirs) {
        for (int r = sq / 8 + d[0], f = sq % 8 + d[1];; r += d[0], f += d[1]) {
            int nr = r + d[0], nf = f + d[1];
            if (r < 0 || r > 7 || f < 0 || f > 7 || nr < 0 || nr > 7 || nf < 0 || nf > 7) break;
            m |= 1ULL << (r * 8 + f);
        }
    }
    return m;
}

// ---------- state ----------
struct Square {
    std::vector<u64> occ, att;
    int min_bits = 0;                 // ceil(log2(#distinct attack sets)): hard floor
    int target_bits = 0;              // owner thread only
    std::atomic<int> best_bits{0};    // 0 = nothing found yet
    u64 best_magic = 0;               // guarded by g_mu
    std::atomic<u64> attempts{0};
};

static std::array<Square, 64> g;
static std::mutex g_mu;
static bool g_bishop = true;
static std::string g_file;
static std::atomic<long long> g_last_improve_ms{0};
static const auto g_start = Clock::now();

static long long now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - g_start).count();
}

struct Rng {
    u64 s;
    u64 operator()() {
        u64 x = s;
        x ^= x >> 12; x ^= x << 25; x ^= x >> 27;
        s = x;
        return x * 0x2545F4914F6CDD1DULL;
    }
};

struct Scratch {
    std::vector<u64> table;
    std::vector<std::uint32_t> stamp;
    std::uint32_t cur = 0;
};

// Number of entries whose slot already holds a *different* attack set. 0 == valid magic.
static int collisions(const Square& s, u64 magic, int bits, Scratch& sc) {
    const std::size_t n = std::size_t{1} << bits;
    if (sc.table.size() < n) { sc.table.resize(n); sc.stamp.assign(n, 0); sc.cur = 0; }
    if (++sc.cur == 0) { std::fill(sc.stamp.begin(), sc.stamp.end(), 0u); sc.cur = 1; }
    const int shift = 64 - bits;
    int c = 0;
    for (std::size_t i = 0; i < s.occ.size(); ++i) {
        const std::size_t idx = (s.occ[i] * magic) >> shift;
        if (sc.stamp[idx] != sc.cur) { sc.stamp[idx] = sc.cur; sc.table[idx] = s.att[i]; }
        else if (sc.table[idx] != s.att[i]) ++c;
    }
    return c;
}

static void save_locked() {
    const std::string tmp = g_file + ".tmp";
    {
        std::ofstream f(tmp, std::ios::trunc);
        if (!f) { std::cerr << "\ncannot write " << tmp << '\n'; return; }
        f << "square,magic,index_bits,shift,table_entries,total_attempts\n";
        for (int sq = 0; sq < 64; ++sq) {
            const int b = g[sq].best_bits.load();
            if (!b) continue;
            f << sq << ",0x" << std::hex << g[sq].best_magic << std::dec << ',' << b << ','
              << 64 - b << ',' << (1ULL << b) << ',' << g[sq].attempts.load() << '\n';
        }
    }
    std::filesystem::rename(tmp, g_file);  // atomic replace: Ctrl-C can't corrupt results
}

// Start from a sparse random magic, then flip bits, keeping changes that don't hurt.
static bool climb(Square& s, int bits, Rng& rng, Scratch& sc, int steps, u64& out) {
    u64 m = rng() & rng() & rng();
    int cost = collisions(s, m, bits, sc);
    int i = 0;
    for (; i < steps && cost > 0; ++i) {
        u64 t = m ^ (1ULL << (rng() & 63));
        if (rng() & 1) t ^= 1ULL << (rng() & 63);
        const int c = collisions(s, t, bits, sc);
        if (c <= cost) { m = t; cost = c; }
    }
    s.attempts.fetch_add(i + 1, std::memory_order_relaxed);
    if (cost == 0) { out = m; return true; }
    return false;
}

static void worker(int tid, int nthreads) {
    std::random_device rd;
    Rng rng{(u64{rd()} << 32 | rd()) ^ (0x9E3779B97F4A7C15ULL * (tid + 1))};
    if (!rng.s) rng.s = 1;
    Scratch sc;
    for (;;) {
        bool worked = false;
        for (int sq = tid; sq < 64; sq += nthreads) {
            Square& s = g[sq];
            const int bits = s.target_bits;
            if (bits < 1 || bits < s.min_bits) continue;
            worked = true;
            u64 m;
            if (climb(s, bits, rng, sc, 2000, m)) {
                std::lock_guard lk(g_mu);
                s.best_magic = m;
                s.best_bits = bits;
                s.target_bits = bits - 1;
                g_last_improve_ms = now_ms();
                save_locked();
            }
        }
        if (!worked) return;  // every square is at its floor
    }
}

int main(int argc, char** argv) {
    g_bishop = !(argc > 1 && std::string(argv[1]) == "rook");
    g_file = g_bishop ? "magic-search-bishop.csv" : "magic-search-rook.csv";
    int nthreads = argc > 2 ? std::max(1, std::atoi(argv[2]))
                            : std::max(1u, std::thread::hardware_concurrency());

    for (int sq = 0; sq < 64; ++sq) {
        Square& s = g[sq];
        const u64 mask = relevant_mask(sq, g_bishop);
        u64 sub = 0;
        do {
            s.occ.push_back(sub);
            s.att.push_back(ray_attacks(sq, sub, g_bishop));
            sub = (sub - mask) & mask;
        } while (sub);
        std::vector<u64> a = s.att;
        std::sort(a.begin(), a.end());
        a.erase(std::unique(a.begin(), a.end()), a.end());
        s.min_bits = a.size() <= 1 ? 0 : std::bit_width(a.size() - 1);
        s.target_bits = std::popcount(mask);
    }

    // Resume: only trust magics that re-validate.
    {
        Scratch sc;
        std::ifstream f(g_file);
        std::string line;
        if (f && std::getline(f, line)) {
            while (std::getline(f, line)) {
                std::istringstream row(line);
                std::array<std::string, 6> fl;
                bool ok = true;
                for (auto& x : fl) if (!std::getline(row, x, ',')) { ok = false; break; }
                if (!ok) continue;
                try {
                    const int sq = std::stoi(fl[0]);
                    const u64 magic = std::stoull(fl[1], nullptr, 0);
                    const int bits = std::stoi(fl[2]);
                    const u64 att = std::stoull(fl[5]);
                    if (sq < 0 || sq >= 64 || bits < 1 || bits > 20) continue;
                    Square& s = g[sq];
                    if (s.best_bits && bits >= s.best_bits) {
                        s.attempts = std::max<u64>(s.attempts, att);
                        continue;
                    }
                    if (collisions(s, magic, bits, sc) != 0) continue;
                    s.best_bits = bits;
                    s.best_magic = magic;
                    s.target_bits = bits - 1;
                    s.attempts = std::max<u64>(s.attempts, att);
                } catch (const std::exception&) {}
            }
        }
        std::lock_guard lk(g_mu);
        save_locked();
    }

    std::vector<std::thread> pool;
    for (int t = 0; t < nthreads; ++t) pool.emplace_back(worker, t, nthreads);

    u64 prev_attempts = 0;
    auto prev = Clock::now();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        const auto now = Clock::now();
        u64 total = 0, entries = 0, floor_entries = 0;
        int valid = 0;
        std::array<int, 65> hist{};
        for (const auto& s : g) {
            total += s.attempts;
            floor_entries += 1ULL << s.min_bits;
            if (const int b = s.best_bits) { ++valid; ++hist[b]; entries += 1ULL << b; }
        }
        const double dt = std::chrono::duration<double>(now - prev).count();
        std::ostringstream st;
        st << (g_bishop ? "Bishop" : "Rook") << " | " << valid << "/64 | " << entries
           << " entries (floor " << floor_entries << ") | "
           << static_cast<u64>((total - prev_attempts) / dt) << " cand/s | last improvement "
           << (now_ms() - g_last_improve_ms) / 1000 << "s ago | elapsed " << now_ms() / 1000
           << "s | bits:";
        for (int b = 1; b <= 64; ++b) if (hist[b]) st << ' ' << b << ':' << hist[b];
        std::cout << '\r' << st.str() << "\033[K" << std::flush;
        prev_attempts = total;
        prev = now;
    }
}
