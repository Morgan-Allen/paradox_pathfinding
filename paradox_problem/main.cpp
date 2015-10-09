//
//  main.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 07/10/2015.


#include <iostream>
#include <set>
#include "pathing.h"



void attemptPathing() {
    std::cout << "Will now attempt A* search.";
    
    //*
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
    
    const int result = FindPath(
        startX, startY, endX, endY,
        map, mapW, mapH,
        output, outLen
    );
    
    std::cout << "\n\n\nSearch completed.";
    std::cout << "\n  Path length: " << result;
    std::cout << " (" << (result >= 0 ? "passed" : "failed") << ")";
    
    std::cout << "\n  Output indices: ";
    if (result >= 0) for (int n = 0; n < outLen && n < result; n++) {
        std::cout << "\n    " << output[n];
        map[output[n]] = '*';
    }
    else std::cout << "None";
    map[indexFor(startX, startY, mapW, mapH)] = '.';
    map[indexFor(endX  , endY  , mapW, mapH)] = 'X';
    
    std::cout << "\n  Map display: ";
    for (int y = mapH; y-- > 0;) {
        std::cout << "\n    ";
        for (int x = 0; x < mapW; x++) {
            char at = map[indexFor(x, y, mapW, mapH)];
            if (at == 1) at = '1';
            if (at == 0) at = '0';
            std::cout << " " << at;
        }
    }
}


int main(int argc, const char * argv[]) {
    attemptPathing();
}










