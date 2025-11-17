#include <iostream>
#include <thread>
#include "semaphore.h"

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <chrono>
#include <vector>
#include <queue>

#define TELLERS_COUNT 3
#define CUSTOMERS_COUNT 50

Semaphore bankOpen(0);

Semaphore safe(2);  // 2 tellers can access safe at a time
Semaphore door(2);  // 2 customers can enter at a time
Semaphore manager(1);   // 1 teller can interact with manager at a time;
                        // manager gives perms to access safe

int customerCount = 0;
int tellerCount = 0;

Semaphore tellersAvailable(TELLERS_COUNT);

Semaphore customerReady(1);
int customersReady = CUSTOMERS_COUNT;

int readyTellersCount = 0;
Semaphore tellersReady(0);
std::queue<int> readyTellersQueue;
Semaphore readyQueue(1);

std::vector<int> available_tellers;

// synchronization arrays
Semaphore customerInteractsWithTeller[TELLERS_COUNT];

Semaphore tellerPrompt[TELLERS_COUNT];
Semaphore giveTransaction[TELLERS_COUNT];
Semaphore completeTransaction[TELLERS_COUNT];
Semaphore customerLeft[TELLERS_COUNT];

int customerToTeller[TELLERS_COUNT];
int actionToTeller[TELLERS_COUNT];

void teller(int i) {

    //int teller_id = rand();
    std::cout << "Teller " << i << " []: ready to serve" << std::endl;

    tellersReady.wait();
    tellerCount++;

    if (readyTellersCount == 3) {
        bankOpen.signal();
    }
    
    tellersReady.signal();

    bankOpen.wait();
    bankOpen.signal();

 //   while (tellerCount == 3 && customerCount > 0) {
   // }
    
  // std::cout << customerCount << std::endl;
    
    while (true) {
        customerReady.wait();

        if (customersReady <= 0) {
            customerReady.signal();
            break;
        }

        customersReady--;
        customerReady.signal();

        readyQueue.wait();
        readyTellersQueue.push(i);
        readyQueue.signal();
        tellersReady.signal();

        // wait for customer
        std::cout << "Teller " << std::to_string(i)  << " []: waiting for a customer" << std::endl;
        customerInteractsWithTeller[i].wait();

        int customerId = customerToTeller[i];
        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: serving a customer" << std::endl;
        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: asks for transaction" << std::endl;
        tellerPrompt[i].signal();

        giveTransaction[i].wait();
        int customerAction = actionToTeller[i];
        std::string actionString = (customerAction == 1) ? "withdrawal" : "deposit";

        if (customerAction == 1) {
            std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: handling withdrawal transaction" << std::endl;
            std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: going to the manager" << std::endl;
            // get manager permission
            manager.wait();
            std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: getting manager's permission" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 25 + 6));
            std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: got manager's permission" << std::endl;
            manager.signal();
        } else {
            std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: handling deposit transaction" << std::endl;
        }
        
        // access safe
        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: going to safe" << std::endl;
        safe.wait();
        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: enter safe" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 40 + 11));
        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: leaving safe" << std::endl;
        safe.signal();

        // transaction fulfilled
        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: finishes " << actionString << " transaction." << std::endl;
        completeTransaction[i].signal();

        std::cout << "Teller " << std::to_string(i)  << "[Customer " << std::to_string(customerId) << "]: wait for customer to leave." << std::endl;
        customerLeft[i].wait();
    }   

    std::cout << "Teller " << std::to_string(i)  << " []: leaving for the day" << std::endl;

}

void customer(int i) {
    // generate randomized customer action where 0 = deposit & 1 = withdraw
    customerCount++;
    int customerAction = rand() % 2;
    std::string actionString = (customerAction == 1) ? "withdrawal" : "deposit";
    std::cout << "Customer " << std::to_string(i)  << " []: wants to perform a " << actionString << " transaction" << std::endl;

    // generate customer wait time between 0 - 100 ms
    int customerWait = rand() % 101;
    std::this_thread::sleep_for(std::chrono::milliseconds(customerWait));
    
    bankOpen.wait();
    bankOpen.signal();

    // customer enters door
    std::cout << "Customer " << std::to_string(i)  << " []: going to bank." << std::endl;
    door.wait();
    std::cout << "Customer " << std::to_string(i)  << " []: entering bank." << std::endl;
    door.signal();

    // customer gets in line
    std::cout << "Customer " << std::to_string(i)  << " []: getting in line." << std::endl;
    tellersReady.wait();
    std::cout << "Customer " << std::to_string(i)  << " []: selecting a teller." << std::endl;
    
    readyQueue.wait();
    int tellerId = readyTellersQueue.front();
    readyTellersQueue.pop();
    readyQueue.signal();

    // get a free teller
    tellersAvailable.wait();
    // introduce self to teller (give id)
    customerToTeller[tellerId] = i;
    std::cout << "Customer " << std::to_string(i) << " [Teller " << std::to_string(tellerId) << "]: selects teller" << std::endl;
    std::cout << "Customer " << std::to_string(i)  << " [Teller " << std::to_string(tellerId) << "]: introduces itself" << std::endl;
    customerInteractsWithTeller[tellerId].signal();
    
    // wait for teller to ask for transaction
    tellerPrompt[tellerId].signal();

    actionToTeller[tellerId] = customerAction;
    std::cout << "Customer " << std::to_string(i)  << " [Teller " << std::to_string(tellerId) << "]: asks for " << actionString << " transaction" << std::endl;
    giveTransaction[tellerId].signal();

    // complete transaction
    completeTransaction[tellerId].wait();

    std::cout << "Customer " << std::to_string(i)  << " [" << available_tellers[0] << "]: leaves teller" << std::endl;
    customerLeft[tellerId].signal();

    // customer leaves door
    //door.wait();
    std::cout << "Customer " << std::to_string(i)  << " []: goes to door" << std::endl;
    // customer leaves simulation
    std::cout << "Customer " << std::to_string(i)  << " []: leaves the bank" << std::endl; 
    //door.signal();
}



int main() {
    // seed time
    std::srand(std::time(NULL));

    std::thread tellers[TELLERS_COUNT];
    std::thread customers[CUSTOMERS_COUNT];

    for (int i = 0; i < TELLERS_COUNT; i++) {
        tellers[i] = std::thread(teller, i);
    }

    for (int i = 0; i < CUSTOMERS_COUNT; i++) {
        customers[i] = std::thread(customer, i);
    }

    for (int i = 0; i < CUSTOMERS_COUNT; i++) {
        customers[i].join();
    }

    for (int i = 0; i < TELLERS_COUNT; i++) {
        tellers[i].join();
    }

    // all threads have completed; close bank
    std::cout << "The bank closes for the day." << std::endl;

    return 0;
}