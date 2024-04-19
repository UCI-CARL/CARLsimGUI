//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "SpikeStreamMainWindow.h"
#include "GlobalVariables.h"
#include "ConfigLoader.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "NetworksWidget.h"
#include "ArchiveWidget.h"
#include "NeuronGroupWidget.h"
#include "ConnectionWidget.h"
#include "NetworkViewer.h"
#include "NetworkViewerProperties.h"
#include "SimulationLoaderWidget.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <qsplitter.h>
#include <qsplashscreen.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qfile.h>
#include <QCloseEvent>
#include <QDebug>
#include <QScrollArea>
#include <QStatusBar>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>

#include <QDockWidget>
#include <QFlags>
#include <QListWidget>

//Other includes
#include <string>
using namespace std;



/*! Constructor. */
SpikeStreamMainWindow::SpikeStreamMainWindow() : QMainWindow(), fromNeuronIdEdit(NULL), toNeuronIdEdit(NULL) {
	//setWindowTitle("SpikeStream");
	setWindowTitle("CARLsimGUI");

	try{
		//Get the working directory and store in global scope
		QString rootDir = Util::getRootDirectory();
		Globals::setSpikeStreamRoot(rootDir);

		// QSettings
		QCoreApplication::setOrganizationName("UCI");
		//QCoreApplication::setApplicationName("SpikeStream");
		QCoreApplication::setApplicationName("CARLsimGUI");
		//QCoreApplication::setOrganizationDomain("cortex2.org"); Not Used for Windows

		//Set up splash screen to give feedback whilst application is loading
		QSplashScreen *splashScreen = 0;
		//QPixmap splashPixmap(Globals::getSpikeStreamRoot() + "/images/spikestream_splash.png");
		QPixmap splashPixmap(Globals::getSpikeStreamRoot() + "/images/carlsimgui_splash.png");
		splashScreen = new QSplashScreen( splashPixmap );
		splashScreen->show();

Globals::setMainWindow(this);

		//Set up application and GUI
		initializeApplication();

		splashScreen->showMessage("loading plugins..", Qt::AlignLeft|Qt::AlignBottom, Qt::white);

		//https://doc.qt.io/qt-5/qsplashscreen.html
		//Get rid of splash screen if it is showing
		if(splashScreen){
			splashScreen->finish( this );
			delete splashScreen;
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		exit(1);
	}
	catch(...){
		qCritical()<<"An unknown exception occurred setting up application.";
		exit(1);
	}
}


/*! Destructor. */
SpikeStreamMainWindow::~SpikeStreamMainWindow(){
	//Clean up globals - everything stored in globals is cleaned up by globals
	Globals::cleanUp();

	delete databaseManager;
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Overridden close to check that network is saved and simulation or analysis is not running. */
void SpikeStreamMainWindow::closeEvent (QCloseEvent* event){
	if(databaseManager->isRunning()){
		qWarning()<<"Database manager is still running, it is recommended that you wait for it to complete";
		event->ignore();
		return;
	}

	//Nothing will be running or unsaved if no network is loaded
	if(!Globals::networkLoaded()) {
		event->accept();
		return;
	}

	//Check for simulation loaded
	if(Globals::isSimulationLoaded()){
//		qWarning()<<"SpikeStream should not be quit with a simulation loaded.\nUnload the simulation and try again.";
//		event->ignore();
qDebug()<<"CAUTION, Warning disabled"<<__FUNCTION__<<__FILE__<<__LINE__;
//		return;
	}

	//Check for analysis running
	if(Globals::isAnalysisRunning()){
		int response = QMessageBox::warning(this, "Quit SpikeStream?", "Analysis is running.\nAre you sure that you want to quit SpikeStream?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok){
			event->ignore();
			return;
		}
	}

	//Check for network save
	if(!Globals::getNetwork()->isSaved()){
		int response = QMessageBox::warning(this, "Quit SpikeStream?", "Network has not been saved.\nAre you sure that you want to quit SpikeStream?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok){
			event->ignore();
			return;
		}
	}

	// Layout Management
	if(this->property("configAutosaveLayout").value<bool>())
		saveLayout();


	//Clean up Globals
	Globals::cleanUp();

	//Accept event if checks have been passed.
	event->accept();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! Displays brief information about the application. */
void SpikeStreamMainWindow::about(){
	QMessageBox::about(this, "About", "CARLsim GUI 2024\n \nA graphical user interface for CARLsim\nsupporting custom Plug-ins.");
}


/*! Called when the database manager has finished its task */
void SpikeStreamMainWindow::databaseManagerFinished(){
	//Check for errors
	if(databaseManager->isError())
		qCritical()<<databaseManager->getErrorMessage();

	if(databaseManager->getTaskID() == DatabaseManager::CLEAR_DATABASES_TASK){
		//Inform other classes about the change
		Globals::setNetwork(NULL);
		Globals::getEventRouter()->reloadSlot();
		progressDialog->hide();
		delete progressDialog;
	}
}

// OBSOLETE with new Docking Layout
//
///*! Displays the analysis widget */
//void SpikeStreamMainWindow::showAnalysisWidget(){
////	tabWidget->setCurrentIndex(5);
//}
//
//
///*! Displays the archive widget */
//void SpikeStreamMainWindow::showArchiveWidget(){
////	tabWidget->setCurrentIndex(4);
//}
//
//
///*! Displays the editor widget */
//void SpikeStreamMainWindow::showEditorWidget(){
////	tabWidget->setCurrentIndex(1);
//}
//
//
///*! Displays the networks widget */
//void SpikeStreamMainWindow::showNetworkWidget(){
////	tabWidget->setCurrentIndex(0);
//}
//
//
///*! Displays the simulation widget */
//void SpikeStreamMainWindow::showSimulationWidget(){
////	tabWidget->setCurrentIndex(3);
//}
//
//
///*! Displays the viewer widget */
//void SpikeStreamMainWindow::showViewerWidget(){
////	tabWidget->setCurrentIndex(2);
//}


/*! Empties all databases except the Neuron, Synapse and Probe types. Useful when
	an error has led to out of date information in one of the tables. However all
	data will be lost if this method is called. */
void SpikeStreamMainWindow::clearDatabases(){
	//Confirm that user wants to take this action.
	QMessageBox msgBox;
	msgBox.setText("Clearing Database");
	msgBox.setInformativeText("Are you sure that you want to clear the database completely?\nAll data will be lost and this step cannot be undone.");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	if(ret != QMessageBox::Ok)
		return;

	//Instruct database manager to delete all networks
	progressDialog = new QProgressDialog("Clearing databases, please wait", "", 0, 0, this, Qt::CustomizeWindowHint);
	progressDialog->setWindowModality(Qt::WindowModal);
	progressDialog->setCancelButton(0);//Cannot implement cancel sensibly
	databaseManager->startClearDatabases();
	progressDialog->show();
}


/*! Loads databases from a file selected by the user.
	Launches database manager, which allows user to select which database to load. */
void SpikeStreamMainWindow::loadDatabases(){
	qWarning()<<"Load database method not implemented";
}


/*! Saves databases
	Allows user to choose file and then launches database manager so that user can choose
	which databases to save. */
void SpikeStreamMainWindow::saveDatabases(){
	qWarning()<<"Save databases method not implementd";
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

void SpikeStreamMainWindow::createActions(){

	//Actions
	//Add OpenGL actions
	QAction* moveUpAction = new QAction(this);
	moveUpAction->setShortcut(QKeySequence(Qt::Key_Up));
	connect(moveUpAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveUpSlot()));
	this->addAction(moveUpAction);

	QAction* moveDownAction = new QAction(this);
	moveDownAction->setShortcut(QKeySequence(Qt::Key_Down));
	connect(moveDownAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveDownSlot()));
	this->addAction(moveDownAction);

	QAction* moveLeftAction = new QAction(this);
	moveLeftAction->setShortcut(QKeySequence(Qt::Key_Left));
	connect(moveLeftAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveLeftSlot()));
	this->addAction(moveLeftAction);

	QAction* moveRightAction = new QAction(this);
	moveRightAction->setShortcut(QKeySequence(Qt::Key_Right));
	connect(moveRightAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveRightSlot()));
	this->addAction(moveRightAction);

	QAction* moveForwardAction = new QAction(this);
	moveForwardAction->setShortcut(QKeySequence(Qt::Key_PageUp));
	connect(moveForwardAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveForwardSlot()));
	this->addAction(moveForwardAction);

	QAction* moveBackwardAction = new QAction(this);
	moveBackwardAction->setShortcut(QKeySequence(Qt::Key_PageDown));
	connect(moveBackwardAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(moveBackwardSlot()));
	this->addAction(moveBackwardAction);

	QAction* resetViewAction = new QAction(this);
	resetViewAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	connect(resetViewAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(resetViewSlot()));
	this->addAction(resetViewAction);

	QAction* rotateUpAction = new QAction(this);
	rotateUpAction->setIconText(tr("Up")); // Toolbar
	rotateUpAction->setToolTip(tr("Rotate the Network upwards. Hotkey: Ctrl+Up"));
	rotateUpAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
	connect(rotateUpAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateUpSlot()));
	this->addAction(rotateUpAction);

	QAction* rotateDownAction = new QAction(this);
	rotateDownAction->setIconText(tr("Down")); 
	rotateDownAction->setToolTip(tr("Rotate the Network downwards. Hotkey: Ctrl+Down"));
	rotateDownAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
	connect(rotateDownAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateDownSlot()));
	this->addAction(rotateDownAction);

	QAction* rotateLeftAction = new QAction(this);
	rotateLeftAction->setIconText(tr("Left")); 
	rotateLeftAction->setToolTip(tr("Rotate the Network to the left. Hotkey: Ctrl+Left"));
	rotateLeftAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left));
	connect(rotateLeftAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateLeftSlot()));
	this->addAction(rotateLeftAction);

	QAction* rotateRightAction = new QAction(this);
	rotateRightAction->setIconText(tr("Right")); 
	rotateRightAction->setToolTip(tr("Rotate the Network to the right. Hotkey: Ctrl+Right"));
	rotateRightAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right));
	connect(rotateRightAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(rotateRightSlot()));
	this->addAction(rotateRightAction);

	//Simulation actions
	QAction* startStopSimulationAction = new QAction(this);
	startStopSimulationAction->setIcon(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"));  
	startStopSimulationAction->setShortcut(QKeySequence(Qt::Key_Space));
	connect(startStopSimulationAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(startStopSimulationSlot()));
	this->addAction(startStopSimulationAction);

	QAction* stepSimulationAction = new QAction(this);
	stepSimulationAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
	connect(stepSimulationAction, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(stepSimulationSlot()));
	stepSimulationAction->setIcon(QIcon(Globals::getSpikeStreamRoot() + "/images/step.png"));
	
	this->addAction(stepSimulationAction);

	//Show/hide all connection groups
	QAction* showAllNoneConnectionsActions = new QAction(this);
	showAllNoneConnectionsActions->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	connect(showAllNoneConnectionsActions, SIGNAL(triggered()), Globals::getEventRouter(), SLOT(showAllOrNoneConnectionsSlot()));
	this->addAction(showAllNoneConnectionsActions);


	// Alternative creation like TextEdit Demo (3 Lines instead of one!)
	rotationToolBar = new QToolBar(this); 
	rotationToolBar->setObjectName("rotation:ToolBar");
	rotationToolBar->setWindowTitle(tr("Rotation"));
	addToolBar(rotationToolBar);

	rotationToolBar->addAction(rotateRightAction);
	rotationToolBar->addAction(rotateLeftAction);
	rotationToolBar->addSeparator();
	rotationToolBar->addAction(rotateUpAction);
	rotationToolBar->addAction(rotateDownAction);


}

