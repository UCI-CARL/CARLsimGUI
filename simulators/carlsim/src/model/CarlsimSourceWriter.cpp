
#include "CarlsimSourceWriter.h"

#include "CarlsimWrapper.h"


CarlsimSourceWriter::CarlsimSourceWriter(Content content, bool append): content(content) {
	QDir dir(Dir + "/" + Name);
	switch (content) {
		case Main: file = fopen(dir.absoluteFilePath("main.cpp").toStdString().c_str(), append ? "a" : "w"); break;
		case Groups: file = fopen(dir.absoluteFilePath("groups.h").toStdString().c_str(), append ? "a" : "w"); break;
		case Connections: file = fopen(dir.absoluteFilePath("connections.h").toStdString().c_str(), append ? "a" : "w"); break;
		case Generators: file = fopen(dir.absoluteFilePath("generators.h").toStdString().c_str(), append ? "a" : "w"); break;
		case Deletes: file = fopen(dir.absoluteFilePath("deletes.h").toStdString().c_str(), append ? "a" : "w"); break;
		case Events: file = fopen(dir.absoluteFilePath("events.h").toStdString().c_str(), append ? "a" : "w"); break;
		case Monitors: file = fopen(dir.absoluteFilePath("monitors.h").toStdString().c_str(), append ? "a" : "w"); break;
	}
}

CarlsimSourceWriter::~CarlsimSourceWriter() {
	if (file) 
		fclose(file);
}


const QString CarlsimSourceWriter::ContainerPath() {
	QDir dir(Dir + "/" + Name);
	return dir.absolutePath();
}

void CarlsimSourceWriter::TouchFiles() {
	if (!Generate)
		return; // wp violation

	// ensure container directory 
	QDir dir(Dir + "/" + Name);
	if (!dir.exists()) 
		dir.mkpath(".");
	if(!dir.exists())
		return;

	// Ok, iterate over enum with a for
	CarlsimSourceWriter(Main, false);
	CarlsimSourceWriter(Groups, false);
	CarlsimSourceWriter(Connections, false);
	CarlsimSourceWriter(Generators, false);
	CarlsimSourceWriter(Deletes, false);
	CarlsimSourceWriter(Monitors, false);
	CarlsimSourceWriter(Events, false);


}

void CarlsimSourceWriter::SetWrapper(spikestream::CarlsimWrapper* w) {
	Wrapper = w; 
	Generate = Wrapper->carlsimConfig->generator > 0; 
	Generator = Wrapper->carlsimConfig->generator;
	Dir = "generated";  // "csgen"
	Name = Wrapper->carlsimConfig->netName.c_str();
};

spikestream::CarlsimWrapper* CarlsimSourceWriter::Wrapper = nullptr;

bool CarlsimSourceWriter::Generate = false; 

int CarlsimSourceWriter::Generator = 0; // NONE_GEN;

QString CarlsimSourceWriter::Dir = "";

QString CarlsimSourceWriter::Name = "";
