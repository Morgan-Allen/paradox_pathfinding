//
//  pathing2.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 28/11/2015.


#include "pathing2.h"
#include <list>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <cstring>


//
//  This variant on the A* pathfinding algorithm relies on the following:
//    *  Only directly-adjacent tiles on a unit-cost 2d-grid are accessible.
//    *  Manhattan distance is therefore an optimistic heuristic.
//    *  Step-costs (and distance-estimates) therefore have integer values.
//    *  A* is only interested in the most promising node at a given time.
//  Therefore-
//    *  Successors from a given step will never have a _lower_ cost-estimate,
//       because the heuristic is always optimistic.
//    *  Successors from a given step never have a cost-estimate more than 2
//       points higher- and in fact, can only be _exactly_ 2 points higher.
//
//        N            For successors NSEW of step '*', aiming toward X, E will
//      W * E ---- X   be the closest and have the lowest estimate (C).  N & S
//        S            will have estimates C + 2 (y+/-1, x-1), while W will
//                     also have estimate C + 2 (x-2).
//           /--- X
//        N /          In this case, N & E are both 'closest', while W & S will
//      W * E          have estimates C + 2 (x-1, y-1 in both cases.)
//        S            In all cases, estimates are either C, or C + 2.
//
//    *  If we only investigate the most promising step at a time, we only need
//       to keep a list of steps that are _equally_ promising (the first
//       generation), or have a cost 2 points higher (the second generation).
//       Anything less promising (more costly) will never be investigated until
//       the first generation is exhausted, at which point we simply 'promote'
//       the second generation and update our best-estimate.
//
//  NOTE:  Credit goes to mahrgell and ArcticReloaded on the paradox forums for
//         dropping several hints on my head.

//
//  TODO:  In principle, this could be multithreaded relatively easily.  See if
//         there's any gains from that?



/*  Fixed constants that help mark progress within the search-grid and iterate
 *  over neighbours:
 */
static const int
    OFF_GRID = -1,
    NO_PATH  = -1,
    BLOCKED  =  0,
    OPEN     =  1,
    MARKED   =  2;
static const int
    ENTER_COST      = 1,
    GENERATION_DIFF = 2;
static const int
    NUM_ADJ    = 4,
    X_ADJ[]    = {  1,  0, -1,  0 },
    Y_ADJ[]    = {  0,  1,  0, -1 };


/*  Records a point to search within the grid.
 */
struct Entry {
    short x, y;
    int index;
};

typedef std::stack <Entry*> List;


/*  Stores most or all the state-variables associated with the search for ease
 *  of later reference.
 */
struct MapSearch {
    
    int wide, high;
    const unsigned char *rawData;
    unsigned char *usageMask;
    
    int origX, origY;
    int targX, targY;
    int bestEstimate;
    List* firstGen;
    List* secondGen;
};

void cleanupMap(MapSearch &map) {
    delete map.usageMask;
    delete map.firstGen ;
    delete map.secondGen;
}


/*  Helper methods for the search algorithm-
 */
inline int estimate(const int x, const int y, const MapSearch &search) {
    //
    //  Just return manhattan distance.
    return abs(x - search.targX) + abs(y - search.targY);
}


int indexFor(const int x, const int y, const int wide, const int high) {
    //
    //  Useful for external debug functions...
    return (y * wide) + x;
}


inline int indexFor(const int x, const int y, const MapSearch &search) {
    //
    //  Do some basic bounds-checking before returning row-major coords:
    if (x < 0 || x >= search.wide) return OFF_GRID;
    if (y < 0 || y >= search.high) return OFF_GRID;
    return (y * search.wide) + x;
}


inline void addEntries(const Entry &entry, const MapSearch &search) {
    //
    //  We take each of the compass-point directions and calculate coords
    //  and index within the grid.
    for (int n = NUM_ADJ; n-- > 0;) {
        const short
            x = X_ADJ[n] + entry.x,
            y = Y_ADJ[n] + entry.y;
        const int index = indexFor(x, y, search);
        //
        //  If the point is blocked, off-grid, or already searched, skip it.
        if (index == OFF_GRID || search.usageMask[index] != OPEN) continue;
        //
        //  Otherwise, create a new entry:
        Entry *made = new Entry { x, y, index };
        search.usageMask[index] = MARKED + n;
        //
        //  If the cost-estimate is no worse than the current best estimate,
        //  push it onto the first generation- otherwise keep it 'in reserve.'
        const int cost = estimate(x, y, search) + ENTER_COST;
        const bool firstGen = cost == search.bestEstimate;
        if (firstGen) (*search.firstGen ).push(made);
        else          (*search.secondGen).push(made);
    }
}


/*  Finally, the main search method itself, along with some debug options (and
 *  prototypes for use below.)
 */

#define debugMode       false
#define loopLimit       20
#define shouldLimitLoop false
#define say             std::cout

void printMap(MapSearch &search);
void printEntry(const char *intro, Entry &entry);
void printAgenda(const char *intro, MapSearch &search);


