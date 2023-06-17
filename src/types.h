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
{-45,46,-72,109,67,38,75,-4,-14,43,14,11,50,-34,49,-7,-7,-35,-17,-13,-23,32,44,81,7,-3,-40,-40,28,18,36,17,47,13,-8
,-20,-127,-46,60,50,77,30,-17,7,47,-36,-3,36,-13,29,28,3,28,90,42,20,-6,-77,0,-21,6,44,71,-75,-8,53,-57,-99,-69
,-9,13,-72,44,-75,-14,-48,-9,-9,-13,-29,-49,27,-68,21,55,-16,-29,-20,77,-31,-35,90,5,-25,39,-32,42,69,-68,76,108,12,-53
,55,60,25,42,15,-28,31,-28,36,35,-43,-4,-1,-11,1,52,11,-6,8,-5,18,-10,-55,53,-91,-32,-28,53,8,-97,-23,21,32,-15
,-120,54,-27,115,72,68,63,-48,-18,44,29,7,24,24,-13,-33,17,6,39,27,-25,-23,121,31,12,132,-34,-15,-69,-69,13,55,30,22
,61,-3,79,13,43,18,30,37,-137,-66,14,3,25,-9,46,2,-79,-10,28,81,-44,32,-27,100,-9,-14,-10,-67,11,82,-36,25,23,52
,56,14,-20,-3,9,-35,9,-59,70,-31,38,27,43,86,-1,2,35,60,37,-88,-50,71,-8,-49,-15,-54,-14,-18,-20,12,0,34,-13,25
,-31,13,-55,3,-7,-117,-69,-8,-48,-25,-3,-46,2,5,26,4,10,3,-4,10,1,-28,-40,-5,-15,45,-83,-3,-1,-11,1,90,0,-49
,-71,-29,23,52,98,58,-40,-16,59,59,18,-4,31,28,23,-53,46,7,-11,-22,-48,73,12,3,8,-10,-39,6,-3,0,86,8,42,77
,58,-34,-43,18,-1,-4,27,-47,-13,3,18,94,-56,80,-5,52,15,-47,-17,0,81,-28,-81,23,-63,-3,4,-40,70,-75,-27,6,8,61
,37,30,19,-35,-6,-49,31,81,-22,72,73,-30,-15,91,-59,29,40,-31,-47,17,-8,33,30,21,-27,-60,31,-26,60,23,-41,15,-17,21
,21,-8,-39,2,11,13,-27,-23,-80,3,46,-40,-35,40,51,-57,-15,-102,11,-42,14,88,-53,-18,25,-39,-65,-2,-97,-43,-10,-20,50,-31
,60,-43,77,9,-8,30,-18,45,59,-33,29,0,-4,-18,-40,51,-19,-39,-15,49,25,30,29,28,29,33,33,2,85,27,-26,-14,-9,51
,20,84,0,37,27,49,31,-3,59,-40,96,9,115,-33,70,-27,17,-50,33,0,-24,-58,-26,14,-7,34,25,57,-75,-18,25,76,-47,60
,-51,6,-26,20,2,-65,10,-1,8,74,-29,-28,3,-23,-20,-39,48,-27,-11,15,-18,14,-16,6,12,11,2,-18,-18,46,-5,-40,-1,-81
,-24,34,-7,-64,96,52,-9,-9,-12,53,3,-16,-5,-26,5,-9,32,-22,37,-40,-22,-41,-32,11,-9,52,-9,-7,10,-20,-46,26,-35,-57
,23,60,75,13,25,-14,-56,17,-67,37,-16,-5,-22,18,60,42,-47,-9,-19,65,55,-22,-45,35,-69,-57,56,22,27,-4,-53,16,-15,94
,34,34,30,-38,45,22,-23,4,-27,20,0,27,-86,-162,13,15,21,-6,23,-1,-8,157,43,4,19,69,96,16,82,-30,-59,59,-19,56
,68,33,71,76,1,-51,-37,-42,30,6,97,42,33,-86,79,-48,75,12,-12,-76,-25,31,52,41,-56,-27,0,-27,7,22,-41,60,33,12
,-21,-7,57,-16,-10,-63,-23,7,35,-41,-5,102,35,-39,22,-30,3,-11,-19,-60,-35,57,6,43,-3,62,-32,-28,78,-40,26,68,-103,-91
,0,-97,-31,-3,-3,-69,-56,21,-55,-103,24,13,41,50,18,49,-49,9,26,104,158,56,-48,-102,-66,-17,-4,-45,-56,51,-16,59,32,-40,27,-20,24,15},

