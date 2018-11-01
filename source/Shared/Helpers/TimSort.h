//
//  TimSort.h
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
#include <functional>

namespace CubeWorld {
   
namespace Shared {
   
template <typename T>
void TimSortInPlace(std::vector<T>& arr, std::function<bool(T,T)> GreaterThan);
    
template <typename T>
void InsertionSortInPlace(std::vector<T>& arr, std::function<bool(T,T)> GreaterThan);
   
template <typename T>
void MergeSort(std::vector<T> arr1, std::vector<T> arr2);
   
template <typename T>
int BinarySearch(const std::vector<T>& arr, T target, std::function<bool(T,T)> GreaterThan);

} // Shared
   
} // Cubeworld

#include "TimSort_inl.h"
