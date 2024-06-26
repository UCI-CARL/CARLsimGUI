//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestArchiveDao.h"
#include "ArchiveDao.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;



/*----------------------------------------------------------*/
/*-----                    TESTS                       -----*/
/*----------------------------------------------------------*/

void TestArchiveDao::testAddArchive(){
    //Add a test network so we get a valid network id
    addTestNetwork1();

    //Create the archive dao
    ArchiveDao archiveDao (archiveDBInfo);

    //Information about the archive to be added
    ArchiveInfo archInfo(0, testNetID, 1212121, "Test archive 1");
    try{
		//Invoke method that is being tested
		archiveDao.addArchive(archInfo);

		//Check that archive has been added
		QSqlQuery query = getArchiveQuery("SELECT NetworkID, StartTime, Description FROM Archives");
		executeQuery(query);

		//Should only be one row
		QCOMPARE(query.size(), 1);
		query.next();

		//Check details of archive
		QVERIFY( archInfo.getID() != 0);
		QCOMPARE(query.value(0).toUInt(), testNetID);
		unsigned timeStamp = query.value(1).toUInt();
		QVERIFY( timeStamp > (QDateTime::currentDateTime().toTime_t() - 10) );
		QCOMPARE(query.value(2).toString(), QString("Test archive 1"));
    }
    catch(SpikeStreamException& ex){
		QFAIL(ex.getMessage().toLatin1());
    }
}


void TestArchiveDao::testAddArchiveData(){
    //Add test archive without data
    addTestNetwork1();//Not automatically added by addTestArchive2() method
    addTestArchive2();

    //Create the archive dao
    ArchiveDao archiveDao (archiveDBInfo);

    //Invoke method being tested
    try{
		archiveDao.addArchiveData(testArchive2ID, 1, "12,13,14,15");
		archiveDao.addArchiveData(testArchive2ID, 3, "121,131,141,151");
    }
    catch(SpikeStreamException ex){
		QFAIL(ex.getMessage().toLatin1());
    }

    //Check data was added correctly
    QSqlQuery query = getArchiveQuery("SELECT ArchiveID, TimeStep, FiringNeurons FROM ArchiveData");
    executeQuery(query);

    //Should only be two rows
    QCOMPARE(query.size(), 2);

    //Check the rows
    query.next();
    QCOMPARE(query.value(0).toUInt(), testArchive2ID);
    QCOMPARE(query.value(1).toUInt(), (unsigned int)1);
    QCOMPARE(query.value(2).toString(), QString("12,13,14,15"));

    query.next();
    QCOMPARE(query.value(0).toUInt(), testArchive2ID);
    QCOMPARE(query.value(1).toUInt(), (unsigned int)3);
    QCOMPARE(query.value(2).toString(), QString("121,131,141,151"));
}


void TestArchiveDao::testDeleteArchive(){
    //Add a test archive
    addTestArchive1();

    //Check that test archive is in database
    QSqlQuery query = getArchiveQuery("SELECT * FROM Archives WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be a single network
    QCOMPARE(query.size(), (int)1);

    //Check that archive data is in database
    query = getArchiveQuery("SELECT * FROM ArchiveData WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be a three time steps
    QCOMPARE(query.size(), (int)3);

    //Invoke method being tested
    ArchiveDao archiveDao(archiveDBInfo);
    archiveDao.deleteArchive(testArchive1ID);

    //Check to see if archive with this id has been removed from the database
    query = getArchiveQuery("SELECT * FROM Archives WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be no archive
    QCOMPARE(query.size(), (int)0);

    //Check that delete has cascaded properly
    query = getArchiveQuery("SELECT * FROM ArchiveData WHERE ArchiveID = " + QString::number(testArchive1ID));
    executeQuery(query);

    //Should be no time steps
    QCOMPARE(query.size(), (int)0);
}


void TestArchiveDao::testGetArchivesInfo(){
    //Add two test archives
    addTestArchive1();
    addTestArchive2();

    //Check that the information returned is correct
    ArchiveDao archiveDao(archiveDBInfo);
    QList<ArchiveInfo> archInfoList = archiveDao.getArchivesInfo(testNetID);

    //Should be two archives
    QCOMPARE(archInfoList.size(), (int)2);

    //Check details from database
    QCOMPARE(archInfoList[0].getDescription(), QString("testArchive1Description"));
    QCOMPARE(archInfoList[1].getDescription(), QString("testArchive2Description"));
    QCOMPARE(archInfoList[0].getDateTime(), QDateTime::fromTime_t(1011));
    QCOMPARE(archInfoList[1].getDateTime(), QDateTime::fromTime_t(2022211));
    QCOMPARE(archInfoList[0].getNetworkID(), testNetID);
    QCOMPARE(archInfoList[1].getNetworkID(), testNetID);
}


void TestArchiveDao::testGetArchiveSize(){
    //Add a test archive
    addTestArchive1();

    //Should be three rows
    ArchiveDao archiveDao(archiveDBInfo);
    int archiveSize = archiveDao.getArchiveSize(testArchive1ID);
    QCOMPARE(archiveSize, (int)3);
}


void TestArchiveDao::testGetFiringNeuronIDs(){
    //Add a test archive
    addTestArchive1();

    ArchiveDao archiveDao(archiveDBInfo);

    //Check that correct IDs are returned for each time step
	QList<unsigned> firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 1);
    QCOMPARE(firingNeuronIDs.size(), (int)4);
	QCOMPARE(firingNeuronIDs[0], (unsigned)256);
	QCOMPARE(firingNeuronIDs[1], (unsigned)311);
	QCOMPARE(firingNeuronIDs[2], (unsigned)21);
	QCOMPARE(firingNeuronIDs[3], (unsigned)4);

    firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 2);
    QCOMPARE(firingNeuronIDs.size(), (int)3);
	QCOMPARE(firingNeuronIDs[0], (unsigned)22);
	QCOMPARE(firingNeuronIDs[1], (unsigned)31);
	QCOMPARE(firingNeuronIDs[2], (unsigned)4888888); //exceeds 2097151 (b111111111111111111111)
	
    firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 3);
    QCOMPARE(firingNeuronIDs.size(), (int)0); // \remark QWARN is correct and to be ignored (QSqlQuery::value: not positioned on a valid record) 

    firingNeuronIDs = archiveDao.getFiringNeuronIDs(testArchive1ID, 5);
    QCOMPARE(firingNeuronIDs.size(), (int)1);
	QCOMPARE(firingNeuronIDs[0], (unsigned)3);
}



void TestArchiveDao::testGetMaxTimeStep(){
    //Add a test archive
    addTestArchive1();

    ArchiveDao archiveDao(archiveDBInfo);

    //Check maximum time step is correct
    unsigned int maxTimeStep = archiveDao.getMaxTimeStep(testArchive1ID);
    QCOMPARE(maxTimeStep, (unsigned int)5);
}


void TestArchiveDao::testNetworkIsLocked(){
    //Add test network without any archive data
    addTestNetwork1();

    //Network should be unlocked
    ArchiveDao archiveDao(archiveDBInfo);
	QCOMPARE(archiveDao.networkHasArchives(testNetID), false);

    //Add archive
    addTestArchive2();

    //Network should be locked
	QCOMPARE(archiveDao.networkHasArchives(testNetID), true);
}


