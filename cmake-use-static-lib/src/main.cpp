#include <iostream>
#include <vector>

#include <cmake-static-lib/message.h>
#include <cmake-static-lib/sort_algorithms.h>

int main()
{
    // 调用 message_utils 静态库提供的消息函数。
    std::cout << build_message() << std::endl;

    // 使用同一组输入演示 sort_utils 中的三种排序算法。
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
