
#ifndef RESULTSCURSOR_H
#define RESULTSCURSOR_H

#if defined(WIN32) || defined(WIN64)
	#include <Windows.h>
#else
	#include <sys/time.h>
	#include <ctime>
#endif

#include <QString>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QProcess>


class ResultsCursor
{
public:

	//! Reference to paraent or nullptr if root
	const ResultsCursor* parent;   
	const ResultsCursor* root;

	/* QFileInfo  https://doc.qt.io/qt-6/qfileinfo.htm
	Relevant public functions
	 bool isDir()  => Container
	 bool isFile   => Terminal object, in a container
	 setFile(file) => absolute or relative path,
	 QString absolutePath()

	 alternative std::filesystem::path
	*/
	//QFileInfo info;

	/* QDir https://doc.qt.io/qt-6/qdir.html
	  toNativeSeparators(pathName)
	  seperator()   specific
	  QDir mkpath  creates all neccessary parent dirs
	  https://doc.qt.io/qt-6/qdir.html#toNativeSeparators

	  Debug in VS by manually cast --> Signed up on Stack Overflow/ C++ Reference
	  // https://stackoverflow.com/questions/44763417/debug-qt-app-in-visual-studio-and-inspect-opaque-objects-e-g-qdir-or-qfileinfo
	  (Qt5Cored.dll!QDirPrivate*)dir->d_ptr.d
	  [Raw View]
	  +		m_filePath	results/E-Prop_Morris_2022-12-06_1106	Qt5Cored.dll!QString
	  +		m_filePath	C:/cockroach-ut3/build/install/plugins/simulation/results/E-Prop_Morris_2022-12-06_1106	Qt5Cored.dll!QString



	*/
	QDir* dir;    // set if is a container,

	/*! QFile  https://doc.qt.io/qt-6/qfile.html
		caches the file and manages its status, e.g. open close
	*/
	QFile* file;   // set if it s afile. parent is 


	// Container = Root = where results are stored by CARLsim or Relativ 
	/*
		relative with if parent is define create relative to it
	*/
	ResultsCursor(QString pathName = "results", ResultsCursor* parent = nullptr): file(nullptr) {     
		
		//Distiguise between root container and relative
		QString path;
		if (parent != nullptr) {
			// create a relative relative
			dir = new QDir(parent->dir->path());  // ensure copy
			path = dir->path() + "/" + pathName;
			root = parent->root;
		}
		else {
			// root container
			//dir = new QDir(QDir::current());
			dir = new QDir();
			path = pathName;
			root = this;
		}
		
		//invariant to root or relative
		//dir = new QDir(QDir::toNativeSeparators(path));
		//dir = new QDir();
		
		dir->setPath(path); 
		if(!dir->exists())
			if (!dir->mkpath(dir->absolutePath()))
				qWarning("Cannot create container %s", dir->absolutePath());
		printf("%s\n", dir->absolutePath().toStdString().c_str());
		//info = QFileInfo(dir->absolutePath());
	};

	ResultsCursor(ResultsCursor* parent, QString fileName): parent(parent), dir(nullptr) {
		assert(parent != nullptr);
		assert(!fileName.isEmpty());
		assert(parent->dir->exists());

		root = parent->root;
		
		auto path = parent->dir->path() + "/" + fileName;
		file = new QFile(path);

		// 1. QFile only returns a handle, but C interfaces required FILE*
		// 2. abs path is sufficient for existing routines, fopen/close can be handled best in that context
		// 3. needs to be written double: current and cursor, current = results (root)
		// 4. \sa  FILE* f = fdopen(dup(file->handle()), "wb")  https://stackoverflow.com/questions/9465727/convert-qfile-to-file
		//if (!file->open(QIODevice::ReadWrite)) {
		//	qWarning("Cannot create the file %s", file->fileName());
		//}



		//info = QFileInfo(*file);

	}
		
	~ResultsCursor() {
		if (dir)
			delete dir;  // Qt deletes by deref

		if (file) {
			if (file->isOpen())
				file->close();
			delete file;
		}
	};

