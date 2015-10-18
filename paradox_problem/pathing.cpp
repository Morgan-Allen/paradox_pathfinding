//
//  pathing.cpp
//  paradox_problem
//
//  Created by Morgan Allen on 08/10/2015.
//  Copyright (c) 2015 Inhance. All rights reserved.
//

#include "pathing.h"
#include <iostream>
#include <thread>
#include <set>
#include <list>
#include <queue>
#include <math.h>


//  NOTE:  Multi-threading considerations not implemented yet.  This is a trial
//  run.  (Also, my C/++ is pretty rusty, so apologies to anyone reading.)


#define printResult false
#define verbose false
#define pathLimit 20
#define shouldLimitPath false


/*  Basic class definitions, setup and support functions first.  In essence,
 *  I'm planning to insert an Entry for every tile of the grid (whether used or
 *  not,) as either actual or potential steps in the search.
 */
static const int
  INIT = -1,
  GRID =  0,
  OPEN =  1,
  USED =  2;

inline int indexFor(const int x, const int y, const int wide, const int high) {
    return (y * wide) + x;
}

struct Entry {
    
    //  Permanent data-
    bool blocked;
    int  x, y;
    
    //  Mutable search-related data-
    int   state      = INIT;
    Entry *prior     = nullptr;
    float costBefore = -1;
    float costAfter  = -1;
    float costTotal  = -1;
};


struct Comparison {
    bool operator() (const Entry* a, const Entry* b) const {
        return (*a).costTotal > (*b).costTotal;
    }
};


struct MapSearch {
    int wide, high;
    Entry *entryGrid;
    const unsigned char *passGrid;
    
    std::priority_queue <Entry*, std::vector <Entry*>, Comparison> agenda;
    int pushCount = 0;
};


Entry* entryAt(int x, int y, MapSearch &search) {
    if (x < 0 || y < 0  ) return nullptr;
    if (x >= search.wide) return nullptr;
    if (y >= search.high) return nullptr;
    
    const int index = indexFor(x, y, search.wide, search.high);
    Entry* found = &(search.entryGrid[index]);
    if ((*found).state != INIT) return found;
    
    (*found).state   = GRID;
    (*found).x       = x;
    (*found).y       = y;
    (*found).blocked = (search.passGrid[index] == 0);
    return found;
}


void printEntry(const char *intro, Entry &entry, bool longForm, bool allowed) {
    if (! allowed) return;
    if (! longForm) {
        std::cout << intro << entry.x << "|" << entry.y; return;
    }
    std::cout <<
        intro << "Entry at: " << entry.x << "|" << entry.y <<
        "\n  Cost before: " << entry.costBefore <<
        "\n  Cost after:  " << entry.costAfter  <<
        "\n  Cost total:  " << entry.costTotal  <<
        "\n  Prior entry: "
    ;
    if (entry.prior != nullptr) {
        Entry prior = *entry.prior;
        std::cout << prior.x << "|" << prior.y;
    }
    else std::cout << "None";
}


void printAgenda(const char *intro, MapSearch &search, bool allowed) {
    if (! allowed) return;
    std::cout << intro << "\n  Agenda size: " << search.agenda.size() << "";
    float lastCost = -1;
    std::priority_queue <Entry*, std::vector<Entry*>, Comparison> copy = search.agenda;
    //
    //  We now print all entries on the agenda in what should be a strictly
    //  descending order of estimated pathing-cost.
    while (copy.size() > 0) {
        Entry *next = copy.top();
        copy.pop();
        
        float cost = (*next).costTotal;
        Entry *gridded = entryAt((*next).x, (*next).y, search);
        
        printEntry("\n    ", *next, false, allowed);
        std::cout << " (" << cost << ")";
        
        if (gridded != next) std::cout << " MISMATCH WITH GRID";
        if (lastCost != -1 && cost < lastCost) std::cout << " OUT OF ORDER";
        lastCost = cost;
    }
}


void printMap(MapSearch &search, bool allowed) {
    if (! allowed) return;
    
    std::cout << "\n  Map display: ";
    int numOpen = 0;
    for (int y = search.high; y-- > 0;) {
        std::cout << "\n    ";
        for (int x = 0; x < search.wide; x++) {
            Entry* at = entryAt(x, y, search);
            
            char tile = (*at).blocked ? '@' : '.';
            int mode = 0;
            
            if ((*at).state == OPEN) mode = 1;
            if ((*at).state == USED) mode = 5;
            if (mode > 0) tile = '0' + mode;
            std::cout << tile << " ";
            
            if ((*at).state == OPEN) numOpen++;
        }
    }
    if (numOpen != search.agenda.size()) {
        std::cout << "\n\nERROR WITH AGENDA MANAGEMENT (WHEN PRINTING) ";
        std::cout << "\n  Agenda size:  " << search.agenda.size();
        std::cout << "\n  Push count:   " << search.pushCount;
        std::cout << "\n  Open on grid: " << numOpen << "\n\n";
        std::cout.flush();
        exit(0);
    }
}


