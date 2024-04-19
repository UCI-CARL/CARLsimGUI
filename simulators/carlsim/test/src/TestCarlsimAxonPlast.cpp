
#include "TestCarlsimAxonPlast.h"

//SpikeStream includes
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cassert>

#define DEBUG true

#include "carlsim.h"
#include "carlsim_datastructures.h"
#include "stopwatch.h"

#include "ResultsCursor.h"

void TestCarlsimAxonPlast::testResultsCursor() {

	auto results = ResultsCursor();

	// std::unique_ptr<ResultsCursor>
	auto experimentResults = results.spanExperiment("Morris", "E-Prop");

	auto phaseResults = experimentResults->spawnDir("1_train");

	auto loopResults = phaseResults->spawnLoop(1);

	auto segmentResults = loopResults->spawnSegment(0);

	auto fileResults = segmentResults->spawnFile("path.txt");

	//auto path = fileResults->file->fileName().toStdString(); // needs to be referenced

	std::string path; 
	fileResults->getFilePath(path);

	auto f = fopen(path.c_str(), "w");
	if (f) {
		fprintf(f, "8;3;1;");
		fclose(f);
	}

	//cause the unique ptr to loose its reference and force a call to the destructor 
	fileResults = nullptr; 

	
	auto loop = 3; auto total_loss = 3500;
	auto trainingResults = phaseResults->spawnFile("training.txt");
	trainingResults->openFile(); // w
	QTextStream trainingLog(trainingResults->file);
	trainingLog << loop << " " << total_loss << endl;


	// assert file exists and has content
	printf("end\n");


}




void TestCarlsimAxonPlast::test1() {

	printf("test1\n");

	//QString route = "(4,12),(12,12),(12,6),(9,6),(9,4),(11,3),(11,2),(7,2),(3,2),(3,5),(7,5),(7,8),(4,8)";
	// with \s
	//QString route = "(4, 12), (12, 12), (12, 6), (9, 6), (9, 4), (11, 3), (11, 2), (7, 2), (3, 2), (3, 5), (7, 5), (7, 8), (4, 8)"; 
	QString route = "(4,12);(12,12);(12,6);(9,6);(9,4);(11,3);(11,2);(7,2);(3,2);(3,5);(7,5);(7,8);(4,8);(4,12);";

	// route trim all blanks (normalize)
	// next chunk upto ','   or end?
	// regexp scan next "\\((\d+),(\d+)\\)

	// QString split (';')
	// for each regex
	//QStringList segments = route.split(QRegularExpression("\s*\(\s*\d+\,\d+\)\s*,?"));  
	//QStringList segments = route.split(QRegularExpression("\(.*\),?"));

	QStringList segments = route.split(';', Qt::SkipEmptyParts);  // ignore redundant separator
	QCOMPARE(segments.size(), 13);

	foreach(auto segment, segments) {
		QRegularExpression re(R"(\((\d+)\,(\d+)\))");
		auto result = re.match(segment);
		if (result.hasMatch()) {
			//auto x = result.capturedTexts[1];
			//auto y = result[2];
			auto x = result.captured(1);
			auto y = result.captured(2);
		}

	}

	printf("done\n");

}

