#include "semaphore2.h"
//#include <QDir>
//#include <QSemaphore>
#include <iostream>
#include <sys/file.h>

int main() {

//	QElapsedTimer timer;
//	timer.start();
//	int fd = open("file.lock", O_CREAT | O_RDWR, 0666);
//	if (fd == -1) {
//		qDebug() << "error opening";
//		exit(1);
//	}
//	auto t1 = timer.nsecsElapsed();

//	if (flock(fd, LOCK_SH | LOCK_NB) == -1) {
//		qCritical() << "is already locked, I refuse to start.\n"
//		               "(The application is already running.)";
//	}
//	auto t2 = timer.nsecsElapsed();

//	std::cin.get();
//	qDebug() << t1 << t2;

//	exit(0);
	try {
		Semaphore2 sem;
		sem.init(50,"geppetto");
		using namespace std::chrono_literals;
		sem.acquire(1s);
		sem.release();

	} catch (const std::exception& e) {
		printf("%s",e.what());
	}

//	auto f   = fopen("testfile", "w+");
//	auto fNo = fileno(f);
	//	ftruncate(fNo, 1);
	//	using namespace mmap_allocator_namespace;
	//	mmappable_vector<int, mmap_allocator<int>> v =
	//	    mmappable_vector<int, mmap_allocator<int>>(mmap_allocator<int>("testfile", READ_WRITE_SHARED, 0, ALLOW_REMAP));

	//	for (int var = 0; var < 50; ++var) {
	//		v.push_back(var);
	//	}

	//resize to accomodate whole vector
	//	ftruncate(fNo, v.size() * sizeof(int));
}

//		auto shm = QSharedMemory("/tmp/shmop1");
//		shm.create(1024);
//		shm.attach();

//		qDebug() << shm.errorString() << shm.nativeKey();
//		auto ptr = shm.data();

//	{
//		int   a = 3;
//		float b = 6.412355;
//		printf("%.*f\n", a, b);
//	}
//	{
//		int X::*ptiptr = &X::a;
//		double X::*b   = &X::b;
//		char X::*c     = &X::c;
//		char X::*d     = &X::d;

//		void (X::*ptfptr)(int) = &X::f;

//		X xobject;

//		// initialize data member
//		xobject.*ptiptr = 10;
//		xobject.*c      = 5;

//		// call member function
//		(xobject.*ptfptr)(20);
//	}
//	float            a = 6;
//	ieee754_float_t* f = (ieee754_float_t*)&a;
//	f->negative        = 1;

//	float b = a;
//	exit(1);

//	exit(1);
//	QDateTime ciao = QDateTime::currentDateTime();
//	QTimeZone tz("UTC+03:00");
//	QTimeZone tz2("UTC+03:00");
//	ciao       = ciao.toTimeZone(tz);
//	printf("%s\n",ciao.toString(Qt::DateFormat::ISODate).toUtf8().constData());

//	uint control = 15;
//	switch (control) {
//	case 12 ... 20:
//	case 25 ... 40:
//	case 100 ... 101:
//		break;
//	case 22 ... 24:
//	default:
//		break;
//	}
//	//Definisco la lista che voglio
//	std::vector<char> lista = {'a', 'b', 'c', 'd'};
//	int               max   = 1 << lista.size(); //con 4 input il max vale 16
//	for (int i = 0; i < max; i++) {
//		QByteArray line;
//		for (int j = 0; j < lista.size(); j++) {
//			int mask = 1 << j;
//			if ((i & mask) != 0) {
//				line.append(lista[j]);
//			}
//		}
//		if (line.size() > 1) {
//			printf("%s\n",line.constData());
//			qDebug() << line;
//		}
//	}
//	//printf("%u\n",req);

//#include <QDebug>
//#include "boo.h"

//#include <string>

//int decoder(int a, QByteArray b, QByteArray* res){
//	qDebug()  << b;
//	res->append(b);
//	return 0;
//}

//template<typename T>
//int ipoteticaFunzioneCheSCarica(T callback, QString url){
//	//famo finta che scarichiamo i dati da url
//	QByteArray datiScaricati = "geppetto";

//	callback(datiScaricati);

//}

//using namespace std::placeholders;

//int main(int argc, char* argv[]) {
//		int a = 3;
//	QByteArray res;

//	auto call = std::bind(decoder, a, _1, &res);

//	ipoteticaFunzioneCheSCarica(call, "seisho.us");

//}

//#include <QByteArray>
//#include <QDateTime>
//#include <QDebug>
//#include <any>
//#include <errno.h>
//#include <sys/mman.h>

//void free(void* ptr){
//    //nada
//};

//void* calloc(size_t nmemb, size_t size) {
//	return malloc(nmemb * size);
//};

//void* realloc(void* ptr, size_t size) {
//	auto neu = malloc(size);
//	memcpy(neu, ptr, size);
//	return neu;
//}

//uint req = 0;
//void* malloc(size_t __size) {
//	static void*  chunk  = nullptr; //dove inizerà la memoria
//	static size_t offset = 0;       //fino a dove la è mem già allocata

//	if (!chunk) {                   //alloco un blocco
//		chunk = mmap(nullptr, 1024 * 1024 * 10, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//		if (chunk == MAP_FAILED) {
//			auto err = errno;
//			auto msg = strerror(err);
//			return 0;
//		}
//	}
//	auto baseAddr = (char*)chunk + offset;
//	offset += __size;
//	req++;
//	return baseAddr;
//};

//#include <QTimeZone>
//struct point {
//	int x, y, z;
//};
//typedef union {
//	float value;
//	struct {
//		unsigned int mantissa : 23;
//		unsigned int exponent : 8;
//		unsigned int negative : 1;
//	};
//} ieee754_float_t;
//#include <stdio.h>
//class X {
//      public:
//	int    a;
//	double b;
//	char   c;
//	char   d;
//	char   e;
//	void   f(int b) {
//                printf("The value of b is %d \n", b);
//	}
//};