void SpikeStreamMainWindow::createMenus(){

	//Set up menus.
	//Add file menu.
	fileMenu = new QMenu(tr("&File"));  // SpikeStream 0.3, Qt4
	menuBar()->addMenu(fileMenu);
	fileMenu->addAction("Clear databases", this, SLOT(clearDatabases()));
	fileMenu->addSeparator();
	fileMenu->addAction("Quit", qApp, SLOT(closeAllWindows()), Qt::CTRL+Qt::Key_Q);

	//Add refresh menu for when changes have been made in the database independently of the application
	viewMenu = new QMenu(tr("&View")); // SpikeStream 0.3, Qt4
	menuBar()->addMenu(viewMenu);

	//Reload everything is broadcast to all classes connected to the event router
	viewMenu->addAction("Refresh", Globals::getEventRouter(), SLOT(refreshSlot()), Qt::Key_F5);

	//Reload everything is broadcast to all classes connected to the event router
	viewMenu->addAction("Reload everything", Globals::getEventRouter(), SLOT(reloadSlot()), Qt::CTRL + Qt::Key_F5);

	//! High end Layout Management
	windowMenu = menuBar()->addMenu(tr("Windows"));
	QMenu* layoutMenu = windowMenu->addMenu(tr("Layout"));
	layoutMenu->addAction(tr("Save"), this, SLOT(saveLayout()));
	layoutMenu->addAction(tr("Restore"), this, SLOT(restoreLayout()));
	layoutMenu->addAction(tr("Reset"), this, SLOT(resetLayout()));
	windowMenu->addSeparator();

	//Add help menu
	QMenu *helpMenu = new QMenu(tr("&Help")); // SpikeStream 0.3, Qt4
	menuBar()->addMenu(helpMenu);
	helpMenu->addAction("About", this , SLOT(about()), Qt::Key_F1);
}

