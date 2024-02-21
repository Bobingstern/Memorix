#include "board.h"
#include "movegen.h"
#include <stdint.h>
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>



static const char PieceChars[] = "PNBRQKpnbrqk.";



void moveSet(uint16_t &move, uint8_t from, uint8_t to, bool doublePawnPush, bool capture, bool enPassant, int promo, uint8_t castle) {
  move = (uint16_t)from;
  move |= (uint16_t)to << 6;
  uint8_t flags = 0;
  if (doublePawnPush) {
    flags = 0b00000001;
  } else if (castle != 0) {
    flags = castle;
  } else if (capture) {
    if (promo != 0) {
      flags = 0b1100 + (promo - 1);
    } else {
      flags = 0b0100;
    }
  } else if (promo != 0) {
    flags = 0b1000 + (promo - 1);
  } else if (enPassant) {
    flags = 0b0101;
  }

  move |= (uint16_t)flags << 12;
}

uint16_t movePack(uint8_t from, uint8_t to, bool doublePawnPush, bool capture, bool enPassant, int promo, uint8_t castle) {
  uint16_t move = (uint16_t)0b0000000000000000;
  moveSet(move, from, to, doublePawnPush, capture, enPassant, promo, castle);
  return move;
}
int chrType(char c){
  switch (c){
    case 'P': return 0;
    case 'N': return 1;
    case 'B': return 2;
    case 'R': return 3;
    case 'Q': return 4;
    case 'K': return 5;

    case 'p': return 6;
    case 'n': return 7;
    case 'b': return 8;
    case 'r': return 9;
    case 'q': return 10;
    case 'k': return 11;  
    case '.': return 12;
  }
  return -1;
}

int Board::makeSquare(int file, int rank){
  return (7-file) + 8*rank;
}

int Board::strToSq(char* str){
  return makeSquare(str[0]-'a', str[1]-'1');
}

void Board::printBitboard(uint64_t bb) {
  for (uint8_t y = 0; y < 8; y++) {
    for (uint8_t x = 0; x < 8; x++) {
      const uint8_t index = y * 8 + x;
      printf("%d", bitRead(bb, index));
    }
    printf("\n");
  }
  printf("\n");
}

void Board::printBoard() {
  for (uint8_t y = 0; y < 8; y++) {
    for (uint8_t x = 0; x < 8; x++) {
      const uint8_t index = y * 8 + x;
      if (bitRead(pieces(PAWN, WHITE), index)) {
        printf("P");
      } else if (bitRead(pieces(PAWN, BLACK), index)) {
        printf("p");
      } else if (bitRead(pieces(KNIGHT, WHITE), index)) {
        printf("N");
      } else if (bitRead(pieces(KNIGHT, BLACK), index)) {
        printf("n");
      } else if (bitRead(pieces(BISHOP, WHITE), index)) {
        printf("B");
      } else if (bitRead(pieces(BISHOP, BLACK), index)) {
        printf("b");
      } else if (bitRead(pieces(ROOK, WHITE), index)) {
        printf("R");
      } else if (bitRead(pieces(ROOK, BLACK), index)) {
        printf("r");
      } else if (bitRead(pieces(QUEEN, WHITE), index)) {
        printf("Q");
      } else if (bitRead(pieces(QUEEN, BLACK), index)) {
        printf("q");
      } else if (bitRead(pieces(KING, WHITE), index)) {
        printf("K");
      } else if (bitRead(pieces(KING, BLACK), index)) {
        printf("k");
      } else {
        printf(".");
      }
      printf(" ");
    }
    printf("\n");
  }
  printf("\n");
}


bool Board::sqColor(int sq){
  if ( (this->bitboards[WHITE_BB] & (0x1ULL << sq)) != 0 ){
    return WHITE;
  }
  return BLACK;
}

int Board::sqType(int sq){
  uint64_t bb = 0x1ULL << sq;
  for (int i=0;i<6;i++){
    if ( (this->bitboards[i] & bb) != 0)
      return i;
  }
}


