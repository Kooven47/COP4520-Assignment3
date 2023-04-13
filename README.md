# COP4520 Assignment 3
This project is a C++ implementation using std::thread of the problems below.

## Problem 1: The Birthday Presents Party (50 points)
The Minotaur’s birthday party was a success. The Minotaur received a lot of presents from his guests. The next day he decided to sort all of his presents and start writing “Thank you” cards. Every present had a tag with a unique number that was associated with the guest who gave it. Initially all of the presents were thrown into a large bag with no particular order. The Minotaur wanted to take the presents from this unordered bag and create a chain of presents hooked to each other with special links (similar to storing elements in a linked-list). In this chain (linked-list) all of the presents had to be ordered according to their tag numbers in increasing order. The Minotaur asked 4 of his servants to help him with creating the chain of presents and writing the cards to his guests. Each servant would do one of three actions in no particular order: 1. Take a present from the unordered bag and add it to the chain in the correct location by hooking it to the predecessor’s link. The servant also had to make sure that the newly added present is also linked with the next present in the chain. 2. Write a “Thank you” card to a guest and remove the present from the chain. To do so, a servant had to unlink the gift from its predecessor and make sure to connect the predecessor’s link with the next gift in the chain. 3. Per the Minotaur’s request, check whether a gift with a particular tag was present in the chain or not; without adding or removing a new gift, a servant would scan through the chain and check whether a gift with a particular tag is already added to the ordered chain of gifts or not. As the Minotaur was impatient to get this task done quickly, he instructed his servants not to wait until all of the presents from the unordered bag are placed in the chain of linked and ordered presents. Instead, every servant was asked to alternate adding gifts to the ordered chain and writing “Thank you” cards. The servants were asked not to stop or even take a break until the task of writing cards to all of the Minotaur’s guests was complete. After spending an entire day on this task the bag of unordered presents and the chain of ordered presents were both finally empty! Unfortunately, the servants realized at the end of the day that they had more presents than “Thank you” notes. What could have gone wrong? Can we help the Minotaur and his servants improve their strategy for writing “Thank you” notes? Design and implement a concurrent linked-list that can help the Minotaur’s 4 servants with this task. In your test, simulate this concurrent “Thank you” card writing scenario by dedicating 1 thread per servant and assuming that the Minotaur received 500,000 presents from his guests.

## Compilation Instructions
g++ -o BirthdayPresentsParty -pthread BirthdayPresentsParty.cpp
<br> ./BirthdayPresentsParty

## Proof of Correctness, Efficiency, and Experimental Evaluation
I use a course-grained singly-linked list with a head pointer to represent the chain of presents for the Minotaur. Basically, the code is exactly like a normal linked list, but with a mutex/lock locking at the start of the add(), removeHead(), and contains() operations and then unlocking at the end. This works well due to the low levels of concurrency (only 4 elves/threads) and is easy to implement. The correctness for this problem is proved by the while loop running until a card has been written for every guest, ensuring no guest is left behind, and that extra cards are not written. When a present is added, it is removed from the giftbag and added to the presents chain, ensuring uniqueness. When a card is written, the present is removed from the chain and a card is written for that tag number, also ensuring uniqueness.
<br> Runtimes averaged over ten trials on my computer without any printing, where N = NUM_GUESTS:
<br> For N = 1,000:   2ms
<br> For N = 10,000:  24.9ms
<br> For N = 100,000: 262.5ms
<br> For N = 250,000: 638.5ms
<br> For N = 500,000: 1306.8ms

## Problem 2: Atmospheric Temperature Reading Module (50 points)
You are tasked with the design of the module responsible for measuring the atmospheric temperature of the next generation Mars Rover, equipped with a multicore CPU and 8 temperature sensors. The sensors are responsible for collecting temperature readings at regular intervals and storing them in shared memory space. The atmospheric temperature module has to compile a report at the end of every hour, comprising the top 5 highest temperatures recorded for that hour, the top 5 lowest temperatures recorded for that hour, and the 10-minute interval of time when the largest temperature difference was observed. The data storage and retrieval of the shared memory region must be carefully handled, as we do not want to delay a sensor and miss the interval of time when it is supposed to conduct temperature reading. Design and implement a solution using 8 threads that will offer a solution for this task. Assume that the temperature readings are taken every 1 minute. In your solution, simulate the operation of the temperature reading sensor by generating a random number from -100F to 70F at every reading. In your report, discuss the efficiency, correctness, and progress guarantee of your program.

## Compilation Instructions
g++ -o AtmosphericTemperatureReadingModule -pthread AtmosphericTemperatureReadingModule.cpp
<br> ./AtmosphericTemperatureReadingModule

## Proof of Correctness, Efficiency, and Experimental Evaluation
I decided to let the sensors measure for 24 hours because one day makes the most sense, but that can be changed by editing the HOURS defined at the top of the program.
<br> Runtimes averaged over ten trials on my computer without any printing, where N = HOURS:
<br> For N = 8:  42ms
<br> For N = 12: 60.6ms
<br> For N = 24: 119.1ms
<br> For N = 120: ms
<br> For N = 240: ms
