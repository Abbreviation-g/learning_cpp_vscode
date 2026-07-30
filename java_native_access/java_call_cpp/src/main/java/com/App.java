package com;

import java.util.Arrays;

public class App {
    public static void main(String[] args) {
        System.out.println("Hello World!");

        {
            int[] arr = { 2, 5, 1, -1, 9, 8, 6 };
            JnaDll.jnaDll.bubble_sort(arr, arr.length);
            System.out.println(Arrays.toString(arr));
        }
        {
            int[] arr = { 2, 5, 1, -1, 9, 8, 6 };
            JnaDll.jnaDll.selection_sort(arr, arr.length);
            System.out.println(Arrays.toString(arr));
        }
    }
}