{-4,97,-37,22,18,-23,-63,57,-65,-85,-4,-17,-51,-66,9,-26,-42,14,-65,2,-28,27,-30,14,10,-34,-3,107,-71,-14,3,4,47,7,31
,-54,-67,-11,-23,57,-49,-19,20,30,60,-16,33,-33,56,65,-31,-9,38,0,-10,-42,-22,-18,10,-66,36,36,-72,35,31,-13,1,40,-55
,-22,9,-53,-73,-58,34,-15,-49,112,6,27,6,-8,-35,-11,49,17,-19,-7,22,-48,5,41,-23,-59,-21,2,33,-5,-23,31,13,11,-13
,-27,19,-39,2,10,25,-9,-51,-92,6,61,-26,-51,-83,25,-44,-58,-60,9,-25,57,35,6,59,-41,30,15,9,-32,-9,56,35,26,-12
,43,41,70,81,-66,29,8,-48,16,28,1,4,79,-99,-1,-39,-88,85,-22,43,31,-9,83,27,43,-52,-13,-70,-21,64,-30,-21,13,32
,-1,-27,-19,12,26,-74,26,11,9,-37,55,-14,60,-19,-101,1,18,30,-45,38,-13,14,-1,-48,33,81,61,8,-11,-57,71,-14,-7,-18
,35,2,7,-15,-87,71,-46,-30,29,-6,15,5,-24,-40,-39,-2,30,43,57,-2,-42,-68,15,-18,104,35,47,-1,-41,15,-107,60,-18,11
,31,-69,17,12,75,59,-41,-34,63,-95,58,-36,-60,-12,-25,7,29,-83,33,-15,-3,66,12,-56,5,-65,-17,14,23,68,29,44,-72,-2
,15,17,-60,-4,131,71,-42,31,-49,15,-42,92,-56,-42,72,-3,-19,10,26,27,-27,-58,-30,-29,7,-18,-43,-65,18,-54,0,-97,-46,-86
,-4,16,-55,-55,-47,83,-58,12,-48,-6,33,-18,-35,-11,32,-62,34,10,-3,-48,-45,-93,70,-2,-3,-3,76,-17,-19,32,34,89,112,54
,18,-62,-23,1,-47,24,-30,-46,0,-38,-75,37,120,18,7,1,41,-9,-62,69,-17,30,-81,25,-38,40,1,29,50,-28,1,-65,2,-55
,0,-58,37,55,-11,-53,-70,-11,-67,26,62,33,-97,40,-69,21,-31,-61,-17,6,-34,-1,4,23,14,7,-28,29,14,-1,-1,17,4,107
,6,12,49,-1,-10,-13,-28,49,-56,-6,-14,-19,0,13,-7,7,12,-5,35,-63,62,-30,91,30,-34,38,12,16,-59,-25,8,50,32,27
,35,-18,0,42,32,14,-19,76,16,69,-22,-14,17,-108,69,-19,53,15,25,17,50,-12,40,32,-42,14,-54,-109,37,-61,7,8,-29,60
,-54,-45,59,23,-10,2,-22,-44,-25,-14,-27,11,-35,-9,-5,-3,-14,-47,-99,26,44,-35,-76,-50,-4,37,87,-18,59,-17,-40,63,-46,22
,-25,60,-75,-11,-43,28,-8,-12,23,-4,-32,-34,89,-11,77,81,0,-3,48,28,-17,21,29,44,13,-21,25,-37,-68,-6,-20,-24,5,-14
,-31,13,16,-56,29,98,0,-11,52,35,55,51,-69,70,66,-11,42,-36,50,33,22,-28,127,-64,-84,21,-42,28,-73,31,-10,19,-108,27
,28,31,13,-6,-11,9,-30,45,-44,46,-13,-2,-9,49,-56,9,34,45,-84,-38,-32,60,13,27,38,69,73,-22,50,33,40,33,51,-48
,-67,-50,82,7,-58,-71,-115,-25,57,-35,67,-87,-22,24,93,-38,54,79,5,2,-40,-10,-10,-98,3,0,-28,53,101,-36,-38,-110,9,17
,19,52,84,3,22,-38,-10,-2,21,-34,-4,-30,46,-72,43,4,-37,-3,-54,44,-69,-28,11,-20,26,38,23,77,-83,-66,-48,96,-19,12
,-8,-23,81,51,-56,-7,40,31,-76,18,25,-1,36,19,-29,-27,-11,3,-29,-24,-20,-2,42,4,-88,30,-9,84,-52,-28,4,-96,-46,-61,-84,85}
};
// Hidden layer biases
constexpr int nb[2] = {-212,-647};
// Output layer weights
constexpr int nwo[2] = {17,27};
// Output layer bias
constexpr double nbo = 0.5827;

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
