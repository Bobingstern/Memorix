#include <stdint.h>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

#define WHITE_KING_CASTLE_BLOCKERS 0x6ULL
#define WHITE_QUEEN_CASTLE_BLOCKERS 0x70ULL
#define BLACK_KING_CASTLE_BLOCKERS 0x600000000000000ULL
#define BLACK_QUEEN_CASTLE_BLOCKERS 0x7000000000000000ULL


enum {
	NO_STAGE, KING_CASTLE_STAGE, QUEEN_CASTLE_STAGE, CAPTURE_STAGE, QUIET_STAGE	
};


class StagedMoveHandler{
	public:
		int stage = NO_STAGE;
		int pieceType = -1;
		int targetType = QUEEN;
		int promo = 0;
		uint64_t moves = 0ULL;
		uint64_t piecesBB = 0ULL;
		uint64_t singular = 0ULL;
		Board* board = nullptr;

		StagedMoveHandler(Board* b);
		void reset();
		uint16_t nextMove();
};