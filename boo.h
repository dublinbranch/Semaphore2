#ifndef BOO_H
#define BOO_H

#include <QMetaEnum>
#include <QMetaType>
#include <qobject.h>

class MyClass : QObject {
	Q_GADGET

      public:
	enum class Priority : int{
		High = 2,
		Low = 5,
		VeryHigh,
		VeryLow
	};
	Q_ENUM(Priority)
	void srly();

	int m2() {
		return 1;
	}
};

#endif // BOO_H
