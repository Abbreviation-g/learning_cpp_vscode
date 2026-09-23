// https://subingwen.cn/cpp/filesystem1
// 文件系统 - 文件路径以及文件的状态和属性
#include <iostream>
#include <filesystem>
#include <system_error>

// 2. 路径的使用
namespace path_ns {
    namespace fs = std::filesystem;
    // 2.1 构造与赋值
    // 我们可以使用各种字符串类型来构造 path：
    void test1() {
        // 1.默认构造
        fs::path p1; // 空路径
        // 2.使用字符串字面量
        fs::path p2 = "c:/";
        fs::path p3 = L"c:/tmp"; // windows宽字符
        // 3.使用std::string
        std::string s = "c:/";
        // 4. 拷贝构造
        fs::path p4 = s;
        fs::path p5 = p4;
    }
    // 2.2 路径连接与修改
    // 路径连接与修改是 path 最常用的功能之一。C++17 重载了 operator/ 和 operator/=，用于拼接路径。它会自动处理路径分隔符，避免出现双斜杠或缺失斜杠的问题。
    // 代码中建议统一使用 /，std::filesystem 会自动将其转换为操作系统所需的分隔符（Windows 下为 \，Linux 下为 /）。
    void test2() {
        fs::path base = "/user/";
        fs::path dir = "local/";
        fs::path file = "data.txt";

        // 自动添加分隔符
        fs::path full = base / dir / file;
        std::cout << full << std::endl; // "/user/local/data.txt"

        fs::path base1 = "/user/";
        fs::path dir1 = "/local";
        fs::path file1 = "data.txt";

        // 自动添加分隔符
        fs::path full1 = base1 / dir1 / file1;
        // 结果: /local/data.txt

        // 类似 append
        base /= "include";
        // 结果: /user/include

        // 规范化操作
        fs::path p = "/usr/../usr/local/./bin";
        fs::path normalized = p.lexically_normal();
        std::cout << "normalized: " << normalized << "\n"; // normalized: "\\usr\\local\\bin"

        std::cout << "full = " << full << std::endl; // full = "/user/local/data.txt"
        std::cout << "full = " << full1 << std::endl; // full = "/local\\data.txt"

        // 在 C++17 的 std::filesystem 中，使用 / 运算符拼接路径时，如果右操作数（dir1）是绝对路径（以根目录 /
        // 开头），它会替换/丢弃左操作数（base1）之前的内容，而不是简单的字符串拼接。
        // 第一步 base1 / dir1：由于 dir1 是 /local，它会覆盖 base1。结果变为 /local。
        // 第二步 (结果) / file1：拼接 data.txt。结果变为 /local/data.txt。
        // p.lexically_normal() 是 std::filesystem::path
        // 类的一个成员函数。它的作用是生成一个新的路径对象，该对象代表了当前路径的规范形式。这个函数仅通过字符串分析（词法分析）来消除路径中的冗余部分，它不会访问硬盘（即不检查文件或目录是否真实存在）。
    }
    // 2.3 路径分解
    // path 提供了成员函数来将路径拆解为各个组成部分。这些函数返回新的 path 对象。

    // 函数	描述	示例路径: /foo/bar.txt
    // root_name()	根名称（如驱动器盘符、网络位置）	Linux : 空，Win 下为 "C:"
    // root_directory()	根目录	/
    // root_path()	根名称 + 根目录	/ (Win 下为 "C:/")
    // relative_path()	相对路径部分（去掉 root_path）	foo/bar.txt
    // parent_path()	父目录路径	/foo
    // filename()	文件名或目录名	bar.txt
    // stem()	主文件名（不含扩展名）	bar
    // extension()	扩展名（含点）	.txt
    void test3() {
        fs::path p = R"(C:\install\my_sort_static_lib\lib\cmake\my_sort_shared_lib\my_sort_shared_lib-config.cmake)";

        std::cout << "root_name: " << p.root_name() << std::endl; // root_name: "C:"
        std::cout << "root_directory: " << p.root_directory() << std::endl; // root_directory: "\\"
        std::cout << "root_path: " << p.root_path() << std::endl; // root_path: "C:\\"
        std::cout << "relative_path: " << p.relative_path()
                  << std::endl; // relative_path: "install\\my_sort_static_lib\\lib\\cmake\\my_sort_shared_lib\\my_sort_shared_lib-config.cmake"
        std::cout << "Parent:   " << p.parent_path() << std::endl; // Parent:   "C:\\install\\my_sort_static_lib\\lib\\cmake\\my_sort_shared_lib"
        std::cout << "Filename: " << p.filename() << std::endl; // Filename: "my_sort_shared_lib-config.cmake"
        std::cout << "Stem:     " << p.stem() << std::endl; // Stem:     "my_sort_shared_lib-config"
        std::cout << "Extension:" << p.extension() << std::endl; // Extension:".cmake"

        std::cout << (".cmake" == p.extension().string()) << std::endl; // 1
        //         注意 stem 的特殊情况：
        // 文件 ." (只有点) -> stem 是 .
        // 文件 .. (双点) -> stem 是 ..
        // 文件 .profile -> stem 是 .profile (没有扩展名)
    }

