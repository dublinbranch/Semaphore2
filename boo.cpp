#include "boo.h"

void MyClass::srly() {
	auto m1 = this->staticMetaObject.methodCount();
	auto m  = this->staticMetaObject;


	MyClass b;

	const QMetaObject* metaObject = &b.staticMetaObject;
	QStringList methods;
	for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
		methods << QString::fromLatin1(metaObject->method(i).methodSignature());


	auto en = m.enumerator(m.indexOfEnumerator("Priority"));
	auto vv = en.valueToKey(2);
	int  x;



	return;
}
