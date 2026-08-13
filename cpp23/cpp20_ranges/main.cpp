#include <cassert>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <ranges>
#include <string>
#include <vector>

// https://blog.csdn.net/fengbingchun/article/details/143081872
// C++20中头文件ranges的使用
//    <ranges>是C++20中新增加的头文件，提供了一组与范围(ranges)相关的功能，此头文件是ranges库的一部分。包括：
// 1.concepts:

// (1).std::ranges::range:指定类 型为range，即它提供开始迭代器和结束标记(it provides a begin iterator and an end sentinel)。

// (2).std::ranges::sized_range:指定range在恒定时间(constant time)内知道其大小。

// (3).std::ranges::contiguous_range：指定range的迭代器类型满足std::contiguous_iterator。
namespace {
 
struct SimpleRange { // a minimum range
    int* begin();
    int* end();
};
 
struct NotRange { // not a range: begin does not return an input_or_output_iterator
    void* begin();
    int* end();
};
 
} // namespace
 
int test_ranges_concepts()
{
#ifdef _MSC_VER
	static_assert(std::ranges::range<SimpleRange>);
	static_assert(!std::ranges::range<NotRange>);
#endif
 
	static_assert(std::ranges::sized_range<std::list<int>> and not std::ranges::sized_range<std::forward_list<int>>);
 
	static_assert(std::ranges::contiguous_range<const std::vector<int>> and not std::ranges::contiguous_range<std::list<int>>);
 
	return 0;
}

//  2.classes: std::views是std::ranges::views的别名

// (1).std::ranges::subrange：将迭代器-标记对(iterator-sentinel pair)组合成一个视图(view)。

// (2).std::ranges::views::empty、std::ranges::empty_view：没有任何元素的空视图。

// (3).std::ranges::views::single、std::ranges::single_view：包含指定值的单个元素的视图。

// (4).std::ranges::views::iota、std::ranges::iota_view：由重复增加初始值生成的序列构成的视图。

// (5).std::ranges::views::istream、std::ranges::basic_istream_view：在相关输入流上连续应用运算符>>所获得的元素组成的视图。

// (6).std::ranges::views::all、std::ranges::views::all_t：包含range的所有元素的视图。

// (7).std::ranges::ref_view:其它range的元素的视图。

// (8).std::ranges::views::take、std::ranges::take_view：由另一个视图的前N​​个元素组成的视图。

// (9).std::ranges::owning_view：对某一range具有唯一所有权的视图。

// (10).std::ranges::views::drop、std::ranges::drop_view：由另一个视图的元素组成的视图，跳过前N个元素。

// (11).std::ranges::views::join、std::ranges::join_view：由展平(flattening)ranges视图获得的序列组成的视图。

// (12).std::ranges::views::counted:从迭代器和计数创建一个子range。

