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
{-4,11,-1,19,10,4,9,4,-18,15,-23,-6,28,-13,29,6,15,-19,23,-23,-28,27,1,-1,18,-2,-9,8,20,15,31,-12,7,31,-8,4,-57,23,36,
12,4,18,-23,-5,8,-14,-15,24,1,12,19,2,34,12,-21,7,-7,-54,-17,19,-6,-8,-25,-7,-20,8,-7,-24,-48,-9,21,-2,-12,-10,-35,-25,
-4,-4,12,-7,-51,-4,-28,-31,-7,38,-13,-9,47,9,-21,8,14,42,-6,3,20,20,-25,10,4,28,-6,12,-17,8,11,-25,26,-9,-24,2,5,9,-18,
-17,-17,15,10,6,-15,-22,-16,17,12,-20,7,-21,2,-15,-1,16,-17,-6,3,4,16,-22,34,2,23,32,-13,27,-9,-12,40,13,-11,19,3,7,13,
7,-4,16,7,-3,-18,8,24,1,33,15,-11,18,-2,6,-16,9,6,-2,-14,-23,-1,33,-15,-13,-37,-12,-31,-14,-25,5,-5,-3,34,-29,-1,-4,10,
-16,20,-12,41,17,-12,27,8,13,12,7,6,3,-28,-21,21,-21,-5,-10,-6,-22,-1,-2,-8,-20,21,18,7,-18,27,1,-3,17,-22,3,-9,-15,
-23,25,1,-14,-22,-4,-35,-1,21,-26,-2,-38,21,-29,2,2,-26,-25,25,-19,-6,3,-6,10,8,26,-3,-20,-11,8,-5,-14,-19,9,-12,5,59,
-8,-2,-12,-2,-5,8,30,-22,-21,-1,27,4,61,24,-19,-19,22,22,31,-7,-3,19,36,14,25,-2,-14,2,-23,45,-5,28,7,-6,4,-2,-8,36,23,
-2,17,-11,1,-34,-23,-5,16,-8,-3,-22,14,19,11,-24,-13,19,6,-14,17,13,5,-5,1,6,-33,-19,21,-23,-20,26,12,-13,-5,4,-26,13,
8,-25,-6,-27,19,-10,27,-8,-8,19,-10,-3,-15,36,-5,-8,23,-10,13,28,-7,-10,2,8,-8,-27,-28,9,26,-3,-25,2,3,8,2,-13,-18,25,
20,-1,-3,21,-2,-9,21,3,4,15,-7,-14,-2,-7,-8,4,-13,13,-15,-14,-12,-30,-17,-8,-5,-15,-6,9,31,-33,9,9,22,-8,-6,3,12,16,18,
-12,21,-9,-11,18,24,40,2,-1,5,3,-5,10,19,-3,5,10,4,5,33,-8,15,-9,15,15,10,11,-19,12,16,6,3,-27,3,-20,8,16,5,-9,34,16,
28,-11,-8,12,12,3,9,18,-11,17,2,21,14,19,3,-6,25,5,3,11,-15,-2,9,-22,-22,-9,27,14,9,8,-21,-6,17,8,23,-1,15,-5,30,6,-26,
13,-6,22,-5,16,16,14,14,-34,11,-3,-18,9,13,-18,22,11,3,14,-26,-27,16,-1,-11,4,-6,-15,30,-17,16,-35,-10,-3,2,-30,-16,5,
2,21,14,1,2,-17,-19,-22,19,6,9,-4,26,12,-32,-10,1,8,-27,3,9,3,29,-5,-9,-9,-11,29,12,14,7,-4,-13,-10,17,15,14,1,-18,10,
5,35,1,9,-9,4,6,-24,22,26,8,-9,-17,8,-8,-35,11,-21,8,3,-5,-2,-19,12,15,8,25,19,25,-16,20,14,6,17,29,17,5,27,25,13,-18,
10,29,-15,-11,13,22,11,-10,-22,3,-12,18,6,29,-4,-36,-8,4,36,18,-5,14,3,3,6,13,37,-34,4,-24,10,-8,-19,-11,-33,8,-25,-10,
-3,11,22,-3,7,10,-2,25,-29,-20,-6,-19,39,3,-22,-4,-2,-1,-11,16,-12,11,-5,-26,-56,-10,-20,-15,2,8,-5,6,10,-21,-9,9,13,
-3,-24,-11,22,-1,7,4,36,34,-13,-17,-27,-18,3,-7,22,-22,17,-10,20,-6,10,9,15,9,-39},

