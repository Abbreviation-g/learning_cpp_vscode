#ifndef SORT_ALGORITHMS_H
#define SORT_ALGORITHMS_H

#include <vector>

namespace sort_algorithms {

std::vector<int> bubble_sort(std::vector<int> values);
std::vector<int> selection_sort(std::vector<int> values);
std::vector<int> insertion_sort(std::vector<int> values);
std::vector<int> merge_sort(std::vector<int> values);
std::vector<int> quick_sort(std::vector<int> values);

} // namespace sort_algorithms

#endif
