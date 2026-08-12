#include "sort_int_arr.h"

void bubble_sort(int* values, int length) {
    // 外层循环控制排序轮数，每轮确定一个最大值的位置
    for (int i = 0; i < length - 1; ++i) {
        // 内层循环比较相邻元素，将较大的元素向后交换
        for (int j = 0; j < length - 1 - i; ++j) {
            // 如果当前元素大于后一个元素，则交换它们
            if (values[j] > values[j + 1]) {
                // 临时变量存储当前元素
                int temp = values[j];
                // 将后一个元素赋值给当前元素
                values[j] = values[j + 1];
                // 将临时变量（原当前元素）赋值给后一个元素
                values[j + 1] = temp;
            }
        }
    }
}

void selection_sort(int* values, int length) {
    // 外层循环控制排序轮数，每轮确定一个最小值的位置
    for (int i = 0; i < length - 1; ++i) {
        // 假设当前元素是最小值
        int min_index = i;
        // 内层循环在未排序区间寻找真正的最小值
        for (int j = i + 1; j < length; ++j) {
            // 如果找到更小的元素，更新最小值索引
            if (values[j] < values[min_index]) {
                min_index = j;
            }
        }
        // 如果最小值不是当前元素，则交换它们
        if (min_index != i) {
            // 临时变量存储当前元素
            int temp = values[i];
            // 将最小值赋值给当前位置
            values[i] = values[min_index];
            // 将原当前位置的值赋值给最小值位置
            values[min_index] = temp;
        }
    }
}