int FindPath(
    const int nStartX , const int nStartY ,
    const int nTargetX, const int nTargetY,
    const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
    int *pOutBuffer, const int nOutBufferSize
) {
    //
    //  Firstly, set up the Map-Search itself-
    MapSearch search;
    const int mapArea = nMapWidth * nMapHeight;
    search.rawData = pMap      ;
    search.wide    = nMapWidth ;
    search.high    = nMapHeight;
    search.origX   = nStartX   ;
    search.origY   = nStartY   ;
    search.targX   = nTargetX  ;
    search.targY   = nTargetY  ;
    search.usageMask = new unsigned char[mapArea];
    std::memcpy(search.usageMask, pMap, mapArea * sizeof(char));
    search.firstGen  = new List;
    search.secondGen = new List;
    //
    //  Then set up the first entry on the agenda-
    Entry *first = new Entry;
    (*first).x = search.origX;
    (*first).y = search.origY;
    (*first).index      = indexFor(search.origX, search.origY, search);
    search.bestEstimate = estimate(search.origX, search.origY, search);
    search.usageMask[(*first).index] = MARKED;
    (*search.firstGen).push(first);
    
    #if debugMode
    say << "\nBEGINNING SEARCH!";
    printMap(search);
    printEntry("\n  First Entry: ", *first);
    say << "\n  Target point: " << search.targX << "|" << search.targY;
    int loopCount = 0;
    #endif
    //
    //  Then pop successive entries off the first generation until exhausted:
    bool success = false;
    while (true) {
        
        #if debugMode
        printAgenda("\n\nBEGINNING NEXT STEP...", search);
        loopCount++;
        if (shouldLimitLoop && loopCount > loopLimit) {
            say << "\nLoop limit exceeded- will bail.";
            break;
        }
        #endif
        
        List* gen = search.firstGen;
        Entry *next = (*gen).top();
        (*gen).pop();
        
        #if debugMode
        printEntry("\nNEXT ENTRY IS: ", *next);
        #endif
        
        //
        //  If you've reached the destination, quit and report success (and
        //  delete the popped entry either way.)
        success |= (*next).x == search.targX && (*next).y == search.targY;
        if (! success) addEntries(*next, search);
        delete next;
        if (success) break;
        
        if ((*gen).empty()) {
            //
            //  If the first generation is exhausted, increment the cost
            //  estimate and swap the lists...
            List *temp           = search.secondGen;
            search.secondGen     = search.firstGen;
            search.firstGen      = temp;
            search.bestEstimate += GENERATION_DIFF;
            //
            //  If the second generation was empty, we're done.
            if ((*temp).empty()) break;
        }
    }
    //
    //  In the event of failure, just clean up and return.
    if (! success) {
        cleanupMap(search);
        return NO_PATH;
    }
    //
    //  Otherwise, use directional cues in the usageMask to trace a path back
    //  from the destination to the origin point...
    int currX = search.targX, currY = search.targY;
    std::list <int> pathIndices;
    
    #if debugMode
    say << "\n\nTRACING PATH";
    #endif
    
    while (currX != search.origX || currY != search.origY) {
        const int index = indexFor(currX, currY, search);
        
        #if debugMode
        say << "\n  " << currX << "|" << currY << " (" << index << ")";
        #endif
        
        const int dir = search.usageMask[index] - MARKED;
        currX -= X_ADJ[dir];
        currY -= Y_ADJ[dir];
        pathIndices.push_front(index);
    }
    //
    //  Store as much of the path as can fit into the output buffer, clean up
    //  and return-
    int outIndex = 0, pathLength = (int) pathIndices.size();
    while (outIndex < nOutBufferSize && ! pathIndices.empty()) {
        pOutBuffer[outIndex++] = pathIndices.front();
        pathIndices.pop_front();
    }
    cleanupMap(search);
    return pathLength;
}



/*  Assorted printout functions used in debugging:
 */
void printMap(MapSearch &search) {
    say << "\n  Map display: ";
    int numOpen = 0;
    for (int y = search.high; y-- > 0;) {
        say << "\n    ";
        for (int x = 0; x < search.wide; x++) {
            const int index = indexFor(x, y, search);
            const int mask = search.usageMask[index];
            if (mask == OPEN) numOpen++;
            
            char mark = -1;
            if (x == search.origX && y == search.origY) mark = 'O';
            if (x == search.targX && y == search.targY) mark = 'X';
            if (mark == -1 && mask >= MARKED) mark += mask - MARKED;
            if (mark == -1) mark = mask == BLOCKED ? '@' : '.';
            say << mark << " ";
        }
    }
    say << "\n  Tiles open: " << numOpen;
}


void printEntry(const char *intro, Entry &entry) {
    say << intro << entry.x << "|" << entry.y; return;
}


void printAgenda(const char *intro, MapSearch &search) {
    say <<
        intro << "\n  Agenda size: " <<
        (*search.firstGen).size() << " + " << (*search.secondGen).size()
    ;
    
    say << "\n  Cost " << search.bestEstimate;
    List copy1 = *search.firstGen;
    while (! copy1.empty()) {
        printEntry("\n    ", *copy1.top());
        copy1.pop();
    }
    
    say << "\n  Cost " << (search.bestEstimate + GENERATION_DIFF);
    List copy2 = *search.secondGen;
    while (! copy2.empty()) {
        printEntry("\n    ", *copy2.top());
        copy2.pop();
    }
}
















