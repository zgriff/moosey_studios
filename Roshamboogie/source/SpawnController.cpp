//
//  SpawnController.cpp
//  Roshamboogie
//
//  Created by Ikra Monjur on 3/30/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include <cugl/cugl.h>
#include "SpawnController.h"
#include "Orb.h"

using namespace cugl;


void SpawnController::setWorld(std::shared_ptr<World> w){
    world = w;
}

//scan all player positions and respawn orbs where there are not many people
//divide map and check each region --> how many people in each region and
    //spawn where less people
void SpawnController::spawnOrbs() {
//    Size dimen = Application::get()->getDisplaySize();
    int rows = 2;
    int cols = 2;
//    float scale = world->getScale();
    
    float roomWidth = 36.0f / cols;
    float roomHeight = 18.0f / rows;
    
    std::vector<std::shared_ptr<Player>> players = world->getPlayers();
    
    std::vector<std::vector<int>> spaces (cols , vector<int> (rows, 0));
         
    for (int k = 0; k < players.size(); k++) {
        Vec2 playerPos = players[k]->getPosition();
        int j = (int) playerPos.x / roomWidth;
        int i = (int) playerPos.y / roomHeight;
        
//        CULog("i %d", i);
//        CULog("j %d", j);
        spaces[i][j] += 1;
    }
    
    
    //get the list of empty rooms
    std::vector<Vec2> emptySpaces;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            if (spaces[i][j] == 0) {
                emptySpaces.push_back(Vec2(i,j));
            }
        }
    }
    
    CULog("emptyspaces %lu", emptySpaces.size());
    
    //get which room to spawn orb in
    std::uniform_int_distribution<int> rand_int(0, 99);
    int factor = 100 / emptySpaces.size();
    int randRoom = rand_int(e1) / factor;
    
    CULog("randroom %d", randRoom);
    //get rand position of orb
    CULog("randroom x %f", emptySpaces[randRoom].x);
    CULog("randroom y %f", emptySpaces[randRoom].y);
    std::uniform_int_distribution<float> rand_x(emptySpaces[randRoom].x, emptySpaces[randRoom].x + roomWidth);
    std::uniform_int_distribution<float> rand_y(emptySpaces[randRoom].y, emptySpaces[randRoom].y + roomHeight);
    world->addOrb(Vec2(rand_x(e1), rand_y(e1)));
    
}