	void openFile() {
		assert(file);  // is a file result

		// 1. QFile only returns a handle, but C interfaces required FILE*
		// 2. abs path is sufficient for existing routines, fopen/close can be handled best in that context
		// 3. needs to be written double: current and cursor, current = results (root)
		// 4. \sa  FILE* f = fdopen(dup(file->handle()), "wb")  https://stackoverflow.com/questions/9465727/convert-qfile-to-file
		if (!file->open(QIODevice::ReadWrite)) {
			qWarning("Cannot create the file %s", file->fileName());
		}
	}


	/*
	 high level usage interface, e.g. 
	 auto results = &ResultsCursor();
	 auto experimentResults = results->spanExperiment(expName, "E-Prop");
	 */
	std::unique_ptr<ResultsCursor> spanExperiment(QString name, QString type = "") {
		//FIXME time stamp
		
		//char* ts = "2022-12-06_1106";
		auto ts = QDateTime::currentDateTime().toString("yyyy-MM-dd_HHmm");

		// e.g. E-Prop_Morris_2021-12-07_1102";
		QString pathName(type + (type.isEmpty()?"":"_") + name + "_" + ts);   
		
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(pathName, this));
	}

	/*
	  auto segmentResults = segmentResults->spanDir("1_tain");
	 */
	std::unique_ptr<ResultsCursor> spawnDir(QString pathName) {
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(pathName, this));
	}

	/*
	 auto loopResults = experimentResults->spanLoop(1);
	 */
	std::unique_ptr<ResultsCursor> spawnLoop(int loop) {
		auto pathName = QString::asprintf("loop_%02d", loop); 
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(pathName, this));
	}

	/*
	  auto segmentResults = experimentResults->spanSegment(0);
	 */
	std::unique_ptr<ResultsCursor> spawnSegment(int segment_i) {
		auto pathName = QString::asprintf("segment_%02d", segment_i);
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(pathName, this));
	}

	/*
	  auto landmarkResults = phaseResults->spanLandmark(0);
	 */
	std::unique_ptr<ResultsCursor> spawnLandmark(int landmark_i) {
		auto pathName = QString::asprintf("landmark_%02d", landmark_i);
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(pathName, this));
	}

	/* Morris
	  auto sessionResults = phaseResults->spanLandmark(0);
	 */
	std::unique_ptr<ResultsCursor> spawnSession(int landmark_i) {
		auto pathName = QString::asprintf("landmark_%02d", landmark_i);
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(pathName, this));
	}




	/*
	  auto fileResults = segmentResults->spanFile("path.txt");
	 */
	std::unique_ptr<ResultsCursor> spawnFile(QString fileName) {
		return std::unique_ptr<ResultsCursor>(new ResultsCursor(this, fileName));
	}

	///* do not close it, see https://stackoverflow.com/a/16876527
	//*/
	//FILE* getCFilePointer() {
	//	FILE *f = fdopen(file->handle(), "w"); 
	//	return f;
	//}

	//const char* getFilePath() {
	//	auto fileName = file->fileName();
	//	return fileName.toStdString().c_str();
	//}

	void getFilePath(std::string &path) {	
		path = file->fileName().toStdString();
	}


	void exec(QString command) {
		//save context
		auto current = QDir::current(); 
		//set context for script
		//FIXME try catch
		QDir::setCurrent(dir->absolutePath());
		system(command.toStdString().c_str());
		//restore context
		QDir::setCurrent(current.absolutePath());
	}

	////arguments = QStringList() << "arg1";
	//void exec(QString program, QStringList arguments) {
	//	QProcess process;
	//	QString wd = dir->absolutePath();
	//	process.setWorkingDirectory(wd);
	//	//process.startCommand(command);  // Qt 6
	//	//bool ok = process.startDetached(command);  
	//	process.setArguments(arguments);
	//	process.setProgram(program); 
	//	qDebug() << "starting process..." << endl;
	//	process.start();
	//	if (process.waitForStarted()) {
	//		qDebug() << "process started." << endl;
	//	}
	//}

private:


};


/*

// Template to avoid Subclass,  .. 

ResultsCursor<Type> {

Type QDir

Type QFile

--> Wie def. Template, ... dass Interface nutzbar ist???
containsn ??

Polymorüy 

ResultsCursor<QDir> {

  return <type>
}

}

*/

#endif // RESULTSCURSOR_H