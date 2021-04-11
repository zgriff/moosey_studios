//
//  MapConstants.h
//  Roshamboogie
//
//  Created by Zach Griffin on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef MapConstants_h
#define MapConstants_h

/** The global fields of the map model */
#define WIDTH_FIELD         "width"
#define HEIGHT_FIELD        "height"

#define X_FIELD             "x"
#define Y_FIELD             "y"
#define ASSET_FIELD         "asset"

#define WALLS_FIELD         "boundaries"
#define TILES_FIELD         "tiles"
#define DECORATIONS_FIELD   "decorations"
#define GAME_OBJECTS_FIELD  "gameObjects"

#define TYPE_FIELD          "type"
#define SWAP_STATION        "swap_station"
#define EGG_SPAWN           "egg_spawn"
#define ORB_SPAWN           "active_orb"
#define ORB_LOCATION        "inactive_orb"
#define PLAYER_SPAWN        "player_spawn"

#define TILE_SIZE           50.0f

#define GRASS_MAP_JSON      "maps/forest.json"
#define GRASS_MAP_KEY       "grass_map"


enum class GameObjectType {
    PlayerSpawn, OrbSpawn, OrbLocation, Station, EggSpawn
};


#endif /* MapConstants_h */