    // 2.4 路径查询与状态
    // 在 C++17 的 std::filesystem::path 类中，有一系列函数用于检查路径自身的语法属性（即检查字符串本身的特征，而不是检查磁盘上文件的实际状态）。

    // 1. 检查路径是否为空
    // 函数原型	返回值	说明
    // bool empty() const noexcept	bool	检查路径是否为空（即路径字符串的长度是否为 0）。

    // 2. 检查特定路径组件是否存在
    // 这些函数检查路径中是否包含根目录、相对路径或文件名。
    // 函数原型	返回值	说明
    // bool has_root_path() const	bool	检查是否有根路径（包含根名称和根目录，如 Windows 的 C: 或 Unix 的 /）。
    // bool has_root_name() const	bool	检查是否有根名称（如 Windows 的 C: 或网络路径 \\server）。
    // bool has_root_directory() const	bool	检查是否有根目录（如 Unix 的 /）。
    // bool has_relative_path() const	bool	检查是否有相对路径部分（即除去根路径后的剩余部分）。
    // bool has_parent_path() const	bool	检查是否有父路径（即路径的上一级目录是否存在）。
    // bool has_filename() const	bool	检查是否有文件名（通常指路径的最后一个组件）。
    // bool has_stem() const	bool	检查是否有主文件名（即不包含扩展名的文件名）。
    // bool has_extension() const	bool	检查是否有文件扩展名（如 .txt）。

    // 3. 检查路径的性质（绝对 vs 相对）
    // 函数原型	返回值	说明
    // bool is_absolute() const	bool	检查是否为绝对路径（Unix 下以 / 开头，Windows 下以盘符开头或开头有 /）。
    // bool is_relative() const	bool	检查是否为相对路径（非绝对路径）。
    // 4. 返回迭代器相关的函数
    // 虽然这些函数不直接返回布尔值，但它们提供了遍历路径组件的能力，常被用于精细的路径分析或迭代器检查。
    // 函数原型	返回值类型	说明
    // begin() const	iterator	返回指向路径第一个组件的迭代器。
    // end() const	iterator	返回指向路径末尾的迭代器。
    void test4() {
        fs::path p = "/user/local/data.txt";

        // 检查属性
        if (p.has_root_directory()) {
            std::cout << "Has root directory (/)" << std::endl;
        }
        if (p.has_filename()) {
            std::cout << "Has filename: " << p.filename() << std::endl;
        }
        if (p.has_extension()) {
            std::cout << "Has extension: " << p.extension() << std::endl;
        }
        if (p.is_absolute()) {
            std::cout << "This is an absolute path." << std::endl;
        }

        // 使用迭代器遍历
        std::cout << "\nPath components:" << std::endl;
        for (auto it = p.begin(); it != p.end(); ++it) {
            std::cout << "Component: " << *it << std::endl;
        }
        // Component: "/"
        // Component: "user"
        // Component: "local"
        // Component: "data.txt"
    }
} // namespace path_ns
// 3. 文件状态和属性
// 在 C++17 std::filesystem (文件系统库) 中，查看文件状态的核心机制是通过 std::filesystem::file_status 类以及相关的辅助函数来实现的。
// 这套机制允许程序在不打开文件的情况下，获取文件的元数据，例如：文件是否存在、是普通文件还是目录、文件大小、权限等。
namespace status_ns {
    //     3.1 核心类 file_status
    // file_status 是一个轻量级的类，用于存储文件的类型和权限。

