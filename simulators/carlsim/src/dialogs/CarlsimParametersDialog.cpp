//SpikeStream includes
#include "CarlsimParametersDialog.h"
#include "CarlsimLib.h"
#include "ParametersDialog.h"
#include "SpikeStreamException.h"
#include "STDPFunctions.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


#define NO_CUDA_DEVICES_TEXT "No CUDA devices available."

/*! Constructor */
CarlsimParametersDialog::CarlsimParametersDialog(CarlsimLib* lib_, carlsim41::carlsim_configuration_t carlsimConfig, unsigned stdpFunctionID, QWidget* parent) :  carlsim(lib_), QDialog(parent) {
	// FIXED Warning: QWindowsWindow::setGeometry: Unable to set geometry
	setMinimumSize(250, 30);

	//Store current config and create a default config
	this->currentCarlsimConfig = carlsimConfig;
	defaultCarlsimConfig = carlsim41::carlsim_new_configuration();
	this->stdpFunctionID = stdpFunctionID;

	//Create layouts to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();

	//Tracts rows added.
	int rowCntr = 0;

	//Version
	QString versionStr = "CARLsim version ";
	gridLayout->addWidget(new QLabel(versionStr + carlsim41::carlsim_version()), rowCntr, 0);
	++rowCntr;

	//Combo box to select backend
	backendCombo = new QComboBox();
	backendCombo->addItem(carlsim41::ComputingBackend_string[carlsim41::CPU_CORES]); 
	backendCombo->addItem(carlsim41::ComputingBackend_string[carlsim41::GPU_CORES]); 
	gridLayout->addWidget(new QLabel("Backend: "), rowCntr, 0);
	gridLayout->addWidget(backendCombo, rowCntr, 1);
	connect(backendCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(backendChanged(int)));
	++rowCntr;

	//CUDA device list
	cudaDeviceCombo = new QComboBox();
	getCudaDevices(cudaDeviceCombo);
	cudaDeviceLabel = new QLabel("CUDA device: ");
	gridLayout->addWidget(cudaDeviceLabel, rowCntr, 0);
	gridLayout->addWidget(cudaDeviceCombo, rowCntr, 1);
	++rowCntr;

	//Add combo with STDP functions and button to edit parameters
	stdpCombo = new QComboBox();
	getStdpFunctions(stdpCombo);
	stdpLabel = new QLabel("STDP parameters: ");
	gridLayout->addWidget(stdpLabel, rowCntr, 0);
	gridLayout->addWidget(stdpCombo, rowCntr, 1);
	QPushButton* stdpParamButton = new QPushButton("...");
	connect(stdpParamButton, SIGNAL(clicked()), this, SLOT(stdpParameterButtonClicked()));
	gridLayout->addWidget(stdpParamButton, rowCntr, 2);
	++rowCntr;

	//Sets the fields to match the config
	loadParameters(currentCarlsimConfig);
	if((int)stdpFunctionID > stdpCombo->count())
		throw SpikeStreamException("STDP function ID is out of range: " + QString::number(stdpFunctionID));
	stdpCombo->setCurrentIndex(stdpFunctionID);

	//Add the grid layout
	mainVBox->addLayout(gridLayout);

	//Add the buttons
	addButtons(mainVBox);
}


/*! Destructor */
CarlsimParametersDialog::~CarlsimParametersDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Called when the backend combo changes. Shows or hides appropriate inputs
	for CUDA or CPU backends. */
void CarlsimParametersDialog::backendChanged(int index){
	if(index == carlsim41::GPU_CORES){
		cudaDeviceLabel->show();
		cudaDeviceCombo->show();
	}
	else if(index == carlsim41::CPU_CORES){
		cudaDeviceLabel->hide();
		cudaDeviceCombo->hide();
	}
}


/*! Resets the inputs with the default parameters.  */
void CarlsimParametersDialog::defaultButtonClicked(){
	loadParameters(defaultCarlsimConfig);

	//Set STDP function
	stdpCombo->setCurrentIndex(0);//0 is the default
}



/*! Tests the parameters and stores them in the parameter map.
	Closes the dialog if everything is ok.  */
void CarlsimParametersDialog::okButtonClicked(){
	try{
		//Throws an exception if parameter values are empty or invalid
		storeParameterValues();
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qWarning()<<ex.getMessage();
	}
}




