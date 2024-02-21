#include "board.h"
#include "movegen.h"
#include "staged.h"
#include <stdint.h>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>




StagedMoveHandler::StagedMoveHandler(Board* b){
	board = b;
}

void StagedMoveHandler::reset(){
	stage = KING_CASTLE_STAGE;
	pieceType = -1;
	targetType = QUEEN;
	promo = 0;
	moves = 0ULL;
	piecesBB = 0ULL;
	singular = 0ULL;
}

uint16_t StagedMoveHandler::nextMove(){
	bool color = board->sideToMove;
	
	if (stage == KING_CASTLE_STAGE){
		stage++;
		if ((bool)(board->occ() & (color == WHITE ? WHITE_KING_CASTLE_BLOCKERS : BLACK_KING_CASTLE_BLOCKERS)) 
			|| board->attackedBy(color == WHITE ? 1 : 57, !color) 
			|| board->attackedBy(color == WHITE ? 2 : 58, !color)
			|| board->inCheck(color)
			|| (board->pieces(ROOK, color) & (color == WHITE ? 1ULL : BB(56))) == 0
			|| !(bool)(board->castleHistory[board->ply] & (color == WHITE ? WHITE_OO : BLACK_OO)) ){
			return nextMove();
		}
		else
			return color == WHITE ? WHITE_OO_MOVE : BLACK_OO_MOVE;
	}
	if (stage == QUEEN_CASTLE_STAGE){
		stage++;
		if ((bool)(board->occ() & (color == WHITE ? WHITE_QUEEN_CASTLE_BLOCKERS : BLACK_QUEEN_CASTLE_BLOCKERS)) 
			|| board->attackedBy(color == WHITE ? 4 : 60, !color) 
			|| board->attackedBy(color == WHITE ? 5 : 61, !color)
			|| board->inCheck(color)
			|| (board->pieces(ROOK, color) & (color == WHITE ? BB(7) : BB(63))) == 0
			|| !(bool)(board->castleHistory[board->ply] & (color == WHITE ? WHITE_OOO : BLACK_OOO))){
			// board->printBitboard(board->pieces(ROOK, WHITE));
			// board->printBitboard(board->pieces(ROOK, BLACK));
			return nextMove();
		}
		else
			return color == WHITE ? WHITE_OOO_MOVE : BLACK_OOO_MOVE;
	}
	if (stage == CAPTURE_STAGE){
		if (piecesBB == 0){
			pieceType ++;
			if (pieceType > KING){
				stage ++;
				pieceType = -1;
				targetType = QUEEN;
				moves = 0ULL;
				return nextMove();
			}
			if (targetType < PAWN)
				return 0;
			piecesBB = board->pieces(pieceType, color);
			singular = rightBit(piecesBB);
			moves = 0ULL;
		}
		if (singular == 0){
			return nextMove();
		}
		if (moves == 0){
			if (pieceType == PAWN){
				uint64_t epTarget = 0;
				moves = board->genPawn(singular, color, CAPTURE, epTarget);
				moves &= board->pieces(targetType, !color) | (targetType == PAWN ? epTarget : 0ULL);
				
			}
			else
				moves = board->genPiece(singular, color, pieceType, CAPTURE) & board->pieces(targetType, !color);
		}
		if (moves != 0) {
			uint16_t m = 0ULL;
			if (pieceType == PAWN){
				m = board->readPawn(singular, moves, color, promo);
			}
			else
				m = board->readPiece(singular, moves, color);
			//printf("promo %d\n", promo);
			//board->printBitboard(moves);
			if (moves == 0 && targetType == PAWN){

				piecesBB &= ~singular;
				singular = rightBit(piecesBB);
				targetType = QUEEN;
				promo = 0;
			}
			else if (moves == 0){
				targetType--;
				promo = 0;
			}
			return m;
		}
		if (moves == 0 && targetType == PAWN){
			promo = 0;
			piecesBB &= ~singular;
			singular = rightBit(piecesBB);
			targetType = QUEEN;

			return nextMove();
		}
		else if (moves == 0){
			promo = 0;
			targetType--;
			return nextMove();
		}
	}
	if (stage == QUIET_STAGE){
		if (piecesBB == 0){
			pieceType++;
			if (pieceType > KING){
				reset();
				return 0;
			}
			piecesBB = board->pieces(pieceType, color);
			singular = rightBit(piecesBB);
			moves = 0ULL;
		}
		if (singular == 0){
			return nextMove();
		}
		if (moves == 0){
			if (pieceType == PAWN){
				uint64_t epTarget;
				moves = board->genPawn(singular, color, QUIET, epTarget) & ~board->occ();
				//moves &= ~epTarget;
			}
			else
				moves = board->genPiece(singular, color, pieceType, QUIET) & ~board->occ();
			//board->printBitboard(moves);

		}
		if (moves != 0) {
			uint16_t m = 0ULL;
			if (pieceType == PAWN){

				m = board->readPawn(singular, moves, color, promo);
			}
			else
				m = board->readPiece(singular, moves, color);
			if (moves == 0){
				promo = 0;
				piecesBB &= ~singular;
				singular = rightBit(piecesBB);
			}
			return m;
		}
		if (moves == 0){
			promo = 0;
			piecesBB &= ~singular;
			singular = rightBit(piecesBB);
			return nextMove();
		}
	}
	return 0;
}