void printPath(
    const char *intro, std::list<Entry*> path, MapSearch &search, bool allowed
) {
    if (! allowed) return;
    std::cout << intro << "\n  Path length: " << path.size() << "";
    for (auto const& step : path) {
        const int x = (*step).x, y = (*step).y;
        printEntry("\n  ", *step, false, allowed);
        std::cout << " (" << indexFor(x, y, search.wide, search.high) << ")";
    }
}



/*  And here's the actual search algorithm, as per specification.
 *
 */
static const int
    NUM_ADJ    = 4,
    ENTER_COST = 1,
    X_ADJ[]    = {  1,  0, -1,  0 },
    Y_ADJ[]    = {  0,  1,  0, -1 };


float guessTravelCost(Entry &orig, Entry &dest) {
    float xd = orig.x - dest.x, yd = orig.y - dest.y;
    return (fabs(xd) + fabs(yd)) * 0.8f;
}


bool addToAgenda(
    Entry &adjacent, MapSearch &search, Entry &target, Entry* prior
) {
    const float olderCost  = adjacent.costBefore;
    const float costBefore = prior ? ((*prior).costBefore + ENTER_COST) : 0;
    if (olderCost >= 0) return false;
    
    const float costAfter = guessTravelCost(adjacent, target);
    adjacent.prior      = prior;
    adjacent.costBefore = costBefore;
    adjacent.costAfter  = costAfter;
    adjacent.costTotal  = costBefore + costAfter;
    adjacent.state      = OPEN;
    
    search.agenda.push(&adjacent);
    search.pushCount++;
    
    printEntry("\n  Pushed to agenda: ", adjacent, true, verbose);
    
    if (search.pushCount != search.agenda.size()) {
        std::cout << "\n\nERROR WITH AGENDA MANAGEMENT (WHEN ADDING) ";
        std::cout << search.agenda.size() << "=/=" << search.pushCount;
        std::cout.flush();
        exit(0);
    }
    return true;
}


void addAdjacentEntries(Entry &point, Entry &target, MapSearch &search) {
    for (int n = NUM_ADJ; n-- > 0;) {
        const int x = X_ADJ[n] + point.x, y = Y_ADJ[n] + point.y;
        Entry* adjacent = entryAt(x, y, search);
        if (adjacent == nullptr || (*adjacent).blocked) continue;
        addToAgenda(*adjacent, search, target, &point);
    }
}


Entry* popBestOffAgenda(MapSearch &search) {
    Entry* next = search.agenda.top();
    search.agenda.pop();
    search.pushCount--;
    (*next).state = USED;
    return next;
}


int FindPath(
    const int nStartX , const int nStartY ,
    const int nTargetX, const int nTargetY,
    const unsigned char *pMap, const int nMapWidth, const int nMapHeight,
    int *pOutBuffer, const int nOutBufferSize
) {
    MapSearch search;
    search.wide      = nMapWidth;
    search.high      = nMapHeight;
    search.entryGrid = new Entry[search.wide * search.high];
    search.passGrid  = pMap;
    
    Entry* starts  = entryAt(nStartX , nStartY , search);
    Entry* target  = entryAt(nTargetX, nTargetY, search);
    Entry* best    = starts;
    bool   success = false;
    printEntry("\n\nTARGET POINT IS:   ", *target, true, verbose);
    printEntry("\n\nSTARTING POINT IS: ", *starts, true, verbose);
    
    addToAgenda(*starts, search, *target, nullptr);
    
    int safeLimit = (search.wide * search.high * 2), stepCount = 0;
    if (shouldLimitPath && safeLimit > pathLimit) safeLimit = pathLimit;
    
    while (search.agenda.size() > 0) {
        if (stepCount++ >= safeLimit) {
            std::cout << "\n\n RAN OUT OF STEPS: " << safeLimit;
            break;
        }
        if (stepCount % 10 == 1) printMap(search, verbose);
        
        printAgenda("\n\nBEGINNING NEXT STEP...", search, verbose);
        Entry* next = popBestOffAgenda(search);
        
        printEntry("\n\nNext entry is: ", *next, true, verbose);
        
        if (next == target) {
            success = true;
            best    = next;
            break;
        }
        else if ((*best).costAfter > (*next).costAfter) {
            best = next;
        }
        
        addAdjacentEntries(*next, *target, search);
    }
    
    std::list <Entry*> path;
    while (best != nullptr && best != starts) {
        path.push_front(best);
        best = (*best).prior;
    }
    
    printPath(success ?
        "\n\nCOMPLETE PATH FOUND..." :
        "\n\nSEARCH FAILED, CLOSEST APPROACH WAS...",
        path, search, verbose || printResult
    );
    printMap(search, verbose || printResult);
    
    if (success) {
        int outCounter = 0, pathLength = (int) path.size();
        while (path.size() > 0 && outCounter < nOutBufferSize) {
            Entry next = *path.front();
            path.pop_front();
            int index = indexFor(next.x, next.y, search.wide, search.high);
            pOutBuffer[outCounter++] = index;
        }
        return pathLength;
    }
    else return -1;
}




