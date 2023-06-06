/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2023 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

/// When compiling with provided Makefile (e.g. for Linux and OSX), configuration
/// is done automatically. To get started type 'make help'.
///
/// When Makefile is not used (e.g. with Microsoft Visual Studio) some switches
/// need to be set manually:
///
/// -DNDEBUG      | Disable debugging mode. Always use this for release.
///
/// -DNO_PREFETCH | Disable use of prefetch asm-instruction. You may need this to
///               | run on some very old machines.
///
/// -DUSE_POPCNT  | Add runtime support for use of popcnt asm-instruction. Works
///               | only in 64-bit mode and requires hardware with popcnt support.
///
/// -DUSE_PEXT    | Add runtime support for use of pext asm-instruction. Works
///               | only in 64-bit mode and requires hardware with pext support.

#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <algorithm>

#if defined(_MSC_VER)
// Disable some silly and noisy warning from MSVC compiler
#pragma warning(disable: 4127) // Conditional expression is constant
#pragma warning(disable: 4146) // Unary minus operator applied to unsigned type
#pragma warning(disable: 4800) // Forcing value to bool 'true' or 'false'
#endif

/// Predefined macros hell:
///
/// __GNUC__           Compiler is gcc, Clang or Intel on Linux
/// __INTEL_COMPILER   Compiler is Intel
/// _MSC_VER           Compiler is MSVC or Intel on Windows
/// _WIN32             Building on Windows (any)
/// _WIN64             Building on Windows 64 bit

#if defined(__GNUC__ ) && (__GNUC__ < 9 || (__GNUC__ == 9 && __GNUC_MINOR__ <= 2)) && defined(_WIN32) && !defined(__clang__)
#define ALIGNAS_ON_STACK_VARIABLES_BROKEN
#endif

#define ASSERT_ALIGNED(ptr, alignment) assert(reinterpret_cast<uintptr_t>(ptr) % alignment == 0)

#if defined(_WIN64) && defined(_MSC_VER) // No Makefile used
#  include <intrin.h> // Microsoft header for _BitScanForward64()
#  define IS_64BIT
#endif

#if defined(USE_POPCNT) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#  include <nmmintrin.h> // Intel and Microsoft header for _mm_popcnt_u64()
#endif

#if !defined(NO_PREFETCH) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#  include <xmmintrin.h> // Intel and Microsoft header for _mm_prefetch()
#endif

#if defined(USE_PEXT)
#  include <immintrin.h> // Header for _pext_u64() intrinsic
#  define pext(b, m) _pext_u64(b, m)
#else
#  define pext(b, m) 0
#endif

