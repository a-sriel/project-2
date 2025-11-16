#include <iostream>
#include <thread>
#include "semaphore.h"

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <random>
#include <chrono>

Semaphore mutex(3);
int customerAction;

void customer(int i) {
    mutex.wait();
    if (customerAction == 0) {
        std::cout << "Customer " << i << "[]:" << " wants to perform a deposit transaction "<< std::endl;
    } else {
        std::cout << "Customer " << i << "[]:" << " wants to perform a withdrawal transaction "<< std::endl;
    }
    mutex.signal();
}

int main() {
    // seed time
    std::srand(std::time(0));
    customerAction = rand() % 2;

    std::thread cust[3];

    cust[0] = std::thread(customer, 0);
    cust[0].join();
    cust[1] = std::thread(customer, 1);
    cust[1].join();
    cust[2] = std::thread(customer, 2);
    cust[2].join();

    return 0;
}