void Board::addPiece(int sq, int type, bool color){

  this->bitboards[type] |= 0x1ULL << sq;
  this->bitboards[6+color] |= 0x1ULL << sq;
  //printBitboard(bitboards[BLACK_BB]);
}

void Board::addPiece(int sq, char t){
  int type = chrType(t);
  if (type > 11)
    return;
  this->bitboards[type%6] |= 0x1ULL << sq;
  this->bitboards[6+(type>=6)] |= 0x1ULL << sq;
}

void Board::removePiece(int sq){
  this->bitboards[sqType(sq)] &= ~(0x1ULL << sq);
  this->bitboards[sqColor(sq)+6] &= ~(0x1ULL << sq);
}

uint64_t Board::pieces(int type, bool color){
  return this->bitboards[type] & this->bitboards[color + 6];
}

uint64_t Board::pieces(int type){
  return this->bitboards[type];
}

uint64_t Board::pieces(bool color){
    return bitboards[color + 6];
}


uint64_t Board::occ(){
  return bitboards[WHITE_BB] | bitboards[BLACK_BB];
}

uint16_t Board::parseMove(char *str){
  int from = strToSq(str);
  int to = strToSq(str+2);
  bool capture = (0x1ULL<<to) & occ();
  int type = sqType(from);
  int promo = str[4] == 'n' ? 1
            : str[4] == 'b' ? 2
            : str[4] == 'r' ? 3
            : str[4] == 'q' ? 4
            : 0;
  uint8_t castle = 0;
  if (type == KING)
    castle = (from == 3 && to == 1) ? KING_CASTLE
            : (from == 59 && to == 57) ? KING_CASTLE
            : (from == 3 && to == 5) ? QUEEN_CASTLE
            : (from == 59 && to == 61) ? QUEEN_CASTLE
            : 0;

  bool ep = (from - to == 9 || from - to == 7 || to - from == 9 || from - to == 7) && !capture && type == PAWN;
  return movePack(from, to, (from-to == 16 || to-from == 16) && type == PAWN, capture, ep, promo, castle);
}


void sqToStr(int sq, char *str) {
    str[1] = '1' + ((sq) / 8);
    str[0] = 'a' + (7-(int)((sq) % 8));
}
char *Board::moveToStr(uint16_t m){
    static char moveStr[6] = "";
    sqToStr(getFrom(m), moveStr);
    sqToStr(getTo(m), moveStr+2);
    switch (getFlag(m)){
        case (KNIGHT_PROMO): {
            moveStr[4] = 'n';break;
        }
        case (KNIGHT_PROMO_CAPTURE): {
                moveStr[4] = 'n';break;
        }

        case (BISHOP_PROMO): {
            moveStr[4] = 'b';break;
        }
        case (BISHOP_PROMO_CAPTURE): {
                moveStr[4] = 'b';break;
        }

        case (ROOK_PROMO): {
            moveStr[4] = 'r';break;
        }
        case (ROOK_PROMO_CAPTURE): {
                moveStr[4] = 'r';break;
        }
        case (QUEEN_PROMO): {
            moveStr[4] = 'q';break;
        }
        case (QUEEN_PROMO_CAPTURE): {
                moveStr[4] = 'q';break;
        }
        default:{
            moveStr[4] = ' ';
        }
    }
    return moveStr;

}

void Board::parseFen(const char *fen){
  for (int i=0;i<8;i++){
    bitboards[i] *= 0;
  }
  castleRights = 0;
  char c, *copy = strdup(fen);
  char *token = strtok(copy, " ");
  int sq = A8;
  while ((c = *token++))
    switch (c) {
      case '/': sq -= 0; break;
      case '1' ... '8': sq -= c - '0'; break;
      default: { addPiece(sq--, c);};
    }

  this->sideToMove = *strtok(NULL, " ") == 'w' ? WHITE : BLACK;
  token = strtok(NULL, " ");
  while ((c = *token++)) {
    switch (c) {
      case 'K': castleRights |= WHITE_OO; break;
      case 'k': castleRights |= BLACK_OO; break;
      case 'Q': castleRights |= WHITE_OOO; break;
      case 'q': castleRights |= BLACK_OOO; break;
    }
  }
  castleHistory[ply] = castleRights;
}

