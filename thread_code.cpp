#include <iostream>
#include <thread>
#include "semaphore.h"

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <chrono>

Semaphore bankOpen(3);
Semaphore mutex2(3);
int customerAction;
int customerWait;

Semaphore safe(2);  // 2 tellers can access safe at a time

Semaphore door(2);  // 2 customers can enter at a time
int doorCount = 0;

Semaphore manager(1);   // 1 teller can interact with manager at a time;
                        // manager gives perms to access safe

Semaphore customerLine(30);
int customerCount = 3;
int tellerCount = 0;

Semaphore tellersAvailable(3);

Semaphore customersLeft(3);

Semaphore customerRequest(3);

Semaphore mutex(30);

Semaphore customerReady(3);

void introduceSelf(int id) {
    
}

void deposit() {
    manager.wait();
    std::cout << "deposit" << std::endl;
    manager.signal();
}

void withdraw() {
    manager.wait();
    std::cout << "withdraw" << std::endl;
    manager.signal();
}

void teller(int i) {
    mutex.wait();
    int teller_id = rand();
    std::cout << "Teller " << i << " []: ready to serve" << std::endl;
    std::cout << "Teller " << i << " []: waiting for a customer" << std::endl;

    tellerCount++;
    if (tellerCount == 3) {
        bankOpen.signal();
    }

 //   while (tellerCount == 3 && customerCount > 0) {
   // }
    
   std::cout << customerCount << std::endl;

   while (tellerCount == 3 && customerCount > 0) {
    customerReady.wait();
    std::cout << customerCount << std::endl;

   }
    if (customerCount == 0) {
        std::cout << "Teller " << i << " []: leaving for the day" << std::endl;
    }
    
    
}

void customer(int i) {
    mutex.signal();
    // generate customer id
    int customer_id = rand();
    // generate randomized customer action where 0 = deposit & 1 = withdraw
    customerAction = rand() % 2;

    //mutex.wait();
    bankOpen.wait();
    if (customerAction == 0) {
        std::cout << "Customer " << i << " []:" << " wants to perform a deposit transaction " << std::endl;
    } else {
        std::cout << "Customer " << i << " []:" << " wants to perform a withdrawal transaction " << std::endl;
    }

    // generate customer wait time between 0 - 100 ms
    customerWait = rand() % 101;
    std::this_thread::sleep_for(std::chrono::milliseconds(customerWait));
    
    // customer enters door
    door.wait();
    std::cout << "Customer " << i << " []: going to bank." << std::endl;
    std::cout << "Customer " << i << " []: entering bank." << std::endl;
    door.signal();

    // customer gets in line
    customerLine.wait();
    std::cout << "Customer " << i << " []: getting in line." << std::endl;
    customerLine.signal();
    customerReady.signal();

    // get a free teller
    tellersAvailable.wait();
    std::string tellerName = "hi";
    std::cout << "Customer " << i << " [" << tellerName << "]: selects teller" << std::endl;
    std::cout << "Customer " << i << " [" << tellerName << "]: introduces itself" << std::endl;

    // introduce self to teller (give id)
    introduceSelf(customer_id);
    // wait for teller to ask for transaction
    std::cout << "Customer " << i << " [" << tellerName << "]: leaves teller" << std::endl;
    // release occupied teller
    tellersAvailable.signal();

    // customer leaves door;
    door.wait();
    std::cout << "Customer " << i << " []: goes to door" << std::endl;
    customerCount--;
    door.signal();

    //mutex.signal();
}



int main() {
    // seed time
    std::srand(std::time(0));

    std::thread tell[3];
    for (int i = 0; i < 3; i++) {
        tell[i] = std::thread(teller, i);
    }

    std::thread cust[3];

    cust[0] = std::thread(customer, 0);
    cust[1] = std::thread(customer, 1);
    cust[2] = std::thread(customer, 2);

    for (int i = 0; i < 3; i++) {
        tell[i].join();
    }

    for (int i = 0; i < 3; i++) {
        cust[i].join();
        // customer leaves simulation
        std::cout << "Customer " << i << " []: leaves the bank" << std::endl;
        customersLeft.signal();
    }


    // all threads have completed; close bank
    std::cout << "The bank closes for the day." << std::endl;

    return 0;
}