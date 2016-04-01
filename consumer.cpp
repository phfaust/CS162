#include <iostream>  // getline, string
#include <fstream>   // ifstream
#include <cstdio>    // printf, scanf, perror
#include <cstdlib>   // srand, atoi 
#include <ctime>     // time
#include <queue>     // queue
#include "semaphore.hpp"
#include "shared_memory.hpp"

using namespace std;

// assume one producer/consumer
int main(int argc, char* args[]) {
	srand(time(NULL)); // for random

	// check valid parameters
	if (argc < 3) {
		printf("Missing some arguments!\nUsage: %s <textfile> <shared memory size in bytes>", args[0]);
		return 1;
	}

	// get arguments
	char* file = args[1];
	int bytes = atoi(args[2]);

	// file stream
	ofstream fout(file);

	// concurrency members
	semaphore access(0xACCE55);
	memory<short> feeding(0xFEEEEED);
	memory<char> food(0xF00000D, bytes);

	printf("Preparing for consumption...\n");\
	enum {IDLE, FEEDING, EXIT};

	// main loop
	while (true) {
		access.wait();
		if (feeding.read() == FEEDING) {
			// there's food for consumer
			string buffer = food.data();
			feeding.write(IDLE);
			access.signal();

			fout << buffer << endl;
			printf("FOOD!!! Eats (%s)\n", buffer.c_str());
		}

		else if (feeding.read() == IDLE) {
			access.signal();
			printf("Waiting for producer...\n");
		}

		else { // feeding.read() == EXIT
			feeding.write(IDLE); // make it idle for the next consumer
			access.signal();
			printf("Producer has no more food. Quitting huhu.\n");
			break;
		}
		sleep(4);
	}

	return 0;
}