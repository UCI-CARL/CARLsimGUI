#ifndef TESTCARLSIMLIBRARY_H
#define TESTCARLSIMLIBRARY_H

//Qt includes
#include <QtTest>
#include <QString>

//#define DO_testCarlsimDLL1
#define DO_testCarlsimDLL2


class TestCarlsimLibrary : public QObject {
	Q_OBJECT

	private slots:
#ifdef DO_testCarlsimDLL1
	void testCarlsimDLL1();
#endif
#ifdef DO_testCarlsimDLL2
	void testCarlsimDLL2();
#endif
};


#endif //TESTCARLSIMLIBRARY
