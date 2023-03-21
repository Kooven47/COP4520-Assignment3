#ifndef CONCURRENTLINKEDLIST_HPP
#define CONCURRENTLINKEDLIST_HPP

#include <mutex>
#include <iostream>

class Node
{
public:
    int value;
    Node* next;
    std::mutex mutex;

    Node(int value)
    {
        this->value = value;
        this->next = nullptr;
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
    int size;
    std::mutex mutex;

public:
    ConcurrentLinkedList()
    {
        this->head = nullptr;
        this->size = 0;
    }

    ~ConcurrentLinkedList()
    {
        Node* current = head;
        while (current != nullptr)
        {
            Node* next = current->next;
            delete current;
            current = next;
        };
    }

    // Insert to linked list while keeping nodes in ascending order
    void add(int value)
    {
        mutex.lock();

        Node* node = new Node(value);

        if (head == nullptr)
        {
            head = node;
        }
        else if (head->value >= node->value)
        {
            node->next = head;
            head = node;
        }
        else
        {
            // Iterate until appropriate value
            Node* current = head;
            while (current->next != nullptr && current->next->value < node->value)
            {
                current = current->next;
            }

            // Point new node's next to next node 
            node->next = current->next;
            // Point previous node's next to new node
            current->next = node;
        }
        
        size++;
        mutex.unlock();
    }

    // void remove(int value);

    int removeHead()
    {
        mutex.lock();

        if (head == nullptr)
        {
            mutex.unlock();
            return -1;
        }

        // Get head node and its value
        Node* node = head;
        int value = node->value;

        // Move head over
        head = head->next;

        // Delete head
        delete node;
        size--;

        mutex.unlock();

        return value;
    }

    bool contains(int value)
    {
        mutex.lock();

        // Iterate until value is found (if it exists)
        Node* current = head;
        while (current != nullptr)
        {
            if (current->value == value)
            {
                mutex.unlock();
                return true;
            }
            current = current->next;
        }

        mutex.unlock();
        return false;
    }

    int getSize()
    {
        return size;
    }

    bool isEmpty()
    {
        return size == 0;
    }

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