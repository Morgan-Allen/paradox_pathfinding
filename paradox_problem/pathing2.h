//
//  pathing2.h
//  paradox_problem
//
//  Created by Morgan Allen on 28/11/2015.

#include <stdio.h>


int indexFor(const int x, const int y, const int wide, const int high);

int FindPath(
    const int nStartX , const int nStartY ,
    const int nTargetX, const int nTargetY,
    const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
    int *pOutBuffer, const int nOutBufferSize
);