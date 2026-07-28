#ifndef SORT_ALGORITHMS_H
#define SORT_ALGORITHMS_H

#include <vector>

namespace sort_algorithms {

// 冒泡排序：重复交换相邻逆序元素，返回排序后的副本。
std::vector<int> bubble_sort(std::vector<int> values);

// 选择排序：每轮选择未排序区间的最小值，返回排序后的副本。
std::vector<int> selection_sort(std::vector<int> values);

// 插入排序：把当前元素插入到前方已排序区间，返回排序后的副本。
std::vector<int> insertion_sort(std::vector<int> values);

// 归并排序：递归拆分并合并有序区间，返回排序后的副本。
std::vector<int> merge_sort(std::vector<int> values);

// 快速排序：基于枢轴分区后递归排序，返回排序后的副本。
std::vector<int> quick_sort(std::vector<int> values);

} // namespace sort_algorithms

#endif
