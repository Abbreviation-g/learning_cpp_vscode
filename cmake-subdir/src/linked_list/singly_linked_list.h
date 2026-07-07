#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <cstddef>
#include <vector>

namespace linked_list {

class SinglyLinkedList {
public:
    SinglyLinkedList();
    ~SinglyLinkedList();

    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    void push_front(int value);
    void push_back(int value);
    bool remove_first(int value);
    bool contains(int value) const;
    void reverse();
    void clear();

    std::size_t size() const;
    bool empty() const;
    std::vector<int> to_vector() const;

private:
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
