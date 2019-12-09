#include "semaphore2.h"
#include "sys/file.h"
#include <thread>
#include <atomic>
using namespace std::chrono_literals;

void openManyFIle(){
	for (int var = 0; var < 100; ++var) {
		open(std::to_string(var).c_str(), O_CREAT | O_RDWR, 0666);
	}
}

std::atomic<uint> ok = 0;
std::atomic<uint> no = 0;
void tryal(){
	try {
		Semaphore2 sem;
		sem.init(10,"g17");
		//incredible but true, first use of this lib is to test itself!!!
		if(sem.acquire(420ms)){
			ok++;
			//openManyFIle();
			//system("./test &");
			std::this_thread::sleep_for(0.27s);
			sem.release();
		}else{
			no++;
		}

	} catch (const std::exception& e) {
		printf("%s \n",e.what());
	}
}

int main() {
	uint thread = 1000;
	std::thread* tArray[thread];
	for (int var = 0; var < thread; ++var) {
		tArray[var] = new std::thread(tryal);
		std::this_thread::sleep_for(0.001s);
	}
	for (int var = 0; var < thread; ++var) {
		tArray[var]->join();
	}
	printf("ok %d vs no %d \n", ok.load(), no.load());

	//std::this_thread::sleep_for(9999s);
}