void SpikeStreamMainWindow::createToolBars(){

}

void SpikeStreamMainWindow::createStatusBar(){
	
	// Status Bar  
	QStatusBar* bar = statusBar();
	bar->showMessage(tr("Ready"), 30000); 
	//QCheckBox* neuronIdButton = new QCheckBox("Neuron-Id:");
	//QPushButton* neuronIdButton = new QPushButton("Neuron-Id:");
	//neuronIdButton->setFlat(true);
	QToolButton* neuronIdButton = new QToolButton();
	neuronIdButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
	neuronIdButton->setAutoRaise(true);
	neuronIdButton->setText("Neuron-IDs");
	neuronIdButton->setToolTip(tr("Press this Button to display the ID of the selected Neuron."));
	neuronIdButton->setCheckable(true);
	neuronIdButton->setChecked(false);
	connect(neuronIdButton, SIGNAL(toggled(bool)), this, SLOT(connectNeuronId(bool)));
	fromNeuronIdEdit = new QLineEdit();
	toNeuronIdEdit = new QLineEdit();
	fromNeuronIdEdit->hide();	
	toNeuronIdEdit->hide();	
	QColor windowClr = QPalette().color(QPalette::Normal, QPalette::Window); 
	fromNeuronIdEdit->setStyleSheet(QString("*{background-color:rgb(%1,%2,%3);border:0px;}").arg(windowClr.red()).arg(windowClr.green()).arg(windowClr.blue())); 
	toNeuronIdEdit->setStyleSheet(QString("*{background-color:rgb(%1,%2,%3);border:0px;}").arg(windowClr.red()).arg(windowClr.green()).arg(windowClr.blue())); 
	connect(fromNeuronIdEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setSelectedFromNeuronId(const QString &)));
	connect(toNeuronIdEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setSelectedToNeuronId(const QString &)));

	fromNeuronIdEdit->setMaximumWidth(50);
	toNeuronIdEdit->setMaximumWidth(50);
	bar->addPermanentWidget(neuronIdButton);
	bar->addPermanentWidget(fromNeuronIdEdit);
	bar->addPermanentWidget(toNeuronIdEdit);
	
}


