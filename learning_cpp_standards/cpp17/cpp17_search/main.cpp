// https://subingwen.cn/cpp/search/
// 搜索器
#include <cstddef>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <functional>
#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>
#include <filesystem>
// 1. 搜索器概述
// 在 C++17 之前，如果我们想在一个字符串中查找子串，通常使用 std::string::find。虽然它很简单，但有几个明显的局限性：

// 算法单一：C++ 标准库只提供了简单的朴素算法（逐个字符比较）。
// 无法指定算法：如果你想用更高效的算法（如 KMP 或 Boyer-Moore），必须自己实现或寻找第三方库。
// 无法用于容器：find 是 std::string 的成员函数，不能直接用于 std::vector<char> 或其他容器。
// 接口不统一：字符串查找和泛型算法（如 std::search）的接口不一致。
// C++17 引入了搜索器概念。它允许我们把 搜索算法 和 搜索操作 分离开来。现在，标准库里直接提供了Boyer-Moore 和 Boyer-Moore-Horspool 这两个高性能算法！

// C++17 在 <functional> 头文件里引入了三种搜索器，每种都基于不同的算法：

// std::default_searcher：
// 算法：就是标准库原来用的那种朴素算法。
// 用途：默认选项，短字符串用它足够了。
// std::boyer_moore_searcher：
// 算法：Boyer-Moore 算法。工业界最强悍的字符串搜索算法之一。
// 特点：对于短模式不友好，预处理慢，但长文本搜索极快。
// std::boyer_moore_horspool_searcher：
// 算法：Boyer-Moore 的简化版（BM-Horspool）。
// 特点：预处理比标准的 BM 快，通常性能也非常好，实际应用中往往比标准 BM 更受欢迎。
// 这些搜索器是可调用对象（类重载了 operator()），可以传递给 std::search 算法使用。使用时需要包含头文件：

// #include <functional> // 搜索器定义在这里
// #include <algorithm>  // std::search 定义在这里
// std::search函数原型：

// template< class ForwardIt, class Searcher >
// ForwardIt search(ForwardIt first, ForwardIt last, const Searcher& searcher);
// first, last：要搜索的原始字符串的起始位置和结束位置对应的迭代器，区间为左闭右开
// searcher：指定的搜索器
// 注意：std::search 是 C++17 之前就有的，但在 C++17 中，它增加了一个重载版本，专门接收搜索器对象。

// 搜索器类构造函数如下：

// default_searcher(ForwardIt pat_first,
//                   ForwardIt pat_last,
//                   BinaryPredicate pred = BinaryPredicate());

// boyer_moore_searcher(RandomIt1 pat_first,
//                       RandomIt1 pat_last,
//                       Hash hf = Hash(),
//                       BinaryPredicate pred = BinaryPredicate());

// boyer_moore_horspool_searcher(RandomIt1 pat_first,
//                                RandomIt1 pat_last,
//                                Hash hf = Hash(),
//                                BinaryPredicate pred = BinaryPredicate());
// pat_first, pat_last：要搜索的子字符串的起始位置和结束位置对应的迭代器，区间为左闭右开
// hf：一个可调用对象，用于对字符串元素进行哈希
// pred：一个可调用对象，用于确定相等性
// 让我们通过一个具体例子来展示搜索器的使用：
namespace search_ns1 {
    void test1() {
        // 1. 准备数据
        std::string hay = "This is a long string containing the substring 'algorithm' at the end.";
        std::string needle = "algorithm";

        // 2. 创建搜索器
        std::boyer_moore_searcher searcher(needle.begin(), needle.end());

        // 3. 执行搜索
        auto it = std::search(hay.begin(), hay.end(), searcher);

        // 4. 判断结果
        if (it != hay.end()) {
            std::cout << "found. " << std::distance(hay.begin(), it) << std::endl;
        } else {
            std::cout << "没找到" << std::endl;
        }
    }
} // namespace search_ns1
namespace search_ns2 {
    class MultiPatternSearcher {
    public:
        MultiPatternSearcher(const std::vector<std::string> &patterns) : m_patterns(patterns) {
        }

        // 使用 Boyer-Moore 搜索所有模式
        std::vector<std::pair<std::string, size_t>> search_in_file(const std::string &filename) {
            std::vector<std::pair<std::string, size_t>> results;
            std::ifstream file(filename);

            if (!file) {
                throw std::runtime_error{ "connot open file: " + filename };
            }
            // 读取整个文件
            std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
            // 对每个模式进行搜索
            for (const auto &pattern : m_patterns) {
                auto search = std::boyer_moore_searcher(pattern.begin(), pattern.end());
                auto it = content.begin();
                while (it != content.end()) {
                    auto result = std::search(it, content.end(), search);
                    if (result != content.end()) {
                        size_t pos = std::distance(content.begin(), result);
                        results.emplace_back(pattern, pos);
                        it = result + 1; // 继续搜索
                    } else {
                        break;
                    }
                }
            }
            return results;
        }

    private:
        std::vector<std::string> m_patterns;
    };