/*! Sets the parameters for the STDP function */


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void CarlsimParametersDialog::addButtons(QVBoxLayout* mainVLayout){
	QHBoxLayout *buttonBox = new QHBoxLayout();
	QPushButton* cancelButton = new QPushButton("Cancel");
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QPushButton* defaultsButton = new QPushButton("Load defaults");
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(defaultButtonClicked()));
	QPushButton* okButton = new QPushButton("Ok");
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	mainVLayout->addLayout(buttonBox);
}


/*! Checks the output from a function call and throws exception if there is an error */
//void CarlsimParametersDialog::checkCarlsimOutput(carlsim41::carlsim_status_t result, const QString& errorMessage){
void CarlsimParametersDialog::checkCarlsimOutput(int result, const QString& errorMessage){
	//if(result != CARLSIM_OK)  // 
	//	throw SpikeStreamException(errorMessage + ": " + carlsim41::carlsim_strerror());
}


/*! Loads up a list of the currently available CUDA devices */
void CarlsimParametersDialog::getCudaDevices(QComboBox* combo){
	carlsim41::carlsim_status_t result = CARLSIM_OK;  
	int numCudaDevices = carlsim->cudaDeviceCount();   
	if(result != CARLSIM_OK){
		combo->addItem(NO_CUDA_DEVICES_TEXT);
		return;
	}

	if(numCudaDevices == 0){
		combo->addItem(NO_CUDA_DEVICES_TEXT);
		return;
	}

	for(unsigned i=0; i<numCudaDevices; ++i){
		const char* devDesc;
		carlsim->cudaDeviceDescription(i, &devDesc);  		
		combo->addItem(QString("GPU[%1]: %2").arg(i).arg(QString::fromLocal8Bit(devDesc)));  

	}
}


/*! Loads up all of the STDP functions */
void CarlsimParametersDialog::getStdpFunctions(QComboBox *combo){
	QList<unsigned> functionIDs = STDPFunctions::getFunctionIDs();
	for(int i=0; i<functionIDs.size(); ++i){
		combo->addItem(STDPFunctions::getFunctionDescription(functionIDs.at(i)));
	}
	combo->setCurrentIndex(stdpFunctionID);
}


/*! Loads parameters from the supplied configuration into the graphical components of the dialog. */
void CarlsimParametersDialog::loadParameters(carlsim41::carlsim_configuration_t config){
	
	int index = config->backend;  


	/* Set backend combo to index and call function to make sure inputs are correctly hidden or shown
	   It may not change the current index, so need to call this function */
	backendCombo->setCurrentIndex(index);
	backendChanged(index);

	int cudaDev = config->cudaDev;

	if(cudaDev < 0)
		cudaDeviceCombo->setCurrentIndex(0);
	else
		cudaDeviceCombo->setCurrentIndex(cudaDev);
}


/*! Saves the parameter values to the parameter map.
	The validity of the values is tested using CARLsim and an exception is thrown if they cannot be used. */
void CarlsimParametersDialog::storeParameterValues(){
	//Set hardware configuration
	if(backendCombo->currentIndex() == carlsim41::GPU_CORES){ 
		if(cudaDeviceCombo->currentText() == NO_CUDA_DEVICES_TEXT)
			throw SpikeStreamException("Cannot use CUDA - no CUDA devices are available.");
		currentCarlsimConfig->backend = carlsim41::GPU_CORES; 
		currentCarlsimConfig->cudaDev = cudaDeviceCombo->currentIndex(); 
	}
	else if(backendCombo->currentIndex() == carlsim41::CPU_CORES){ 
		currentCarlsimConfig->backend = carlsim41::CPU_CORES; 
	}
	else{
		throw SpikeStreamException("Backend combo index not recognized.");
	}

	//If we have reached this point, hardware configuration is ok

	//STDP function
	stdpFunctionID = stdpCombo->currentIndex();
}


/*! Shows a dialog that enables the user to edit the parameters of an STDP function. */
void CarlsimParametersDialog::stdpParameterButtonClicked(){
	try{
		stdpFunctionID = stdpCombo->currentIndex();

		ParametersDialog paramDialog(
				STDPFunctions::getParameterInfoList(stdpFunctionID),
				STDPFunctions::getDefaultParameters(stdpFunctionID),
				STDPFunctions::getParameters(stdpFunctionID),
				this
		);
		if(paramDialog.exec() == QDialog::Accepted){
			QHash<QString, double> newParameters = paramDialog.getParameters();
			STDPFunctions::setParameters(stdpFunctionID, newParameters);
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


