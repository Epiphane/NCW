//
//  TimSort_inl.h
//  Shared
//
//  Created by Elliot Fiske on 10/21/18.
//

#include <vector>
#include <functional>

#define SHORT_ENOUGH_FOR_INSERTION_SORT 64  ///< If a run or array is shorter than 64 elements, simple insertion sort is fastest

namespace CubeWorld {
   
namespace Shared {
   
struct Run {
   int startIndex, endIndex; // Inclusive/Exclusive (start = 0, end = 2 means [x x o o o o o ...])
};
   
template <typename T>
inline void InsertionSortInPlace(std::vector<T>& arr, std::function<bool(T,T)> GreaterThan) {
   int i = 1;
   while (i < arr.size()) {
      int j = i;
                              //  arr[j-1] > arr[j]
      while (j > 0 && GreaterThan(arr[j-1],  arr[j])) {
         T temp = arr[j];
         arr[j] = arr[j-1];
         arr[j-1] = temp;
         
         j--;
      }
      
      i++;
   }
}

/**
 * Perform a binary search for a given target. Returns the index
 *  where `target` should be inserted to maintain a sorted array.
 *
 * Note that `arr` must be sorted in ascending order.
 */
template <typename T>
int BinarySearch(const std::vector<T>& arr, T target, std::function<bool(T,T)> GreaterThan) {
   int startNdx = 0; 
   int endNdx   = arr.size() - 1;
   
   if (endNdx <= startNdx) {
      if (target < arr[startNdx]->doubleValue()) {
         return startNdx;
      }
      else {
         return startNdx + 1;
      }
   }
   
   int currNdx;
   
   while (startNdx != endNdx) {
      currNdx = (startNdx + endNdx) / 2;
      
                  //  arr[currNdx] > target
      if (GreaterThan(arr[currNdx],  target)) {
         endNdx = currNdx;
      }
                  //       arr[currNdx] > target
      else if (GreaterThan(arr[currNdx],  target)) {
         startNdx = currNdx;
      }
      else {
         break;
      }
   }
   
   return currNdx;
}

/**
 *  Perform TimSort on arr in-place, using GreaterThan as the comparison function.
 */
template <typename T>
void TimSortInPlace(std::vector<T>& arr, std::function<bool(T,T)> GreaterThan) {
   
   if (arr.size() < SHORT_ENOUGH_FOR_INSERTION_SORT) {
      InsertionSortInPlace(arr, GreaterThan);
      return;
   }
   
   // Find runs in our data
   std::vector<Run> runs;
   
   Run firstRun;
   firstRun.startIndex = 0;
   runs.push_back(firstRun);
   
   for (int ndx = 1; ndx < arr.size(); ndx++) {
                  //  arr[ndx - 1] > arr[ndx]
      if (GreaterThan(arr[ndx - 1],  arr[ndx])) {
         // Current run is over, we just found an element like [5 6 7 8 2 <-- ] 
         runs.back().endIndex = (ndx - 1);
         
         Run newRun;
         newRun.startIndex = ndx;
         runs.push_back(newRun);
      }
   }
   
   runs.back().endIndex = arr.size() - 1;
   
   // Merge all the runs together
   while (runs.size() >= 2) {
      const Run& mergingRun1 = runs[runs.size() - 2];
      const Run& mergingRun2 = runs[runs.size() - 1];
      
      std::vector<T> mergedRun;
      
      int i = mergingRun1.startIndex, j = mergingRun2.startIndex;
      
      while (i <= mergingRun1.endIndex && j <= mergingRun2.endIndex) {
         // Take the smallest boy of the two and place it at the end of the mergedRun
         if (arr[i] < arr[j]) {
            mergedRun.push_back(arr[i]);
            i++;
         }
         else {
            mergedRun.push_back(arr[j]);
            j++;
         }
      }
      
      // mergedRun += remainder of mergingRun1
      mergedRun.insert(mergedRun.end(), arr.begin() + i, arr.begin() + mergingRun1.endIndex + 1);
      
      // mergedRun += remainder of mergingRun2
      mergedRun.insert(mergedRun.end(), arr.begin() + j, arr.begin() + mergingRun2.endIndex + 1);
      
      std::copy(mergedRun.begin(), mergedRun.end(), arr.begin() + mergingRun1.startIndex);
      
      Run newRun;
      newRun.startIndex = mergingRun1.startIndex;
      newRun.endIndex = mergingRun2.endIndex;
      
      runs.pop_back();
      runs.pop_back();
      runs.push_back(newRun);  ///< We just merged the last 2 runs into 1 run.
   }
   
   // We're now all tasty and sorted!
}
   
} // Shared
   
} // Cubeworld
