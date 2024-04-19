//SpikeStream includes
#include "ConfigLoader.h"
#include "ModelParametersDialog.h"
#include "ParametersDialog.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QDoubleSpinBox>


#define NO_CUDA_DEVICES_TEXT "No CUDA devices available."

/*! Constructor */
ModelParametersDialog::ModelParametersDialog(
	const QDateTime &simtime,
	double speed, // realtime vs. Simulation 
	unsigned step_ms,	// ms of model time per each step (alpha)
	ModelStart start,  // snyc if any buffer of any channel is filled
	ModelAutosync autosync,  // snyc if any buffer of any channel is filled
	int lag_ms, // ms behind real time 

	QWidget* parent) : 

		m_simtime(simtime), m_speed(speed), m_step_ms(step_ms), m_start(start), m_autosync(autosync), m_lag_ms(lag_ms),
	
	QDialog(parent) {

	//FIXED Warning: QWindowsWindow::setGeometry: Unable to set geometry
	setMinimumSize(250, 30);

	//Create layouts to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	QHBoxLayout* valueHBox = NULL; // transient

	int rowCntr = 0;

	// simtime with date pick
	gridLayout->addWidget(new QLabel("SimTime"), rowCntr, 0);
	valueHBox = new QHBoxLayout();
	simtimeEdit = new QDateTimeEdit(m_simtime, this);  // start value is set according to model parameter, default settings at start
	simtimeEdit->setTimeSpec(Qt::OffsetFromUTC);
	simtimeEdit->setDisplayFormat("yyyy-MM-ddThh:mm:ss.zzz"); 
	simtimeEdit->setCalendarPopup(true);
	simtimeEdit->setMinimumSize(200, 20);
	simtimeEdit->setMaximumSize(200, 20);
	simtimeEdit->setAlignment(Qt::AlignRight);
	valueHBox->addWidget(simtimeEdit);
	QPushButton* simtimeResetButton = new QPushButton("Now");
	connect(simtimeResetButton, SIGNAL(clicked()), this, SLOT(resetSimtime()));
	valueHBox->addWidget(simtimeResetButton);
	valueHBox->addStretch(1);
	gridLayout->addLayout(valueHBox, rowCntr, 1);
	valueHBox = NULL;
	++rowCntr;

	// speed
	gridLayout->addWidget(new QLabel("Speed (factor)"), rowCntr, 0);
	valueHBox = new QHBoxLayout();
	speedSpin = new QDoubleSpinBox();
	speedSpin->setToolTip("Speed-Factor (double) against realtime. If neccessary the simulation will slowdown to match the defined time.");
	speedSpin->setDecimals(2);
	speedSpin->setMinimum(0.01);
	speedSpin->setMaximum(1000);
	speedSpin->setSingleStep(0.1);
	speedSpin->setValue(m_speed);
	valueHBox->addWidget(speedSpin);
	valueHBox->addStretch(1);
	gridLayout->addLayout(valueHBox, rowCntr, 1);
	valueHBox = NULL;
	++rowCntr;

	// step_ms
	gridLayout->addWidget(new QLabel("Step (ms)"), rowCntr, 0);
	valueHBox = new QHBoxLayout();
	stepSpin = new QSpinBox();
	stepSpin->setMinimum(1);
	stepSpin->setMaximum(1000);
	stepSpin->setValue(m_step_ms);
	valueHBox->addWidget(stepSpin, 1, Qt::AlignLeft);
	gridLayout->addLayout(valueHBox, rowCntr, 1);
	valueHBox = NULL;
	++rowCntr;

	// start combo
	gridLayout->addWidget(new QLabel("Start"), rowCntr, 0);
	startCombo = new QComboBox();
	startCombo->addItem("Realtime"); // 0
	startCombo->addItem("Fixed "); 
	startCombo->addItem("Channel");
	startCombo->setCurrentIndex(m_start);  // indices correspond to enum
	gridLayout->addWidget(startCombo, rowCntr, 1);		
	++rowCntr;

	// autosync combo
	gridLayout->addWidget(new QLabel("Autosync"), rowCntr, 0);
	autosyncCombo = new QComboBox();
	autosyncCombo->addItem("OFF"); // 0
	autosyncCombo->addItem("First Message"); 
	autosyncCombo->addItem("Last Message");
	autosyncCombo->setCurrentIndex(m_autosync);  // indices correspond to enum
	gridLayout->addWidget(autosyncCombo, rowCntr, 1);		
	++rowCntr;

	// lag_ms
	gridLayout->addWidget(new QLabel("Lag (ms)"), rowCntr, 0);
	valueHBox = new QHBoxLayout();
	lagSpin = new QSpinBox();
	lagSpin ->setMinimum(-90000);
	lagSpin->setMaximum(90000);  // max 1,5 min
	lagSpin->setValue(m_lag_ms);
	lagSpin->setSingleStep(100);
	valueHBox->addWidget(lagSpin, 1, Qt::AlignLeft);
	gridLayout->addLayout(valueHBox, rowCntr, 1);
	valueHBox = NULL;
	++rowCntr;

	//Add the grid layout
	mainVBox->addLayout(gridLayout);

	//Add the buttons
	addButtons(mainVBox);
}