/*! Complete new design of the GUI 
*/
void SpikeStreamMainWindow::createDocking(ConfigLoader* configLoader){
	
	//Set up panes

	//windowMenu->addSeparator();

	// Note: ForceTabbedDocks has no effect on QMainWindow::addDockWidget !  --> use tabifyDockWidget
	// Note: If Nested is on, the Widget may move below the visible borders
	//setDockOptions(QMainWindow::ForceTabbedDocks | QMainWindow::AnimatedDocks);  // Nested may be an improvement with redesigned Widgets
	setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AnimatedDocks);  

	QDockWidget::DockWidgetFeatures features = 
		QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable
		; //| QDockWidget::DockWidgetVerticalTitleBar;

	Qt::DockWidgetAreas areas = Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea;



	//Set up networks tab
	QDockWidget* networksDock = new QDockWidget(tr("Networks"), this);
	networksDock->setObjectName("networks:DockWidget");
	NetworksWidget* networksWidget = new NetworksWidget(networksDock);  
	networksDock->setWidget(networksWidget);

	//
	networksDock->setFeatures(features);
	networksDock->setAllowedAreas(areas);

	addDockWidget(Qt::LeftDockWidgetArea, networksDock);
	windowMenu->addAction(networksDock->toggleViewAction());

	networksDock->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));  // Replaces 10 Lines of Code
	//networksDock->close();

	////Set up editor window
	//QDockWidget* editorDock = new QDockWidget(tr("Editor"), this);
	//QScrollArea* editorScroll = new QScrollArea(editorDock);
	//QSplitter *layerSplitterWidget = new QSplitter(editorScroll);
	//NeuronGroupWidget* neuronGrpWidget = new NeuronGroupWidget();
	//ConnectionWidget* conWidget = new ConnectionWidget();
	//layerSplitterWidget->addWidget(neuronGrpWidget);
	//layerSplitterWidget->addWidget(conWidget);
	//layerSplitterWidget->setOrientation(Qt::Vertical);
	//editorScroll->setWidget(layerSplitterWidget);
	//editorDock->setWidget(editorScroll);
	//editorDock->setFeatures(features);
	//editorDock->setAllowedAreas(areas);
	////addDockWidget(Qt::LeftDockWidgetArea, editorDock);	
	//tabifyDockWidget(networksDock, editorDock);
	//viewMenu->addAction(editorDock->toggleViewAction());
	//editorDock->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	////editorDock->close();


	//Set up editor window
	// Neurons
	QDockWidget* neuronDock = new QDockWidget(tr("Neuron-Groups"), this);
	neuronDock->setObjectName("neuron:DockWidget");
	//QScrollArea* neuronScroll = new QScrollArea(neuronDock);
	//NeuronGroupWidget* neuronGrpWidget = new NeuronGroupWidget(neuronScroll);
	//neuronScroll->setWidget(neuronGrpWidget);
	//neuronDock->setWidget(neuronScroll);
	NeuronGroupWidget* neuronGrpWidget = new NeuronGroupWidget(neuronDock);
	neuronDock->setWidget(neuronGrpWidget);
	neuronDock->setFeatures(features);
	neuronDock->setAllowedAreas(areas);
	//tabifyDockWidget(networksDock, neuronDock);
	addDockWidget(Qt::RightDockWidgetArea, neuronDock, Qt::Vertical); // show below	
	windowMenu->addAction(neuronDock->toggleViewAction());
	neuronDock->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
	//neuronDock->close();
	// Connections
	QDockWidget* connectionDock = new QDockWidget(tr("Synaptic-Connections"), this);
	connectionDock->setObjectName("connection:DockWidget");
	QScrollArea* connectionScroll = new QScrollArea(connectionDock);
	ConnectionWidget* conWidget = new ConnectionWidget(connectionScroll);
	connectionScroll->setWidget(conWidget);
	connectionDock->setWidget(connectionScroll);
	connectionDock->setFeatures(features);
	connectionDock->setAllowedAreas(areas);
	//tabifyDockWidget(networksDock, connectionDock);
	addDockWidget(Qt::RightDockWidgetArea, connectionDock, Qt::Vertical); // show below	
	windowMenu->addAction(connectionDock->toggleViewAction());
	connectionDock->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_2));
	//connectionDock->close();

	//Set up viewer tab
	QDockWidget* viewerDock = new QDockWidget(tr("Viewer"), this);
	viewerDock->setObjectName("viewer:DockWidget");
	QScrollArea* viewerScroll = new QScrollArea(viewerDock);
	NetworkViewerProperties* networkViewerProperties = new NetworkViewerProperties(viewerScroll);
	viewerScroll->setWidget(networkViewerProperties);
	viewerDock->setWidget(viewerScroll);
	viewerDock->setFeatures(features);
	viewerDock->setAllowedAreas(areas);
	//addDockWidget(Qt::LeftDockWidgetArea, viewerDock);	
	tabifyDockWidget(networksDock, viewerDock);
	windowMenu->addAction(viewerDock->toggleViewAction());
	viewerDock->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	//viewerDock->close();


	//Set up simulation tab
	QDockWidget* simulationDock = new QDockWidget(tr("Simulation"), this);
	simulationDock->setObjectName("simulation:DockWidget");
	QScrollArea* simulationScroll = new QScrollArea(simulationDock);
	simulationScroll->setStyleSheet("QScrollArea {border-style: none; }");
	SimulationLoaderWidget* simulationWidget = new SimulationLoaderWidget(simulationScroll);
	simulationScroll->setWidget(simulationWidget);
	simulationDock->setWidget(simulationScroll);
	simulationDock->setFeatures(features);
	simulationDock->setAllowedAreas(areas);
	//addDockWidget(Qt::RightDockWidgetArea, simulationDock);	
	tabifyDockWidget(networksDock, simulationDock);
	windowMenu->addAction(simulationDock->toggleViewAction());
	simulationDock->toggleViewAction()->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
	//simulationDock->close();
	
	//Set up archive tab
	QDockWidget* archiveDock = new QDockWidget(tr("Archives"), this);
	archiveDock->setObjectName("archive:DockWidget");
	QScrollArea* archiveScroll = new QScrollArea(archiveDock);
	ArchiveWidget* archiveWidget = new ArchiveWidget(archiveScroll);
	archiveScroll->setWidget(archiveWidget);
	archiveDock->setWidget(archiveScroll);
	archiveDock->setFeatures(features);
	archiveDock->setAllowedAreas(areas);
	addDockWidget(Qt::RightDockWidgetArea, archiveDock);		
	windowMenu->addAction(archiveDock->toggleViewAction());
	archiveDock->close();

	//Set up analysis tab
	QDockWidget* analysisDock = new QDockWidget(tr("Analysis"), this);
	analysisDock->setObjectName("analysis:DockWidget");
	QScrollArea* analysisScroll = new QScrollArea(analysisDock);
	AnalysisLoaderWidget* analysisLoaderWidget = new AnalysisLoaderWidget(analysisScroll); 
	analysisScroll->setWidget(analysisLoaderWidget);
	analysisDock->setWidget(analysisScroll);
    analysisDock->setFeatures(features);
	analysisDock->setAllowedAreas(areas);
    //addDockWidget(Qt::RightDockWidgetArea, analysisDock);		
	tabifyDockWidget(archiveDock, analysisDock);
	windowMenu->addAction(analysisDock->toggleViewAction());
	analysisDock->close();


	//// Test Layout etc
	//QDockWidget *dock1 = new QDockWidget(tr("Customers"), this);
 //   dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
 //   QListWidget* customerList = new QListWidget(dock1);
 //   customerList->addItems(QStringList()
 //           << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
 //           << "Jane Doe, Memorabilia, 23 Watersedge, Beaton");
 //   dock1->setWidget(customerList);
 //   addDockWidget(Qt::RightDockWidgetArea, dock1);
 //   viewMenu->addAction(dock1->toggleViewAction());

	//QDockWidget* dock2 = new QDockWidget(tr("Customers2"), this);
 //   dock2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
 //   customerList = new QListWidget(dock2);
 //   customerList->addItems(QStringList()
 //           << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
 //           << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
 //           << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
 //           << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
 //   dock2->setWidget(customerList);
 //   //addDockWidget(Qt::RightDockWidgetArea, dock2);	// append widget vertically below, do not tab!
	//tabifyDockWidget(dock1, dock2);
 //   viewMenu->addAction(dock2->toggleViewAction());
	//dock2->close();

	//QDockWidget* dock3 = new QDockWidget(tr("Customers3"), this);
 //   dock3->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
 //   customerList = new QListWidget(dock2);
 //   customerList->addItems(QStringList()
 //           << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
 //           << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
 //   dock3->setWidget(customerList);
 //   //addDockWidget(Qt::RightDockWidgetArea, dock3);
	//tabifyDockWidget(dock1, dock3);
 //   viewMenu->addAction(dock3->toggleViewAction());


	// Var1: Fixed Configuration of Docking Areas
    QMainWindow::setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    QMainWindow::setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    QMainWindow::setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    QMainWindow::setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	// Var2: User Customizing Docking Areas (too much)
	// --> Demos, mainwindow, : MainWindow::setupDockWidgets
	//QSignalMapper* mapper = new QSignalMapper(this);
 //   connect(mapper, SIGNAL(mapped(int)), this, SLOT(setDockingCorner(int)));

	//viewMenu->addSeparator();
	//QMenu* dockWidgetMenu = viewMenu->addMenu(tr("Docking Areas"));
 //   QMenu* corner_menu = dockWidgetMenu->addMenu(tr("Top left corner"));
 //   QActionGroup *group = new QActionGroup(this);
 //   group->setExclusive(true);
 //   ::addAction(corner_menu, tr("Top dock area"), group, mapper, 0);
 //   ::addAction(corner_menu, tr("Left dock area"), group, mapper, 1);

 //   corner_menu = dockWidgetMenu->addMenu(tr("Top right corner"));
 //   group = new QActionGroup(this);
 //   group->setExclusive(true);
 //   ::addAction(corner_menu, tr("Top dock area"), group, mapper, 2);
 //   ::addAction(corner_menu, tr("Right dock area"), group, mapper, 3);

 //   corner_menu = dockWidgetMenu->addMenu(tr("Bottom left corner"));
 //   group = new QActionGroup(this);
 //   group->setExclusive(true);
 //   ::addAction(corner_menu, tr("Bottom dock area"), group, mapper, 4);
 //   ::addAction(corner_menu, tr("Left dock area"), group, mapper, 5);

 //   corner_menu = dockWidgetMenu->addMenu(tr("Bottom right corner"));
 //   group = new QActionGroup(this);
 //   group->setExclusive(true);
 //   ::addAction(corner_menu, tr("Bottom dock area"), group, mapper, 6);
 //   ::addAction(corner_menu, tr("Right dock area"), group, mapper, 7);


	// autoload network
		int loadNetworkId = Util::getInt(configLoader->getParameter("nc_load_network_id", "-1"));
		if(loadNetworkId >= 0)
			networksWidget->loadNetwork(loadNetworkId);

}




