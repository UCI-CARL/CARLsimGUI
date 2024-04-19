//SpikeStream includes
#include "SpikeStreamMainWindow.h"
#include "SpikeStreamApplication.h"
#include "SpikeStreamException.h"
#include "Globals.h"
#include "GlobalVariables.h"
using namespace spikestream;

//Qt includes
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

//Other includes
#include <iostream>
#include <stdio.h>
using namespace std;



///*! Function to handle QDebug messages */
//void logMessageOutput(QtMsgType type, const char *msg){
//    switch (type) {
//	case QtDebugMsg:
//		cout<<"Debug: "<<msg<<endl;
//	break;
//	case QtWarningMsg:
//		QMessageBox::warning( 0, "Warning", msg);
//		fprintf(stderr, "Warning: %s\n", msg);
//	break;
//	case QtCriticalMsg:
//		QMessageBox::critical( 0, "Critical Error", msg);
//		fprintf(stderr, "Critical: %s\n", msg);
//	break;
//	case QtFatalMsg:
//		QMessageBox::critical( 0, "Fatal Error", msg);
//		fprintf(stderr, "Fatal: %s\n", msg);
//		abort();
//    }
//}



/*! Function to handle QDebug messages */
void logMessageOutput5(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
	int x, y;
	switch (type) {
		case QtDebugMsg:
			cout << "Debug: " << msg.toStdString() << "  (" << context.function << "#" << context.line << ")" << endl;
			break;
		case QtWarningMsg:
			fprintf(stderr, "Warning: %s\n", msg.toStdString().c_str());
			QMessageBox::warning(0, "Warning", msg);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s\n", msg.toStdString().c_str());
			QMessageBox::critical(0, "Critical Error", msg);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s\n", msg.toStdString().c_str());
			QMessageBox::critical(0, "Fatal Error", msg);
			abort();
	}
}



/*! Main method for SpikeStream that launches the application. */
int main( int argc, char ** argv ) {
	//Install message handler for logging
	//https://doc.qt.io/qt-5/qtglobal-obsolete.html
	//	qInstallMsgHandler(logMessageOutput);
	//-->
	qInstallMessageHandler(logMessageOutput5);
	
	//Patch "OpenGL ERROR(S): 0x0502: invalid operation;
	// https://stackoverflow.com/questions/43468522/qt-cant-find-right-opengl-api-to-use
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    spikestream::Globals::getNetworkDisplay()->initConnects();

	try{
		//Create QApplication
		SpikeStreamApplication spikeStrApp(argc, argv);

		//Start up main window of application
		SpikeStreamMainWindow *spikeStrMainWin = new SpikeStreamMainWindow();

		spikeStrMainWin->show();

		//Listen for window closing events
		spikeStrApp.connect( &spikeStrApp, SIGNAL(lastWindowClosed()), &spikeStrApp, SLOT(quit()) );

		//Execute application
		return spikeStrApp.exec();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Fatal error: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"SpikeStream has thrown an unknown exception and will now exit.";
		return EXIT_FAILURE;
	}
}

