#ifndef SORT_INT_ARR_H
#define SORT_INT_ARR_H

#ifdef __cplusplus
extern "C" {
#endif

// 冒泡排序：重复交换相邻逆序元素，原地排序。
void bubble_sort(int* values, int length);

// 选择排序：每轮选择未排序区间的最小值，原地排序。
void selection_sort(int* values, int length);

#ifdef __cplusplus
}
#endif

#endif