namespace Stockfish {

#ifdef USE_POPCNT
constexpr bool HasPopCnt = true;
#else
constexpr bool HasPopCnt = false;
#endif

#ifdef USE_PEXT
constexpr bool HasPext = true;
#else
constexpr bool HasPext = false;
#endif

#ifdef IS_64BIT
constexpr bool Is64Bit = true;
#else
constexpr bool Is64Bit = false;
#endif

using Key = uint64_t;
using Bitboard = uint64_t;

constexpr int MAX_MOVES = 256;
constexpr int MAX_PLY   = 246;

// Net weights and biases of a small neural network for time management
constexpr int nw[2][736] =
{
//Hidden layer weights
{6,28,-33,37,21,-12,20,28,-57,26,-8,4,4,-17,28,7,13,2,37,-45,-16,29,-7,18,13,-43,-38,-8,-7,-7,-20,8,29,29,-15
,0,-89,4,32,49,13,57,-37,-9,-8,-5,-16,45,19,21,33,0,38,40,-16,29,-34,-37,-4,3,-8,-7,-17,22,-27,8,-42,-25,-61
,9,7,-9,23,-16,-25,-59,18,23,-29,-15,-45,0,-31,-32,14,58,-5,0,81,-3,-15,57,17,23,6,-23,18,32,-56,32,16,37,-48
,25,25,19,58,-47,31,-2,-1,-5,45,-25,-25,-37,-1,21,10,-16,-38,2,9,-8,18,-60,23,-40,11,-18,41,-16,-30,-4,1,18,-23
,-49,20,20,48,33,7,40,-21,-10,42,9,2,15,-7,-17,-8,6,-20,34,29,-7,10,8,32,8,27,-20,15,26,-22,25,-5,35,40
,9,-10,9,-15,8,-39,13,-27,-66,-43,3,-18,-3,2,16,36,-35,8,19,53,-21,57,-27,68,14,-38,10,-10,15,15,21,49,36,11
,11,31,-16,-15,-4,1,8,-9,17,-34,-38,30,15,28,-12,3,-22,2,44,-47,-20,53,-10,-48,68,-2,-37,-36,-33,14,0,52,-29,0
,-54,17,3,-2,-11,-46,-42,35,-22,11,-11,16,-8,-12,42,38,-31,-12,-5,13,8,-15,11,-30,16,21,-36,-22,0,-12,-7,49,27,-28
,-8,-13,47,50,88,33,-17,-21,18,28,31,-6,-1,7,0,-23,27,-15,-2,7,-14,53,-2,27,-34,-39,-20,6,6,23,35,-38,6,-11
,-1,-42,-23,-21,6,31,-19,-38,18,18,2,-33,14,10,15,-11,7,13,20,19,27,28,-44,-19,-33,-26,-3,38,21,-24,-25,7,-29,28
,15,-3,0,-24,21,-14,14,-23,-1,50,11,-39,-6,72,-28,25,56,-15,8,4,21,20,-2,0,-22,-31,-7,-11,45,32,-51,-9,6,-8
,7,8,-3,23,46,-15,-6,0,-10,3,35,-18,1,38,29,-64,6,-11,-20,-2,5,34,-40,7,-31,-56,3,3,-24,-30,15,28,51,-50
,4,20,41,-4,-24,44,25,6,33,-19,21,2,10,39,12,16,2,-18,16,27,1,-2,27,9,5,-6,14,-3,28,5,42,-11,7,41
,4,40,-68,3,27,41,0,2,39,-33,0,34,19,-25,22,-6,34,-17,-5,12,5,7,18,4,-3,18,15,15,-20,51,6,1,6,40
,-6,11,-8,32,3,-12,-8,-23,28,24,-10,9,-22,-4,-7,-6,28,-37,-3,12,10,16,-26,19,-1,5,-5,28,0,-6,-2,-25,34,-2
,-65,25,8,-14,58,7,0,34,13,20,9,-7,12,11,20,-8,23,-31,44,-57,11,6,-30,-38,2,10,2,20,5,8,-48,-33,-19,-25
,41,36,25,2,1,25,-72,-20,-25,29,-40,49,29,2,61,12,3,-14,-18,21,30,-16,-4,6,-19,-22,31,27,44,1,-28,-8,-6,36
,-31,4,-15,-9,8,-1,14,10,12,-4,-6,11,-29,-55,0,-21,-12,-2,4,2,-15,56,29,-1,39,44,65,20,7,-6,-20,18,10,34
,-6,43,33,19,-8,4,12,-13,17,11,15,14,-6,-36,19,-40,14,18,8,-27,-49,8,27,41,17,-8,26,-43,9,-10,45,7,-43,-18
,-22,1,0,-24,10,-34,-7,11,3,1,27,11,-12,-20,6,-18,-6,12,3,-42,-41,28,7,-46,20,9,-21,-10,25,4,36,17,-40,-69
,7,-37,-34,30,-12,-30,6,-35,-65,-44,-2,-33,7,-13,29,1,-35,-20,32,59,76,-2,-26,-46,-34,9,0,27,-31,48,2,27,3,11,29,-24,22,-3
},

{1,37,-43,10,13,-29,-8,34,-49,-40,40,47,-37,17,34,11,-20,-23,23,16,-27,4,-31,36,35,-26,-2,6,-67,-15,-29,0,-2,-6,-2
,-7,-35,22,-10,18,-46,1,23,13,22,6,23,-15,53,21,-19,0,30,5,2,6,-20,-47,9,-21,4,8,-39,25,41,-41,13,9,-64
,-13,-11,-14,-40,-22,5,-2,-6,34,-9,25,6,24,-7,3,32,16,-28,-21,28,-19,23,28,-40,-9,-27,-21,10,14,15,37,21,13,-27
,-46,27,-44,4,-36,24,13,10,-65,-20,25,-21,5,-36,15,-17,-34,-10,20,-25,-14,18,-23,7,-15,23,0,-1,15,26,19,-19,31,-13
,3,17,35,59,-31,-25,-17,-14,16,-50,-3,-30,25,-35,5,6,-63,-2,-19,-4,-25,-19,23,29,21,-35,0,-15,11,4,12,-25,-26,-2
,-16,1,-31,-32,2,-18,24,-1,10,4,10,-15,18,-64,-34,-12,-24,6,-4,63,0,24,-9,-17,-22,9,-23,-17,-31,28,40,-5,58,32
,-25,2,30,-21,-24,55,-22,-20,16,-34,16,6,-25,-14,-1,-17,-17,66,40,-24,1,-24,-6,-44,44,-2,9,32,-28,3,-18,-1,-3,-20
,14,-19,12,31,21,39,-23,-22,55,-58,-11,1,-39,27,7,25,15,-44,38,-4,4,18,29,-11,1,26,19,20,16,22,4,-11,-40,-13
,-21,-16,-1,8,47,19,-18,67,3,35,1,29,10,-40,5,-32,26,-4,-5,2,-5,-42,-21,-11,25,-3,-9,16,-16,-11,12,-82,-38,-72
,7,41,18,-8,-31,34,-15,7,6,-15,23,1,16,7,-5,-5,-31,9,-1,36,5,-35,66,20,24,-10,30,-17,-3,36,27,10,14,8
,46,40,4,2,-21,-9,-8,-1,32,-5,-26,0,48,38,53,-53,-1,-28,-10,-39,-12,0,-17,-7,-56,-14,5,-16,-5,-24,16,-24,-4,-2
,-32,0,27,17,19,-39,-60,-45,40,-20,27,45,-23,35,-52,40,-22,-25,-11,26,-44,25,21,1,18,29,-15,17,-4,9,-43,-17,4,3
,9,52,12,9,-30,0,-7,21,8,6,-16,-18,-26,47,-30,8,-31,-8,37,-15,5,-60,8,-8,4,2,24,16,4,25,-16,-9,-28,22
,-6,4,32,-9,63,12,15,40,-22,21,-7,32,-8,-9,28,8,23,30,3,-9,10,-65,38,49,-11,-8,0,-42,-18,-32,34,-11,-30,23
,-44,-3,18,8,18,-34,-19,-17,-19,-6,3,-15,2,4,-22,-12,4,-33,-37,35,-29,13,-25,-28,28,62,2,-37,-28,-39,14,19,-13,-8
,8,4,-27,5,28,-24,-46,-1,25,53,-15,32,27,16,-9,36,-16,-16,42,-12,13,24,-59,31,-11,-14,42,-9,7,-17,-7,-7,18,1
,-18,-35,0,-7,-13,57,17,-4,-20,32,11,10,-18,29,5,-18,6,-17,7,6,28,-2,67,-57,-53,8,-9,-34,-38,34,-83,12,-52,6
,8,30,22,-31,6,17,-11,40,-3,41,-34,19,15,26,-7,27,-30,30,-60,-17,-46,15,27,14,26,52,32,-52,33,-9,24,12,56,-29
,-47,-14,50,-46,-50,-27,-43,-30,3,-10,-11,-33,-3,-21,41,-18,20,24,-16,33,1,-13,23,-22,-16,-17,-6,33,77,-21,-16,-13,-34,2
,-9,23,50,21,-27,-24,-7,6,-6,4,9,-5,10,-13,34,0,-6,-2,-13,-9,-7,-9,36,0,-3,31,8,25,-40,-55,-20,26,2,15
,-3,-6,32,28,-45,28,2,5,4,-19,13,16,36,-1,-31,22,-11,15,-26,-7,8,38,21,34,-20,6,5,7,-20,-14,29,-30,-30,-30,-34,-7}
};
// Hidden layer biases
constexpr int nb[2] = {-200,-453};
// Output layer weights
constexpr int nwo[2] = {2,65};
// Output layer bias
constexpr double nbo = 1.2150;

/// A move needs 16 bits to be stored
///
/// bit  0- 5: destination square (from 0 to 63)
/// bit  6-11: origin square (from 0 to 63)
/// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
/// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
/// NOTE: en passant bit is set only when a pawn can be captured
///
/// Special cases are MOVE_NONE and MOVE_NULL. We can sneak these in because in
/// any normal move destination square is always different from origin square
/// while MOVE_NONE and MOVE_NULL have the same origin and destination square.

enum Move : int {
  MOVE_NONE,
  MOVE_NULL = 65
};

enum MoveType {
  NORMAL,
  PROMOTION = 1 << 14,
  EN_PASSANT = 2 << 14,
  CASTLING  = 3 << 14
};

enum Color {
  WHITE, BLACK, COLOR_NB = 2
};

enum CastlingRights {
  NO_CASTLING,
  WHITE_OO,
  WHITE_OOO = WHITE_OO << 1,
  BLACK_OO  = WHITE_OO << 2,
  BLACK_OOO = WHITE_OO << 3,

