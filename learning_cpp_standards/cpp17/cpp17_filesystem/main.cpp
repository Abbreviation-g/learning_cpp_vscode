// https://subingwen.cn/cpp/filesystem2/
// 文件系统 - 文件与目录的 CRUD

#include <sys/stat.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace create_ns {
    namespace fs = std::filesystem;
    // 创建操作主要分为：创建空目录、创建目录树（多级目录）、以及创建空文件。

    // 创建目录
    // bool create_directory(const std::filesystem::path& p);
    // 功能：尝试创建一个由路径 p 指定的目录，如果父目录不存在，则会抛出异常。
    // 返回值：
    // 如果目录创建成功，返回 true。
    // 如果创建失败（例如父目录不存在，或者路径指向一个文件而非目录），返回 false。或者目录已经存在，返回 false
    // 特点：它只能创建单级目录。父目录必须已经存在，否则失败。
    void test1() {
        // 创建单级目录
        bool created = fs::create_directory("data");
        if (created) {
            std::cout << "创建成功" << std::endl;
            bool removed = fs::remove("data");
            std::cout << (removed ? "删除成功" : "删除失败") << "\n";
        }
        // 再次尝试创建，返回 true (表示目录已存在)
        // bool exists = fs::create_directory("data");
        // std::cout << "exists : " << exists << std::endl;
    }

    // 创建多级目录
    // bool create_directories(const std::filesystem::path& p);
    // 功能：创建路径 p 指定的目录，如果路径中包含不存在的父目录，它会递归地自动创建所有缺失的父目录。
    // 返回值：
    // 如果目录被创建（包括创建的父目录），返回 true。
    // 如果目录已经存在，返回 false。
    // 注意：如果路径 p 已经是一个存在的目录，它什么也不做并返回 false。
    void test2() {
        fs::path p = fs::path{ "a/b/c" };
        bool created = fs::create_directories(p);
        if (created) {
            std::cout << "创建成功" << std::endl;
            bool removed = fs::remove_all(p);
            std::cout << (removed ? "删除成功" : "删除失败") << "\n";
        } else {
            std::cout << "创建失败" << std::endl;
        }
    }

    // 创建空文件
    // std::filesystem 没有直接提供 create_file 函数。标准做法是利用传统的 std::ofstream 来创建一个空文件。示例代码如下：
    void test3() {
        std::filesystem::path folder{ "data" };
        fs::create_directory(folder);
        std::cout << "创建成功" << std::endl;

        // 方法1 如果文件不存在，这会创建它；如果存在，这会截断它（清空内容）
        std::ofstream{ "data/config.ini" }.close();

        // 或者使用 C++17 的 fopen 风格
        std::FILE *fp = std::fopen("data/empty.txt", "w");
        if (fp != nullptr) {
            std::fclose(fp);
        }
        bool removed = fs::remove_all(folder);
        std::cout << (removed ? "删除成功" : "删除失败") << "\n";
    }
} // namespace create_ns
namespace copy_ns {
    // 1.2 复制
    // C++17 提供了非常灵活的复制功能，支持递归复制目录、控制覆盖行为以及仅复制元数据。

    // 拷贝文件

    // void copy(const std::filesystem::path& from, const std::filesystem::path& to);
    // void copy(const std::filesystem::path& from, const std::filesystem::path& to, std::error_code& ec) noexcept;
    // void copy(const std::filesystem::path& from, const std::filesystem::path& to, copy_options options);
    // 功能：将 from 复制到 to。

    // 参数：

    // 如果 from 是文件，则复制内容。
    // 如果 from 是目录，默认情况下不会递归复制内容（见下文 copy_options）。
    // copy_options 枚举：
    // none：默认行为。
    // skip_existing：如果目标已存在，跳过复制（不覆盖）。
    // overwrite_existing：如果目标已存在，覆盖它。
    // update_existing：如果目标已存在，仅当源文件比目标文件新时才覆盖。
    // recursive：关键选项。递归复制子目录及其内容。
    // copy_symlinks：复制符号链接本身，而不是链接指向的文件。
    // directories_only：仅复制目录，不复制文件。
    // 异常：如果目标已存在，默认会抛出异常（除非使用特定的 copy_options 或者错误由 error_code 捕获）。

    // 示例：

