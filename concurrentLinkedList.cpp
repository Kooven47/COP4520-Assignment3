#include "concurrentLinkedList.hpp"

ConcurrentLinkedList::~ConcurrentLinkedList()
{
    Node* current = head;
    while (current != nullptr)
    {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

// Insert a node while keeping the list sorted in ascending order
void ConcurrentLinkedList::add(int value)
{
    mutex.lock();

    Node* node = new Node(value);

    if (head == nullptr)
    {
        head = node;
        tail = node;
    }
    else if (head->value >= node->value)
    {
        node->next = head;
        head->prev = node;
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
        // Point next node's prev to new node
        if (current->next != nullptr)
        {
            current->next->prev = node;
        }
        // Make the tail the new node if it was last value inserted
        else
        {
            tail = node;
        }

        // Point previous node's next to new node
        current->next = node;
        // Point new node's prev to previous node
        node->prev = current;
    }
    
    size++;
    mutex.unlock();
}

int ConcurrentLinkedList::removeHead()
{
    mutex.lock();

    if (head == nullptr)
    {
        mutex.unlock();
        return -1;
    }

    Node* node = head;
    int value = node->value;

    if (head == tail)
    {
        head = nullptr;
        tail = nullptr;
    }
    else
    {
        head = head->next;
        if (head != nullptr)
        {
            head->prev = nullptr;
        }
    }

    delete node;
    size--;
    mutex.unlock();

    return value;
}

bool ConcurrentLinkedList::contains(int value)
{
    mutex.lock();

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

int ConcurrentLinkedList::getSize()
{
    return size;
}

bool ConcurrentLinkedList::isEmpty()
{
    return size == 0;
}