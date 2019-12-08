#include "semaphore2.h"
#include "sys/file.h"
#include <thread>
void openManyFIle(){
	for (int var = 0; var < 100; ++var) {
		open(std::to_string(var).c_str(), O_CREAT | O_RDWR, 0666);
	}
}
using namespace std::chrono_literals;
int main() {
	//openManyFIle();
	//system("./test &");
	try {
		Semaphore2 sem;
		sem.init(50,"geppetto");
		sem.acquire(1s);
		sem.recount();
	} catch (const std::exception& e) {
		printf("%s",e.what());
	}
	std::this_thread::sleep_for(60s);
}