    namespace fs = std::filesystem;
    void test1() {
        fs::path folder{ "data" };
        fs::path s1 = folder / "file.txt";
        fs::path s2 = folder / "project";
        fs::path t1 = folder / "file_backup.txt";
        fs::path t2 = folder / "project_backup";
        // 将 file.txt 复制为 file_backup.txt，如果文件已存在则覆盖
        fs::copy(s1, t1, fs::copy_options::overwrite_existing);

        // 递归复制整个 "project" 文件夹到 "project_backup"
        fs::copy(s2, t2, fs::copy_options::recursive);

        fs::remove_all(t1);
        fs::remove_all(t2);
    }

    // 复制文件元数据
    // 如果我们只想复制文件的权限、修改时间等属性，而不想复制文件内容（或者内容已经复制完了，只想同步属性）可以使用copy_file函数：
    // void copy_file(const std::filesystem::path& from, const std::filesystem::path& to, copy_options options = copy_options::none);
    // std::filesystem 对复制文件内容和复制文件属性在概念上是区分的，但 copy_file
    // 函数通常两者都做。如果你想只复制属性，通常需要手动读取并写入时间戳和权限，或者使用 copy 加上 skip_existing（但这不一定适用于属性同步，最标准的方式是用
    // permissions 和 last_write_time 函数单独设置）。
    void initilaze_project(const std::string &project_name) {
        fs::path data{ "data" };
        fs::path root{ data / project_name };
        fs::path src{ root / "src" };
        fs::path config_file{ root / "config.json" };
        try {
            // 1. 创建多级目录
            if (fs::create_directories(src)) {
                std::cout << "Created directories: " << src << std::endl;
            }
            // 2. 创建空文件(并写入内容)
            std::ofstream{ src / "main.cpp" } << "#include <iostream>\nint main() {}";
            std::cout << "Created source file." << std::endl;
            std::ofstream{ config_file } << "{ \"version\": 1.0 }";
            std::cout << "Created config file." << std::endl;
            // 3. 复制文件
            fs::path backup_dir{ root / "backup" };
            fs::create_directory(backup_dir);
            // 将config.json复制到backup/ 目录，如果存在则覆盖
            fs::copy_file(config_file, backup_dir / "config.json.bak", fs::copy_options::overwrite_existing);
            std::cout << "Created backup." << std::endl;
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
    void test2() {
        initilaze_project("copy_prj");
    }
} // namespace copy_ns
namespace remove_ns {
    namespace fs = std::filesystem;
    void test1() {
        fs::path p{ "data/file.txt" };
        try {
            bool deleted = fs::remove(p);
            if (deleted) {
                std::cout << "File deleted successfully.\n";
            } else {
                std::cout << "File does not exist or cannot be deleted.\n";
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    void test2() {
        // 虽然 remove 可以删除空目录，但在语义上，有时我们需要明确仅当目录为空时才删除。
        // 严格来说，C++17 没有单独提供名为 remove_empty_directory 的函数。
        // std::remove 就是删除空目录的标准函数。
        // remove(p) 在 p 是目录时，只会在目录为空的情况下成功删除。
        // 并且remove 不会递归，如果目录里有东西，它会拒绝删除（失败）。
        fs::path dir_path{ "data" };
        if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
            // 尝试删除
            // 如果目录里有文件，这个调用会返回 false 或抛出异常
            try {
                if (fs::remove(dir_path)) {
                    std::cout << "Empty directory removed.\n";
                } else {
                    std::cout << "Directory is not empty.\n";
                }
            } catch (const fs::filesystem_error &e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        }
    }

    // 2.2 删除所有内容
    // 删除所有内容这是实际开发中最常用的删除函数，因为它足够智能。函数为remove_all:
    // uintmax_t remove_all(const std::filesystem::path& p);
    // uintmax_t remove_all(const std::filesystem::path& p, std::error_code& ec) noexcept;
    // 功能：递归地删除路径 p 指向的内容（包括所有子目录和文件）。
    // 行为：
    // 如果 p 是一个文件，等同于 remove。
    // 如果 p 是一个目录，它会删除该目录下的所有内容（递归），最后删除目录本身。
    // 如果 p 是一个符号链接，删除符号链接本身（不删除指向的目标）。
    // 返回值：
    // 返回被删除的文件和目录的总数量。
    // 如果 p 不存在，返回 0。
    // 性能注意：remove_all 需要遍历目录树，对于包含大量文件的深层目录，耗时可能较长。
    bool safe_remove(const fs::path &p, bool recursive = true) {
        std::error_code ec;
        if (recursive) {
            // remove_all 会递归删除，并且如果 p 不存在，返回 0，不抛异常
            auto count = fs::remove_all(p, ec);
            if (ec) {
                std::cerr << "Error deleting " << p << ": " << ec.message() << std::endl;
                return false;
            }
            std::cout << "Removed " << count << " items at " << p << std::endl;
        } else {
            // 仅尝试删除单个文件或空目录
            bool success = fs::remove(p, ec);
            if (ec) {
                std::cerr << "Error deleting " << p << ": " << ec.message() << std::endl;
                return false;
            }
            if (!success) {
                std::cout << "Path " << p << " does not exist (not an error)." << std::endl;
            } else {
                std::cout << "Removed " << p << std::endl;
            }
        }
        return true;
    }
    void test3() {
        // 创建测试结构
        fs::create_directories("data/subdir");
        std::ofstream("data/file.txt") << "content";

        std::cout << " Unsafe remove failed (expected). Using remove_all instead.\n";
        safe_remove("data", true);
    }
} // namespace remove_ns
namespace directory_iterator_ns {
    namespace fs = std::filesystem;
    //     3.1 遍历目录
    // 目录遍历这是 std::filesystem 最强大的功能之一，替代了老旧的 POSIX opendir / Windows FindFirstFile API。

    std::size_t count_entries(const fs::path &path) {
        std::size_t count = 0;
        for (const auto &entry : fs::directory_iterator{ path }) {
            (void)entry;
            ++count;
        }
        return count;
    }

    // 3.1.1 单层遍历
    // 单层目录遍历使用的类为std::filesystem::directory_iterator
    void list_current_directory() {
        for (const auto &entry : fs::directory_iterator{ "." }) {
            // 检查是否为普通文件
            if (entry.is_regular_file()) {
                std::cout << "File: " << entry.path().filename() << " | Size: " << entry.file_size() << "\n";
            } else if (entry.is_directory()) {
                // 如果是目录，你可以选择只打印名称，或者计算目录内文件的总大小（需要递归）
                std::cout << "Dir : " << entry.path().filename() << " | Count: " << count_entries(entry.path()) << "\n";
            }
        }
    }
    void test1() {
        list_current_directory();
    }

    // 3.1.2 递归遍历
    // std::filesystem::recursive_directory_iterator 是 C++17 标准库 <filesystem> 中提供的一个迭代器类，用于递归地遍历目录树及其所有子目录的内容。

    // recursive_directory_iterator() noexcept;
    // explicit recursive_directory_iterator(const std::filesystem::path& p,
    //                                      std::filesystem::directory_options opts = directory_options::none);
    // explicit recursive_directory_iterator(const std::filesystem::path& p,
    //                                      std::filesystem::directory_options opts,
    //                                      std::error_code& ec) noexcept;
    // 默认构造函数：创建一个结束迭代器，通常用作遍历结束的判断条件。
    // 带路径的构造函数：
    // p: 要遍历的目录路径。
    // opts: 遍历选项（例如是否跳过符号链接、是否允许权限错误等）
    // 详细选项信息可查询在线文档。
    // ec: 错误码，用于非抛出异常的重载。
    // 在这个类中还为我们提供了一组用于控制递归层级的 API 函数：
    // 跳过特定子目录
    // void pop();
    // 如果当前迭代器指向的是一个目录，调用 pop() 会跳过该目录下的所有内容，直接返回到该目录的父目录，并继续遍历。
    // 注意：这个函数的行为实际上是结束对当前目录的遍历，常用于跳过特定子目录。
    // 使用场景：已经进入了一个目录（或者正在深度遍历中），这是想强制停止当前的层级，直接回溯到上一层。
    // 禁用递归进入
    // void disable_recursion_pending();
    // 这是一个非常实用的函数。当你遍历到一个目录时，如果调用此函数，迭代器在下一次递增时将不会进入该目录（尽管 directory_entry::is_directory() 返回
    // true），而是像对待普通文件一样跳过它。 使用场景：在遍历列表时，发现一个目录，想在后续遍历时忽略它（把它当作普通文件处理），无进入。 深度查询 int depth()
    // const; 返回当前迭代器在目录树中的深度。起始目录深度为 0，其子目录为 1，以此类推。
    void test2() {
        std::string path = ".";

        // 创建递归迭代器
        for (auto it = fs::recursive_directory_iterator(path); it != fs::recursive_directory_iterator(); ++it) {
            // 如果当前是一个目录且名字是 "skip_this_folder"
            if (it->is_directory() && it->path().filename() == "skip_this_folder") {
                // 禁止递归进入，或者在进入后调用 it.pop()
                it.disable_recursion_pending();
            }

            std::cout << it->path() << " (depth: " << it.depth() << ")\n";
        }
    }

    // 3.2 过滤器与查找
    // C++17 文件系统库本身不提供 find 或 filter 函数，但利用迭代器和 C++ 算法库可以轻松实现。
    // 示例：查找所有 .cpp 文件
    void find_cpp_files(const fs::path &root, std::vector<fs::path> &cpp_files) {
        auto begin = fs::recursive_directory_iterator(root);
        auto end = fs::recursive_directory_iterator();
        auto search_func = [](const fs::directory_entry &entry) {
            //  只要常规文件，且扩展名.cpp
            return entry.is_regular_file() && entry.path().extension() == ".cpp";
        };
        std::copy_if(begin, end, std::back_inserter(cpp_files), search_func);
    }
    void test3() {
        std::vector<fs::path> cpp_files;
        find_cpp_files(fs::path{ "." }, cpp_files);
        for (const auto &it : cpp_files) {
            std::cout << it << std::endl;
        }
    }
} // namespace directory_iterator_ns
namespace modify_ns {
    //     4. 文件修改
    // 在 std::filesystem 中，修改的操作主要体现为对文件系统对象的重命名、移动以及属性修改。

    // 4.1 重命名和移动
    // 在底层系统中，重命名和移动通常是同一个系统调用。只要源文件和目标文件在同一个文件系统（挂载点）上，移动操作仅仅是修改目录元数据，速度极快，且不涉及文件数据的物理拷贝。

    // bool rename(const std::filesystem::path& old_p, const std::filesystem::path& new_p);
    // bool rename(const std::filesystem::path& old_p, const std::filesystem::path& new_p, std::error_code& ec) noexcept;
    // 功能：
    // 将 old_p 重命名为 new_p。
    // 如果 new_p 路径中包含不同的目录名，则实现了移动效果。
    // 重要特性：
    // 覆盖行为：如果new_p已经存在，它的行为取决于编译器和操作系统。
    // 在大多数 POSIX 系统上，如果 new_p 是非空目录，会失败；如果是空目录或文件，通常原子性地替换。
    // 标准建议：如果 new_p 存在，结果是实现定义的。为了代码健壮性，强烈建议在移动前检查目标是否存在，或者确保期望覆盖。
    // 跨卷移动：如果源和目标在不同的磁盘分区（挂载点），单纯的 rename 可能会失败（错误码为 EXDEV）。标准的 rename 不执行跨文件系统的数据拷贝。
    namespace fs = std::filesystem;
    void rename_file() {
        fs::path old_name = "data/old.txt";
        fs::path new_name = "data/new.txt";

        std::error_code ec;

        // 检查旧文件是否存在
        if (fs::exists(old_name)) {
            fs::rename(old_name, new_name, ec);

            if (ec) {
                std::cerr << "Rename failed: " << ec.message() << std::endl;
            } else {
                std::cout << "Successfully renamed to " << new_name << std::endl;
            }
        }
    }
    // 示例：移动文件到新目录
    void move_file_to_folder() {
        fs::path src = "data/report.pdf";
        fs::path dest_folder = "data/archive/2023/";
        fs::path dest = dest_folder / "report.pdf"; // 使用 / 运算符拼接路径

        // 创建目标目录（如果不存在）
        fs::create_directories(dest_folder);

        std::error_code ec;
        fs::rename(src, dest, ec);

        if (ec) {
            // 这里的失败可能是跨磁盘移动失败
            std::cerr << "移动失败: " << ec.message() << std::endl;
        }
    }

    //     4.2 文件权限修改
    // 修改文件的读写执行权限是“改”的另一重要部分。使用的函数如下：

    // void permissions(const std::filesystem::path& p, perms prms, perm_options opts = perm_options::replace);
    // void permissions(const std::filesystem::path& p, perms prms, std::error_code& ec) noexcept;
    // perms：枚举类型，如 owner_read, group_write, all_exec 等。

    // perm_options（重要）：控制如何应用新权限，默认是replace（替换）。还可以使用：

    // add：在现有权限基础上添加（按位或）。
    // remove：在现有权限基础上移除（按位异或/取反）。
    // nofollow：如果是符号链接，不修改指向的目标，只修改链接本身（支持情况视系统而定）。
    // std::filesystem::perm_options 属性在线查询
    // 示例：将文件设置为只读
    void make_readonly(const fs::path &p) {
        // 使用 remove 选项移除写权限
        // owner_all = 0700, group_all = 0070, others_all = 0007
        // 我们要移除所有的 write 权限
        fs::permissions(p, fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write, fs::perm_options::remove);

        std::cout << p << " is now read-only.\n";
    }
    void call_make_readonly() {
        make_readonly(fs::path{ "data/new.txt" });
    }
} // namespace modify_ns
namespace clean_temp_ns {
    namespace fs = std::filesystem;
    bool mark_tmp_file(const fs::directory_entry &entry);
    void clean_temp(const fs::path &dir) {
        // 1. 检查路径有效性
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            std::cerr << "Path is invalid or not a directory.\n";
            return;
        }

        // 用于存储待删除的文件，避免在遍历过程中删除文件导致迭代器失效
        std::vector<fs::path> files_to_remove;
        try {
            auto options = fs::directory_options::skip_permission_denied;
            for (const auto &entry : fs::recursive_directory_iterator{ dir, options }) {
                bool is_tmp_file = mark_tmp_file(entry);
                if (is_tmp_file) {
                    files_to_remove.push_back(entry);
                }
            }
            // 4. 遍历结束后，统一执行删除操作
            for (const auto &p : files_to_remove) {
                std::error_code ec; // 使用 error_code 避免异常抛出
                bool removed = fs::remove(p, ec);
                if (removed) {
                    std::cout << "Successfully deleted: " << p << std::endl;
                } else {
                    std::cerr << "Failed to delete " << p << ". Reason: " << ec.message() << std::endl;
                }
            }
        } catch (const fs::filesystem_error &e) {
            // 捕获迭代器层面的严重错误
            std::cerr << "Filesystem critical error: " << e.what() << std::endl;
        }
    }
    bool mark_tmp_file(const fs::directory_entry &entry) {
        try {
            // 检查文件是否为临时文件
            if (entry.is_regular_file() && entry.path().extension() == ".tmp") {
                // 3. 安全地获取时间，确保时钟类型一致
                auto ftime = fs::last_write_time(entry);
                // 使用filesystem自带始终的now()
                auto now = fs::file_time_type::clock::now();

                // 计算时间差(60s)
                auto duration = now - ftime;
                if (duration > std::chrono::seconds(60)) {
                    std::cout << "Marking for deletion: " << entry.path() << std::endl;
                    return true;
                }
            }
        } catch (const fs::filesystem_error &e) {
            // 捕获单个文件处理时的错误（例如该文件刚好被其他进程占用无法读取元数据）
            std::cerr << "Skipping file (Error): " << e.what() << std::endl;
            return false;
        }
        return false;
    }
    void call_clean_temp() {
        clean_temp(fs::path{ "." });
    }
} // namespace clean_temp_ns
namespace array_ns {
    template <typename T, size_t size>
    class My_Array {
    public:
        explicit My_Array(const T *arr) {
            for (int i = 0; i < size; i++) {
                m_arr[i] = *(arr + i);
            }
        }
        // 数组引用参数
        explicit My_Array(const T (&arr)[size]) {
            for (size_t i = 0; i < size; i++) {
                m_arr[i] = arr[i];
            }
        }
        My_Array(const My_Array &other) {
            std::copy(other.begin(), other.end(), m_arr);
        }
        My_Array(My_Array &&other) noexcept {
            std::move(other.begin(), other.end(), m_arr);
        }
        My_Array &operator=(const My_Array &other) {
            if (this != &other) {
                for (size_t i = 0; i < size; ++i) {
                    m_arr[i] = other.m_arr[i];
                }
            }
            return *this;
        }
        My_Array &operator=(My_Array &&other) noexcept {
            if (this != &other) {
                for (size_t i = 0; i < size; ++i) {
                    m_arr[i] = std::move(other.m_arr[i]);
                }
            }
            return *this;
        }
        ~My_Array() = default;

        T *begin() {
            return m_arr;
        }
        T *end() {
            return m_arr + size;
        }
        const T *begin() const {
            return m_arr;
        }
        const T *end() const {
            return m_arr + size;
        }
        void swap(My_Array &other) noexcept {
            std::swap(m_arr, other.m_arr); // 数组可直接 swap（std::swap 有数组重载）
        }
        T &operator[](std::size_t i) {
            return m_arr[i];
        }
        const T &operator[](std::size_t i) const {
            return m_arr[i];
        }
        const T &at(std::size_t i) const {
            return m_arr[i];
        }
        constexpr std::size_t get_size() const noexcept {
            return size;
        }
        T *data() noexcept {
            return m_arr;
        }
        const T *data() const noexcept {
            return m_arr;
        }

        static void print_s(const My_Array<T, size> &arr) {
            for (auto i : arr) {
                std::cout << i << ", ";
            }
            std::cout << "\n";
        }

    private:
        T m_arr[size]{};
    };

    std::vector<int> create() {
        return std::vector<int>{ 1, 2 };
    }
    void test() {
        const int size = 5;
        int arr[size]{ 1, 3, 5, 7, 9 };
        My_Array<int, size> my_arr{ arr };
        My_Array<int, size>::print_s(my_arr); // 1, 3, 5, 7, 9,

        int *ptr = arr;
        My_Array<int, size> my_arr2{ ptr };
        My_Array<int, size>::print_s(my_arr2); // 1, 3, 5, 7, 9,

        My_Array<int, size> my_arr3{ my_arr };
        my_arr3 = my_arr2;
        my_arr3 = std::move(my_arr2);

        my_arr2.print_s(my_arr2); // 1, 3, 5, 7, 9,
        my_arr3.print_s(my_arr3); // 1, 3, 5, 7, 9,

        std::vector v = create();

        const char *strs[size] = { "x", "y", "z", "a", "b" };
        using mm = My_Array<const char *, size>;
        mm strs1{ strs };
        My_Array<const char *, size> strs2{ strs1 };
        strs2 = std::move(strs1);
        My_Array<const char *, size>::print_s(strs1); // x, y, z, a, b,
        My_Array<const char *, size>::print_s(strs2); // x, y, z, a, b,

        std::string strss[size] = { "x", "y", "z", "a", "b" };
        My_Array<std::string, size> strss1{ strss };
        My_Array<std::string, size> strss2{ strss1 };
        strss2 = std::move(strss1);
        My_Array<std::string, size>::print_s(strss1); // , , , , ,
        My_Array<std::string, size>::print_s(strss2); // x, y, z, a, b,
        My_Array<std::string, size> strss3{ std::move(strss2) };

        auto print_all = [](const auto &x) {
            for (const auto &i : x) {
                std::cout << i << ", ";
            }
            std::cout << "\n";
        };
        print_all(strss2);
    }
} // namespace array_ns
int main() {
    // std::cout << "--------------\n";
    // create_ns::test1();
    // std::cout << "--------------\n";
    // create_ns::test2();
    // std::cout << "--------------\n";
    // create_ns::test3();

    // std::cout << "--------------\n";
    // copy_ns::test1();
    // std::cout << "--------------\n";
    // copy_ns::test2();

    // std::cout << "--------------\n";
    // remove_ns::test1();
    // std::cout << "--------------\n";
    // remove_ns::test2();
    // std::cout << "--------------\n";
    // remove_ns::test3();
    // std::cout << "--------------\n";
    // directory_iterator_ns::test1();
    // std::cout << "--------------\n";
    // directory_iterator_ns::test2();
    // std::cout << "--------------\n";
    // directory_iterator_ns::test3();
    // std::cout << "--------------\n";
    // modify_ns::rename_file();
    // modify_ns::move_file_to_folder();
    // modify_ns::call_make_readonly();

    clean_temp_ns::call_clean_temp();

    std::cout << "--------------\n";
    array_ns::test();
}