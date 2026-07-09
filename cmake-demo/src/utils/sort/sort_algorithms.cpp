#include "sort_algorithms.h"

#include <algorithm>
#include <cstddef>

namespace sort_algorithms {
namespace {

// 对 [left, right) 区间做归并排序，buffer 用作合并时的临时空间。
void merge_sort_range(std::vector<int>& values, std::vector<int>& buffer, std::size_t left, std::size_t right)
{
    if (right - left <= 1) {
        return;
    }

    const std::size_t mid = left + (right - left) / 2;
    merge_sort_range(values, buffer, left, mid);
    merge_sort_range(values, buffer, mid, right);

    // 两个子区间都已经有序，把较小元素依次写入 buffer。
    std::size_t i = left;
    std::size_t j = mid;
    std::size_t k = left;

    while (i < mid && j < right) {
        buffer[k++] = values[i] <= values[j] ? values[i++] : values[j++];
    }
    while (i < mid) {
        buffer[k++] = values[i++];
    }
    while (j < right) {
        buffer[k++] = values[j++];
    }

    // 把合并后的有序结果复制回原数组对应区间。
    for (std::size_t index = left; index < right; ++index) {
        values[index] = buffer[index];
    }
}

// 对闭区间 [left, right] 做快速排序分区和递归处理。
void quick_sort_range(std::vector<int>& values, int left, int right)
{
    if (left >= right) {
        return;
    }

    const int pivot = values[left + (right - left) / 2];
    int i = left;
    int j = right;

    // 从两端向中间扫描，把小于 pivot 的元素放左侧，大于 pivot 的元素放右侧。
    while (i <= j) {
        while (values[i] < pivot) {
            ++i;
        }
        while (values[j] > pivot) {
            --j;
        }
        if (i <= j) {
            std::swap(values[i], values[j]);
            ++i;
            --j;
        }
    }

    if (left < j) {
        quick_sort_range(values, left, j);
    }
    if (i < right) {
        quick_sort_range(values, i, right);
    }
}

} // namespace

std::vector<int> bubble_sort(std::vector<int> values)
{
    for (std::size_t end = values.size(); end > 1; --end) {
        bool swapped = false;

        // 每轮把当前未排序区间的最大值“冒泡”到末尾。
        for (std::size_t i = 1; i < end; ++i) {
            if (values[i - 1] > values[i]) {
                std::swap(values[i - 1], values[i]);
                swapped = true;
            }
        }

        // 如果本轮没有交换，说明数组已经有序，可以提前结束。
        if (!swapped) {
            break;
        }
    }
    return values;
}

std::vector<int> selection_sort(std::vector<int> values)
{
    for (std::size_t i = 0; i < values.size(); ++i) {
        std::size_t min_index = i;

        // 在未排序区间中寻找最小值的位置。
        for (std::size_t j = i + 1; j < values.size(); ++j) {
            if (values[j] < values[min_index]) {
                min_index = j;
            }
        }
        std::swap(values[i], values[min_index]);
    }
    return values;
}

std::vector<int> insertion_sort(std::vector<int> values)
{
    for (std::size_t i = 1; i < values.size(); ++i) {
        const int current = values[i];
        std::size_t j = i;

        // 将比 current 大的元素右移，给 current 腾出插入位置。
        while (j > 0 && values[j - 1] > current) {
            values[j] = values[j - 1];
            --j;
        }
        values[j] = current;
    }
    return values;
}

std::vector<int> merge_sort(std::vector<int> values)
{
    // buffer 只分配一次，递归过程中复用，避免每层递归重复分配。
    std::vector<int> buffer(values.size());
    merge_sort_range(values, buffer, 0, values.size());
    return values;
}

std::vector<int> quick_sort(std::vector<int> values)
{
    // 空数组不进入递归，避免 size_t 到 int 转换时出现下溢。
    if (!values.empty()) {
        quick_sort_range(values, 0, static_cast<int>(values.size() - 1));
    }
    return values;
}

} // namespace sort_algorithms