    std::string prepare_file() {
        namespace fs = std::filesystem;
        fs::path folder{ "data" };
        fs::path file_path{ folder / "logfile.txt" };
        std::ofstream file{ file_path };

        std::string content =
            R"(2026-05-14 11:16:20.726 [main] INFO  c.s.c.platform.starter.StarterApplication - Starting StarterApplication using Java 21.0.2 with PID 25660 (C:\source\cloud-platform-backend-v3\platform-starter-service\target\classes started by Enjing.Guo in C:\source\cloud-platform-backend-v3)
2026-05-14 11:16:20.729 [main] ERROR  c.s.c.platform.starter.StarterApplication - No active profile set, falling back to 1 default profile: "default"
2026-05-14 11:16:20.840 [main] WARNING  c.a.cloud.nacos.configdata.NacosConfigDataLoader - [Nacos Config] Load config[dataId=platform-service.yaml, group=DEFAULT_GROUP] success
2026-05-14 11:16:24.697 [main] FATAL  o.s.d.r.config.RepositoryConfigurationDelegate - Multiple Spring Data modules found, entering strict repository configuration mode
2026-05-14 11:16:24.709 [main] CRITICAL  o.s.d.r.config.RepositoryConfigurationDelegate - Bootstrapping Spring Data Redis repositories in DEFAULT mode.
2026-05-14 11:16:24.805 [main] INFO  o.s.d.r.config.RepositoryConfigurationDelegate - Finished Spring Data repository scanning in 45 ms. Found 0 Redis repository interfaces.
2026-05-14 11:16:25.503 [main] INFO  o.springframework.cloud.context.scope.GenericScope - BeanFactory id=c7d9be8d-cee5-32ff-82e7-d69a2e0a7c00
2026-05-14 11:16:27.412 [main] INFO  o.s.boot.web.embedded.tomcat.TomcatWebServer - Tomcat initialized with port 30002 (http)
2026-05-14 11:16:27.448 [main] INFO  org.apache.coyote.http11.Http11NioProtocol - Initializing ProtocolHandler ["http-nio-30002"]
2026-05-14 11:16:27.465 [main] INFO  org.apache.catalina.core.StandardService - Starting service [Tomcat]
2026-05-14 11:16:27.466 [main] INFO  org.apache.catalina.core.StandardEngine - Starting Servlet engine: [Apache Tomcat/10.1.28]
2026-05-14 11:16:27.774 [main] FATAL  o.a.c.core.ContainerBase.[Tomcat].[localhost].[/] - Initializing Spring embedded WebApplicationContext
2026-05-14 11:16:27.777 [main] INFO  o.s.b.w.s.c.ServletWebServerApplicationContext - Root WebApplicationContext: initialization completed in 6922 ms
2026-05-14 11:16:27.983 [main] INFO  c.s.c.platform.starter.config.TraceHttpFilter - TraceFilter init
2026-05-14 11:16:30.889 [main] INFO  o.a.r.s.support.DefaultRocketMQListenerContainer - running container: DefaultRocketMQListenerContainer{consumerGroup='cloud-platform-v3-consumer', namespace='', nameServer='127.0.0.1:9876', topic='opt_log_topic', consumeMode=CONCURRENTLY, selectorType=TAG, selectorExpression='*', messageModel=CLUSTERING', tlsEnable=false, instanceName=DEFAULT}
2026-05-14 11:16:30.890 [main] INFO  o.a.r.s.s.RocketMQMessageListenerContainerRegistrar - Register the listener to container, listenerBeanName:optLogConsumer, containerBeanName:org.apache.rocketmq.spring.support.DefaultRocketMQListenerContainer_1
2026-05-14 11:16:30.913 [main] WARNING  o.a.r.s.autoconfigure.RocketMQAutoConfiguration - a producer (cloud-platform-v3-producer) init on namesrv 127.0.0.1:9876
2026-05-14 11:16:33.016 [main] INFO  n.d.b.g.c.a.GrpcClientAutoConfiguration - Detected grpc-netty-shaded: Creating ShadedNettyChannelFactory + InProcessChannelFactory
2026-05-14 11:16:34.188 [main] INFO  c.a.c.nacos.annotation.NacosAnnotationProcessor - [Nacos Config] Listening config for annotation: dataId=springboot3x.properties, group=DEFAULT_GROUP
2026-05-14 11:16:34.231 [main] ERROR  c.a.c.nacos.annotation.NacosAnnotationProcessor - [Nacos Config] register myRateConfigService#method#rate(String) listener on com.streamcomputing.cloudplatform.account.service.MyRateConfigService@11f752d1 
2026-05-14 11:16:34.252 [main] CRITICAL  c.s.cloudplatform.platform.config.DiskTypeConfig - 磁盘类型配置加载成功，共配置 3 种磁盘类型：hdd_sata(hdd_sata), ssd_sata(ssd_sata), ssd_nvme(ssd_nvme)
2026-05-14 11:16:36.523 [main] INFO  n.d.b.g.s.a.GrpcServerFactoryAutoConfiguration - Detected grpc-netty-shaded: Creating ShadedNettyGrpcServerFactory
2026-05-14 11:16:37.022 [main] INFO  org.apache.coyote.http11.Http11NioProtocol - Starting ProtocolHandler ["http-nio-30002"]
2026-05-14 11:16:37.054 [main] INFO  o.s.boot.web.embedded.tomcat.TomcatWebServer - Tomcat started on port 30002 (http) with context path ''
2026-05-14 11:16:37.130 [main] CRITICAL  c.a.n.p.auth.spi.client.ClientAuthPluginManager - [ClientAuthPluginManager] Load ClientAuthService com.alibaba.nacos.client.auth.impl.NacosClientAuthServiceImpl success.
2026-05-14 11:16:37.131 [main] INFO  c.a.n.p.auth.spi.client.ClientAuthPluginManager - [ClientAuthPluginManager] Load ClientAuthService com.alibaba.nacos.client.auth.ram.RamClientAuthServiceImpl success.
2026-05-14 11:16:37.245 [main] INFO  c.a.cloud.nacos.registry.NacosServiceRegistry - nacos registry, DEFAULT_GROUP platform-service 172.24.240.1:30002 register finished
2026-05-14 11:16:37.264 [main] INFO  n.d.b.g.s.serverfactory.AbstractGrpcServerFactory - Registered gRPC service: grpc.health.v1.Health, bean: grpcHealthService, class: io.grpc.protobuf.services.HealthServiceImpl
2026-05-14 11:16:37.265 [main] INFO  n.d.b.g.s.serverfactory.AbstractGrpcServerFactory - Registered gRPC service: grpc.reflection.v1alpha.ServerReflection, bean: protoReflectionService, class: io.grpc.protobuf.services.ProtoReflectionService
2026-05-14 11:16:37.547 [main] CRITICAL  n.d.b.g.server.serverfactory.GrpcServerLifecycle - gRPC Server started, listening on address: *, port: 9090)";
        file << content;
        file.close();

        return file_path.string();
    }
    void test() {
        std::string file_path = prepare_file();
        // 定义要搜索的多个关键词
        std::vector<std::string> keywords = { "ERROR", "WARNING", "FATAL", "CRITICAL" };

        MultiPatternSearcher searcher{ keywords };
        try {
            auto results = searcher.search_in_file(file_path);
            std::cout << "Found " << results.size() << " occurrences:\n";
            for (const auto &[pattern, position] : results) {
                std::cout << "  '" << pattern << "' at position " << position << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
} // namespace search_ns2
namespace search_ns3 {
    struct Person {
    public:
        int id;
        std::string name;
        int age;
        bool operator==(const Person &other) const {
            return id == other.id && name == other.name && age == other.age;
        }
    };
    void test() {
        // 创建人员数据库
        std::vector<Person> database = { { 1, "Alice", 30 }, { 2, "Bob", 25 }, { 3, "Charlie", 35 }, { 4, "David", 28 }, { 5, "Eve", 32 } };
        // 要搜索的人员序列
        std::vector<Person> search_pattern = { { 2, "Bob", 25 }, { 3, "Charlie", 35 } };

        auto searcher = std::default_searcher(search_pattern.begin(), search_pattern.end());
        auto result = std::search(database.begin(), database.end(), searcher);
        if (result != database.end()) {
            std::cout << "Pattern found at position: " << std::distance(database.begin(), result) << std::endl;

            // 显示找到的序列
            for (auto it = result; it != result + search_pattern.size(); ++it) {
                std::cout << "  ID: " << it->id << ", Name: " << it->name << ", Age: " << it->age << std::endl;
            }
        } else {
            std::cout << "Pattern not found" << std::endl;
        }
    }
    void test2() {
        // 创建人员数据库
        std::vector<Person> database = { { 3, "Charlie", 35 }, { 1, "Alice", 30 }, { 2, "Bob", 25 }, { 2, "Bob", 25 }, { 3, "Charlie", 35 }, { 4, "David", 28 },
            { 5, "Eve", 32 } };
        // 要搜索的人员序列
        std::vector<Person> search_pattern = { { 2, "Bob", 25 }, { 3, "Charlie", 35 } };

        for (const auto &person : search_pattern) {
            auto searcher = std::default_searcher(&person, &person + 1);
            auto search_begin = database.begin();
            bool found = false;
            while (search_begin != database.end()) {
                auto result = std::search(search_begin, database.end(), searcher);
                if (result == database.end()) {
                    break;
                }

                found = true;
                std::cout << "Found " << person.name << " at position: " << std::distance(database.begin(), result) << std::endl;
                std::cout << "  ID: " << result->id << ", Name: " << result->name << ", Age: " << result->age << std::endl;
                search_begin = result + 1;
            }
            if (!found) {
                std::cout << person.name << " not found" << std::endl;
            }
        }
    }
} // namespace search_ns3
int main() {
    search_ns1::test1();
    search_ns2::test();
    search_ns3::test();
    search_ns3::test2();
}