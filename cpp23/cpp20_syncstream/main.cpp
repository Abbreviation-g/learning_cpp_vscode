#include <iostream>
#include <syncstream>
#include <fstream>
#include <thread>
#include <vector>

// https://blog.csdn.net/fengbingchun/article/details/143082575
// C++20中头文件syncstream的使用
// <syncstream>是C++20中新增加的头文件，提供了对同步输出流的支持，即在多个线程中可安全地进行输出操作，此头文件是Input/Output库的一部分。包括：

// 	1.std::basic_syncbuf：是std::basic_streambuf的包装器(wrapper)，它将输出累积在其自己的内部缓冲区中，并在销毁时以及明确请求时将其全部内容自动传输到包装的缓冲区，以便它们显示为连续的字符序列。此类 模板主要用于创建与多线程程序无缝协作的自定义流类。如果多个线程试图同时写入同一个流，std::basic_syncbuf将确保每个线程的输出都以正确的顺序写入流，而不会出现任何交错。
// 	2.std::basic_osyncstream：是std::basic_syncbuf的便捷包装器。它提供了一种机制来同步写入同一流的线程。

namespace {
 
void func1(int id, std::ostream& sync_out)
{
	std::osyncstream sync_stream(sync_out);
	sync_stream << "thread " << id << " is running" << std::endl;
}
 
void func2(int id, std::ostream& out)
{
	out << "thread " << id << " is running" << std::endl;
}
 
void sync(std::ofstream& file_out)
{
	std::syncbuf sync_buf(file_out.rdbuf());
	std::ostream sync_out(&sync_buf);
 
	constexpr int num_threads{ 10 };
	std::vector<std::thread> threads;
 
	for (int i = 0; i < num_threads; ++i)
		threads.emplace_back(func1, i, std::ref(sync_out));
 
	for (auto& t : threads)
		t.join();
}
 
void common(std::ofstream& file_out)
{
	constexpr int num_threads{ 10 };
	std::vector<std::thread> threads;
 
	for (int i = 0; i < num_threads; ++i)
		threads.emplace_back(func2, i, std::ref(file_out));
 
	for (auto& t : threads)
		t.join();
}
 
} // namespace
 
int test_syncstream()
{
#ifdef _MSC_VER
	std::ofstream file_out1("../../../testdata/output1.txt");
	std::ofstream file_out2("../../../testdata/output2.txt");
#else
	std::ofstream file_out1("testdata/output1.txt");
	std::ofstream file_out2("testdata/output2.txt");
#endif
 
	if (!file_out1 || ! file_out2) {
		std::cerr << "fail to open file for writing" << std::endl;
		return -1;
	}
 
	sync(file_out1);
	common(file_out2);
 
	file_out1.close();
	file_out2.close();
	return 0;
}

int main(){
    test_syncstream();
    return 0;
}