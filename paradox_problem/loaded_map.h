//
//  loaded_map.h
//  paradox_problem
//
//  Created by Morgan Allen on 09/10/2015.




#include <stdio.h>


struct LoadedMap {
    
    int mapWide;
    int mapHigh;
    unsigned char *mapData;
    
    int origX = -1, origY = -1, destX = -1, destY = -1;
};


void attemptPathing(LoadedMap &map, int outputLen);
void testAllMapsInDirectory(const char* dirPath);
LoadedMap* loadMapFrom(const char* fileName, LoadedMap &map);