    // 主要功能：封装了文件类型（file_type）和文件权限（perms）。
    // 获取方式：通常不直接构造，而是通过 status() 或 symlink_status() 函数获取。
    // 文件类型std::filesystem::file_type 描述了文件系统的对象类型。常见的值包括：

    // 枚举值	含义	说明
    // none	未知类型	状态尚未确定或发生错误。
    // not_found	文件未找到	路径不存在。
    // regular	普通文件	常规的磁盘文件（如 .txt, .exe）。
    // directory	目录	文件夹。
    // symlink	符号链接	软链接（快捷方式）。
    // block	块设备	块特殊文件（如硬盘分区）。
    // character	字符设备	字符特殊文件（如键盘、终端）。
    // fifo	管道	命名管道 (IPC)。
    // socket	套接字	Unix 域套接字。
    // unknown	类型未知	文件存在但类型无法识别。
    // 文件权限std::filesystem::perms 通常对应 POSIX 系统的权限位。主要值包括（通过位掩码组合使用）：

    // none: 无权限。
    // owner_read, owner_write, owner_exec: 文件所有者的读、写、执行权限。
    // group_read, group_write, group_exec: 文件所属组的读、写、执行权限。
    // others_read, others_write, others_exec: 其他用户的读、写、执行权限。
    // all: 所有权限（等同于 0777）。
    // set_uid, set_gid, sticky_bit: 特殊位（设置用户ID、设置组ID、粘滞位）。
    // mask, unknown: 掩码和未知标志。
    // 除了直接获取 file_status 对象并检查其类型，C++17 还提供了一组极为便捷的辅助函数。这些函数内部会自动调用
    // status()，因此你无需手动处理中间对象（这些is_xxx函数都是重载函数，最常用的就是给他们的参数传递一个文件路径path）。

    // 文件类型	函数功能	返回值
    // is_block_file	检查给定的路径是否表示块设备	bool
    // is_character_file	检查给定的路径是否表示字符设备	bool
    // is_directory	检查给定的路径是否表示一个目录	bool
    // is_empty	检查给定的路径是否表示一个空文件或空目录	bool
    // is_fifo	检查给定的路径是否表示一个命名管道	bool
    // is_other	检查参数是否表示一个其他文件 （非文件、非目录、非链接）	bool
    // is_regular_file	检查参数是否表示一个常规文件	bool
    // is_socket	检查参数是否表示一个具名 IPC 套接字	bool
    // is_symlink	检查参数是否表示一个符号链接	bool
    // status_known	检查文件状态是否已知（参数为std::filesystem::file_status类型）	bool
    // 上面说完了类型和权限，std::filesystem 还提供了获取其他元数据的函数，它们通常直接作用于 path 或 directory_entry。

    // 函数名	作用	返回类型
    // file_size(path)	获取文件大小（字节）	uintmax_t
    // hard_link_count(path)	获取硬链接数	uintmax_t
    // last_write_time(path)	获取最后修改时间	file_time_type
    // 3.2 查询状态的函数
    // 在 C++17 中，获取文件状态的功能主要通过头文件 <filesystem>（即 std::filesystem
    // 库）提供。有三个主要的函数用于获取文件状态，它们分别处理不同的场景和符号链接的行为。

    // 这三个核心函数是：

    // std::filesystem::status
    // std::filesystem::symlink_status
    // std::filesystem::status_known
    // 3.2.1 std::filesystem::status
    // 这是最常用的获取文件状态的函数。它的主要作用是获取文件路径所指向的文件系统对象的状态信息（如类型、权限等）。函数原型为：

    // std::filesystem::file_status status(const std::filesystem::path& p);
    // std::filesystem::file_status status(const std::filesystem::path& p, std::error_code& ec) noexcept;
    // 关键行为（符号链接处理）：

    // 跟随符号链接：如果路径 p 指向的是一个符号链接，status 函数会自动解引用。也就是说，它会返回符号链接所指向的实际目标文件的状态，而不是符号链接本身的状态。
    // 例如：如果 link.txt -> target.txt，调用 status("link.txt") 将返回 target.txt 的属性（如是否为常规文件、大小等）。
    // 返回值：

