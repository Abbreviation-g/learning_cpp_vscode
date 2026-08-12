#include <array>
#include <iostream>
#include <span>
#include <string>
#include <vector>
#include <cstring>

// https://blog.csdn.net/fengbingchun/article/details/143082300
// C++20中头文件span的使用
//  <span>是C++20中新增加的头文件，此头文件是containers库的一部分。包括：

//   1.模板类std::span：连续对象序列的非拥有视图(view)。std::span可以具有static
//   extent，在这种情况下，序列中的元素数量在编译时已知并以type进行编码，或者具有dynamic
//   extent。

int test_span_span() {
  int a[]{8, 7, 6};
  std::span<int> sp1(a);
  std::cout << sp1.front() << "," << sp1.back() << "," << sp1[1] << ","
            << *sp1.data() << std::endl; // 8,6,7,8

  auto print = [](auto sp) {
    for (auto v : sp)
      std::cout << v << ",";
    std::cout << std::endl;
  };

  std::vector<int> vec{0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::span<int> sp2(vec.data() + 1, 6);
  print(sp2); // 1,2,3,4,5,6,
  std::cout << sp2.size() << "," << sp2.size_bytes() << "," << sp2.empty()
            << std::endl; // 6,24,0

  std::span<int> sp3 = sp2.subspan(1, 4);
  print(sp3); // 2,3,4,5,

  auto sp4 = sp3.first<3>();
  print(sp4); // 2,3,4,

  auto extent = [](auto sp) {
    if (std::dynamic_extent == sp.extent)
      std::cout << "dynamic extent:" << sp.extent << std::endl;
    else
      std::cout << "static extent:" << sp.extent << std::endl;
  };

  auto sp5 = sp3.last<3>();
  print(sp5);  // 3,4,5,
  extent(sp5); // static extent:3

  std::span<int, std::dynamic_extent> sp6{vec};
  extent(sp6); // dynamic extent:18446744073709551615

  return 0;
}

// 2.函数：

//     (1).std::begin, std::cbegin:返回容器或数组开头的迭代器。

//     (2).std::end, std::cend:返回容器或数组末尾的迭代器。

//     (3).std::rbegin, std::crbegin:返回容器或数组开头的反向迭代器。

//     (4).std::rend, std::crend:返回容器或数组的反向结束迭代器。

//     (5).std::size, std::ssize:返回容器或数组的大小。其中ssize为signed size。

//     (6).std::data:获取指向底层数组(underlying
//     array)的指针。不能用于空容器或空数组

//     (7).std::empty：检查容器是否为空。
int test_span_functions() {
  std::vector<int> vec{3, 1, 4, 6, 8};

  std::cout << *std::begin(vec) << "," << *(std::end(vec) - 1)
            << std::endl; // 3,8
  std::cout << *std::crbegin(vec) << "," << *(std::crend(vec) - 1)
            << std::endl; // 8,3

  std::cout << std::size(vec) << "," << std::ssize(vec) << std::endl; // 5,5

  std::string s{"Hello world!"};
  char a[20]; // storage for a C-style string
  strcpy(a, s.c_str());

  std::cout << a << std::endl; // Hello world!

  std::cout << std::empty(vec) << std::endl; // 0
  vec.clear();
  std::cout << std::empty(vec) << std::endl; // 1

  return 0;
}

int main() {
  test_span_span();
  test_span_functions();
  return 0;
}