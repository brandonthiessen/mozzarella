#pragma once

#include "bitboard.h"

namespace mozzarella {

inline constexpr Bitboard PAWN_ADVANCE_MASKS[2][64] = {
    {
        Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0},
        Bitboard{0x1010000}, Bitboard{0x2020000}, Bitboard{0x4040000}, Bitboard{0x8080000}, Bitboard{0x10100000}, Bitboard{0x20200000}, Bitboard{0x40400000}, Bitboard{0x80800000} ,
        Bitboard{0x1000000}, Bitboard{0x2000000}, Bitboard{0x4000000}, Bitboard{0x8000000}, Bitboard{0x10000000}, Bitboard{0x20000000}, Bitboard{0x40000000}, Bitboard{0x80000000} ,
        Bitboard{0x100000000}, Bitboard{0x200000000}, Bitboard{0x400000000}, Bitboard{0x800000000}, Bitboard{0x1000000000}, Bitboard{0x2000000000}, Bitboard{0x4000000000}, Bitboard{0x8000000000} ,
        Bitboard{0x10000000000}, Bitboard{0x20000000000}, Bitboard{0x40000000000}, Bitboard{0x80000000000}, Bitboard{0x100000000000}, Bitboard{0x200000000000}, Bitboard{0x400000000000}, Bitboard{0x800000000000} ,
        Bitboard{0x1000000000000}, Bitboard{0x2000000000000}, Bitboard{0x4000000000000}, Bitboard{0x8000000000000}, Bitboard{0x10000000000000}, Bitboard{0x20000000000000}, Bitboard{0x40000000000000}, Bitboard{0x80000000000000},
        Bitboard{0x100000000000000}, Bitboard{0x200000000000000}, Bitboard{0x400000000000000}, Bitboard{0x800000000000000}, Bitboard{0x1000000000000000}, Bitboard{0x2000000000000000}, Bitboard{0x4000000000000000}, Bitboard{0x8000000000000000},
        Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}
    },
    {
        Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0},
        Bitboard{0x1}, Bitboard{0x2}, Bitboard{0x4}, Bitboard{0x8}, Bitboard{0x10}, Bitboard{0x20}, Bitboard{0x40}, Bitboard{0x80},
        Bitboard{0x100}, Bitboard{0x200}, Bitboard{0x400}, Bitboard{0x800}, Bitboard{0x1000}, Bitboard{0x2000}, Bitboard{0x4000}, Bitboard{0x8000},
        Bitboard{0x10000}, Bitboard{0x20000}, Bitboard{0x40000}, Bitboard{0x80000}, Bitboard{0x100000}, Bitboard{0x200000}, Bitboard{0x400000}, Bitboard{0x800000},
        Bitboard{0x1000000}, Bitboard{0x2000000}, Bitboard{0x4000000}, Bitboard{0x8000000}, Bitboard{0x10000000}, Bitboard{0x20000000}, Bitboard{0x40000000}, Bitboard{0x80000000},
        Bitboard{0x100000000}, Bitboard{0x200000000}, Bitboard{0x400000000}, Bitboard{0x800000000}, Bitboard{0x1000000000}, Bitboard{0x2000000000}, Bitboard{0x4000000000}, Bitboard{0x8000000000},
        Bitboard{0x10100000000}, Bitboard{0x20200000000}, Bitboard{0x40400000000}, Bitboard{0x80800000000}, Bitboard{0x101000000000}, Bitboard{0x202000000000}, Bitboard{0x404000000000}, Bitboard{0x808000000000},
        Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}
    }
};