    // 返回一个 std::filesystem::file_status 对象。
    // 该对象可以通过 .type() 获取文件类型（如 regular_file, directory, block_file 等）。
    // 该对象可以通过 .permissions() 获取文件权限。
    // 异常与错误处理：

    // 如果文件不存在（或无法访问），默认会抛出 std::filesystem::filesystem_error 异常。
    // 使用带有 std::error_code& ec 参数的重载版本，不会抛出异常。如果出错，会将 ec 设置为相应的错误码，并且返回的 file_status 对象的类型为 file_type::not_found
    // 或 file_type::unknown。 返回值 file_status 类的成员函数：

    // 这些函数用于获取或修改 file_status 对象中存储的状态信息。
    // std::filesystem::file_type type() const noexcept:
    // 获取文件类型。
    // 返回对象当前存储的文件类型（std::filesystem::file_type 枚举值）。
    // void type(std::filesystem::file_type ft) noexcept:
    // 设置文件类型。
    // 将对象的文件类型设置为 ft。
    // std::filesystem::perms permissions() const noexcept:
    // 获取文件权限。
    // 返回对象当前存储的文件权限（std::filesystem::perms 枚举值，通常是位掩码）。
    // void permissions(std::filesystem::perms prms) noexcept:
    // 设置文件权限。
    // 将对象的文件权限设置为 prms。
    namespace fs = std::filesystem;
    void test1() {
        fs::path p{ R"("C:\install\my_sort_static_lib\lib\cmake\my_sort_shared_lib\my_sort_shared_lib-config.cmake")" };
        // 获取文件状态
        fs::file_status status = fs::status(p);
        // 使用成员函数检查类型和权限
        std::cout << "File type: ";
        switch (status.type()) {
        case fs::file_type::regular:
            std::cout << "Regular file." << "\n";
        case fs::file_type::directory:
            std::cout << "Directory." << "\n";
        case fs::file_type::not_found:
            std::cout << "Not Found." << "\n";
        default:
            std::cout << "Other" << "\n";
        }
        // 检查是否有读权限
        fs::perms perms = status.permissions();
        if ((perms & fs::perms::owner_read) != fs::perms::none) {
            std::cout << "has owner read permission" << "\n";
        }
        if ((perms & fs::perms::owner_write) != fs::perms::none) {
            std::cout << "has owner write permission" << "\n";
        }
        if ((perms & fs::perms::owner_exec) != fs::perms::none) {
            std::cout << "has owner exec permission" << "\n";
        }
    }
    // 3.2.1 std::filesystem::symlink_status
    // std::filesystem::symlink_status 是 C++17 <filesystem> 库中的一个自由函数（非成员函数），与 std::filesystem::status
    // 函数非常相似，但它们之间存在一个关键的区别：对符号链接的处理方式。

    // symlink_status 是一个重载函数，通常有两种形式：

    // 形式1：接受路径

    // std::filesystem::file_status symlink_status( const std::filesystem::path& p );
    // std::filesystem::file_status symlink_status( const std::filesystem::path& p, std::error_code& ec ) noexcept;
    // 参数:

    // p: 要检查的文件系统路径（可以是符号链接）。

    // ec: (可选) 错误码。如果提供此参数，函数在出错时不会抛出异常，而是将错误码写入 ec。

    // 如果提供的路径 p 不存在，或者发生权限错误：

    // 不带 error_code 的版本: 抛出 std::filesystem::filesystem_error 异常。
    // 带 error_code 的版本: 不抛出异常。
    // 将 ec 设置为相应的错误代码。
    // 返回一个特殊的 file_status 对象，其 type() 为 file_type::none。
    // 返回值: 一个 std::filesystem::file_status 对象。

    // 形式2：接受目录