/*! Sets up application including databases and GUI. */
void SpikeStreamMainWindow::initializeApplication(){
	//Set up config

	ConfigLoader* configLoader = new ConfigLoader();

	//Set up the data access objects wrapping the network database
	DBInfo netDBInfo(
			configLoader->getParameter("spikeStreamNetworkHost"),
			configLoader->getParameter("spikeStreamNetworkUser"),
			configLoader->getParameter("spikeStreamNetworkPassword"),
			"SpikeStreamNetwork"
	);

	//Create network data access object
	NetworkDao* networkDao = new NetworkDao(netDBInfo);
	Globals::setNetworkDao(networkDao);

	//Set up the data access objects wrapping the archive database.
	DBInfo archiveDBInfo(
			configLoader->getParameter("spikeStreamArchiveHost"),
			configLoader->getParameter("spikeStreamArchiveUser"),
			configLoader->getParameter("spikeStreamArchivePassword"),
			"SpikeStreamArchive"
	);

	//Create archive data access object
	ArchiveDao* archiveDao = new ArchiveDao(archiveDBInfo);
	Globals::setArchiveDao(archiveDao);

	//Set up the data access object wrapping the analysis database.
	DBInfo analysisDBInfo(
			configLoader->getParameter("spikeStreamAnalysisHost"),
			configLoader->getParameter("spikeStreamAnalysisUser"),
			configLoader->getParameter("spikeStreamAnalysisPassword"),
			"SpikeStreamAnalysis"
	);

	//Create analysis data access object
	AnalysisDao* analysisDao = new AnalysisDao(analysisDBInfo);
	Globals::setAnalysisDao(analysisDao);

	//Create Database manager
	databaseManager = new DatabaseManager(Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo(), Globals::getAnalysisDao()->getDBInfo());
	connect(databaseManager, SIGNAL(finished()), this, SLOT(databaseManagerFinished()), Qt::UniqueConnection);
	progressDialog = NULL;

	//Get the default location for saving and loading databases
	Globals::setWorkingDirectory(configLoader->getParameter("default_file_location"));

	//Load display settings from configuration file
	Globals::getNetworkDisplay()->loadDisplaySettings(configLoader);


	createActions();

	createMenus();
	
	createToolBars();
	
	createStatusBar();
	
	createDocking(configLoader);


	//Create network viewer for right half of screen
//	NetworkViewer* networkViewer = new NetworkViewer(mainSplitterWidget);
	NetworkViewer* networkViewer = new NetworkViewer(this);


	//Finish off
	QPixmap iconPixmap(Globals::getSpikeStreamRoot() + "/images/spikestream_icon_64.png" );
	setWindowIcon(iconPixmap);
	setCentralWidget( networkViewer );

	// QSettings
	if( Util::getBool(configLoader->getParameter("maximize_gui") ))
		setWindowState(Qt::WindowMaximized);

	networkViewer->reset();


	// User Settings  HKEY_CURRENT_USER\Software\UCI\SpikeStream	
	QSettings settings;  
	
	// System Settings HKEY_LOCAL_MACHINE\SOFTWARE\UCI\SpikeStream
	QSettings systemSettings(QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	


	setProperty("configAutorestoreLayout", Util::getBool(configLoader->getParameter("nc_autorestore_layout", "false")));
	if(this->property("configAutorestoreLayout").value<bool>())
		restoreLayout();
	// to be used for the close event 
	//  and in order that ::resetLayout() can work 
	setProperty("configAutosaveLayout", Util::getBool(configLoader->getParameter("nc_autosave_layout", "false")));

	//Delete config loader
	delete configLoader;


	
}



// Lambda of Qt5 would be nice here...

void SpikeStreamMainWindow::updateFromNeuronId() {
	unsigned int id = Globals::getNetworkDisplay()->getSingleNeuronID();	
	if(id==0)
		fromNeuronIdEdit->setText("");
	else
		fromNeuronIdEdit->setText(QString::number(id));		
}

void SpikeStreamMainWindow::updateToNeuronId() {
	unsigned int id = Globals::getNetworkDisplay()->getToNeuronID();	
	if(id==0)
		toNeuronIdEdit->setText("");
	else	
		toNeuronIdEdit->setText(QString::number(id));		
}

/* Connect/Disconnect according to Status Bar
*/
void SpikeStreamMainWindow::connectNeuronId(bool checked) {
	if(checked) {
		fromNeuronIdEdit->show();
		toNeuronIdEdit->show();
		connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(updateFromNeuronId()), Qt:: QueuedConnection);
		connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(updateToNeuronId()), Qt:: QueuedConnection);
		//neuronIdEdit->setText(QString("state: %1").arg(checked));
		fromNeuronIdEdit->setText(QString::number(Globals::getNetworkDisplay()->getSingleNeuronID()));
		toNeuronIdEdit->setText(QString::number(Globals::getNetworkDisplay()->getToNeuronID()));
	} else {
		fromNeuronIdEdit->hide();
		toNeuronIdEdit->hide();
		fromNeuronIdEdit->setText("");
		toNeuronIdEdit->setText("");
		disconnect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(updateFromNeuronId()));
		disconnect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(updateToNeuronId()));
	}
}