inline constexpr Bitboard PAWN_ATTACK_MASKS[2][64] = {
    {
        Bitboard{0x200}, Bitboard{0x500}, Bitboard{0xa00}, Bitboard{0x1400}, Bitboard{0x2800}, Bitboard{0x5000}, Bitboard{0xa000}, Bitboard{0x4000},
        Bitboard{0x20000}, Bitboard{0x50000}, Bitboard{0xa0000}, Bitboard{0x140000}, Bitboard{0x280000}, Bitboard{0x500000}, Bitboard{0xa00000}, Bitboard{0x400000},
        Bitboard{0x2000000}, Bitboard{0x5000000}, Bitboard{0xa000000}, Bitboard{0x14000000}, Bitboard{0x28000000}, Bitboard{0x50000000}, Bitboard{0xa0000000}, Bitboard{0x40000000},
        Bitboard{0x200000000}, Bitboard{0x500000000}, Bitboard{0xa00000000}, Bitboard{0x1400000000}, Bitboard{0x2800000000}, Bitboard{0x5000000000}, Bitboard{0xa000000000}, Bitboard{0x4000000000},
        Bitboard{0x20000000000}, Bitboard{0x50000000000}, Bitboard{0xa0000000000}, Bitboard{0x140000000000}, Bitboard{0x280000000000}, Bitboard{0x500000000000}, Bitboard{0xa00000000000}, Bitboard{0x400000000000},
        Bitboard{0x2000000000000}, Bitboard{0x5000000000000}, Bitboard{0xa000000000000}, Bitboard{0x14000000000000}, Bitboard{0x28000000000000}, Bitboard{0x50000000000000}, Bitboard{0xa0000000000000}, Bitboard{0x40000000000000},
        Bitboard{0x200000000000000}, Bitboard{0x500000000000000}, Bitboard{0xa00000000000000}, Bitboard{0x1400000000000000}, Bitboard{0x2800000000000000}, Bitboard{0x5000000000000000}, Bitboard{0xa000000000000000}, Bitboard{0x4000000000000000},
        Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}
    },
    {
        Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0}, Bitboard{0x0},
        Bitboard{0x2}, Bitboard{0x5}, Bitboard{0xa}, Bitboard{0x14}, Bitboard{0x28}, Bitboard{0x50}, Bitboard{0xa0}, Bitboard{0x40},
        Bitboard{0x200}, Bitboard{0x500}, Bitboard{0xa00}, Bitboard{0x1400}, Bitboard{0x2800}, Bitboard{0x5000}, Bitboard{0xa000}, Bitboard{0x4000},
        Bitboard{0x20000}, Bitboard{0x50000}, Bitboard{0xa0000}, Bitboard{0x140000}, Bitboard{0x280000}, Bitboard{0x500000}, Bitboard{0xa00000}, Bitboard{0x400000},
        Bitboard{0x2000000}, Bitboard{0x5000000}, Bitboard{0xa000000}, Bitboard{0x14000000}, Bitboard{0x28000000}, Bitboard{0x50000000}, Bitboard{0xa0000000}, Bitboard{0x40000000},
        Bitboard{0x200000000}, Bitboard{0x500000000}, Bitboard{0xa00000000}, Bitboard{0x1400000000}, Bitboard{0x2800000000}, Bitboard{0x5000000000}, Bitboard{0xa000000000}, Bitboard{0x4000000000},
        Bitboard{0x20000000000}, Bitboard{0x50000000000}, Bitboard{0xa0000000000}, Bitboard{0x140000000000}, Bitboard{0x280000000000}, Bitboard{0x500000000000}, Bitboard{0xa00000000000}, Bitboard{0x400000000000},
        Bitboard{0x2000000000000}, Bitboard{0x5000000000000}, Bitboard{0xa000000000000}, Bitboard{0x14000000000000}, Bitboard{0x28000000000000}, Bitboard{0x50000000000000}, Bitboard{0xa0000000000000}, Bitboard{0x40000000000000}
    }
};

inline constexpr Bitboard KNIGHT_MOVE_MASKS[64] = {
    Bitboard{0x20400}, Bitboard{0x50800}, Bitboard{0xa1100}, Bitboard{0x142200}, Bitboard{0x284400}, Bitboard{0x508800}, Bitboard{0xa01000}, Bitboard{0x402000},
    Bitboard{0x2040004}, Bitboard{0x5080008}, Bitboard{0xa110011}, Bitboard{0x14220022}, Bitboard{0x28440044}, Bitboard{0x50880088}, Bitboard{0xa0100010}, Bitboard{0x40200020},
    Bitboard{0x204000402}, Bitboard{0x508000805}, Bitboard{0xa1100110a}, Bitboard{0x1422002214}, Bitboard{0x2844004428}, Bitboard{0x5088008850}, Bitboard{0xa0100010a0}, Bitboard{0x4020002040},
    Bitboard{0x20400040200}, Bitboard{0x50800080500}, Bitboard{0xa1100110a00}, Bitboard{0x142200221400}, Bitboard{0x284400442800}, Bitboard{0x508800885000}, Bitboard{0xa0100010a000}, Bitboard{0x402000204000},
    Bitboard{0x2040004020000}, Bitboard{0x5080008050000}, Bitboard{0xa1100110a0000}, Bitboard{0x14220022140000}, Bitboard{0x28440044280000}, Bitboard{0x50880088500000}, Bitboard{0xa0100010a00000}, Bitboard{0x40200020400000},
    Bitboard{0x204000402000000}, Bitboard{0x508000805000000}, Bitboard{0xa1100110a000000}, Bitboard{0x1422002214000000}, Bitboard{0x2844004428000000}, Bitboard{0x5088008850000000}, Bitboard{0xa0100010a0000000}, Bitboard{0x4020002040000000},
    Bitboard{0x400040200000000}, Bitboard{0x800080500000000}, Bitboard{0x1100110a00000000}, Bitboard{0x2200221400000000}, Bitboard{0x4400442800000000}, Bitboard{0x8800885000000000}, Bitboard{0x100010a000000000}, Bitboard{0x2000204000000000},
    Bitboard{0x4020000000000}, Bitboard{0x8050000000000}, Bitboard{0x110a0000000000}, Bitboard{0x22140000000000}, Bitboard{0x44280000000000}, Bitboard{0x88500000000000}, Bitboard{0x10a00000000000}, Bitboard{0x20400000000000}
};