  KING_SIDE      = WHITE_OO  | BLACK_OO,
  QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
  WHITE_CASTLING = WHITE_OO  | WHITE_OOO,
  BLACK_CASTLING = BLACK_OO  | BLACK_OOO,
  ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

  CASTLING_RIGHT_NB = 16
};

enum Phase {
  PHASE_ENDGAME,
  PHASE_MIDGAME = 128,
  MG = 0, EG = 1, PHASE_NB = 2
};

enum ScaleFactor {
  SCALE_FACTOR_DRAW    = 0,
  SCALE_FACTOR_NORMAL  = 64,
  SCALE_FACTOR_MAX     = 128,
  SCALE_FACTOR_NONE    = 255
};

enum Bound {
  BOUND_NONE,
  BOUND_UPPER,
  BOUND_LOWER,
  BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
};

enum Value : int {
  VALUE_ZERO      = 0,
  VALUE_DRAW      = 0,
  VALUE_KNOWN_WIN = 10000,
  VALUE_MATE      = 32000,
  VALUE_INFINITE  = 32001,
  VALUE_NONE      = 32002,

  VALUE_TB_WIN_IN_MAX_PLY  =  VALUE_MATE - 2 * MAX_PLY,
  VALUE_TB_LOSS_IN_MAX_PLY = -VALUE_TB_WIN_IN_MAX_PLY,
  VALUE_MATE_IN_MAX_PLY  =  VALUE_MATE - MAX_PLY,
  VALUE_MATED_IN_MAX_PLY = -VALUE_MATE_IN_MAX_PLY,

