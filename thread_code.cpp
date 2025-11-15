#include<iostream>
#include <thread>
#include "semaphore.h"

int gCount = 0;
Semaphore gLock(1);

int tCount = 0;
Semaphore tLock(3);

int cCount = 0;
Semaphore custLock(50);

void threadCode(int i) {
    gLock.wait();
    std::cout << "Thread " << i << " has count " << gCount << std::endl;
    gCount++;
    gLock.signal();
}

void teller(int i) {
    int randomInt;
    bool ready;
    tLock.wait();
    std::cout << "Thread " << i << " has count " << tCount << std::endl;
    tCount++;
    tLock.signal();

    // ready
    // wait for customer
    // receive customer signal -> ask for transaction
    // go to safe
    // talk to customer
    // wait for customer to leave
}

void customer(int i) {
    custLock.wait();
    std::cout << "Thread " << i << " has count " << cCount << std::endl;
    cCount++;
    custLock.signal();

    // generates random behavior; 0 or 1
    // where 0 = withdraw, 1 = deposit
    bool action = rand() % 2;
    // generate wait from 0-100
    int wait = rand() % 101;
    if (doorOccupancy > 2) {
        // enter
    } else {
        // wait
    }

    // join line
    // give id
    // wait for teller
    // give transaction
    // wait for teller to finish
    // leave
}

int doorOccupancy = 0; // max 2

int customerBalance = 0;
int safeBalance = 999;

bool withdraw(int amount) {
    if (amount > safeBalance) {
        return false;
    }

    safeBalance - amount;
    return true;
}

void deposit(int amount) {
    customerBalance += amount;
}

int main() {
/*
    std::thread threads[5];

    for(int i=0; i<5;i++) {
        threads[i] = std::thread(threadCode, i);
    }

    for(int i=0; i<5; i++) {
        threads[i].join();
    }

    return 0;
*/

     // teller
    std::thread tellers[3];

    for (int i = 0; i < 3; i++) {
        tellers[i] = std::thread(teller, i);
    }

    for (int i = 0; i < 3; i++) {
        tellers[i].join();
    }

    // customer
    std::thread customers[50];

    for (int i = 0; i < 50; i++) {
        customers[i] = std::thread(customer, i);
    }

    for (int i = 0; i < 50; i++) {
        customers[i].join();
    }


    return 0;
}
