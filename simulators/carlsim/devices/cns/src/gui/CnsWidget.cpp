//SpikeStream includes
#include "AddChannelDialog.h"
#include "ChannelTableView.h"
#include "Globals.h"
#include "CnsWidget.h"
#include "CnsXmlLoader.h"
#include "Util.h"
#include "CarlsimWrapper.h" 
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLabel>
#include <QDateTime>

//Cns includes
#include "CARLsimIO/Channel/InboundChannel/InboundChannel.hpp"

// New without Dependencies
#include <CARLsimIO/Channel/InboundChannel/InboundChannelFactory.hpp>
#include <CARLsimIO/Reader/ReaderFactory.hpp>

//#include "Cns/Writer/WriterFactory.hpp"
#include "CARLsimIO/CARLsimIOException.hpp"
using namespace carlsimio;

//Other includes 
#include <iostream>
#include <map>
using namespace std;


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CnsWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("CARLsimCNS"); 
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API const char* getNameCstr(){
		return "CARLsimCNS";  
	}


}


/*! Constructor */
CnsWidget::CnsWidget(QWidget* parent) : translator(NULL), AbstractDeviceWidget(parent){

	translator = new QTranslator;
	QLocale locale; 
	if(!translator->load(":/cns/translation/"+ locale.name())) {
		qDebug() << "Could not load translations " << locale.name() << ". Please check the LANG environment variable.";
		delete translator;
		translator = NULL;
	} else {
		QApplication::installTranslator(translator);  
	}


	//Create layout for the entire widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	ConfigLoader* configLoader = new ConfigLoader();

	Log::ReportingLevel() = (TLogLevel)
		Util::getUInt(configLoader->getParameter("cns_reporting_level", "3"));

	// Display Status and provide Buttons for Channels
	QHBoxLayout* diagBox = new QHBoxLayout();

	updateChannelsButton = new QPushButton("Update");
	diagBox->addWidget(updateChannelsButton);	
	connect(updateChannelsButton, SIGNAL(clicked()), this, SLOT(updateChannels()));

	resetChannelsButton = new QPushButton("Reset");
	diagBox->addWidget(resetChannelsButton);	
	connect(resetChannelsButton, SIGNAL(clicked()), this, SLOT(resetChannels()));

	diagBox->addWidget(new QLabel("Next:"));
	diagBox->addWidget(new QTimeEdit(this));
	QPushButton* diagButton = new QPushButton("Diag");
	diagBox->addWidget(diagButton);

	QIntValidator* unsignedIntValidator = new QIntValidator(0, 1000000, this);
	QLineEdit* unsignedEdit = new QLineEdit("10");
	unsignedEdit->setValidator(unsignedIntValidator);
	unsignedEdit->setMaximumSize(60,20);
	diagBox->addWidget(unsignedEdit);

	diagBox->addWidget(new QLabel(tr("logging")+":")); 
	QComboBox* reportingLevelCombo = new QComboBox();
	reportingLevelCombo->addItems(QStringList() << "0:Emergency" << "1:Alert" << "2:Critical" << "3:Error" << "4:Warning" << "5:Notice" << "6:Info" << "7:Debug");
	reportingLevelCombo->setCurrentIndex(Log::ReportingLevel());
	diagBox->addWidget(reportingLevelCombo);
	connect(reportingLevelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setReportingLevel(int)));

	diagBox->addStretch(1);
	mainVBox->addLayout(diagBox);


	//Combo boxes to connect layers
	QHBoxLayout* channelBox = new QHBoxLayout();
	fireOrCurrentCombo = new QComboBox();
	fireOrCurrentCombo->addItem("Fire");
	fireOrCurrentCombo->addItem("1");
	for(int i=10; i<=100; i += 10)
		fireOrCurrentCombo->addItem(QString::number(i));
	fireOrCurrentCombo->setMinimumSize(60, 20);
	connect(fireOrCurrentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setFireOrCurrent(int)));
	channelBox->addWidget(new QLabel("Input fire or current: "));
	channelBox->addWidget(fireOrCurrentCombo);
	channelBox->addSpacing(10);
	addChannelButton = new QPushButton("Add channel");
	//connect(addChannelButton, SIGNAL(clicked()), this, SLOT(addChannel()));
	connect(addChannelButton, SIGNAL(clicked()), this, SLOT(addChannelModeless())); 
	channelBox->addWidget(addChannelButton);
	channelBox->addStretch(5);
	mainVBox->addLayout(channelBox);



	//Create manager
	cnsManager = new CnsManager();

	//Model and view to display active channels
	channelModel = new ChannelModel(cnsManager);
	ChannelTableView* channelView = new ChannelTableView(channelModel);
	mainVBox->addWidget(channelView);

	//Listen for changes in network
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

	// if auto device
	connect(Globals::getEventRouter(), SIGNAL(carlsimSimulationLoadedSignal()), this, SLOT(autoaddChannels())); // experimental
	connect(this, SIGNAL(channelsLoaded()), Globals::getEventRouter(), SLOT(finspikesDeviceLoadedSlot())); // experimental

	//! config if autoupdateDevices	
	if (Util::getBool(configLoader->getParameter("cns_autoupdate_channels", "false"))) {
		xmlConfigPath = configLoader->getParameter("cns_xml_config", "CnsChannel.xml");
		qDebug() << " xmlConfigPath: " << xmlConfigPath;
		connect(Globals::getEventRouter(), SIGNAL(carlsimSimulationLoadedSignal()),
			this, SLOT(connectAutoUpdateChannels()));
	}

	mainVBox->addStretch(1);

	delete configLoader;

}


