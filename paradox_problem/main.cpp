//
//  main.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 07/10/2015.


#include <iostream>
#include <set>
#include "pathing.h"
#include "loaded_map.h"



void attemptPathing(LoadedMap &map, int outputLen) {
    std::cout << "\n\nWill now attempt A* search.";
    
    /*
    const int
      mapW   = 4, mapH   = 4,
      startX = 0, startY = 0,
      endX   = 2, endY   = 0;
    unsigned char map[] = {
        1, 0, 1, 1,
        1, 0, 0, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };
    int outLen = 20, output[outLen];
    //*/
    
    /*
    const int
      mapW   = 4, mapH   = 3,
      startX = 0, startY = 0,
      endX   = 1, endY   = 2;
    unsigned char map[] = {
        1, 1, 1, 1,
        0, 1, 0, 1,
        0, 1, 1, 1
    };
    int outLen = 12, output[outLen];
    //*/
    
    /*
    const int
      mapW   = 3, mapH   = 3,
      startX = 2, startY = 0,
      endX   = 0, endY   = 2;
    unsigned char map[] = {
        0, 0, 1,
        0, 1, 1,
        1, 0, 1
    };
    int outLen = 12, output[outLen];
    //*/
    
    int output[outputLen];
    
    //const int result = -1;
    //*
    const int result = FindPath(
        map.origX, map.origY, map.destX, map.destY,
        map.mapData, map.mapWide, map.mapHigh,
        output, outputLen
    );
    //*/
    
    std::cout << "\n\n\nSearch completed.";
    std::cout << "\n  Path length: " << result;
    std::cout << " (" << (result >= 0 ? "passed" : "failed") << ")";
    
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
            std::cout << " " << at;
        }
    }
}


int main(int argc, const char * argv[]) {
    LoadedMap map;
    loadMapFrom("pathing maps/simple_map.map", map);
    attemptPathing(map, 20);
}













