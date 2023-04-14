// Kevin Alfonso
// COP4520

#include <mutex>
#include <set>
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <iterator>

#define NUM_THREADS 8
#define MINUTES 60
#define HOURS 24

std::mutex mutex;

void printFiveHighestTemperatures(std::vector<int>& sensorReadings) 
{
    // Check for unique temperatures, not needed if uniqueness not needed
    std::set<int> fiveHighestTemperatures;
    
    std::cout << "Five Highest temperatures: ";

    // Iterate through the vector in reverse order
    for (auto it = sensorReadings.rbegin(); it != sensorReadings.rend(); it++) 
    {
        // Make sure temperature is unique (not already in set)
        if (fiveHighestTemperatures.find(*it) == fiveHighestTemperatures.end())
        {
            fiveHighestTemperatures.insert(*it);
            std::cout << *it << "°F";
            if (fiveHighestTemperatures.size() == 5) 
            {
                std::cout << std::endl;
                break;
            }
            else 
            {
                std::cout << ", ";
            }
        }
    }
}

void printFiveLowestTemperatures(std::vector<int>& sensorReadings) 
{
    // Check for unique temperatures, not needed if uniqueness not needed
    std::set<int> fiveLowestTemperatures;

    std::cout << "Five Lowest temperatures: ";

    for (auto it = sensorReadings.begin(); it != sensorReadings.end(); it++) 
    {
        // Make sure temperature is unique (not already in set)
        if (fiveLowestTemperatures.find(*it) == fiveLowestTemperatures.end())
        {
            fiveLowestTemperatures.insert(*it);
            std::cout << *it << "°F";
            if (fiveLowestTemperatures.size() == 5) 
            {
                std::cout << std::endl;
                break;
            }
            else 
            {
                std::cout << ", ";
            }
        }
    }
}

void printTenMinuteLargestDifference(std::vector<int>& sensorReadings) 
{
    // 10 minute interval
    int intervalSize = 10;
    int startInterval = 0;
    int maxDifference = 0;
    int largerTemperature = 0;
    int smallerTemperature = 0;

    // Loop through each sensor/thread
    for (int currentThread = 0; currentThread < NUM_THREADS; currentThread++) 
    {
        int firstSensorMinute = currentThread * MINUTES;

        // For that current sensor, loop from minute 0 to minute 50 (since we add 10 minutes in the search)
        for (int i = firstSensorMinute; i < MINUTES - intervalSize + 1; i++) 
        {
            // Get min and max element for current 10 minute interval
            auto intervalStart = sensorReadings.begin() + i;
            auto intervalEnd = intervalStart + intervalSize;
            int currentMax = *std::max_element(intervalStart, intervalEnd);
            int currentMin = *std::min_element(intervalStart, intervalEnd);
            int currentDifference = currentMax - currentMin;

            if (currentDifference > maxDifference) 
            {
                maxDifference = currentDifference;
                largerTemperature = currentMax;
                smallerTemperature = currentMin;
                startInterval = i;
            }
        }
    }

    std::cout << "Largest Ten Minute Temperature Difference: " << maxDifference << "°F (" << largerTemperature << "°F to " << smallerTemperature << "°F)" << ", from minute " << startInterval << " to minute " << (startInterval + 10) << std::endl;
}

void generateReport(int hour, std::vector<int>& sensorReadings) 
{
    std::cout << "Hour " << hour + 1 << " Report" << std::endl;
    std::cout << "--------------" << std::endl;

    // Have to print difference first because sorting is done for next two prints, and that would mess this up
    printTenMinuteLargestDifference(sensorReadings);

    std::sort(sensorReadings.begin(), sensorReadings.end());

    printFiveHighestTemperatures(sensorReadings);
    printFiveLowestTemperatures(sensorReadings);

    std::cout << std::endl;
}

bool allsensorsFinishedCurrentReading(int originalThreadId, std::vector<bool>& sensorsFinishedCurrentReading) 
{
    for (int i = 0; i < sensorsFinishedCurrentReading.size(); i++) 
    {
        // If a sensor hasnt been read yet, and it's not the original sensor that called the function, return false
        if (!sensorsFinishedCurrentReading[i] && i != originalThreadId) 
        {
            return false;
        }
    }
    return true;
}

// Random integer generator, inclusive of min and max
int generateRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);

    return dist(mt);
}

// Thread constructor was complaining, so I added &s here
void measureTemperature(int threadId, std::vector<int>& sensorReadings, std::vector<bool>& sensorsFinishedCurrentReading) 
{
    for (int currentHour = 0; currentHour < HOURS; currentHour++) 
    {
        for (int currentMinute = 0; currentMinute < MINUTES; currentMinute++) 
        {
            sensorsFinishedCurrentReading[threadId] = false;
            
            // Gets first minute/reading index of current sensor and assigns it the current minute's randomly generated reading
            int firstSensorMinute = threadId * MINUTES;
            sensorReadings[firstSensorMinute + currentMinute] = generateRandomNumber(-100, 70);

            sensorsFinishedCurrentReading[threadId] = true;            

            // Make sure to wait for all sensors to finish their reading before we make another one
            // Helps ensure synchronized temperature readings
            while (!allsensorsFinishedCurrentReading(threadId, sensorsFinishedCurrentReading)) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
        // Only have one thread (the first one) generate the report
        if (threadId == 0) 
        {
            mutex.lock();
            generateReport(currentHour, sensorReadings);
            mutex.unlock();
        }
    }
}

int main() 
{
    auto start = std::chrono::high_resolution_clock::now();

    // Thread 0 writes to indices 0 - 59, thread 1 writes to 60 - 119, ..., thread 7 writes to 420 - 479
    // Filled with randomly generated numbers
    std::vector<int> sensorReadings(NUM_THREADS * MINUTES);
    // Shows when a sensor is done reading for the current minute
    std::vector<bool> sensorsFinishedCurrentReading(NUM_THREADS);
    std::vector<std::thread> threads(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) 
    {
        // Pass data structures by reference to make sure the threads are using the same shared objects
        threads[i] = std::thread(measureTemperature, i, std::ref(sensorReadings), std::ref(sensorsFinishedCurrentReading));
    }

    for (std::thread& thread : threads) 
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Finished in " << duration << " ms" << std::endl;
}
