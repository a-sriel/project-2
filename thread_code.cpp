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
#include <string>

#define TELLERS_COUNT 3
#define CUSTOMERS_COUNT 50

Semaphore bankOpen(0);

Semaphore outputMutex(1);

Semaphore safe(2);  // 2 tellers can access safe at a time
Semaphore door(2);  // 2 customers can enter at a time
Semaphore manager(1);   // 1 teller can interact with manager at a time;
                        // manager gives perms to access safe

Semaphore customerReady(1);
int customersReady = CUSTOMERS_COUNT;

int readyTellersCount = 0;
Semaphore tellersReady(0);
std::queue<int> readyTellersQueue;
Semaphore readyQueue(1);

Semaphore readyMutex(1);

// synchronization arrays
Semaphore customerInteractsWithTeller[TELLERS_COUNT];

Semaphore tellerPrompt[TELLERS_COUNT];
Semaphore giveTransaction[TELLERS_COUNT];
Semaphore completeTransaction[TELLERS_COUNT];
Semaphore customerLeft[TELLERS_COUNT];

int customerToTeller[TELLERS_COUNT];
int actionToTeller[TELLERS_COUNT];

void threadsafePrint(const std::string& text) {
    outputMutex.wait();
    std::cout << text << std::endl;
    outputMutex.signal();
}

void teller(int i) {

    //int teller_id = rand();
    customerInteractsWithTeller[i].initialize(0);
    tellerPrompt[i].initialize(0);
    giveTransaction[i].initialize(0);
    completeTransaction[i].initialize(0);
    customerLeft[i].initialize(0);

    

    readyMutex.wait();
    readyTellersCount++;

    if (readyTellersCount == TELLERS_COUNT) {
        bankOpen.signal();
    }
    
    readyMutex.signal();

    bankOpen.wait();
    bankOpen.signal();
    
    while (true) {
        customerReady.wait();

        if (customersReady <= 0) {
            customerReady.signal();
            break;
        }

        customersReady--;

        // wait for customer
        threadsafePrint( "Teller " + std::to_string(i) +" []: ready to serve" );
        threadsafePrint( "Teller " +std::to_string(i)  +" []: waiting for a customer" );
        
        readyQueue.wait();
        readyTellersQueue.push(i);
        readyQueue.signal();

        tellersReady.signal();
  
        customerInteractsWithTeller[i].wait();
        customerReady.signal();

        int customerId = customerToTeller[i];
        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: serving a customer" );
        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: asks for transaction" );
        tellerPrompt[i].signal();

        giveTransaction[i].wait();
        int customerAction = actionToTeller[i];
        std::string actionString = (customerAction == 1) ? "withdrawal" : "deposit";

        if (customerAction == 1) {
            threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: handling withdrawal transaction" );
            threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: going to the manager" );
            // get manager permission
            manager.wait();
            threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: getting manager's permission" );
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 26 + 5));
            threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: got manager's permission" );
            manager.signal();
        } else {
            threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: handling deposit transaction" );
        }
        
        // access safe
        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: going to safe" );
        safe.wait();
        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: enter safe" );
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 41 + 10));
        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: leaving safe" );
        safe.signal();

        // transaction fulfilled
        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: finishes " +actionString +" transaction." );
        

        threadsafePrint( "Teller " +std::to_string(i)  +" [Customer " +std::to_string(customerId) +"]: wait for customer to leave." );
        completeTransaction[i].signal();
        customerLeft[i].wait();
    }   

    threadsafePrint( "Teller " +std::to_string(i)  +" []: leaving for the day" );

}

void customer(int i) {
    // generate randomized customer action where 0 = deposit & 1 = withdraw
    int customerAction = rand() % 2;
    std::string actionString = (customerAction == 1) ? "withdrawal" : "deposit";
    threadsafePrint( "Customer " +std::to_string(i)  +" []: wants to perform a " +actionString +" transaction" );

    // generate customer wait time between 0 - 100 ms
    int customerWait = rand() % 101;
    std::this_thread::sleep_for(std::chrono::milliseconds(customerWait));
    
    bankOpen.wait();
    bankOpen.signal();

    // customer enters door
    threadsafePrint( "Customer " +std::to_string(i)  +" []: going to bank." );
    door.wait();
    threadsafePrint( "Customer " +std::to_string(i)  +" []: entering bank." );
    door.signal();

    // customer gets in line
    threadsafePrint( "Customer " +std::to_string(i)  +" []: getting in line." );
    tellersReady.wait();
    threadsafePrint( "Customer " +std::to_string(i)  +" []: selecting a teller." );
    
    readyQueue.wait();
    int tellerId = readyTellersQueue.front();
    readyTellersQueue.pop();
    readyQueue.signal();

    // get a free teller
    // introduce self to teller (give id)
    threadsafePrint( "Customer " +std::to_string(i) +" [Teller " +std::to_string(tellerId) +"]: selects teller" );
    customerToTeller[tellerId] = i;
    threadsafePrint( "Customer " +std::to_string(i)  +" [Teller " +std::to_string(tellerId) +"]: introduces itself" );
    customerInteractsWithTeller[tellerId].signal();
    
    // wait for teller to ask for transaction
    tellerPrompt[tellerId].wait();

    actionToTeller[tellerId] = customerAction;
    threadsafePrint( "Customer " +std::to_string(i)  +" [Teller " +std::to_string(tellerId) +"]: asks for " +actionString +" transaction" );
    giveTransaction[tellerId].signal();

    // complete transaction
    completeTransaction[tellerId].wait();

    threadsafePrint( "Customer " +std::to_string(i)  +" [Teller " +std::to_string(tellerId) +"]: leaves teller" );
    customerLeft[tellerId].signal();

    // customer leaves door
    door.wait();
    threadsafePrint( "Customer " +std::to_string(i)  +" []: goes to door" );
    // customer leaves simulation
    threadsafePrint( "Customer " +std::to_string(i)  +" []: leaves the bank" ); 
    door.signal();

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
    threadsafePrint( "The bank closes for the day." );

    return 0;
}