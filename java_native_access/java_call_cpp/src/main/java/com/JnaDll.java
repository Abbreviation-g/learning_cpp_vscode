package com;
import com.sun.jna.Library;
import com.sun.jna.Native;
public interface JnaDll extends Library {
    // 实例化dll
    JnaDll jnaDll = (JnaDll)Native.load("cpp_native_sort_int_arr", JnaDll.class);

    // 声明dll导出的函数
    // 冒泡排序：重复交换相邻逆序元素，返回排序后的副本。
    void bubble_sort(int[] values, int length);

    // 选择排序：每轮选择未排序区间的最小值，返回排序后的副本。
    void selection_sort(int[] values, int length);
}
