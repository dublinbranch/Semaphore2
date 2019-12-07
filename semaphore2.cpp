#include "semaphore2.h"
#include <QDebug>
#include <QDir>
#include <filesystem>
#include <time.h>
#include <unistd.h>
namespace fs = std::filesystem;
bool Semaphore2::setPath(const std::string& _path) {
	//TODO check if this is a valid path
	if (!basePath.empty()) {
		throw "too late to change path";
	}
	basePath = _path;
}

std::string Semaphore2::getPath() {
	if (basePath.empty()) {
		//if no path just use CWD folder, and create a subone else use the specified one
		//max path lenght https://unix.stackexchange.com/a/32834/165087
		char buffer[4096];
		auto bufWritten = readlink("/proc/self/exe", buffer, 4096);
		if (bufWritten == 0) {
			throw "Impossible to read the path o.O";
		}
		fs::path myFilePath(std::string(buffer, bufWritten));
		basePath = myFilePath.parent_path();
	}
	return basePath;
}


bool Semaphore2::init(uint16_t maxResources, const std::string& _poolName) {
	setPoolName(_poolName);
	fs::create_directory(getPath() + "/" + getPoolName());

	//we now have to check if something exist in the directory
	//what can possibly be inside is the Semaphore2.db
	//and the microTime.lock

	//if the Semaphore2.db is present

	//and unlocked it means sistem has already been bootstrapped


	this->maxResources = maxResources;
	//TODO add some check if we have enought space left to create folder and file name!
	return true;
}

const char* Semaphore2::getRevision() const noexcept {
	static const char semaphore2_library_version[] = "1";
	static const char __xyz[]                      = "semaphore2_library_version 1";
	return semaphore2_library_version;
}

std::string Semaphore2::getPoolName() const {
	return poolName;
}

void Semaphore2::setPoolName(const std::string& value) {
	if (value.empty()) {
		poolName = "SemPool";
	} else {
		poolName = value;
	}
	poolName.append("_").append(getRevision());
}