  // In the code, we make the assumption that these values
  // are such that non_pawn_material() can be used to uniquely
  // identify the material on the board.
  PawnValueMg   = 126,   PawnValueEg   = 208,
  KnightValueMg = 781,   KnightValueEg = 854,
  BishopValueMg = 825,   BishopValueEg = 915,
  RookValueMg   = 1276,  RookValueEg   = 1380,
  QueenValueMg  = 2538,  QueenValueEg  = 2682,

  MidgameLimit  = 15258, EndgameLimit  = 3915
};

enum PieceType {
  NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
  ALL_PIECES = 0,
  PIECE_TYPE_NB = 8
};

enum Piece {
  NO_PIECE,
  W_PAWN = PAWN,     W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
  PIECE_NB = 16
};

constexpr Value PieceValue[PHASE_NB][PIECE_NB] = {
  { VALUE_ZERO, PawnValueMg, KnightValueMg, BishopValueMg, RookValueMg, QueenValueMg, VALUE_ZERO, VALUE_ZERO,
    VALUE_ZERO, PawnValueMg, KnightValueMg, BishopValueMg, RookValueMg, QueenValueMg, VALUE_ZERO, VALUE_ZERO },
  { VALUE_ZERO, PawnValueEg, KnightValueEg, BishopValueEg, RookValueEg, QueenValueEg, VALUE_ZERO, VALUE_ZERO,
    VALUE_ZERO, PawnValueEg, KnightValueEg, BishopValueEg, RookValueEg, QueenValueEg, VALUE_ZERO, VALUE_ZERO }
};

using Depth = int;

enum : int {
  DEPTH_QS_CHECKS     =  0,
  DEPTH_QS_NO_CHECKS  = -1,
  DEPTH_QS_RECAPTURES = -5,