void Board::uciPosition(char* str){
  bool isFen = !strncmp(str, "position fen", 12);
  //printf("isFen %d\n", isFen);
  parseFen(isFen ? str + 13 : START_FEN);
  if ((str = strstr(str, "moves")) == NULL) return;

  char *move = strtok(str, " ");
  while ((move = strtok(NULL, " "))){
    makeMove(parseMove(move));
    ply = 0;
  }
  
}
//------------------------------------------------------------------------

void Board::makeMove(uint16_t move){
  ply++;
  moveHistory[ply] = move;
  castleHistory[ply] = castleRights;
  captureHistory[ply] = -1;
  if (ply > 0)
    castleHistory[ply] = castleHistory[ply-1];


  int from = getFrom(move);
  int to = getTo(move);
  uint8_t flag = getFlag(move);
  int type = sqType(from);
  bool color = sqColor(from);
  removePiece(from);


  if (from == 0 && type == ROOK && color == WHITE)
    castleHistory[ply] &= ~WHITE_OO;
  else if (from == 7 && type == ROOK && color == WHITE)
    castleHistory[ply] &= ~WHITE_OOO;
  else if (from == 56 && type == ROOK && color == BLACK)
    castleHistory[ply] &= ~BLACK_OO;
  else if (from == 63 && type == ROOK && color == BLACK)
    castleHistory[ply] &= ~BLACK_OOO;
  else if (type == KING && color == WHITE)
    castleHistory[ply] &= ~WHITE_CASTLE;
  else if (type == KING && color == BLACK)
    castleHistory[ply] &= ~BLACK_CASTLE;

  switch (flag){
    case CAPTURE:{
      
      switch (color){
        case (WHITE):{
          if (to == 56)
            castleHistory[ply] &= ~BLACK_OO;
          if (to == 63)
            castleHistory[ply] &= ~BLACK_OOO;
          break;
        }
        case (BLACK):{
          if (to == 0){
            castleHistory[ply] &= ~WHITE_OO;
          }
          if (to == 7)
            castleHistory[ply] &= ~WHITE_OOO;
          break;
        }
      }
      captureHistory[ply] = sqType(to); removePiece(to); break;
    }
    case EP_CAPTURE:{
      captureHistory[ply] = PAWN;
      switch (color){
        case WHITE: removePiece( to - from == 9 ? from+1 : from-1);break;
        case BLACK: removePiece( from - to == 9 ? from-1 : from+1);break;
      }
      break;
    }
    case KING_CASTLE: {
      switch (color){
        case WHITE: removePiece(0); addPiece(2, ROOK, WHITE); castleHistory[ply] &= ~WHITE_OO; break;
        case BLACK: removePiece(56); addPiece(58, ROOK, BLACK); castleHistory[ply] &= ~BLACK_OO; break;
      }
      break;
    }
    case QUEEN_CASTLE: {
      switch (color){
        case WHITE: removePiece(7); addPiece(4, ROOK, WHITE); castleHistory[ply] &= ~WHITE_OOO; break;
        case BLACK: removePiece(63); addPiece(60, ROOK, BLACK); castleHistory[ply] &= ~BLACK_OOO; break;
      }
      break;

    }
    case KNIGHT_PROMO: {
      addPiece(to, KNIGHT, color);break;
    }; 
    case BISHOP_PROMO: {
      addPiece(to, BISHOP, color);break;
    };
    case ROOK_PROMO: {
      addPiece(to, ROOK, color);break;
    };
    case QUEEN_PROMO: {
      addPiece(to, QUEEN, color);break;
    };

    case KNIGHT_PROMO_CAPTURE: {
      captureHistory[ply] = sqType(to); removePiece(to); addPiece(to, KNIGHT, color); break;
    }; 
    case BISHOP_PROMO_CAPTURE: {
      captureHistory[ply] = sqType(to); removePiece(to); addPiece(to, BISHOP, color);break;
    }; 
    case ROOK_PROMO_CAPTURE: {
      captureHistory[ply] = sqType(to); removePiece(to); addPiece(to, ROOK, color);break;
    }; 
    case QUEEN_PROMO_CAPTURE: {
      captureHistory[ply] = sqType(to); removePiece(to); addPiece(to, QUEEN, color);break;
    };
  }

  if (flag < KNIGHT_PROMO)
    addPiece(to, type, color);
  

  sideToMove = !sideToMove;
  //printf("From %d to %d flag %u color %d type %d\n", from, to, flag, color, type);
}