// (13).std::ranges::views::elements、std::ranges::elements_view：采用由类似元组的值和数字N组成的视图，并生成每个元组的第N个元素的视图。
int test_ranges_classes()
{
	// std::ranges::views == std::views
	// std::ranges::subrange
	const std::multimap<int, char> mm{ {1, 'a'}, {2, 'a'}, {1, 'c'}, {3, 'd'}, {1, 'e'}, {1, 'f'} };
	auto [first, last] = mm.equal_range(1);
	for (const auto& [_, v] : std::ranges::subrange(first, last))
		std::cout << v << ","; // a,c,e,f,
	std::cout << std::endl;
 
	// std::ranges::empty_view
	std::ranges::empty_view<long> e;
	static_assert(std::ranges::empty(e));
	static_assert(0 == std::ranges::empty_view<long>::size());
	static_assert(nullptr == std::ranges::empty_view<long>::data());
	static_assert(nullptr == std::ranges::empty_view<long>::begin());
	static_assert(nullptr == std::ranges::empty_view<long>::end());
 
	// std::ranges::single_view
	constexpr std::ranges::single_view sv1{ std::numbers::pi };
	static_assert(sv1);
	static_assert(not std::ranges::single_view<double>::empty());
	std::cout << "data:" << *sv1.data() << ";begin:" << *sv1.begin() << ";size:" << sv1.size() << ";distance:" << std::distance(sv1.begin(), sv1.end()) << std::endl; // data:3.14159;begin:3.14159;size:1;distance:1
 
	std::ranges::single_view<std::tuple<int, double, std::string>> sv2{ {1, std::numbers::sqrt2, "^-^"} };
	std::cout << "value:" << std::get<0>(sv2[0]) << "," << std::get<1>(sv2[0]) << "," << std::get<2>(sv2[0]) << std::endl; // value:1,1.41421,^-^
 
	// std::ranges::views::iota/std::ranges::iota_view
	for (auto i : std::ranges::iota_view{ 1, 10 }) {
		std::cout << i << ' '; // 1 2 3 4 5 6 7 8 9
	}
	std::cout << std::endl;
 
	for (auto i : std::ranges::views::iota(1, 10))
		std::cout << i << ' '; // 1 2 3 4 5 6 7 8 9
	std::cout << std::endl;
 
	// std::ranges::views::istream
	auto words = std::istringstream{ "today is yesterday's tomorrow" };
	for (const auto& s : std::views::istream<std::string>(words))
		std::cout << std::quoted(s, '/') << ' '; // /today/ /is/ /yesterday's/ /tomorrow/
	std::cout << '\n';
 
	// std::views::all
	const std::vector<int> vec{ 0, 1, 2, 3, 4, 5 };
	for (auto n : std::views::all(vec))
		std::cout << n << ' '; // 0 1 2 3 4 5
	std::cout << std::endl;
 
	// std::ranges::take_view,std::ranges::ref_view
	const std::string str{ "Messy_Test" };
	const std::ranges::take_view tv{ str, 5 };
	std::cout << "second element:" << *(tv.data()+1) << ",size:" << tv.size() << std::endl; // second element:e,size:5
	const std::ranges::ref_view rv{ tv };
	std::cout << "empty:" << rv.empty() << ",size:" << rv.size() << ",begin:" << *rv.begin() << ",end:" << *(rv.end() - 1) << ",data:" << rv.data() << std::endl; // empty:0,size:5,begin:M,end:y,data:Messy_Test
	for (const auto c : rv)
		std::cout << c; // Messy
	std::cout << std::endl;
 
	// std::ranges::owning_view
	using namespace std::literals;
	std::ranges::owning_view ov{ "Messy_Test"s }; // <== ov{std::string{"Messy_Test"}}
	assert(ov.empty() == false && ov.size() == 10 && ov.front() == 'M' && ov.back() == 't');
 
	// std::ranges::drop_view,std::views::drop
	const auto nums = { 1, 2, 3, 4, 5, 6, 7 };
	for (auto i : std::ranges::drop_view{ nums, 2 })
		std::cout << i << ' '; // 3 4 5 6 7
	std::cout << std::endl;
 
	for (auto i : nums | std::views::drop(3))
		std::cout << i << ' '; // 4 5 6 7
	std::cout << std::endl;;
 
	// std::ranges::join_view
	const std::vector<std::vector<int>> v{ {1, 2}, {3, 4, 5}, {6}, {7, 8, 9} };
	auto jv = std::ranges::join_view(v);
#ifdef __linux__
	for (int const e : jv)
		std::cout << e << ' ';
	std::cout << std::endl;
#endif
 
	// std::views::counted
	const int a[]{ 1, 2, 3, 4, 5, 6, 7 };
	for (int i : std::views::counted(a, 3))
		std::cout << i << ' '; // 1 2 3
	std::cout << std::endl;
 
	// std::views::elements
	const std::vector<std::tuple<int, char>> vt {{1, 'A'},{2, 'B'},{3, 'C'},{4, 'D'},{5, 'E'}};
	for (int const e : std::views::elements<0>(vt))
		std::cout << e << ' '; // 1 2 3 4 5
	std::cout << std::endl;
 
	for (char const e : vt | std::views::elements<1>)
		std::cout << e << ' '; // A B C D E
	std::cout << std::endl;
 
	return 0;
}

// 3.customization point objects:

// (1).std::ranges::begin、std::ranges::cbegin:返回指向range开头的迭代器。

// (2).std::ranges::end、std::ranges::cend:返回指示range结束的哨兵(sentinel)。

// (3).std::ranges::rbegin、std::ranges::crbegin:返回range的反向迭代器。

// (4).std::ranges::rend、std::ranges::crend:返回range的反向结束迭代器。

// (5).std::ranges::size:返回等于range大小的整数。

// (6).std::ranges::ssize:返回等于range大小的有符号整数。

// (7).std::ranges::empty:检查range是否为空。

// (8).std::ranges::data 、std::ranges::cdata：获取指向连续range开头的指针。
int test_ranges_customization_point_objects()
{
	// std::ranges::begin
	std::vector v{ 3, 1, 4, 9, 6};
	auto vi = std::ranges::begin(v);
	std::cout << *vi << std::endl; // 3
	*(++vi) = 66;
	std::cout << v[1] << std::endl; // 66
 
	// std::ranges::end
	vi = std::ranges::end(v);
	std::cout << *(vi-1) << std::endl; // 6
	*(--vi) = 88;
	std::cout << v[4] << std::endl; // 88
 
	for (auto vi = std::ranges::cbegin(v); vi != std::ranges::end(v); ++vi)
		std::cout << *vi << " "; // 3 66 4 9 88
	std::cout << std::endl;
 
	// std::ranges::rbegin
	auto vj = std::ranges::rbegin(v);
	std::cout << *vj << std::endl; // 88
	*(++vj) = 33;
	std::cout << v[3] << std::endl;
 
	// std::ranges::rend
	vj = std::ranges::rend(v);
	std::cout << *(vj-1) << std::endl; // 3
	*(--vj) = 99;
	std::cout << v[0] << std::endl; // 99
 
	// std::ranges::size
	std::cout << "size:" << std::ranges::size(v) << ",ssize:" << std::ranges::ssize(v) << std::endl; // size:5,ssize:5
 
	// std::ranges::empty
	std::cout << "v is empty:" << std::ranges::empty(v) << std::endl; // v is empty:0
	std::map<int, std::string> m{};
	std::cout << "m is empty:" << std::ranges::empty(m) << std::endl; // m is empty:1
 
	std::string str{ "hello world!" };
	std::ranges::data(str)[0] = 'H';
	std::cout << std::ranges::data(str) << std::endl; // Hello world!
 
	return 0;
}

int main(){
    std::cout << "C++20中头文件ranges的使用" << std::endl;
    test_ranges_concepts();
    test_ranges_classes();
    test_ranges_customization_point_objects();
    return 0;
}