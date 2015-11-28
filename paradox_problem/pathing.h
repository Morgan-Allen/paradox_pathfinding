//
//  pathing.h
//  paradox_problem
//
//  Created by Morgan Allen on 08/10/2015.
//  Copyright (c) 2015 Inhance. All rights reserved.
//

#ifndef __paradox_problem__pathing__
#define __paradox_problem__pathing__

#include <stdio.h>


int FindPath(
    const int nStartX , const int nStartY ,
    const int nTargetX, const int nTargetY,
    const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
    int *pOutBuffer, const int nOutBufferSize
);

int indexFor(const int x, const int y, const int wide, const int high);

#endif /* defined(__paradox_problem__pathing__) */
