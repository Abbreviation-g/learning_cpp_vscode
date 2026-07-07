#include "singly_linked_list.h"

namespace linked_list {

SinglyLinkedList::Node::Node(int node_value)
    : value(node_value)
    , next(nullptr)
{
}

SinglyLinkedList::SinglyLinkedList()
    : head_(nullptr)
    , tail_(nullptr)
    , size_(0)
{
}

SinglyLinkedList::~SinglyLinkedList()
{
    clear();
}

void SinglyLinkedList::push_front(int value)
{
    Node* node = new Node(value);
    node->next = head_;
    head_ = node;

    // 如果原链表为空，新节点同时也是尾节点。
    if (tail_ == nullptr) {
        tail_ = node;
    }

    ++size_;
}

void SinglyLinkedList::push_back(int value)
{
    Node* node = new Node(value);

    // 空链表插入第一个节点时，头尾指针都指向它。
    if (tail_ == nullptr) {
        head_ = node;
        tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
    }

    ++size_;
}

bool SinglyLinkedList::remove_first(int value)
{
    Node* previous = nullptr;
    Node* current = head_;

    // previous 跟随 current，用来在删除时重新连接链表。
    while (current != nullptr) {
        if (current->value == value) {
            if (previous == nullptr) {
                head_ = current->next;
            } else {
                previous->next = current->next;
            }

            if (tail_ == current) {
                tail_ = previous;
            }

            // 找到后立即释放节点并维护 size。
            delete current;
            --size_;
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

bool SinglyLinkedList::contains(int value) const
{
    for (Node* current = head_; current != nullptr; current = current->next) {
        if (current->value == value) {
            return true;
        }
    }
    return false;
}

void SinglyLinkedList::reverse()
{
    Node* previous = nullptr;
    Node* current = head_;

    // 反转后原头节点会变成尾节点。
    tail_ = head_;

    // 逐个反转 next 指针方向。
    while (current != nullptr) {
        Node* next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    }

    head_ = previous;
}

void SinglyLinkedList::clear()
{
    Node* current = head_;

    // 保存 next 后再释放 current，避免访问已经释放的节点。
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }

    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}

std::size_t SinglyLinkedList::size() const
{
    return size_;
}

bool SinglyLinkedList::empty() const
{
    return size_ == 0;
}

std::vector<int> SinglyLinkedList::to_vector() const
{
    std::vector<int> values;

    // 已知节点数量，提前预留空间减少 vector 扩容。
    values.reserve(size_);

    for (Node* current = head_; current != nullptr; current = current->next) {
        values.push_back(current->value);
    }

    return values;
}

} // namespace linked_list