{-16,-2,-7,-9,-8,-7,-8,2,-35,-11,32,25,-6,-9,11,12,-13,-8,5,3,-5,5,-33,2,24,-25,-19,27,-45,-21,-24,2,14,-7,-3,17,-13,6,
-16,3,-22,3,18,16,11,2,-20,-17,17,28,-7,-2,11,-43,14,15,-3,-27,26,-26,-12,6,-31,11,6,-29,-7,-7,-24,7,9,9,-20,-18,-2,-2,
16,9,-7,3,-12,4,-20,7,1,-9,-8,-20,4,2,13,20,-21,-14,-4,-8,15,25,8,17,18,24,7,-31,11,7,-10,-13,-11,-3,15,-31,1,2,-15,-6,
-17,-2,-11,-28,13,10,-7,-31,10,-23,-6,-32,4,5,6,23,16,-7,-2,8,-18,7,15,-3,26,-22,-26,-19,27,19,-10,6,-25,10,-16,19,17,
-2,-7,-23,-12,-24,-42,6,11,4,-7,-20,-31,11,19,29,-29,-15,-1,-22,13,-6,1,3,12,6,10,11,15,16,-3,16,-14,-27,-8,-26,20,-9,
34,-12,27,-27,10,-34,33,-23,-13,-24,20,9,-5,38,9,-25,-13,-10,-5,-30,13,-6,-15,7,-2,22,5,-19,2,-14,-39,-23,27,30,-26,1,
26,1,-21,24,-20,-1,22,-16,-7,6,-14,-15,6,4,-1,-8,8,-18,8,-32,-24,31,-37,4,-2,-36,12,-3,35,-3,-22,10,-2,-7,21,3,-10,38,
-2,30,2,19,3,-6,-23,-1,13,-1,-23,17,-21,19,7,4,63,-13,-7,-9,34,-4,-42,23,9,6,17,-18,5,-14,-23,-2,1,13,-3,8,12,-3,12,11,
-4,9,-45,3,33,-5,-13,-2,43,-12,16,5,-12,12,5,20,-3,-3,14,-8,5,-17,12,1,1,32,2,17,10,5,6,4,9,9,11,2,-5,21,11,-5,4,-8,-20,
-10,-6,7,6,-21,-7,25,2,24,-13,8,-10,29,-17,19,5,17,6,-33,-7,-2,-21,4,-3,-8,-35,-3,20,-23,-9,23,15,-11,-12,-9,-29,54,
-10,-4,35,-33,16,-25,-2,-22,7,-6,25,-43,3,18,-3,19,12,3,1,13,-2,-2,-11,20,18,10,33,3,2,-5,-7,-5,21,-3,22,-1,-18,-31,27,
3,-3,-39,-17,10,-7,-1,-27,-13,5,-9,-7,12,17,4,7,9,-5,-7,23,2,-25,21,-11,43,7,4,15,-23,-5,-18,19,-18,-33,16,4,16,-4,12,
-11,7,-18,4,28,-17,14,2,-23,-3,-17,19,-7,-20,7,-36,10,2,-14,3,-13,-30,-14,-9,-2,-6,10,-21,-6,-33,10,-19,2,-12,38,1,8,
-18,-17,8,40,5,-14,4,-14,10,32,-9,-2,-15,-9,-12,-11,12,-4,16,15,-4,7,-46,7,23,10,-17,40,12,-5,-7,8,6,20,-28,24,-3,-9,
13,1,-9,-23,-28,-5,-6,-12,-21,-21,-4,-9,-28,32,-1,17,11,24,11,5,-10,27,-6,-13,18,-5,5,19,21,-5,27,-25,-31,-18,1,-29,
-16,8,-23,20,-23,-8,-5,4,22,-29,-18,4,6,-13,3,5,-15,-6,23,27,6,10,-1,-8,-58,-8,-33,21,-5,-2,21,7,26,-30,-16,-16,-29,
-15,21,2,-36,-12,17,-18,-22,1,-10,-6,1,-15,-2,-26,10,15,27,-12,21,19,-21,8,20,-17,16,-30,19,1,11,20,25,4,30,-10,-36,
-1,-18,-12,9,12,-15,-21,-10,1,14,18,17,-10,19,4,6,12,19,10,-13,4,9,-6,20,-20,-13,-2,14,-18,-21,-20,-19,30,6,8,3,-7,17,
5,-21,14,-14,-7,-3,-27,-1,1,3,13,-3,-3,-7,13,-23,-2,-15,21,13,21,-12,-10,14,19,-2,3,11,-22,-43,-10,-16,3}
};
// Hidden layer biases
constexpr int nb[2] = {-258,-399};
// Output layer weights
constexpr int nwo[2] = {-17,83};
// Output layer bias
constexpr double nbo = 0.9435;

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
