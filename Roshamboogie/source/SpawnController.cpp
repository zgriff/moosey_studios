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
#include "NetworkController.h"

using namespace cugl;


void SpawnController::setWorld(std::shared_ptr<World> w){
    world = w;
}

//scan all player positions and respawn orbs where there are not many people
//divide map and check each region --> how many people in each region and
    //spawn where less people
void SpawnController::spawnOrbs() {
    int rows = 4;
    int cols = 4;
    
    float roomWidth = world->getPhysicsWorld()->getBounds().getMaxX() / cols;
    float roomHeight = world->getPhysicsWorld()->getBounds().getMaxY() / rows;
    
    std::vector<std::shared_ptr<Player>> players = world->getPlayers();
    
    std::vector<std::vector<int>> spaces (cols , vector<int> (rows, 0));
         
    for (int k = 0; k < players.size(); k++) {
        Vec2 playerPos = players[k]->getPosition();
        int j = (int) playerPos.x / roomWidth;
        int i = (int) playerPos.y / roomHeight;
        
//        CULog("player pos (x, y) : %f, %f", playerPos.x, playerPos.y);
//        CULog("player loc (col, row) : %d, %d", j, i);
        
//        CULog("i %d", i);
//        CULog("j %d", j);
        spaces[i][j] += 1;
    }
    
    //TODO: check number of players in the room and assign prob of spawning in each room based on that
    //get the list of empty rooms
    std::vector<Vec2> emptySpaces;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            if (spaces[i][j] == 0) {
//                CULog("empty space (col, row) : %d, %d", j, i);
                emptySpaces.push_back(Vec2(float(i),float(j)));
            }
        }
    }
    
    //get which room to spawn orb in
    std::uniform_int_distribution<int> rand_int(0, (int)emptySpaces.size()-1);
    int randRoom = rand_int(e1);
    
//    CULog("randroom %d", randRoom);
    //get rand position of orb
//    CULog("randroom x %f", emptySpaces[randRoom].x);
//    CULog("randroom y %f", emptySpaces[randRoom].y);
    std::uniform_int_distribution<long> rand_x(emptySpaces[randRoom].x * roomWidth, emptySpaces[randRoom].x * roomWidth + roomWidth);
    std::uniform_int_distribution<long> rand_y(emptySpaces[randRoom].y * roomHeight, emptySpaces[randRoom].y * roomHeight + roomHeight);
//    CULog("new pos %f %f", rand_x(e1), rand_y(e1));
    
    for (int i = 0; i < world->getNumOrbs(); i++) {
        std::shared_ptr<Orb> orb = world->getOrb(i);
        if (orb->getCollected()) {
            orb->setPosition(Vec2(rand_x(e1), rand_y(e1)));
            orb->setCollected(false);
            world->setOrbCount(world->getCurrOrbCount()+1);
            NetworkController::sendOrbRespawn(orb->getID(), orb->getPosition());
        }
    }
    
}

//check which rooms have egg and player and don't spawn in those rooms
void SpawnController::spawnEggs() {
    int maxEggs = int(world->getPlayers().size() / 3);
    int rows = 4;
    int cols = 4;
    
    float roomWidth = world->getPhysicsWorld()->getBounds().getMaxX() / cols;
    float roomHeight = world->getPhysicsWorld()->getBounds().getMaxY() / rows;
    
    std::vector<std::shared_ptr<Player>> players = world->getPlayers();
    std::vector<std::shared_ptr<Egg>> eggs = world->getEggs();
    
    std::vector<std::vector<int>> spaces (cols , vector<int> (rows, 0));
         
    //checking player pos
    for (int k = 0; k < players.size(); k++) {
        Vec2 playerPos = players[k]->getPosition();
        int j = (int) playerPos.x / roomWidth;
        int i = (int) playerPos.y / roomHeight;
        spaces[i][j] += 1;
    }
    
    //checking egg pos
    for (int k = 0; k < eggs.size(); k++) {
        Vec2 eggPos = eggs[k]->getPosition();
        int j = (int) eggPos.x / roomWidth;
        int i = (int) eggPos.y / roomHeight;
        spaces[i][j] += 1;
    }
    
    //get the list of empty rooms
    std::vector<Vec2> emptySpaces;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            if (spaces[i][j] == 0) {
//                CULog("empty space (col, row) : %d, %d", j, i);
                emptySpaces.push_back(Vec2(float(i),float(j)));
            }
        }
    }
    
    //get which room to spawn orb in
    std::uniform_int_distribution<int> rand_int(0, (int)emptySpaces.size()-1);
    int randRoom = rand_int(e1);
    
//    CULog("randroom %d", randRoom);
    //get rand position of orb
//    CULog("randroom x %f", emptySpaces[randRoom].x);
//    CULog("randroom y %f", emptySpaces[randRoom].y);
    std::uniform_int_distribution<long> rand_x(emptySpaces[randRoom].x * roomWidth, emptySpaces[randRoom].x * roomWidth + roomWidth);
    std::uniform_int_distribution<long> rand_y(emptySpaces[randRoom].y * roomHeight, emptySpaces[randRoom].y * roomHeight + roomHeight);
//    CULog("new pos %f %f", rand_x(e1), rand_y(e1));
    
    for (int i = 0; i < eggs.size(); i++) {
        std::shared_ptr<Egg> egg = world->getEgg(i);
        if (egg->getHatched()) {
//            egg->setPosition(Vec2(rand_x(e1), rand_y(e1)));
            egg->setPosition(10, 10); //TODO: change -- just for testing now
            egg->setHatched(false);
            egg->setCollected(false);
            egg->incDistanceWalked(-1 * egg->getDistanceWalked());
            world->setCurrEggCount(world->getCurrEggCount() + 1);
            NetworkController::sendEggRespawn(egg->getID(), egg->getPosition());
        }
    }
    
    
}

