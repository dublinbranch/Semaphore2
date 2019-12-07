#include "semaphore2.h"
#include <atomic>
#include <exception>
#include <filesystem>
#include <mutex>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <system_error>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

class SHMMutex {
      private:
	pthread_mutex_t _handle;

      public:
	explicit SHMMutex() = default;
	void init();
	virtual ~SHMMutex();

	void lock();
	void unlock();
	bool tryLock();
};

void SHMMutex::init() {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP);

	if (pthread_mutex_init(&_handle, &attr) == -1) {
		throw "Unable to create mutex";
	}
}
SHMMutex::~SHMMutex() {
	::pthread_mutex_destroy(&_handle);
}
void SHMMutex::lock() {
	if (::pthread_mutex_lock(&_handle) != 0) {
		throw "Unable to lock mutex";
	}
}
void SHMMutex::unlock() {
	if (::pthread_mutex_unlock(&_handle) != 0) {
		throw "Unable to unlock mutex";
	}
}
bool SHMMutex::tryLock() {
	int tryResult = ::pthread_mutex_trylock(&_handle);
	if (tryResult != 0) {
		if (EBUSY == tryResult)
			return false;
		throw "Unable to lock mutex";
	}
	return true;
}

/**
 * @brief The SharedDB struct
 * think about it as a singleton, only between several process, and different time too
 */
struct SharedDB {
	static const uint32_t currentRevisionId = 7;
	uint32_t              revisionId;

	//We use atomic, because this code is shared between processes!

	//Note that there is NO DEFAULt VALUE (else on init they will be overwritten)
	//This is int to catch negative error / avoid overflow ?
	std::atomic<uint32_t> acquiredCount;
	std::atomic<uint32_t> maxResources;
	//TODO implement This is used to avoid recounting continuously. is useless!
	std::atomic<uint64_t> lastRecount;
	//to speed up lock recount we keep a list of pid that are subscribed to this sempaphore
	//Else we need to scan all process ! and on a busy server this is a ton of syscall and time
	//OFC this need to be at the end, because is dynamic

	//I have no idea if is possible to manipulate a list of fixed size with no mutex.
	SHMMutex mutex;

	//array is inited only to avoid warning, real dimension will be computed later, OFC size can not exceed allowed max resources!
	std::atomic<__pid_t> pidArray[1];

	void init(uint32_t max) {
		revisionId    = currentRevisionId;
		acquiredCount = 0;
		maxResources  = max;
		mutex.init();
		memset(&pidArray, 0, max * sizeof(__pid_t)); //I think is not needed but...
	}
	static uint32_t spaceRequired(uint32_t max) {
		return sizeof(SharedDB) + max * sizeof(__pid_t);
	}
};

/**
 * @brief Semaphore2::init
 * @param maxResources
 * @param _path TODO check a file path and not a folder
 * @return
 */
bool Semaphore2::init(uint32_t maxResources, const std::string& _path) {
	if (!_path.empty()) {
		fs::path path = _path;
		if (!path.has_filename()) {
			auto what = std::string("is not a file!");
			auto err  = std::make_error_code(errc::bad_address);
			throw fs::filesystem_error(what, path, err);
		}

		sharedLockName = fs::absolute(_path + ".shared");
		singleLockName = fs::absolute(_path + ".single");
	} else {
		sharedLockName = fs::absolute("sem2Lock.shared");
		singleLockName = fs::absolute("sem2Lock.single");
	}

	singleLockFD = open(singleLockName.c_str(), O_CREAT | O_RDWR, 0666);
	if (singleLockFD == -1) {
		auto what = std::string("impossible to open ");
		auto err  = std::make_error_code(errc::io_error);
		throw fs::filesystem_error(what, singleLockName, err);
	}

	sharedLockFD = open(sharedLockName.c_str(), O_CREAT | O_RDWR, 0666);
	if (sharedLockFD == -1) {
		auto what = std::string("impossible to open ");
		auto err  = std::make_error_code(errc::io_error);
		throw fs::filesystem_error(what, singleLockName, err);
	}

	//we use the singleLockFD like a spinlock
	//Do not try to use LOCK_NB! You can remain here for hours...
	//also note the initialization part is very few microsecond

	//TODO this is still broken
	uint maxTry = 100;
	uint curTry = 0;
	do {
		curTry++;
		if (flock(singleLockFD, LOCK_EX | LOCK_NB) == 0) {
			break;
		}
		std::this_thread::yield();
	} while (curTry < maxTry);

	//Do the system is already bootstrapped ?
	struct stat buf;
	fstat(singleLockFD, &buf);
	bool bootStrapSharedDb = false;
	if (buf.st_size != SharedDB::spaceRequired(maxResources)) {
		//Initialization in our case is resize to accomodate the struct that need to be stored
		ftruncate(singleLockFD, SharedDB::spaceRequired(maxResources));
		bootStrapSharedDb = true;
	}

	//in any case we need to mmap, in theory is 15usec faster to mmap after we know system is already bootstrapped
	//But is irrelevant for a once off operaton
	auto mmapped = mmap(nullptr, SharedDB::spaceRequired(maxResources), PROT_READ | PROT_WRITE, MAP_SHARED, singleLockFD, 0);
	if (mmapped == MAP_FAILED) {
		auto err = errno;
		auto msg = strerror(err);
		throw std::string("impossible to mmap: ").append(msg);
	}

	sharedDB = static_cast<SharedDB*>(mmapped);
	if (sharedDB->revisionId != SharedDB::currentRevisionId) {
		printf("Revision mismatch, regenerating\n");
		bootStrapSharedDb = true;
	}
	if (sharedDB->maxResources != maxResources) {
		printf("maxResources mismatch, regenerating\n");
		bootStrapSharedDb = true;
	}
	if (bootStrapSharedDb) {
		new (mmapped) SharedDB();     //std constructor initialization stuff
		sharedDB->init(maxResources); //some... other stuff
	}

	//and unlocked so the other can start to run
	if (flock(singleLockFD, LOCK_UN) == -1) {
		throw "impossible to unlock, how is that even possible";
	}

	//TODO add some check if we have enought space left to create folder and file name!
	return true;
}

