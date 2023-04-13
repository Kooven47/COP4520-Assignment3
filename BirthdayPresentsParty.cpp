// Kevin Alfonso
// COP4520

#include "ConcurrentLinkedList.hpp"
#include <mutex>
#include <unordered_set>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <iterator>

#define NUM_THREADS 4
#define NUM_GUESTS 500000

#define ADD_PRESENT 0
#define WRITE_CARD 1

std::mutex mutex;

// Generate an unordered set of random numbers from 0 to size (exclusive)
std::unordered_set<int> generateShuffledUnorderedSet(int size)
{
    std::vector<int> vector;
    for (int i = 0; i < size; i++)
    {
        vector.push_back(i);
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::shuffle(vector.begin(), vector.end(), mt);

    return std::unordered_set<int>(vector.begin(), vector.end());
}

// Thread constructor was complaining, so I added &s here
void completeTasks(int threadId, ConcurrentLinkedList& presentsChain, std::unordered_set<int>& giftBag, std::vector<int>& cards)
{
    mutex.lock();

    // Starts with thread 0 doing task 0, thread 1 doing task 1, thread 3 doing task 0, and thread 4 doing task 1
    int currentThreadTask = (threadId % 2);
    // std::cout << "Thread " << threadId << " started with task " << currentThreadTask << std::endl;
    
    mutex.unlock();

    while (cards.size() < NUM_GUESTS)
    {
        // Take gift from gift bag, add it to list
        if (currentThreadTask == ADD_PRESENT)
        {
            mutex.lock();

            if (giftBag.empty())
            {
                mutex.unlock();
                continue;
            }

            // Grab first gift from gift bag (already shuffled)
            auto firstGift = giftBag.begin();
            int value = *firstGift;
            // Remove from gift bag
            giftBag.erase(firstGift);
            // Add it to chain
            presentsChain.add(value);
            // std::cout << "Present added for guest " << value << " by thread " << threadId << std::endl;

            mutex.unlock();
        }
        // Take gift from list, write card for guest
        else if (currentThreadTask == WRITE_CARD)
        {
            mutex.lock();

            if (presentsChain.isEmpty())
            {
                mutex.unlock();
                continue;
            }

            // Remove first present from chain, ensuring there is something valid to remove
            int guest = presentsChain.removeHead();
            if (guest == -1)
            {
                mutex.unlock();
                continue;
            }
            // Write card for that guest <3
            cards.emplace_back(guest);
            // std::cout << "Card written for guest " << guest << " by thread " << threadId << std::endl;

            mutex.unlock();
        }

        mutex.lock();

        // Alternate between tasks for thread upon completion
        currentThreadTask = (currentThreadTask + 1) % 2;
        // std::cout << "Thread " << threadId << " switched to task " << currentThreadTask << std::endl;

        mutex.unlock();
    }
}

int main(void)
{
    auto start = std::chrono::high_resolution_clock::now();

    ConcurrentLinkedList presentsChain;
    std::unordered_set<int> giftBag = generateShuffledUnorderedSet(NUM_GUESTS);
    std::vector<int> cards;
    std::vector<std::thread> threads(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        // Pass data structures by reference to make sure the threads are using the same shared objects
        threads[i] = std::thread(completeTasks, i, std::ref(presentsChain), std::ref(giftBag), std::ref(cards));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Time taken: " << duration << " ms" << std::endl;
}