  DEPTH_NONE   = -6,

  DEPTH_OFFSET = -7 // value used only for TT entry occupancy check
};

enum Square : int {
  SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
  SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
  SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
  SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
  SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
  SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
  SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
  SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
  SQ_NONE,

  SQUARE_ZERO = 0,
  SQUARE_NB   = 64
};

enum Direction : int {
  NORTH =  8,
  EAST  =  1,
  SOUTH = -NORTH,
  WEST  = -EAST,

  NORTH_EAST = NORTH + EAST,
  SOUTH_EAST = SOUTH + EAST,
  SOUTH_WEST = SOUTH + WEST,
  NORTH_WEST = NORTH + WEST
};

enum File : int {
  FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NB
};

enum Rank : int {
  RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NB
};

// Keep track of what a move changes on the board (used by NNUE)
struct DirtyPiece {

  // Number of changed pieces
  int dirty_num;

  // Max 3 pieces can change in one move. A promotion with capture moves
  // both the pawn and the captured piece to SQ_NONE and the piece promoted
  // to from SQ_NONE to the capture square.
  Piece piece[3];

  // From and to squares, which may be SQ_NONE
  Square from[3];
  Square to[3];
};

/// Score enum stores a middlegame and an endgame value in a single integer (enum).
/// The least significant 16 bits are used to store the middlegame value and the
/// upper 16 bits are used to store the endgame value. We have to take care to
/// avoid left-shifting a signed int to avoid undefined behavior.
enum Score : int { SCORE_ZERO };

constexpr Score make_score(int mg, int eg) {
  return Score((int)((unsigned int)eg << 16) + mg);
}

/// Extracting the signed lower and upper 16 bits is not so trivial because
/// according to the standard a simple cast to short is implementation defined
/// and so is a right shift of a signed integer.
inline Value eg_value(Score s) {
  union { uint16_t u; int16_t s; } eg = { uint16_t(unsigned(s + 0x8000) >> 16) };
  return Value(eg.s);
}

inline Value mg_value(Score s) {
  union { uint16_t u; int16_t s; } mg = { uint16_t(unsigned(s)) };
  return Value(mg.s);
}

#define ENABLE_BASE_OPERATORS_ON(T)                                \
constexpr T operator+(T d1, int d2) { return T(int(d1) + d2); }    \
constexpr T operator-(T d1, int d2) { return T(int(d1) - d2); }    \
constexpr T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, int d2) { return d1 = d1 + d2; }       \
inline T& operator-=(T& d1, int d2) { return d1 = d1 - d2; }

#define ENABLE_INCR_OPERATORS_ON(T)                                \
inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
inline T& operator--(T& d) { return d = T(int(d) - 1); }

#define ENABLE_FULL_OPERATORS_ON(T)                                \
ENABLE_BASE_OPERATORS_ON(T)                                        \
constexpr T operator*(int i, T d) { return T(i * int(d)); }        \
constexpr T operator*(T d, int i) { return T(int(d) * i); }        \
constexpr T operator/(T d, int i) { return T(int(d) / i); }        \
constexpr int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }    \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(Value)
ENABLE_FULL_OPERATORS_ON(Direction)

ENABLE_INCR_OPERATORS_ON(Piece)
ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

ENABLE_BASE_OPERATORS_ON(Score)

#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

/// Additional operators to add a Direction to a Square
constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
inline Square& operator+=(Square& s, Direction d) { return s = s + d; }
inline Square& operator-=(Square& s, Direction d) { return s = s - d; }

/// Only declared but not defined. We don't want to multiply two scores due to
/// a very high risk of overflow. So user should explicitly convert to integer.
Score operator*(Score, Score) = delete;