bool Semaphore2::acquire(const std::chrono::nanoseconds& maxWait) {
	//TODO check if monotonic bla bla bla
	std::chrono::high_resolution_clock timer;
	auto                               startTime = timer.now();

	bool firstLoop = true;
	while (true) {
		if (!firstLoop) {
			//check if this took too much time and just abandon the task
			auto curTime = timer.now();
			auto elapsed = curTime - startTime;
			if (elapsed > maxWait) {
				return false;
			}

			//here we can use a pthread_cond_signal instead of sleeping
			this_thread::sleep_for(1s);
		}

		sharedDB->mutex.lock();
		auto oldValue = sharedDB->acquiredCount.load();
		auto newValue = oldValue + 1;
		if (newValue >= sharedDB->maxResources) {
			//This part is INTENTIONALLY under the mutex, so we avoid having X process doing the same thing, all of them (except one at most) failing!!!
			sharedDB->acquiredCount = recount();
			firstLoop               = false;
			sharedDB->mutex.unlock();
			continue;
		} else {
			sharedDB->acquiredCount.store(newValue);
			auto curPid = getpid();
			for (pidPos = 0; pidPos < sharedDB->maxResources; ++pidPos) {
				if (sharedDB->pidArray[pidPos] == 0) { //find an empty space
					sharedDB->pidArray[pidPos] = curPid;
					break;
				}
			}
			if (flock(sharedLockFD, LOCK_SH | LOCK_NB) == 0) {
				//TODO error check -.-
			}
			sharedDB->mutex.unlock();
			return true;
		}
	}
}

void Semaphore2::release() {
	sharedDB->mutex.lock();
	sharedDB->acquiredCount--;
	//free the record
	sharedDB->pidArray[pidPos] = 0;
	if (flock(sharedLockFD, LOCK_UN) == 0) {
		//TODO error check -.-
	}
	sharedDB->mutex.unlock();
}

uint pmFuser(const std::string& _path, std::atomic<__pid_t> pidArray[], uint pidCount){
	//this function must run under the LOCKED MUTEX!!!
	uint stillUsed = 0;
	fs::path path = _path;
	for (uint pidPos = 0; pidPos < pidCount; ++pidPos) {
		if (pidArray[pidPos] != 0) { //find an empty space
			auto curPid = pidArray[pidPos].load();
			auto dir = fs::path ("/proc/"+ to_string(curPid) +"/fd/");
			if(!fs::exists(dir)){
				pidArray[pidPos] = 0;
				continue;
			}
			for(auto&& file: fs::directory_iterator(dir)){
				cout << file << "\n";
				auto target = fs::read_symlink(file);
				if(target == path){
					stillUsed++;
				}
			}
		}
	}
	return stillUsed;
}


uint32_t Semaphore2::recount() {
	chrono::high_resolution_clock timer;
	auto                          now = timer.now();
	//Poor man lslocks, on a production server of ours
	//lsof takes 0.5 second, lslock... I terminated after 5 second (non busy server)
	//So is totally unconceivable to run the stock version...
	//There is also fuser which takes 0.17 on my machine and 0.28 on prod server (non busy)
	//for fuser systime is > 80% of total...


	sharedDB->lastRecount             = chrono::time_point_cast<chrono::nanoseconds>(now).time_since_epoch().count();

	pmFuser(sharedLockName,sharedDB->pidArray, sharedDB->maxResources.load());
}

