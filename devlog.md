2025-11-14 18:35

For this project, I need to simulate a bank and the interactions between its tellers and customers. A customer can either withdraw or make a deposit. This project will involve waiting in line (queuing) so I will need to utilize semaphores to ensure mutual exclusion is enforced.

I am going to use the provided semaphore class, as well as std::threads, to develop this program. I understand mutex, but am rusty on threads, so I have already done a couple hours of research trying to improve my understanding of how semaphores work. Synchronization is clear conceptually, but in practice, I am unsure how to implement it.

I will probably start off by including most of the logic in the provided thread_code.cpp file, and build off of what has already been provided. I might later decide to separate the customer and teller into their own files later if necessary.

2025-11-14 20:16

This session, I'm going to implement some barebones logic for the customer and teller threads inside the thread_code program. I'm not planning to do much testing just yet; I just want to get a skeleton down so that I don't get lost in the weeds.

I added some teller and customer functions. Within the functions, I simply modified the provided threadCode function that uses semaphores. I also added some global variables for things like, the total amount of money in the safe, etc. I'm not sure if this idea is going to work yet, but at least I have something down.

I had to install several packages to get this program to run using apt on Ubuntu. I installed build-essential so that I could use the features header. Now that I have begun testing, having 50 customers makes it difficult to read the output, so I have temporarily reduced its number to 5.

2025-11-16 9:55

The output is currently working. I want to start getting some of the synchronization down. I just need to understand how to implement the thread queueing, and once I have the logic down, it should be simple from there.

I'm trying to implement some mutex logic now. It's difficult to understand how the customer and teller interacts through threads. I don't think we are supposed to use queues, and instead rely on semaphores, so I'm trying to figure out how to capture the logic.

I added some basic sleep logic to the customer threads, to simulate waiting. I'm not sure if it works correctly or not. I also began creating separate files for the Teller and Customer programs - they have not been fully implemented yet. This session, I only have a bare bones version of the program down, but I'm happy with my progress so far.

2025-11-16 14:16

I really need to get at least one part of the thread synchronization working. I've realized this bank simulation is similar to to the barbershop problem provided in the textbook, so I am going to try to build my semaphore logic based off of that.

I temporarily threw all of my code into a new file titled test.cpp and am going to start from scratch. I need to look at everything with a pair of fresh eyes. I have set up a new customer() function, and created three customer threads in main.

I've set up most of the basic semaphores and where they begin/release their respective mutex locks for the customer function. I might end up moving customer to its own class later depending on what kind of functionality this program requires. My next big step is to get the customer to communicate with the teller (i.e., giving it its id).

2025-11-16 16:25

I noticed an issue in the customer function where it was waiting before making a decision; I therefore moved where it sleeps to come after when a decision is generated. Based on testing, the sync seems to be working (customers will wait based on their wait time, which is seeded by the current clock time, and then the one with the shortest wait time will enter first). Therefore, it looks like I just need to add functionality to make the customer leave, and then the big one will be having it actually interact with the teller.

I have several shared parameters in my thread_code already, including the manager, the safe, the door occupancy, etc. but I may need to add more to ensure the bank simulation runs as intended.

Some unexpected behavior I have encountered: threads for a respective function need to join after they have been created, or there will be synchronization errors when running the program. I have implemented a basic outline for the teller function now, and am attempting to develop a introduceSelf() function to allow a customer to pass its id along to a teller.