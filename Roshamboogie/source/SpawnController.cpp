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
    
    auto spawnLocs = world->getOrbSpawns();
    
    std::vector<std::shared_ptr<Player>> players = world->getPlayers();
    
    std::vector<std::vector<int>> spaces (cols , vector<int> (rows, 0));
         
    for (int k = 0; k < players.size(); k++) {
        Vec2 playerPos = players[k]->getPosition();
        int j = (int) playerPos.x / roomWidth;
        int i = (int) playerPos.y / roomHeight;
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

    std::uniform_int_distribution<long> rand_x(emptySpaces[randRoom].x * roomWidth, emptySpaces[randRoom].x * roomWidth + roomWidth);
    std::uniform_int_distribution<long> rand_y(emptySpaces[randRoom].y * roomHeight, emptySpaces[randRoom].y * roomHeight + roomHeight);
    
    Vec2 orbPos = Vec2(rand_x(e1), rand_y(e1));
    
    int minIdx = -1; //saves which valid spawn location is closest to the generated one
    int minDist = INT_MAX;
    for (int i = 0; i < spawnLocs.size(); i++) {
        int dist = orbPos.distanceSquared(spawnLocs[i]);
        if (dist < minDist) {
            minDist = dist;
            minIdx = i;
        }
    }
    
    if (minIdx == -1) { //currently no available spawn locations
        return;
    }
    
    for (int i = 0; i < world->getNumOrbs(); i++) {
        std::shared_ptr<Orb> orb = world->getOrb(i);
        if (orb->getCollected()) {
            world->removeOrbSpawn(minIdx);
            orb->setPosition(spawnLocs[minIdx]);
            orb->setCollected(false);
            world->setOrbCount(world->getCurrOrbCount()+1);
            NetworkController::sendOrbRespawn(orb->getID(), orb->getPosition());
            return;
        }
    }
    
}

//check which rooms have egg and player and don't spawn in those rooms
void SpawnController::spawnEggs() {
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
    
    std::uniform_int_distribution<long> rand_x(emptySpaces[randRoom].x * roomWidth, emptySpaces[randRoom].x * roomWidth + roomWidth);
    std::uniform_int_distribution<long> rand_y(emptySpaces[randRoom].y * roomHeight, emptySpaces[randRoom].y * roomHeight + roomHeight);
    
    auto spawnLocs = world->getEggSpawns();
    
    auto eggPos = Vec2(rand_x(e1), rand_y(e1));
    
    int minIdx = -1; //saves which valid spawn location is closest to the generated one
    int minDist = INT_MAX;
    for (int i = 0; i < spawnLocs.size(); i++) {
        int dist = eggPos.distanceSquared(spawnLocs[i]);
        if (dist < minDist) {
            minDist = dist;
            minIdx = i;
        }
    }
    
    if (minIdx == -1) { //currently no available spawn locations
        return;
    }
    
    for (int i = 0; i < eggs.size(); i++) {
        std::shared_ptr<Egg> egg = world->getEgg(i);
        if (egg->getHatched()) {
            world->removeEggSpawn(minIdx);
            egg->setPosition(spawnLocs[minIdx]);
            egg->setInitPos(spawnLocs[minIdx]);
            egg->setSpawnLoc(spawnLocs[minIdx]);
            egg->setHatched(false);
            egg->setCollected(false);
            egg->setDistanceWalked(0);
            world->setCurrEggCount(world->getCurrEggCount() + 1);
            NetworkController::sendEggRespawn(egg->getID(), egg->getPosition());
            return;
        }
    }
    
    
}