    // std::filesystem::file_status symlink_status( const std::filesystem::directory_entry& d );
    // std::filesystem::file_status symlink_status( const std::filesystem::directory_entry& d, std::error_code& ec ) noexcept;
    // directory_entry 类本身缓存了状态信息，调用此方法会获取或刷新该目录项的符号链接状态。
    // 下面是 symlink_status 的示例代码：
    std::string file_type_to_string(const fs::file_type &type);
    bool check_is_symlink(const fs::path &p) {
        std::error_code ec;
        // 使用symlink_status获取链接本身的状态
        fs::file_status st = fs::symlink_status(p, ec);
        if (ec) {
            std::cerr << "路径无效或者其他错误" << ec.message() << "\n";
            // 如果路径无效或者其他错误
            return false;
        }
        // 检查类型是否为symlink
        fs::file_type t = st.type();
        std::cout << "st.type()" << file_type_to_string(t) << "\n";
        return t == fs::file_type::symlink;
    }
    std::string file_type_to_string(const fs::file_type &type) {
        switch (type) {
        case fs::file_type::none:
            return "未获取状态";
        case fs::file_type::not_found:
            return "文件不存在";
        case fs::file_type::regular:
            return "普通文件";
        case fs::file_type::directory:
            return "目录";
        case fs::file_type::symlink:
            return "符号链接";
        case fs::file_type::block:
            return "块设备";
        case fs::file_type::character:
            return "字符设备";
        case fs::file_type::fifo:
            return "管道";
        case fs::file_type::socket:
            return "套接字";
        case fs::file_type::unknown:
        default:
            return "未知类型";
        }
    }
    void test2() {
        fs::path link_path{ R"("C:\install\cmake.lnk")" };
        // 2. 调用新的函数名
        if (check_is_symlink(link_path)) {
            std::cout << link_path << " 是一个符号链接。" << std::endl;

            std::error_code ec;
            fs::path target = fs::read_symlink(link_path, ec);
            if (!ec) {
                std::cout << "它指向: " << target << std::endl;

                // status() 会自动跟随符号链接，获取最终目标的状态
                fs::file_status target_status = fs::status(link_path);
                std::cout << "目标文件类型是: " << file_type_to_string(target_status.type()) << std::endl;
            } else {
                std::cerr << "读取目标失败: " << ec.message() << std::endl;
            }
        } else {
            std::cout << "不是一个符号链接 (或路径不可访问)。" << std::endl;
        }
    }
    // 3.2.2 std::filesystem::status_known
    // std::filesystem::status_known(st) 是 C++17 引入的标准库 <filesystem> 中的一个辅助函数。它用于检查给定的 file_status
    // 对象是否包含已知的文件属性信息。函数原型如下： namespace std::filesystem
    // {
    //     bool status_known( file_status s ) noexcept;
    // }
    // 参数：
    // s：一个 std::filesystem::file_status 类型的对象，通常由 status()、symlink_status() 或其他相关函数返回。
    // 返回值：
    // 如果 s 包含已知的文件信息，则返回 true。
    // 如果 s 不包含任何已知信息，则返回 false。
    // 下面的示例展示了不同情况下 status_known 的返回值：
    void test4() {
        // 1. 默认构造的 file_status，此时状态未知
        fs::file_status default_status;
        std::cout << "Default status known? " << fs::status_known(default_status) << std::endl; // false (type is none)

        // 2. 获取现有文件的状态, logfile.txt 是一个存在的文件
        fs::file_status file_status = fs::status(".clangd");
        std::cout << "Existing file status known? " << fs::status_known(file_status) << std::endl; // true

        // 3. 获取不存在文件的状态
        fs::file_status non_exist_status = fs::status("non_existent_file_xyz.txt");
        std::cout << "Non-existent file status known? " << fs::status_known(non_exist_status) << std::endl; // true

        // 4. 模拟特殊状态
        fs::file_status unknown_status1(fs::file_type::unknown);
        std::cout << "Unknown status known? " << fs::status_known(unknown_status1) << std::endl; // true
        fs::file_status unknown_status2(fs::file_type::none);
        std::cout << "none status known? " << fs::status_known(unknown_status2) << std::endl; // false
        fs::file_status unknown_status3(fs::file_type::not_found);
        std::cout << "not_found status known? " << fs::status_known(unknown_status3) << std::endl; // true
    }
} // namespace status_ns
int main() {
    std::cout << "------------\n";
    path_ns::test1();
    std::cout << "------------\n";
    path_ns::test2();
    std::cout << "------------\n";
    path_ns::test3();
    std::cout << "------------\n";
    path_ns::test4();
    std::cout << "------------\n";
    status_ns::test1();
    std::cout << "------------\n";
    status_ns::test2();
    std::cout << "------------\n";
    status_ns::test4();
}