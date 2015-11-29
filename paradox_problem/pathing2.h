//
//  pathing2.h
//  paradox_problem
//
//  Created by Morgan Allen on 28/11/2015.

#include <stdio.h>



int indexFor(const int x, const int y, const int wide, const int high);

int doSearch(
    int origX, int origY, int targX, int targY,
    const unsigned char *rawData, int mapWide, int mapHigh,
    int *outBuffer, int maxPathLength
);