void SpikeStreamMainWindow::setSelectedFromNeuronId(const QString & text) {
	unsigned new_id = text.toInt();
	unsigned old_id = Globals::getNetworkDisplay()->getSingleNeuronID();
	if(new_id != old_id)
		Globals::getNetworkDisplay()->setSelectedNeuronID(new_id, false); // from
}

void SpikeStreamMainWindow::setSelectedToNeuronId(const QString & text) {
	unsigned new_id = text.toInt();
	unsigned old_id = Globals::getNetworkDisplay()->getToNeuronID();
	if(new_id != old_id)
		Globals::getNetworkDisplay()->setSelectedNeuronID(new_id, true); // from
}

void SpikeStreamMainWindow::saveLayout() 
{
	QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());

}


void SpikeStreamMainWindow::resetLayout() 
{
	QSettings settings;
    settings.remove("MainWindow/geometry");
    settings.remove("MainWindow/windowState");

	this->setProperty("configAutosaveLayout", false);

	QMessageBox::warning(this, "CARLsim GUI - Reset Layout", 
		tr("The stored layout was deleted.\nYou have to restart CARLsimGUI in order to get the default layout.")); 
}


void SpikeStreamMainWindow::restoreLayout() 
{
	QSettings settings;

	const QByteArray &bytes = settings.value("MainWindow/geometry", "").toByteArray();
	if(bytes.isEmpty()) 
		return; // Layout was reseted or never stored
    restoreGeometry(bytes);
    restoreState(settings.value("MainWindow/windowState").toByteArray());


}



