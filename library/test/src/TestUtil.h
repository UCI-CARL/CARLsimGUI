#ifndef TESTUTIL_H
#define TESTUTIL_H

//Qt includes
#include <QTest>

class TestUtil : public QObject {
    Q_OBJECT

    private slots:
	void test_rint(); // new
	void testFillSelectionArray();
	void testGetUIntList();
	void testRDouble();
	void testRandom();

};


#endif//TESTUTIL_H
