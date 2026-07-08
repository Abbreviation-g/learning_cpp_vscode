#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cstddef>
#include <vector>

namespace linked_list {

// 一个只存储 int 的单向链表示例类，用于演示链表的基础增删查改操作。
class SinglyLinkedList {
public:
    // 创建空链表。
    SinglyLinkedList();

    // 释放链表中的所有节点。
    ~SinglyLinkedList();

    // 链表拥有裸指针节点，先禁用拷贝，避免浅拷贝导致重复释放。
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    // 在链表头部插入一个节点。
    void push_front(int value);

    // 在链表尾部插入一个节点。
    void push_back(int value);

    // 删除第一个值等于 value 的节点；删除成功返回 true。
    bool remove_first(int value);

    // 判断链表中是否存在值等于 value 的节点。
    bool contains(int value) const;

    // 原地反转链表。
    void reverse();

    // 清空链表并释放所有节点。
    void clear();

    // 返回当前节点数量。
    std::size_t size() const;

    // 判断链表是否为空。
    bool empty() const;

    // 按链表顺序导出为 vector，便于测试和打印。
    std::vector<int> to_vector() const;

private:
    // 链表内部节点，next 指向下一个节点。
    struct Node {
        explicit Node(int node_value);

        int value;
        Node* next;
    };

    Node* head_;
    Node* tail_;
    std::size_t size_;
};

} // namespace linked_list

#endif