inline constexpr Bitboard KING_MOVE_MASKS[64] = {
    Bitboard{0x302}, Bitboard{0x705}, Bitboard{0xe0a}, Bitboard{0x1c14}, Bitboard{0x3828}, Bitboard{0x7050}, Bitboard{0xe0a0}, Bitboard{0xc040},
    Bitboard{0x30203}, Bitboard{0x70507}, Bitboard{0xe0a0e}, Bitboard{0x1c141c}, Bitboard{0x382838}, Bitboard{0x705070}, Bitboard{0xe0a0e0}, Bitboard{0xc040c0},
    Bitboard{0x3020300}, Bitboard{0x7050700}, Bitboard{0xe0a0e00}, Bitboard{0x1c141c00}, Bitboard{0x38283800}, Bitboard{0x70507000}, Bitboard{0xe0a0e000}, Bitboard{0xc040c000},
    Bitboard{0x302030000}, Bitboard{0x705070000}, Bitboard{0xe0a0e0000}, Bitboard{0x1c141c0000}, Bitboard{0x3828380000}, Bitboard{0x7050700000}, Bitboard{0xe0a0e00000}, Bitboard{0xc040c00000},
    Bitboard{0x30203000000}, Bitboard{0x70507000000}, Bitboard{0xe0a0e000000}, Bitboard{0x1c141c000000}, Bitboard{0x382838000000}, Bitboard{0x705070000000}, Bitboard{0xe0a0e0000000}, Bitboard{0xc040c0000000},
    Bitboard{0x3020300000000}, Bitboard{0x7050700000000}, Bitboard{0xe0a0e00000000}, Bitboard{0x1c141c00000000}, Bitboard{0x38283800000000}, Bitboard{0x70507000000000}, Bitboard{0xe0a0e000000000}, Bitboard{0xc040c000000000},
    Bitboard{0x302030000000000}, Bitboard{0x705070000000000}, Bitboard{0xe0a0e0000000000}, Bitboard{0x1c141c0000000000}, Bitboard{0x3828380000000000}, Bitboard{0x7050700000000000}, Bitboard{0xe0a0e00000000000}, Bitboard{0xc040c00000000000},
    Bitboard{0x203000000000000}, Bitboard{0x507000000000000}, Bitboard{0xa0e000000000000}, Bitboard{0x141c000000000000}, Bitboard{0x2838000000000000}, Bitboard{0x5070000000000000}, Bitboard{0xa0e0000000000000}, Bitboard{0x40c0000000000000}
};

