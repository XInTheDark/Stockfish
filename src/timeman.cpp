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

#include <algorithm>
#include <cfloat>
#include <cmath>

#include "search.h"
#include "timeman.h"
#include "uci.h"

namespace Stockfish {

TimeManagement Time; // Our global time management object


int a1 = 50,

    b1 = 100, b2 = 12, b3 = 100, b4 = 112,

    c1 = 120, c2 = 300, c3 = 45, c4 = 39, c5 = 20,

    d1 = 700, d2 = 400, d3 = 1200,

    e1 = 88, e2 = 11640, e3 = 88,

    f1 = 630, f2 = 150, f3 = 11,

    g1 = 80;

TUNE(SetRange(1, 100), a1);
TUNE(SetRange(1, 200), b1);
TUNE(SetRange(1, 24), b2);
TUNE(SetRange(1, 200), b3);
TUNE(SetRange(1, 224), b4);
TUNE(SetRange(1, 240), c1);
TUNE(SetRange(1, 600), c2);
TUNE(SetRange(1, 90), c3);
TUNE(SetRange(1, 78), c4);
TUNE(SetRange(1, 40), c5);
TUNE(SetRange(1, 1400), d1);
TUNE(SetRange(1, 800), d2);
TUNE(SetRange(1, 2400), d3);
TUNE(SetRange(1, 176), e1);
TUNE(SetRange(1, 23280), e2);
TUNE(SetRange(1, 176), e3);
TUNE(SetRange(1, 1260), f1);
TUNE(SetRange(1, 300), f2);
TUNE(SetRange(1, 22), f3);
TUNE(SetRange(1, 160), g1);

/// TimeManagement::init() is called at the beginning of the search and calculates
/// the bounds of time allowed for the current game ply. We currently support:
//      1) x basetime (+ z increment)
//      2) x moves in y seconds (+ z increment)

void TimeManagement::init(Search::LimitsType& limits, Color us, int ply) {

  TimePoint moveOverhead    = TimePoint(Options["Move Overhead"]);
  TimePoint slowMover       = TimePoint(Options["Slow Mover"]);
  TimePoint npmsec          = TimePoint(Options["nodestime"]);

  // optScale is a percentage of available time to use for the current move.
  // maxScale is a multiplier applied to optimumTime.
  double optScale, maxScale;

  // If we have to play in 'nodes as time' mode, then convert from time
  // to nodes, and use resulting values in time management formulas.
  // WARNING: to avoid time losses, the given npmsec (nodes per millisecond)
  // must be much lower than the real engine speed.
  if (npmsec)
  {
      if (!availableNodes) // Only once at game start
          availableNodes = npmsec * limits.time[us]; // Time is in msec

      // Convert from milliseconds to nodes
      limits.time[us] = TimePoint(availableNodes);
      limits.inc[us] *= npmsec;
      limits.npmsec = npmsec;
  }

  startTime = limits.startTime;

  // Maximum move horizon of 50 moves
  int mtg = limits.movestogo ? std::min(limits.movestogo, a1) : a1;

  // Make sure timeLeft is > 0 since we may use it as a divisor
  TimePoint timeLeft =  std::max(TimePoint(1),
      limits.time[us] + limits.inc[us] * (mtg - 1) - moveOverhead * (2 + mtg));

  // Use extra time with larger increments
  double optExtra = std::clamp(b1 / 100.0 + b2 / 100.0 * limits.inc[us] / limits.time[us], b3 / 100.0, b4 / 100.0);

  // A user may scale time usage by setting UCI option "Slow Mover"
  // Default is 100 and changing this value will probably lose elo.
  timeLeft = slowMover * timeLeft / 100;

  // x basetime (+ z increment)
  // If there is a healthy increment, timeLeft can exceed actual available
  // game time for the current move, so also cap to 20% of available game time.
  if (limits.movestogo == 0)
  {
      optScale = std::min(c1 / 10000.0 + std::pow(ply + c2 / 100.0, c3 / 100.0) * c4 / 10000.0,
                           c5 / 100.0 * limits.time[us] / double(timeLeft))
                 * optExtra;
      maxScale = std::min(d1 / 100.0, d2 / 100.0 + ply / (d3 / 100.0));
  }

  // x moves in y seconds (+ z increment)
  else
  {
      optScale = std::min((e1 / 100.0 + ply / (e2 / 100.0)) / mtg,
                            e3 / 100.0 * limits.time[us] / double(timeLeft));
      maxScale = std::min(f1 / 100.0, f2 / 100.0 + f3 / 100.0 * mtg);
  }

  // Never use more than 80% of the available time for this move
  optimumTime = TimePoint(optScale * timeLeft);
  maximumTime = TimePoint(std::min(g1 / 100.0 * limits.time[us] - moveOverhead, maxScale * optimumTime));

  if (Options["Ponder"])
      optimumTime += optimumTime / 4;
}

} // namespace Stockfish
