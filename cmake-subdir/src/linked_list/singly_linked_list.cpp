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

    if (tail_ == nullptr) {
        tail_ = node;
    }

    ++size_;
}

void SinglyLinkedList::push_back(int value)
{
    Node* node = new Node(value);

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
    tail_ = head_;

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
    values.reserve(size_);

    for (Node* current = head_; current != nullptr; current = current->next) {
        values.push_back(current->value);
    }

    return values;
}

} // namespace linked_list