/*! Destructor */
CnsWidget::~CnsWidget(){

	if(translator!=NULL)
	{
		QApplication::removeTranslator(translator);  // does not delete the translator, see
		delete translator;  // http://qt-project.org/doc/qt-4.8/qcoreapplication.html#removeTranslator
	}

}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Returns a pointer to the device manager */
AbstractDeviceManager* CnsWidget::getDeviceManager(){
	return (AbstractDeviceManager*)cnsManager;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/


/*! Adds a new channel, which will produce or listen for neuron spikes. */
void CnsWidget::connectAutoUpdateChannels(){
	qDebug() << __FUNCTION__;

	// Note: downcast to reveive Updates if Monitoring is ON
	// This further slows down the Cns device, so at least it should be configurable 
	AbstractSimulation* abstractSimulation = Globals::getSimulation();
	CarlsimWrapper *carlsimWrapper = static_cast<CarlsimWrapper*>(abstractSimulation);
	connect(carlsimWrapper, SIGNAL(timeChanged(unsigned, unsigned long long, QDateTime, QDateTime, QDateTime, QDateTime, unsigned long long)),
		this, SLOT(updateChannels()),
			Qt::QueuedConnection);
}

/*! Adds a new channel, which will produce or listen for neuron spikes. */
void CnsWidget::autoaddChannels(){
	qDebug() << __FUNCTION__;

	try{
		
		CnsXmlLoader xmlLoader(cnsManager);

		QFile configFile(xmlConfigPath);
		if (!configFile.exists()) {
			QString rootDirectory = Util::getRootDirectory();
			configFile.setFileName(rootDirectory + "/" + xmlConfigPath);
			if (!configFile.exists()) {
				throw "Cannot find xml config file " + xmlConfigPath + " in directory " + rootDirectory + " or as absolute path.";
			}
		}
		qDebug() << "Load Channels from " << configFile.fileName() << endl;
		xmlLoader.loadFromPath(configFile.fileName());


		channelModel->reload();

		emit channelsLoaded(); // see constructor for intrinsic usage

	} 
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	} 
	catch(CARLsimIOException& ex) {
		qCritical()<<ex.what(); 
	} 
	catch (exception& e) {
		qCritical()<<"STD exception occurred creating channel and reader/writer: "<<e.what();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring a channel";
	}

}


/*! Adds a new channel, which will produce or listen for neuron spikes.
    New modeless version to support parallel editing and arbitrary lookups on the MainWindow */
void CnsWidget::addChannelModeless(){
	//Add channel to model
	try{
		// Create a new AddChannelDialog on the heap an keep a local reference
		AddChannelDialog* addChannelDialog = new AddChannelDialog(this);

		// Manage its memory automatically when closed
		addChannelDialog ->setAttribute(Qt::WA_DeleteOnClose);  

		// Connect behavior on accept. The local reference is gathered by QObject::sender() in the SLOT. 
		connect(addChannelDialog, SIGNAL(accepted()), this, SLOT(addChannelAccept()) );

		// Display the Dialog and return immediately
		addChannelDialog ->show();
		addChannelDialog ->raise();
		addChannelDialog ->activateWindow();
	} 
	catch(SpikeStreamException& ex) {
		qCritical()<<ex.getMessage();
	} 
	catch(CARLsimIOException& ex) {
		qCritical()<<ex.what(); 
	} 
	catch (exception& e) {
		qCritical()<<"STD exception occurred creating channel and reader/writer: "<<e.what();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring a channel";
	}
}

