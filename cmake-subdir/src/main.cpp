#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "message.h"
#include "singly_linked_list.h"
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

bool equals(const std::vector<int>& left, const std::vector<int>& right)
{
    return left == right;
}

void print_check(const std::string& name, bool passed)
{
    std::cout << name << ": " << (passed ? "PASS" : "FAIL") << std::endl;
}

void run_linked_list_test()
{
    linked_list::SinglyLinkedList list;

    list.push_back(2);
    list.push_back(3);
    list.push_front(1);

    std::cout << "linked_list after push: ";
    print_values(list.to_vector());
    std::cout << std::endl;
    print_check("linked_list push", equals(list.to_vector(), std::vector<int>{1, 2, 3}));

    print_check("linked_list contains 2", list.contains(2));
    print_check("linked_list remove 2", list.remove_first(2));
    print_check("linked_list after remove", equals(list.to_vector(), std::vector<int>{1, 3}));

    list.push_back(4);
    list.reverse();

    std::cout << "linked_list after reverse: ";
    print_values(list.to_vector());
    std::cout << std::endl;
    print_check("linked_list reverse", equals(list.to_vector(), std::vector<int>{4, 3, 1}));

    list.clear();
    print_check("linked_list clear", list.empty() && list.size() == 0);
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

    run_linked_list_test();

    system("pause");
    return 0;
}