/*! Destructor */
ModelParametersDialog::~ModelParametersDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/


/*! Resets the inputs with the default parameters.  */
void ModelParametersDialog::defaultButtonClicked(){

	// copied from constructor and caller	
	ConfigLoader* configLoader = new ConfigLoader();
	QString param = configLoader->getParameter("carlsim_model_simtime", "");
	if(param=="")
		m_simtime = QDateTime::currentDateTime();
	else
		m_simtime = QDateTime::fromString(param, "yyyy-MM-ddThh:mm:ss.zzz"); 
	m_speed = Util::getFloat(configLoader->getParameter("carlsim_model_speed", "1.0")); 
	m_step_ms = Util::getUInt(configLoader->getParameter("carlsim_model_step", "1"));	
	QString m_autosync_value = configLoader->getParameter("carlsim_model_autosync", "Off");
	setProperty("m_autosync", m_autosync_value); 

	setProperty("m_start", configLoader->getParameter("carlsim_model_start", "REALTIME"));
	
	m_lag_ms = Util::getInt(configLoader->getParameter("carlsim_model_lag", "0")); 

	// transfer values to the widgets
	simtimeEdit->setDateTime(m_simtime);
	speedSpin->setValue(m_speed);
	startCombo->setCurrentIndex((int) m_start);
	autosyncCombo->setCurrentIndex((int) m_autosync);
	stepSpin->setValue(m_step_ms);
	lagSpin->setValue(m_lag_ms);

	delete configLoader;
}


/*! Tests the parameters and stores them in the parameter map.
	Closes the dialog if everything is ok.  */
void ModelParametersDialog::okButtonClicked(){
	try{
		//Throws an exception if parameter values are empty or invalid
		storeParameterValues();
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qWarning()<<ex.getMessage();
	}
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

void ModelParametersDialog::resetSimtime() {
	m_simtime = QDateTime::currentDateTime();
	simtimeEdit->setDateTime(m_simtime);
}


/*! Sets the parameters for the STDP function */


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void ModelParametersDialog::addButtons(QVBoxLayout* mainVLayout){
	QHBoxLayout *buttonBox = new QHBoxLayout();
	QPushButton* okButton = new QPushButton("Ok");
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	QPushButton* cancelButton = new QPushButton("Cancel");
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	//buttonBox->addSpacing(10);
	buttonBox->addStretch(1);
	QPushButton* defaultsButton = new QPushButton("Load defaults");
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(defaultButtonClicked()));
	mainVLayout->addLayout(buttonBox);
}



/*! Saves the parameter values to the parameter map.
	The validity of the values is tested using CARLsim and an exception is thrown if they cannot be used. */
void ModelParametersDialog::storeParameterValues(){

	m_simtime = simtimeEdit->dateTime();

	m_start = (enum ModelStart) startCombo->currentIndex();
	m_autosync = (enum ModelAutosync) autosyncCombo->currentIndex();
	m_speed = speedSpin->value();
	m_step_ms = stepSpin->value();
	m_lag_ms = lagSpin->value();
}


