#include <iostream>
#include <vector>

#include <cmake-lib/message.h>
#include <cmake-lib/sort_algorithms.h>

int main()
{
    // 使用 message_utils 动态库
    std::cout << build_message() << std::endl;

    // 使用 sort_utils 动态库
    std::vector<int> data = {5, 3, 8, 1, 9, 2, 7, 4, 6};

    std::cout << "原始数组: ";
    for (int v : data) std::cout << v << " ";
    std::cout << std::endl;

    auto quick_sorted = sort_algorithms::quick_sort(data);
    std::cout << "快速排序: ";
    for (int v : quick_sorted) std::cout << v << " ";
    std::cout << std::endl;

    auto bubble_sorted = sort_algorithms::bubble_sort(data);
    std::cout << "冒泡排序: ";
    for (int v : bubble_sorted) std::cout << v << " ";
    std::cout << std::endl;

    auto merge_sorted = sort_algorithms::merge_sort(data);
    std::cout << "归并排序: ";
    for (int v : merge_sorted) std::cout << v << " ";
    std::cout << std::endl;

    return 0;
}
