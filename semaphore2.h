#ifndef SEMAPHORE2_H
#define SEMAPHORE2_H

#include <string>
#include <chrono>

class SharedDB;
class Semaphore2 {
      public:

	Semaphore2() = default;
	bool init(uint32_t maxResources, const std::string& _path = std::string());

	/**
	 * @brief lock will block until a lock can be acquired
	 * @param maxWait in ns, default 1 sec (10^9)
	 * @return true (num of still available resources before the lock) if locked false if expired
	 */
	bool acquire(const std::chrono::nanoseconds &maxWait);
	/**
	 * @brief release previously acquired resource
	 * @return number of EXTIMATED free resources
	 */
	void release();
	/**
	 * @brief recount will recount how many lock has sharedLockFD
	 */
	void recount();

      private:
	//This will be used like a mutex to protec shared mem initialization
	std::string singleLockName;
	//This will be used to count how many process are attive
	std::string sharedLockName;

	int singleLockFD = 0;
	int sharedLockFD = 0;

	//in which pos of the sharedDB pidArray we registered ourself
	int pidPos = 0;

	//just to forward declare
	SharedDB* sharedDB = nullptr;
};
#endif // SEMAPHORE2_H
