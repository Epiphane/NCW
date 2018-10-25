//
//  TimSort.hpp
//  Shared
//
//  Timsort is a sorting algorithm optimized for data where there are lots of "runs"
//      of data that's already in order. We'll use it for sorting the list of UI elements
//      in Z-order, since they're mostly already in order because of the parent-child
//      heirarchy.
//
//  Created by Elliot Fiske on 10/21/18.
//

#pragma once

#include <vector>

#include <Engine/UI/UIElement.h>
namespace CubeWorld {
    
class Timsort {
public:
    void SortMeDaddy(std::vector<Engine::UIElement*> arr);
    
private:
    void InsertionSortInPlace(std::vector<Engine::UIElement*> arr);
    
    struct Run {
        Run();
        int startIndex, endIndex; // Inclusive (start = 0, end = 2 means [x x x o o o o ...])
        int minVal, maxVal;
    };
    
    std::vector<Run> mRuns;  // I've got the runs
};


} // Cubeworld
