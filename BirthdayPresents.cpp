#include "ConcurrentLinkedList.hpp"
#include "RandomNumberGenerator.hpp"

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

#define ADD_PRESENT 1
#define WRITE_CARD 2
#define SEARCH_FOR_PRESENT 3

std::mutex mutex;

// Generate an unordered set of random numbers from 0 to size (exclusive)
std::unordered_set<int> generateShuffledSet(int size)
{
    std::vector<int> vector;
    for (int i = 0; i < size; i++)
    {
        vector.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(vector.begin(), vector.end(), g);

    return std::unordered_set<int>(vector.begin(), vector.end());
}

// Thread constructor was complaining, so I added &s here
void completeTasks(ConcurrentLinkedList& list, std::unordered_set<int>& giftBag, std::unordered_set<int>& cards)
{
    while (cards.size() < NUM_GUESTS)
    {
        int task = RandomNumberGenerator::generateRandomNumber(1, 3);
        switch (task)
        {
            // Take gift from gift bag, add it to list
            case ADD_PRESENT:
            {
                mutex.lock();

                if (giftBag.empty() || giftBag.begin() == giftBag.end())
                {
                    mutex.unlock();
                    continue;
                }

                std::unordered_set<int>::iterator it = giftBag.begin();
                int value = *it;
                giftBag.erase(it);
                list.add(value);
                // std::cout << "Present added for guest " << value << std::endl;
                mutex.unlock();

                break;
            }
            // Take gift from list, write card for guest
            case WRITE_CARD:
            {
                mutex.lock();

                if (list.isEmpty())
                {
                    mutex.unlock();
                    continue;
                }
                int guest = list.removeHead();
                if (guest == -1)
                {
                    mutex.unlock();
                    continue;
                }

                cards.insert(guest);
                // std::cout << "Card written for guest " << guest << " by thread " << threadId << std::endl;
                mutex.unlock();

                break;
            }
            // Search for guest in list
            case SEARCH_FOR_PRESENT:
            {
                int guest = RandomNumberGenerator::generateRandomNumber(0, NUM_GUESTS - 1);
                bool isFound = list.contains(guest);
                // std::cout << "Guest " << guest << " is " << (isFound ? "" : "not ") << "found" << std::endl;
                
                break;
            }
        }
    }
}

int main(void)
{
    auto start = std::chrono::high_resolution_clock::now();

    ConcurrentLinkedList presentsChain;
    std::unordered_set<int> cards;
    std::unordered_set<int> giftBag = generateShuffledSet(NUM_GUESTS);
    std::vector<std::thread> threads(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        // Pass by reference to make sure the threads are using the same objects
        threads[i] = std::thread(completeTasks, std::ref(presentsChain), std::ref(giftBag), std::ref(cards));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Time taken: " << duration << " ms" << std::endl;
}
