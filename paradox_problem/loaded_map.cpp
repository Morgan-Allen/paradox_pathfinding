//
//  loaded_map.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 09/10/2015.
//  Copyright (c) 2015 Inhance. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <cstdlib>
#include <time.h>

#include "loaded_map.h"
#include "pathing.h"


void scanDirectory() {
    DIR* directory;
    dirent* entry;
    
    std::cout << "\nScanning directory...";
    directory = opendir(".");
    
    if (directory != nullptr) while (true) {
        entry = readdir(directory);
        if (entry == nullptr) break;
        std::cout << "\n  " << (*entry).d_name;
    }
}






LoadedMap* loadMapFrom(const char* fileName, LoadedMap &map) {
    
    std::ifstream mapFile;
    mapFile.open(fileName);
    std::cout << "\n\nLoading file: " << fileName;
    
    std::string typeLine, highToken, highVal, wideToken, wideVal, initLine;
    std::getline(mapFile, typeLine      );
    std::getline(mapFile, highToken, ' ');
    std::getline(mapFile, highVal       );
    std::getline(mapFile, wideToken, ' ');
    std::getline(mapFile, wideVal       );
    std::getline(mapFile, initLine      );
    std::string trailLine;
    std::getline(mapFile, trailLine);
    
    map.mapWide = atoi(highVal.c_str());
    map.mapHigh = atoi(wideVal.c_str());
    const int mapArea = map.mapWide * map.mapHigh;
    //unsigned char mapData[mapArea];
    map.mapData = new unsigned char[mapArea];
    
    std::cout << "\nMap Width:  " << map.mapWide;
    std::cout << "\nMap Height: " << map.mapHigh;
    
    std::vector<int> freeTiles;
    
    for (int y = map.mapHigh; y-- > 0;) {
        std::string line;
        std::getline(mapFile, line);
        const char* asChars = line.c_str();
        if (map.mapWide < 100) std::cout << "\n  " << asChars;
        
        for (int x = 0; x < map.mapWide; x++) {
            char tile = asChars[x];
            if      (tile == '.') tile = 1;
            else if (tile == '@') tile = 0;
            else std::cout << "_?: " << tile;
            
            const int index = indexFor(x, y, map.mapWide, map.mapHigh);
            map.mapData[index] = tile;
            if (tile == 1) freeTiles.push_back(index);
        }
    }
    
    srand((int) time(NULL));
    int origIndex = freeTiles[rand() % freeTiles.size()];
    map.origX = origIndex % map.mapWide;
    map.origY = origIndex / map.mapHigh;
    int destIndex = freeTiles[rand() % freeTiles.size()];
    map.destX = destIndex % map.mapWide;
    map.destY = destIndex / map.mapHigh;
    
    return &map;
}











