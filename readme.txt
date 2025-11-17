Files:
1) thread_code.cpp - the driver program, simulates a bank program using 50 customer threads and 3 teller threads

2) semaphore.h - header file for the provided semaphor class; allows mutex locks and unlocks

3) semaphor.cpp - includes the function definitions for the above

4) devlog.md - a journal containing all of my thoughts and timestamped work sessions throughout the course of this project

5) readme.txt - you are here. tells you everything you need to know (I hope)

For compilation:
compile semaphore.cpp and thread_code together; I used g++ thread_code.cpp semaphore.cpp -o thread_code

For execution:
simply run ./thread_code

Tools used:
files were compiled, run, and tested using Ubuntu running on WSL on the Windows 10 command line, but any UNIX terminal should work
all code was written (but not tested) in VS code with c++ extensions