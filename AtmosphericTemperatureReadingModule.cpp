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

// Random integer generator, inclusive of min and max
int generateRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);

    return dist(mt);
}

bool allSensorsAreReady(int caller, std::vector<bool>& sensors) 
{
    for (int i = 0; i < sensors.size(); i++) 
    {
        if (!sensors[i]) 
        {
            return false;
        }
    }
    return true;
}

void printLargestDifference(std::vector<int>& sensorReadings) 
{
    // 10 minute interval
    int step = 10;
    int startInterval = 0;
    int maxDifference = 0;
    int largerTemperature = 0;
    int smallerTemperature = 0;

    // Loop through each sensor/thread
    for (int currentThread = 0; currentThread < NUM_THREADS; currentThread++) 
    {
        int firstSensorMinute = currentThread * MINUTES;

        // For that current sensor, loop from minute 0 to minute 50 (since we add 10 minutes in the search)
        for (int i = firstSensorMinute; i < MINUTES - step + 1; i++) 
        {
            // Get min and max element for current 10 minute interval
            int currentMax = *std::max_element(sensorReadings.begin() + i, sensorReadings.begin() + i + step);
            int currentMin = *std::min_element(sensorReadings.begin() + i, sensorReadings.begin() + i + step);
            int currentDifference = currentMax - currentMin;

            if (currentDifference > maxDifference) 
            {
                largerTemperature = currentMax;
                smallerTemperature = currentMin;
                maxDifference = currentDifference;
                startInterval = i;
            }
        }
    }

    std::cout << "Largest Ten Minute Temperature Difference: " << maxDifference << "°F (" << largerTemperature << "°F to " << smallerTemperature << "°F)" << ", from minute " << startInterval << " to minute " << (startInterval + 10) << std::endl;
}

void printHighestTemperatures(std::vector<int>& sensorReadings) 
{
    // Check for unique temperatures, not needed if uniqueness not needed
    std::set<int> FiveHighestTemperatures;
    
    std::cout << "Five Highest temperatures: ";

    // Iterate through the vector in reverse order
    for (auto it = sensorReadings.rbegin(); it != sensorReadings.rend(); it++) 
    {
        // Make sure temperature is unique (not already in set)
        if (FiveHighestTemperatures.find(*it) == FiveHighestTemperatures.end())
        {
            FiveHighestTemperatures.insert(*it);
            std::cout << *it << "°F";
            if (FiveHighestTemperatures.size() == 5) 
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

void printLowestTemperatures(std::vector<int>& sensorReadings) 
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

void generateReport(int hour, std::vector<int>& sensorReadings) 
{
    std::cout << "Hour " << hour + 1 << " Report" << std::endl;
    std::cout << "--------------" << std::endl;

    // Have to print difference first because sorting is done for next two prints, and that would mess this up
    printLargestDifference(sensorReadings);

    std::sort(sensorReadings.begin(), sensorReadings.end());

    printHighestTemperatures(sensorReadings);
    printLowestTemperatures(sensorReadings);

    std::cout << std::endl;
}

// Thread constructor was complaining, so I added &s here
void measureTemperature(int threadId, std::vector<int>& sensorReadings, std::vector<bool>& sensorsAreReady) 
{
    for (int hour = 0; hour < HOURS; hour++) 
    {
        for (int minute = 0; minute < MINUTES; minute++) 
        {
            int currentSensor = threadId * MINUTES;
            sensorsAreReady[threadId] = false;
            sensorReadings[currentSensor + minute] = generateRandomNumber(-100, 70);
            sensorsAreReady[threadId] = true;

            // Make sure to wait for all sensors to finish their reading before we make another one
            while (!allSensorsAreReady(threadId, sensorsAreReady)) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        // Only have one thread (the first one) generate the report
        if (threadId == 0) 
        {
            mutex.lock();
            generateReport(hour, sensorReadings);
            mutex.unlock();
        }
    }
}

int main() 
{
    auto start = std::chrono::high_resolution_clock::now();

    // Thread 0 writes to 0 - 59, thread 1 writes to 60 - 119, ...
    std::vector<int> sensorReadings(NUM_THREADS * MINUTES);
    std::vector<bool> sensorsAreReady(NUM_THREADS);
    std::vector<std::thread> threads(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) 
    {
        // Pass data structures by reference to make sure the threads are using the same shared objects
        threads[i] = std::thread(measureTemperature, i, std::ref(sensorReadings), std::ref(sensorsAreReady));
    }

    for (std::thread& thread : threads) 
    {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Finished in " << duration << " ms" << std::endl;
}
