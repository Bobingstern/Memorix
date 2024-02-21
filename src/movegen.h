#include <stdint.h>

#define NORTH 8
#define WEST 1
#define SOUTH -8
#define EAST -1

enum {
    fileHBB = 0x0101010101010101,
    fileGBB = 0x0202020202020202,
    fileFBB = 0x0404040404040404,
    fileEBB = 0x0808080808080808,
    fileDBB = 0x1010101010101010,
    fileCBB = 0x2020202020202020,
    fileBBB = 0x4040404040404040,
    fileABB = 0x8080808080808080,

    rank1BB = 0xFF,
    rank2BB = 0xFF00,
    rank3BB = 0xFF0000,
    rank4BB = 0xFF000000,
    rank5BB = 0xFF00000000,
    rank6BB = 0xFF0000000000,
    rank7BB = 0xFF000000000000,
    rank8BB = 0xFF00000000000000,

    BlackSquaresBB = 0xAA55AA55AA55AA55,

    QueenSideBB = fileABB | fileBBB | fileCBB | fileDBB,
    KingSideBB  = fileEBB | fileFBB | fileGBB | fileHBB,
};

#define BB(sq) (1ULL << sq)

int ntz(uint64_t x);
uint64_t rightBit(uint64_t x);
uint64_t shift(uint64_t bb, int dir);
uint64_t bit_bswap(uint64_t b);
uint64_t attack(uint64_t pieces, uint32_t x, uint64_t mask);
uint64_t horizontal_attack(uint64_t pieces, uint32_t x);
uint64_t vertical_attack(uint64_t occ, uint32_t sq);
uint64_t diagonal_attack(uint64_t occ, uint32_t sq);
uint64_t antidiagonal_attack(uint64_t occ, uint32_t sq);
uint64_t bishop_attack(int sq, uint64_t occ);
uint64_t rook_attack(int sq, uint64_t occ);
uint64_t queen_attack(int sq, uint64_t occ);
uint64_t knight_attack(int sq);
uint64_t king_attack(int sq);