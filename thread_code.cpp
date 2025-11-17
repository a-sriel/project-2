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

Semaphore bankOpen(6);
Semaphore mutex2(3);
int customerAction;
int customerWait;

Semaphore safe(2);  // 2 tellers can access safe at a time

Semaphore door(2);  // 2 customers can enter at a time
int doorCount = 0;

Semaphore manager(1);   // 1 teller can interact with manager at a time;
                        // manager gives perms to access safe

Semaphore customerLine(30);
int customerCount = 0;
int tellerCount = 0;

Semaphore tellersAvailable(3);

Semaphore customersLeft(3);

Semaphore customerRequest(3);

Semaphore mutex(30);

Semaphore customerReady(3);

Semaphore tellers(3);
Semaphore customers(3);

int unique_id = -1;

std::vector<int> available_tellers;

void teller(int i) {
    tellers.wait();
    //int teller_id = rand();
    std::cout << "Teller " << i << " []: ready to serve" << std::endl;
    std::cout << "Teller " << i << " []: waiting for a customer" << std::endl;

    tellerCount++;
    std::cout << "customers: " << customerCount << std::endl;

    bankOpen.signal();

    available_tellers.push_back(0);
    available_tellers.push_back(1);
    available_tellers.push_back(2);

 //   while (tellerCount == 3 && customerCount > 0) {
   // }
    
  // std::cout << customerCount << std::endl;
    
    while (customerCount > 0) {
        customerReady.wait();
        manager.wait();
        std::cout << unique_id << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 25 + 6));
        manager.signal();
        safe.wait();
    }   

    tellerCount--;
    std::cout << "Teller " << i << " []: leaving for the day" << std::endl;
    tellers.signal();
}

void customer(int i) {
    bankOpen.wait();
    customers.wait();
    // generate customer id
    //int customer_id = rand();
    // generate randomized customer action where 0 = deposit & 1 = withdraw
    customerCount++;
    customerAction = rand() % 2;

    //mutex.wait();
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
    //customerLine.wait();
    std::cout << "Customer " << i << " []: getting in line." << std::endl;
    //customerLine.signal();
    

    // get a free teller
    tellersAvailable.wait();
    std::cout << "Customer " << i << " [" << available_tellers[0] << "]: selects teller" << std::endl;
    std::cout << "Customer " << i << " [" << available_tellers[0] << "]: introduces itself" << std::endl;

    // introduce self to teller (give id)
    unique_id = i;
    customerReady.signal();
    // wait for teller to ask for transaction
    std::cout << "Customer " << i << " [" << available_tellers[0] << "]: leaves teller" << std::endl;
    // release occupied teller
    tellersAvailable.signal();

    // customer leaves door
    door.wait();
    std::cout << "Customer " << i << " []: goes to door" << std::endl;
    // customer leaves simulation
    std::cout << "Customer " << i << " []: leaves the bank" << std::endl; 
    door.signal();

    customerCount--;
    customers.signal();
    //mutex.signal();
}



int main() {
    // seed time
    std::srand(std::time(NULL));

    std::thread tellers[3];
    std::thread customers[3];

    for (int i = 0; i < 3; i++) {
        tellers[i] = std::thread(teller, i);
    }

    for (int i = 0; i < 3; i++) {
        customers[i] = std::thread(customer, i);
    }
    
    for (int i = 0; i < 3; i++) {
        customers[i].join();
    }

    for (int i = 0; i < 3; i++) {
        tellers[i].join();
    }

    // all threads have completed; close bank
    std::cout << "The bank closes for the day." << std::endl;

    return 0;
}