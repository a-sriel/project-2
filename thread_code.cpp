#include<iostream>
#include <thread>
#include "semaphore.h"

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <random>

int gCount = 0;
Semaphore gLock(1);

int tCount = 0;
Semaphore tLock(3);

int cCount = 0;
Semaphore custLock(50);

Semaphore mutex(2);

int doorOccupancy = 0; // max 2
#define MAX_OCCUPANCY 2

int customerBalance = 0;
int customerAction = 0;
int safeBalance = 999;


void threadCode(int i) {
    gLock.wait();
    std::cout << "Thread " << i << " has count " << gCount << std::endl;
    gCount++;
    gLock.signal();
}

void teller(int i) {
    std::thread::id _id;
    bool ready;


    // ready
    tLock.wait();
    std::cout << "Teller " << i << "[]:" << " ready to serve" << std::endl;
    // wait for customer
    std::cout << "Teller " << i << "[]:" << " waiting for a customer" << std::endl;
    // receive customer signal -> ask for transaction
    // go to safe
    // talk to customer
    // wait for customer to leave
    tCount++;
    tLock.signal();
}

void customer(int i) {
    std::thread::id _id;
    custLock.wait();
    cCount++;
    // generates random behavior; 0 or 1
    // where 0 = deposit, 1 = withdrawal

    if (customerAction == 0) {
        std::cout << "Customer " << i << "[]:" << " wants to perform a deposit transaction "<< std::endl;
    } else {
        std::cout << "Customer " << i << "[]:" << " wants to perform a withdrawal transaction "<< std::endl;
    }

    if (doorOccupancy < MAX_OCCUPANCY) {
        std::cout << "HI" << std::endl;
        doorOccupancy++;
        custLock.signal();
        mutex.signal();
        tLock.wait();
    } else {
        mutex.wait();
    }
    custLock.signal();


    
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
    std::thread customers[5];

    for (int i = 0; i < 5; i++) {
        customers[i] = std::thread(customer, i);
    }

    for (int i = 0; i < 5; i++) {
        customers[i].join();
    }


    return 0;
}