void Board::unmakeMove(){
  uint16_t move = moveHistory[ply];
  int from = getFrom(move);
  int to = getTo(move);
  uint8_t flag = getFlag(move);
  int type = sqType(to);
  bool color = sqColor(to);

  removePiece(to);

  switch (flag){
    case CAPTURE:
      addPiece(to, captureHistory[ply], !color); break;
    case EP_CAPTURE: {
      switch (color){
        case WHITE: addPiece( to - from == 9 ? from+1 : from-1, PAWN, BLACK);break;
        case BLACK: addPiece( from - to == 9 ? from-1 : from+1, PAWN, WHITE);break;
      }
    }
    break;
    case KING_CASTLE: {
      switch (color){
        case WHITE: removePiece(2); addPiece(0, ROOK, WHITE); break;
        case BLACK: removePiece(58); addPiece(56, ROOK, BLACK); break;
      }
      break;
    }
    case QUEEN_CASTLE: {
      switch (color){
        case WHITE: removePiece(4); addPiece(7, ROOK, WHITE); break;
        case BLACK: removePiece(60); addPiece(63, ROOK, BLACK); break;
      }
      break;
    }
    case KNIGHT_PROMO ... QUEEN_PROMO: {
      addPiece(from, PAWN, color);break;
    }; 

    case KNIGHT_PROMO_CAPTURE ... QUEEN_PROMO_CAPTURE: {
      addPiece(to, captureHistory[ply], !color); addPiece(from, PAWN, color); break;
    }
  }
  if (flag < KNIGHT_PROMO)
    addPiece(from, type, color);
  captureHistory[ply] = -1;
  moveHistory[ply] = 0;
  castleHistory[ply] = 0;
  ply--;
  sideToMove = !sideToMove;
}


bool Board::inCheck(bool color){
  return attackedBy(ntz(pieces(KING, color)), !color);
}

Board::Board(){
  this->bitboards = new uint64_t[8];
  this->bitboards[PAWN] = 0x000000000000FF00ULL | 0x00FF000000000000ULL;
  this->bitboards[KNIGHT] = 0x0000000000000042ULL | 0x4200000000000000ULL;
  this->bitboards[BISHOP] = 0x0000000000000024ULL | 0x2400000000000000ULL;
  this->bitboards[ROOK] = 0x0000000000000081ULL | 0x8100000000000000ULL;
  this->bitboards[QUEEN] = 0x0000000000000010ULL | 0x1000000000000000ULL;
  this->bitboards[KING] = 0x000000000000008ULL | 0x0800000000000000ULL;

  this->bitboards[WHITE_BB] = 0xffffULL;
  this->bitboards[BLACK_BB] = 0xffff000000000000ULL;

  this->moveHistory = new uint16_t[32];
  this->castleHistory = new uint8_t[32]; //2 castle flags can be stored per 8 bits
  this->captureHistory = new int8_t[32];

  captureHistory[0] = -1;
  moveHistory[0] = 0;
  castleHistory[0] = ALL_CASTLE;
}
