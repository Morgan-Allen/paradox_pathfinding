//
//  main.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 07/10/2015.


#include <iostream>
#include <set>
#include "pathing.h"
#include "loaded_map.h"




int main(int argc, const char * argv[]) {
    
    
    
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
    
    //LoadedMap map;
    //loadMapFrom("pathing maps/random_maps/random512-40-0.map", map);
    //attemptPathing(map, 200);
    
    testAllMapsInDirectory("pathing maps/room_maps/");
}






