#include "sort_int_arr.h"

void bubble_sort(int* values, int length) {
    for (int i = 0; i < length - 1; ++i) {
        for (int j = 0; j < length - 1 - i; ++j) {
            if (values[j] > values[j + 1]) {
                int temp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = temp;
            }
        }
    }
}

void selection_sort(int* values, int length) {
    for (int i = 0; i < length - 1; ++i) {
        int min_index = i;
        for (int j = i + 1; j < length; ++j) {
            if (values[j] < values[min_index]) {
                min_index = j;
            }
        }
        if (min_index != i) {
            int temp = values[i];
            values[i] = values[min_index];
            values[min_index] = temp;
        }
    }
}

