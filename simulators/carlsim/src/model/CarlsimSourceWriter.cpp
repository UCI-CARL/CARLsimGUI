
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

/*
if (carlsimConfig->generator > 0) {
	auto cpp = fopen("csgen\\main.cpp", "w");
	auto grp_h = fopen("csgen\\groups.h", "w");
	auto conn_h = fopen("csgen\\connections.h", "w");
	auto gen_h = fopen("csgen\\generators.h", "w");
	auto del_h = fopen("csgen\\delete.h", "w");
	fclose(cpp);
	fclose(grp_h);
	fclose(conn_h);
	fclose(gen_h);
	fclose(del_h);
}
*/

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

	// Ok, iterate over enum with a fore
	CarlsimSourceWriter(Main, false);
	CarlsimSourceWriter(Groups, false);
	CarlsimSourceWriter(Connections, false);
	CarlsimSourceWriter(Generators, false);
	CarlsimSourceWriter(Deletes, false);
	CarlsimSourceWriter(Monitors, false);
	CarlsimSourceWriter(Events, false);


	//const char* p = dir.absoluteFilePath("main.cpp").toStdString().c_str(); 

 //   // Concatenate strings using std::string and operator+
 //   std::string filePath = Dir + "/" + Name + "/" + "main.cpp";
 //   const char* x = filePath.c_str();

/*
	auto cpp = fopen(dir.absoluteFilePath("main.cpp").toStdString().c_str(), "w");
	auto grp_h = fopen(dir.absoluteFilePath("groups.h").toStdString().c_str(), "w");
	auto conn_h = fopen(dir.absoluteFilePath("connections.h").toStdString().c_str(), "w");
	auto gen_h = fopen(dir.absoluteFilePath("generators.h").toStdString().c_str(), "w");
	auto del_h = fopen(dir.absoluteFilePath("delete.h").toStdString().c_str(), "w");
	fclose(cpp);
	fclose(grp_h);
	fclose(conn_h);
	fclose(gen_h);
	fclose(del_h);
*/

}

void CarlsimSourceWriter::SetWrapper(spikestream::CarlsimWrapper* w) {
	Wrapper = w; 
	Generate = Wrapper->carlsimConfig->generator > 0; 
	Dir = "generated";  // "csgen"
	Name = Wrapper->carlsimConfig->netName.c_str();
};

spikestream::CarlsimWrapper* CarlsimSourceWriter::Wrapper = nullptr;

bool CarlsimSourceWriter::Generate = false; 

QString CarlsimSourceWriter::Dir = "";

QString CarlsimSourceWriter::Name = "";