inline constexpr Bitboard BISHOP_MOVE_MASKS[64] = {
    Bitboard{0x8040201008040200}, Bitboard{0x80402010080500}, Bitboard{0x804020110a00}, Bitboard{0x8041221400}, Bitboard{0x182442800}, Bitboard{0x10204885000}, Bitboard{0x102040810a000}, Bitboard{0x102040810204000}, Bitboard{0x4020100804020002}, Bitboard{0x8040201008050005}, Bitboard{0x804020110a000a}, Bitboard{0x804122140014}, Bitboard{0x18244280028}, Bitboard{0x1020488500050}, Bitboard{0x102040810a000a0}, Bitboard{0x204081020400040}, Bitboard{0x2010080402000204}, Bitboard{0x4020100805000508}, Bitboard{0x804020110a000a11}, Bitboard{0x80412214001422}, Bitboard{0x1824428002844}, Bitboard{0x102048850005088}, Bitboard{0x2040810a000a010}, Bitboard{0x408102040004020}, Bitboard{0x1008040200020408}, Bitboard{0x2010080500050810}, Bitboard{0x4020110a000a1120}, Bitboard{0x8041221400142241}, Bitboard{0x182442800284482}, Bitboard{0x204885000508804}, Bitboard{0x40810a000a01008}, Bitboard{0x810204000402010}, Bitboard{0x804020002040810}, Bitboard{0x1008050005081020}, Bitboard{0x20110a000a112040}, Bitboard{0x4122140014224180}, Bitboard{0x8244280028448201}, Bitboard{0x488500050880402}, Bitboard{0x810a000a0100804}, Bitboard{0x1020400040201008}, Bitboard{0x402000204081020}, Bitboard{0x805000508102040}, Bitboard{0x110a000a11204080}, Bitboard{0x2214001422418000}, Bitboard{0x4428002844820100}, Bitboard{0x8850005088040201}, Bitboard{0x10a000a010080402}, Bitboard{0x2040004020100804}, Bitboard{0x200020408102040}, Bitboard{0x500050810204080}, Bitboard{0xa000a1120408000}, Bitboard{0x1400142241800000}, Bitboard{0x2800284482010000}, Bitboard{0x5000508804020100}, Bitboard{0xa000a01008040201}, Bitboard{0x4000402010080402}, Bitboard{0x2040810204080}, Bitboard{0x5081020408000}, Bitboard{0xa112040800000}, Bitboard{0x14224180000000}, Bitboard{0x28448201000000}, Bitboard{0x50880402010000}, Bitboard{0xa0100804020100}, Bitboard{0x40201008040201}, 
};

inline constexpr Bitboard ROOK_MOVE_MASKS[64] = {
    Bitboard{0x1010101010101fe}, Bitboard{0x2020202020202fd}, Bitboard{0x4040404040404fb}, Bitboard{0x8080808080808f7}, Bitboard{0x10101010101010ef}, Bitboard{0x20202020202020df}, Bitboard{0x40404040404040bf}, Bitboard{0x808080808080807f}, Bitboard{0x10101010101fe01}, Bitboard{0x20202020202fd02}, Bitboard{0x40404040404fb04}, Bitboard{0x80808080808f708}, Bitboard{0x101010101010ef10}, Bitboard{0x202020202020df20}, Bitboard{0x404040404040bf40}, Bitboard{0x8080808080807f80}, Bitboard{0x101010101fe0101}, Bitboard{0x202020202fd0202}, Bitboard{0x404040404fb0404}, Bitboard{0x808080808f70808}, Bitboard{0x1010101010ef1010}, Bitboard{0x2020202020df2020}, Bitboard{0x4040404040bf4040}, Bitboard{0x80808080807f8080}, Bitboard{0x1010101fe010101}, Bitboard{0x2020202fd020202}, Bitboard{0x4040404fb040404}, Bitboard{0x8080808f7080808}, Bitboard{0x10101010ef101010}, Bitboard{0x20202020df202020}, Bitboard{0x40404040bf404040}, Bitboard{0x808080807f808080}, Bitboard{0x10101fe01010101}, Bitboard{0x20202fd02020202}, Bitboard{0x40404fb04040404}, Bitboard{0x80808f708080808}, Bitboard{0x101010ef10101010}, Bitboard{0x202020df20202020}, Bitboard{0x404040bf40404040}, Bitboard{0x8080807f80808080}, Bitboard{0x101fe0101010101}, Bitboard{0x202fd0202020202}, Bitboard{0x404fb0404040404}, Bitboard{0x808f70808080808}, Bitboard{0x1010ef1010101010}, Bitboard{0x2020df2020202020}, Bitboard{0x4040bf4040404040}, Bitboard{0x80807f8080808080}, Bitboard{0x1fe010101010101}, Bitboard{0x2fd020202020202}, Bitboard{0x4fb040404040404}, Bitboard{0x8f7080808080808}, Bitboard{0x10ef101010101010}, Bitboard{0x20df202020202020}, Bitboard{0x40bf404040404040}, Bitboard{0x807f808080808080}, Bitboard{0xfe01010101010101}, Bitboard{0xfd02020202020202}, Bitboard{0xfb04040404040404}, Bitboard{0xf708080808080808}, Bitboard{0xef10101010101010}, Bitboard{0xdf20202020202020}, Bitboard{0xbf40404040404040}, Bitboard{0x7f80808080808080}, 
};

inline constexpr Bitboard W_QS_CASTLE_MASK{0xE};
inline constexpr Bitboard W_KS_CASTLE_MASK{0x60};
inline constexpr Bitboard B_QS_CASTLE_MASK{0xE00000000000000};
inline constexpr Bitboard B_KS_CASTLE_MASK{0x6000000000000000};

} // namespace mozzarella
