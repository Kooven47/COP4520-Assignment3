#ifndef CONCURRENTLINKEDLIST_HPP
#define CONCURRENTLINKEDLIST_HPP

#include <mutex>
#include <iostream>

class Node
{
public:
    int value;
    Node* next;
    Node* prev;
    std::mutex mutex;

    Node(int value)
    {
        this->value = value;
        this->next = nullptr;
        this->prev = nullptr;
    }
    ~Node() {};

    friend std::ostream& operator<<(std::ostream& os, Node* const& node)
    {
        os << (node == nullptr ? "(null)" : std::to_string(node->value));
        return os;
    }
};

class ConcurrentLinkedList
{
private:
    Node* head;
    Node* tail;
    int size;
    std::mutex mutex;

public:
    ConcurrentLinkedList()
    {
        this->head = nullptr;
        this->tail = nullptr;
        this->size = 0;
    }
    ~ConcurrentLinkedList();

    void add(int value);
    // void remove(int value);
    int removeHead();
    bool contains(int value);
    int getSize();
    bool isEmpty();

    friend std::ostream& operator<<(std::ostream& os, ConcurrentLinkedList* const& list)
    {
        if (list == nullptr)
        {
            os << "(null)";
            return os;
        }

        Node* current = list->head;
        while (current != nullptr)
        {
            os << "[" << current->value << "]->";
            current = current->next;
        }
        os << "(null)";

        return os;
    }
};

#endif