void CnsWidget::addChannelAccept() // privat slot
{
	// Handle Accept Event of new Channel
	try {
		// No member applicabe since multiple dialogs can be open at the same time !
		// http://www.cplusplus.com/doc/tutorial/typecasting/
		AddChannelDialog* addChannelDialog = dynamic_cast<AddChannelDialog*>(QObject::sender()); // save for polymorphic objects (virt. mf), but requires RTTI
		//AddChannelDialog* addChannelDialog = static_cast<AddChannelDialog*>(QObject::sender()); // modern C++ style, fast, but may result in runtime errors
		//AddChannelDialog* addChannelDialog = (AddChannelDialog*) QObject::sender();    // old C style
		if (addChannelDialog->isInboundChannel())
			cnsManager->addChannel(addChannelDialog->getInputChannel(), addChannelDialog->getNeuronGroup()); //Add channel to cnsManager
		else
			cnsManager->addChannel(addChannelDialog->getOutboundChannel(), addChannelDialog->getNeuronGroup()); //Add channel to cnsManager
		// explicitly unset the pointer
		addChannelDialog = NULL;

		//Instruct model to reload data.
		channelModel->reload();

	} catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	} catch(CARLsimIOException& ex) {
		qCritical()<<ex.msg().c_str();
	} catch(...) {
		qCritical()<<"An unknown exception occurred configuring a channel";
	}
}


/*! Switches the entire device between fire or current mode */
void CnsWidget::setFireOrCurrent(int index){
	if(index == 0){
		getDeviceManager()->setFireNeuronMode(true);
	}
	else{
		getDeviceManager()->setFireNeuronMode(false);
		getDeviceManager()->setCurrent(Util::getDouble(fireOrCurrentCombo->currentText()));
	}
}


/*! Called when the connect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void CnsWidget::setIPAddressButtonClicked(){
	if(dnsEdit->text().isEmpty() || portEdit->text().isEmpty()){
		qCritical()<<"IP address and/or port is missing";
		return;
	}

	//First test IP and port to see if a reader can access it
	ipAddressSet = false;
	try {
		//If we reach this point without an exception, then IP address should be ok.
		ipAddressSet = true;

		//Enable/disable appropriate graphical components
		connectButton->setEnabled(false);
		dnsEdit->setEnabled(false);
		portEdit->setEnabled(false);
		disconnectButton->setEnabled(true);
	}
	catch(CARLsimIOException& ex){
		qCritical()<<"Cannot connect to DNS server at IP: " + dnsEdit->text() + " and port " + portEdit->text()<<": "<<ex.what();
	}
	catch(...){
		qCritical()<<"Cannot connect to DNS server at IP: " + dnsEdit->text() + " and port " + portEdit->text();
	}
}


/*! Called when the disconnect button is clicked.
	Tries to connect to DNS server and gives an error message if this fails. */
void CnsWidget::unsetIPAddressButtonClicked(){
	ipAddressSet = false;

	connectButton->setEnabled(true);
	dnsEdit->setEnabled(true);
	portEdit->setEnabled(true);
	disconnectButton->setEnabled(false);
}


/*! Called when network is changed */
void CnsWidget::networkChanged(){
	if(channelModel->isEmpty()){
		return;
	}
	if(Globals::networkLoaded()){
		cnsManager->deleteAllChannels();
		channelModel->reload();
	}
}



/*! Called when network is changed */
void CnsWidget::updateChannels(){
	if(channelModel->isEmpty()){
		return;
	}
	channelModel->reload();
}


/*! Called when network is changed */
void CnsWidget::resetChannels(){
	if(channelModel->isEmpty()){
		return;
	}

	cnsManager->reset();
}


void CnsWidget::setReportingLevel(int level) {

	Log::ReportingLevel() = (TLogLevel) level;

}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/


