2025-11-14 18:35

For this project, I need to simulate a bank and the interactions between its tellers and customers. A customer can either withdraw or make a deposit. This project will involve waiting in line (queuing) so I will need to utilize semaphores to ensure mutual exclusion is enforced.

I am going to use the provided semaphore class, as well as std::threads, to develop this program. I understand mutex, but am rusty on threads, so I have already done a couple hours of research trying to improve my understanding of how semaphores work. Synchronization is clear conceptually, but in practice, I am unsure how to implement it.

I will probably start off by including most of the logic in the provided thread_code.cpp file, and build off of what has already been provided. I might later decide to separate the customer and teller into their own files later if necessary.

2025-11-14 20:16

This session, I'm going to implement some barebones logic for the customer and teller threads inside the thread_code program. I'm not planning to do much testing just yet; I just want to get a skeleton down so that I don't get lost in the weeds.

I added some teller and customer functions. Within the functions, I simply modified the provided threadCode function that uses semaphores. I also added some global variables for things like, the total amount of money in the safe, etc. I'm not sure if this idea is going to work yet, but at least I have something down.