/// Division of a Score must be handled separately for each term
inline Score operator/(Score s, int i) {
  return make_score(mg_value(s) / i, eg_value(s) / i);
}

/// Multiplication of a Score by an integer. We check for overflow in debug mode.
inline Score operator*(Score s, int i) {

  Score result = Score(int(s) * i);

  assert(eg_value(result) == (i * eg_value(s)));
  assert(mg_value(result) == (i * mg_value(s)));
  assert((i == 0) || (result / i) == s);

  return result;
}

/// Multiplication of a Score by a boolean
inline Score operator*(Score s, bool b) {
  return b ? s : SCORE_ZERO;
}

constexpr Color operator~(Color c) {
  return Color(c ^ BLACK); // Toggle color
}

constexpr Square flip_rank(Square s) { // Swap A1 <-> A8
  return Square(s ^ SQ_A8);
}

constexpr Square flip_file(Square s) { // Swap A1 <-> H1
  return Square(s ^ SQ_H1);
}

constexpr Square rotate_180(Square s) { // Swap A1 <-> H8
  return Square(flip_file(flip_rank(s)));
}

constexpr Piece operator~(Piece pc) {
  return Piece(pc ^ 8); // Swap color of piece B_KNIGHT <-> W_KNIGHT
}

constexpr CastlingRights operator&(Color c, CastlingRights cr) {
  return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr);
}

constexpr Value mate_in(int ply) {
  return VALUE_MATE - ply;
}

constexpr Value mated_in(int ply) {
  return -VALUE_MATE + ply;
}

constexpr Square make_square(File f, Rank r) {
  return Square((r << 3) + f);
}

constexpr Piece make_piece(Color c, PieceType pt) {
  return Piece((c << 3) + pt);
}

constexpr PieceType type_of(Piece pc) {
  return PieceType(pc & 7);
}

inline Color color_of(Piece pc) {
  assert(pc != NO_PIECE);
  return Color(pc >> 3);
}

constexpr bool is_ok(Move m) {
  return m != MOVE_NONE && m != MOVE_NULL;
}

constexpr bool is_ok(Square s) {
  return s >= SQ_A1 && s <= SQ_H8;
}

constexpr File file_of(Square s) {
  return File(s & 7);
}

constexpr Rank rank_of(Square s) {
  return Rank(s >> 3);
}

constexpr Square relative_square(Color c, Square s) {
  return Square(s ^ (c * 56));
}

constexpr Rank relative_rank(Color c, Rank r) {
  return Rank(r ^ (c * 7));
}

constexpr Rank relative_rank(Color c, Square s) {
  return relative_rank(c, rank_of(s));
}

constexpr Direction pawn_push(Color c) {
  return c == WHITE ? NORTH : SOUTH;
}

constexpr Square from_sq(Move m) {
  assert(is_ok(m));
  return Square((m >> 6) & 0x3F);
}

constexpr Square to_sq(Move m) {
  assert(is_ok(m));
  return Square(m & 0x3F);
}

constexpr int from_to(Move m) {
  return m & 0xFFF;
}

constexpr MoveType type_of(Move m) {
  return MoveType(m & (3 << 14));
}

constexpr PieceType promotion_type(Move m) {
  return PieceType(((m >> 12) & 3) + KNIGHT);
}

constexpr Move make_move(Square from, Square to) {
  return Move((from << 6) + to);
}

template<MoveType T>
constexpr Move make(Square from, Square to, PieceType pt = KNIGHT) {
  return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
}

constexpr bool is_non_pawn_rank(Square s) {
  return rank_of(s) == RANK_1 || rank_of(s) == RANK_8;
}

constexpr bool is_non_piece_type(Piece pc) {
  return pc == 7 || pc == 8;
}

/// Based on a congruential pseudo random number generator
constexpr Key make_key(uint64_t seed) {
  return seed * 6364136223846793005ULL + 1442695040888963407ULL;
}

} // namespace Stockfish

#endif // #ifndef TYPES_H_INCLUDED

#include "tune.h" // Global visibility to tuning setup
