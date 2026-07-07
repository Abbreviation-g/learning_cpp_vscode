#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "message.h"
#include "sort_algorithms.h"

namespace {

void print_values(const std::vector<int>& values)
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

void run_sort_test(const std::string& name, std::vector<int> (*sort_func)(std::vector<int>), const std::vector<int>& input)
{
    const std::vector<int> result = sort_func(input);
    std::cout << name << ": ";
    print_values(result);
    std::cout << (std::is_sorted(result.begin(), result.end()) ? " PASS" : " FAIL") << std::endl;
}

} // namespace

int main()
{
    std::cout << build_message() << std::endl;

    const std::vector<int> values = {9, 4, 6, 2, 8, 1, 5, 3, 7, 0};

    run_sort_test("bubble_sort", sort_algorithms::bubble_sort, values);
    run_sort_test("selection_sort", sort_algorithms::selection_sort, values);
    run_sort_test("insertion_sort", sort_algorithms::insertion_sort, values);
    run_sort_test("merge_sort", sort_algorithms::merge_sort, values);
    run_sort_test("quick_sort", sort_algorithms::quick_sort, values);

    system("pause");
    return 0;
}
