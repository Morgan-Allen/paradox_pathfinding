//
//  loaded_map.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 09/10/2015.


#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <chrono>

#include "loaded_map.h"
#include "pathing2.h"
//#include "pathing.h"



void attemptPathing(LoadedMap &map, int outputLen) {
    std::cout << "\n\nWill now attempt A* search...";
    
    int output[outputLen];
    auto start = std::chrono::high_resolution_clock::now();
    
    const int result = FindPath(
        map.origX, map.origY, map.destX, map.destY,
        map.mapData, map.mapWide, map.mapHigh,
        output, outputLen
    );
    auto end = std::chrono::high_resolution_clock::now();
    long long taken = std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count();
    
    std::cout << "\n\n  Search completed.";
    std::cout << "\n  Path length: " << result;
    std::cout << "\n  Time taken (ms): " << taken;
    std::cout << " (" << (result >= 0 ? "passed" : "failed") << ")";
    
    if (map.mapWide >= 100) return;
    
    unsigned char* data = map.mapData;
    
    std::cout << "\n  Output indices: ";
    if (result >= 0) for (int n = 0; n < outputLen && n < result; n++) {
        std::cout << "\n    " << output[n];
        data[output[n]] = '*';
    }
    else std::cout << "None";
    
    data[indexFor(map.origX, map.origY, map.mapWide, map.mapHigh)] = '0';
    data[indexFor(map.destX, map.destY, map.mapWide, map.mapHigh)] = 'X';
    
    std::cout << "\n  Map display: ";
    for (int y = map.mapHigh; y-- > 0;) {
        std::cout << "\n    ";
        for (int x = 0; x < map.mapWide; x++) {
            char at = data[indexFor(x, y, map.mapWide, map.mapHigh)];
            if (at == 1) at = '.';
            if (at == 0) at = '@';
            std::cout << at << " ";
        }
    }
}



void testAllMapsInDirectory(const char* dirPath) {
    DIR* directory;
    dirent* entry;
    
    std::cout << "\nScanning directory...";
    directory = opendir(dirPath);
    int dirLen = (int) strlen(dirPath);
    std::vector<char*> mapPaths;
    
    if (directory != nullptr) while (true) {
        entry = readdir(directory);
        if (entry == nullptr) break;
        const char* name = (*entry).d_name;
        int nameLen = (int) strlen(name);
        
        std::cout << "\n  " << name;
        if (nameLen <= 3) continue;
        
        char* fullPath = new char[dirLen + nameLen];
        fullPath[0] = '\0';
        strcat(fullPath, dirPath);
        strcat(fullPath, name   );
        mapPaths.push_back(fullPath);
    }
    
    std::cout << "\n\nTesting maps...\n";
    for (auto const &fullPath : mapPaths) {
        LoadedMap map;
        loadMapFrom(fullPath, map);
        attemptPathing(map, 100);
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
    std::cout << "\nMap Width:  " << map.mapWide;
    std::cout << "\nMap Height: " << map.mapHigh;
    
    const int mapArea = map.mapWide * map.mapHigh;
    map.mapData = new unsigned char[mapArea];
    std::vector<int> freeTiles;
    
    for (int y = map.mapHigh; y-- > 0;) {
        std::string line;
        std::getline(mapFile, line);
        if (line.size() < map.mapWide) continue;
        
        const char* asChars = line.c_str();
        if (map.mapWide < 100) std::cout << "\n  " << asChars;
        
        for (int x = 0; x < map.mapWide; x++) {
            const int index = indexFor(x, y, map.mapWide, map.mapHigh);
            
            char tile = asChars[x];
            if      (tile == '.') tile = 1;
            else if (tile == '@') tile = 0;
            else                  tile = 0;
            
            map.mapData[index] = tile;
            if (tile == 1) freeTiles.push_back(index);
        }
    }
    
    if (freeTiles.size() == 0) {
        map.origX = map.origY = 0;
        map.destX = map.destY = 0;
    }
    else {
        srand((int) time(NULL));
        int origIndex = freeTiles[rand() % freeTiles.size()];
        map.origX = origIndex % map.mapWide;
        map.origY = origIndex / map.mapWide;
        int destIndex = freeTiles[rand() % freeTiles.size()];
        map.destX = destIndex % map.mapWide;
        map.destY = destIndex / map.mapWide;
    }
    
    return &map;
}











