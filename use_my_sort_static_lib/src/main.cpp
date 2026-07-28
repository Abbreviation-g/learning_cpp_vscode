#include <iostream>
#include <vector>

#include <my_sort_static_lib/sort_algorithms.h>

int main()
{
    // 使用同一组输入演示 my_sort_static_lib 中的三种排序算法。
    const std::vector<int> data = {5, 3, 8, 1, 9, 2, 7, 4, 6};

    std::cout << "原始数组: ";
    for (const int value : data) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    const auto quick_sorted = sort_algorithms::quick_sort(data);
    std::cout << "快速排序: ";
    for (const int value : quick_sorted) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    const auto bubble_sorted = sort_algorithms::bubble_sort(data);
    std::cout << "冒泡排序: ";
    for (const int value : bubble_sorted) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    const auto merge_sorted = sort_algorithms::merge_sort(data);
    std::cout << "归并排序: ";
    for (const int value : merge_sorted) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}
