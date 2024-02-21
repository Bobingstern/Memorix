#include <stdint.h>


enum PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, ALL, TYPE_NB = 8
};

#define WHITE 0
#define BLACK 1

#define WHITE_BB 6
#define BLACK_BB 7

#define QUIET 0
#define DOUBLE_PAWN_PUSH 1
#define KING_CASTLE 2
#define QUEEN_CASTLE 3
#define CAPTURE 4
#define EP_CAPTURE 5
#define KNIGHT_PROMO 8
#define BISHOP_PROMO 9
#define ROOK_PROMO 10
#define QUEEN_PROMO 11
#define KNIGHT_PROMO_CAPTURE 12
#define BISHOP_PROMO_CAPTURE 13
#define ROOK_PROMO_CAPTURE 14
#define QUEEN_PROMO_CAPTURE 15

#define WHITE_OO_MOVE 0b0010000001000011
#define BLACK_OO_MOVE 0b0010111001111011
#define WHITE_OOO_MOVE 0b0011000101000011
#define BLACK_OOO_MOVE 0b0011111101111011

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

//
enum CastlingRights {
    WHITE_OO  = 1,
    WHITE_OOO = 2,
    BLACK_OO  = 4,
    BLACK_OOO = 8,

    OO  = WHITE_OO  | BLACK_OO,
    OOO = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLE = WHITE_OO | WHITE_OOO,
    BLACK_CASTLE = BLACK_OO | BLACK_OOO,
    ALL_CASTLE = WHITE_CASTLE | BLACK_CASTLE
};

enum Square {
    H1, G1, F1, E1, D1, C1, B1, A1,
    H2, G2, F2, E2, D2, C2, B2, A2,
    H3, G3, F3, E3, D3, C3, B3, A3,
    H4, G4, F4, E4, D4, C4, B4, A4,
    H5, G5, F5, E5, D5, C5, B5, A5,
    H6, G6, F6, E6, D6, C6, B6, A6,
    H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8

};
enum File {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};

enum Rank {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};


#define getFlag(move) ((move & (0b1111000000000000)) >> 12)
#define getFrom(move) (move & (0b111111))
#define getTo(move) ((move & (0b0000111111000000)) >> 6)
#define bitRead(value, bit) (((value) >> (63-bit)) & 0x01)

void moveSet(uint16_t &move, uint8_t from, uint8_t to, bool doublePawnPush, bool capture, bool enPassant, int promo, uint8_t castle);
uint16_t movePack(uint8_t from, uint8_t to, bool doublePawnPush, bool capture, bool enPassant, int promo, uint8_t castle);

class Board {
  public:
    Board();
    void printBitboard(uint64_t bb);
    void printBoard();
    void addPiece(int sq, int type, bool color);
    void addPiece(int sq, char type);
    void removePiece(int sq);
    uint64_t occ();
    int sqType(int sq);
    bool sqColor(int sq);
    int strToSq(char *str);
    int makeSquare(int file, int rank);
    uint64_t pieces(int type, bool color);
    uint64_t pieces(int type);
    uint64_t pieces(bool color);
    void parseFen(const char* fen);
    void uciPosition(char* str);
    uint16_t parseMove(char* str);
    //---
    void makeMove(uint16_t move);
    void unmakeMove();
    bool inCheck(bool color);
    char *moveToStr(uint16_t m);
    //---
    uint64_t genPawn(uint64_t x, bool color, int type, uint64_t& epTarget);
    uint16_t readPawn(uint64_t x, uint64_t& moves, bool color, int& promo);
    uint64_t genPiece(uint64_t x, bool color, int type, int moveType);
    uint16_t readPiece(uint64_t x, uint64_t& moves, bool color);

    bool attackedBy(int sq, bool color);
    uint64_t attackedByR(int sq, bool color);
    //---

    uint64_t *bitboards = nullptr;

    bool sideToMove = WHITE;
    uint8_t castleRights = ALL_CASTLE;

    uint16_t *moveHistory = nullptr;
    uint8_t *castleHistory = nullptr;
    int8_t *captureHistory = nullptr;
    int ply = 0;

};