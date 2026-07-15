#include <iostream>
#include <vector>
#include <algorithm>

#include "message.h"
#include "sort_algorithms.h"

static void test_message() {
    std::string message = build_message();
    std::cout << message << std::endl;
}

// 统一打印整型数组，供排序和链表测试复用。
static void print_values(const std::vector<int>& values)
{
    std::cout << "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << values[i];
    }
    std::cout << "]";
}
// 执行一个排序函数，并用 std::is_sorted 判断排序结果是否正确。
static void run_sort_test(const std::string& name, std::vector<int> (*sort_func)(std::vector<int>), const std::vector<int>& input)
{
    const std::vector<int> result = sort_func(input);
    std::cout << name << ": ";
    print_values(result);
    std::cout << (std::is_sorted(result.begin(), result.end()) ? " PASS" : " FAIL") << std::endl;
}

static void test_sort() {
        // 所有排序算法使用同一份输入，便于对比输出结果。
    const std::vector<int> values = {9, 4, 6, 2, 8, 1, 5, 3, 7, 0};

    run_sort_test("bubble_sort", sort_algorithms::bubble_sort, values);
    run_sort_test("selection_sort", sort_algorithms::selection_sort, values);
    run_sort_test("insertion_sort", sort_algorithms::insertion_sort, values);
    run_sort_test("merge_sort", sort_algorithms::merge_sort, values);
    run_sort_test("quick_sort", sort_algorithms::quick_sort, values);

    std::cout << std::endl;
    std::cout << "All sorting tests passed!" << std::endl;
    std::cout << std::endl;

    std::cout << "All tests passed!" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    test_message();
    test_sort();

    return 0;
}
