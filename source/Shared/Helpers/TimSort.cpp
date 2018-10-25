//
//  TimSort.cpp
//  Shared
//
//  Created by Elliot Fiske on 10/21/18.
//

#include "TimSort.hpp"

#define SHORT_ENOUGH_FOR_INSERTION_SORT 64  ///< If a run or array is shorter than 64 elements, simple insertion sort is fastest

namespace CubeWorld {
    
void Timsort::InsertionSortInPlace(std::vector<Engine::UIElement*> arr) {
    int i = 1;
    while (i < arr.size()) {
        int j = i;
        
        while (j > 0 && arr[j] > arr[i]) {
            Engine::UIElement* temp = arr[j];
            arr[j] = arr[i];
            arr[i] = temp;
            
            j--;
        }
        
        i++;
    }
}
    
void Timsort::SortMeDaddy(std::vector<Engine::UIElement*> arr) {
    
    if (arr.size() < SHORT_ENOUGH_FOR_INSERTION_SORT) {
        InsertionSortInPlace(arr);
        return;
    }
    
    // Find runs in our data
    std::vector<Run> runs;
    Run* currRun = NULL;
    for (int ndx = 0; ndx < arr.size(); ndx++) {
        if (currRun == NULL) {
            Run newRun;
            newRun.startIndex = ndx;
            newRun.minVal = arr[ndx]->GetFrame().z.value();
            runs.push_back(newRun);
            
            currRun = &runs[runs.size() - 1];
        }
        else {
            if (arr[ndx - 1]->GetFrame().z.value() > arr[ndx]->GetFrame().z.value()) {
                // Current run is over, we just found an element like [5 6 7 8 2 <-- ] 
                currRun->maxVal = arr[ndx - 1]->GetFrame().z.value();
                currRun->endIndex = (ndx - 1);
            }
        }
    }
    
    // Mergesort all the runs together (this will later get more complicated)
    while (runs.size() >= 2) {
        Run* mergingRun1 = &runs[runs.size() - 2];
        Run* mergingRun2 = &runs[runs.size() - 1];
        
        int i = mergingRun1->startIndex, j = mergingRun2->startIndex;
        
        while (i < mergingRun1->endIndex && j < mergingRun2->endIndex) {
            // Take the biggest boy of the two
            if (arr[i]->GetFrame().z.value() > arr[j]->GetFrame().z.value()) {
                
            }
        }
    }
}
    
} // Cubeworld
