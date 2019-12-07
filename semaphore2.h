#ifndef SEMAPHORE2_H
#define SEMAPHORE2_H

#include <atomic>
#include <mutex>
#include <string>
struct Process{
	//not reliable for short living process, please do not use alone
	uint32_t pid = 0;
	//two process can not start at the same exact time, we rely on this to surely identify each one

	//int clock_gettime(clockid_t clk_id, struct timespec *tp);
	uint64_t startTime =  0;
};

class Semaphore2 {
public:
	Semaphore2() = default;
	bool setPath(const std::string& _path);
	std::string getPath();
	bool init(uint16_t maxResources, const std::string& _poolName = std::string());
	const char *getRevision() const noexcept;
	std::string getPoolName() const;
	void setPoolName(const std::string &value);

	/**
	 * @brief lock will block until a lock can be acquired
	 * @param maxWait in ns, default 1 sec (10^9)
	 * @return true if locked false if expired
	 */
	bool lock(uint64_t maxWait = 1000000000);
private:
	bool openCreateGeneralLock();

	//the base path where all the lock file will be placed (and also the shared db stuff)
	std::string basePath;
	//probably never used, is the name in case we share folder...
	std::string poolName;
	std::atomic<int32_t> arraySize = 0;
	std::atomic<int32_t> maxResources = 0;
	std::mutex shmopMutex;
	int generalLockFD = 0;
	/**
	 * @brief process is the array that has the info of the running/subscribed process, is dinamically extended via shmop
	 * we do not really care now of having gap of this one growing too big
	 */
	Process* process;
};
#endif // SEMAPHORE2_H
