//
//  Globals.h
//  Roshamboogie
//
//  Created by Joshua Kaplan on 3/22/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef Globals_h
#define Globals_h

namespace globals {

/** Minimum number of players per game */
constexpr uint8_t MIN_PLAYERS = 3;

/** Maximum number of players per game */
constexpr uint8_t MAX_PLAYERS = 8;

/** Maximum number of orbs per game */
constexpr uint8_t MAX_ORBS = 20;

/** Maximum number of swap stations per game */
constexpr uint8_t MAX_SWAP_STATIONS = 5;

constexpr int TAG_SCORE = 15;
constexpr int HATCH_SCORE = 10;

constexpr int GAME_TIMER= 180;

/**convert Tiled coords to box2d w/ scalar*/
constexpr float  TILE_TO_BOX2D = 250.0f/40.0f;

constexpr float  TILE_TO_SCENE = 250.0f;

/** roughly 40 pixels to 1 box2d unit */
constexpr float  SCENE_TO_BOX2D = 1.0f/40.0f;

constexpr float  BOX2D_TO_SCENE = 40.0f;

constexpr int MAX_CHARS_IN_USERNAME = 15;

}

#endif /* Globals_h */
