//SpikeStream includes
#include "CARLsimCNSWbWidget.h"
#include "Globals.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "CARLsimCNSWbBuilderThread.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QGroupBox>
#include <QHash>
#include <QLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QValidator>


//Functions for dynamic library loading
extern "C" {
	/*! Creates a CARLsimCNSWbWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CARLsimCNSWbWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("CARLsimCNSWb connection group builder");
	}
}


/*! Constructor */
CARLsimCNSWbWidget::CARLsimCNSWbWidget(QWidget* parent) : AbstractConnectionWidget(parent) {
	//Construct GUI
	QVBoxLayout* mainVBox = new QVBoxLayout();
	buildGUI(mainVBox);

	//Add button
	QHBoxLayout *addButtonBox = new QHBoxLayout();
	QPushButton *addPushButton = new QPushButton("Add");
	addPushButton->setMaximumSize(100, 30);
	connect(addPushButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
	addButtonBox->addStretch(10);
	addButtonBox->addWidget(addPushButton);
	mainVBox->addLayout(addButtonBox);

	//Create connection builder
	connectionBuilder = new CARLsimCNSWbBuilderThread();
	connect (connectionBuilder, SIGNAL(finished()), this, SLOT(builderThreadFinished()));
	connect(connectionBuilder, SIGNAL( progress(int, int, QString) ), this, SLOT( updateProgress(int, int, QString) ), Qt::QueuedConnection);


	//Set Defaults for Config File applying the same names	

	defaults["prefix"] = "ctx";  // Webots

	// decouple, state requires phasic bursting to be activated reliable
	defaults["tof2vel_weights"] = "0.8";
	defaults["tof2vel_w_factor"] = "100.0";
	defaults["tof2vel_delays"] = "1";

	// decouple, state requires phasic bursting to be activated reliable
	defaults["ps2vel_weights"] = "0.6";
	defaults["ps2vel_w_factor"] = "100.0";
	defaults["ps2vel_delays"] = "1";

	updateTemplate(0);

}


/*! Destructor */
CARLsimCNSWbWidget::~CARLsimCNSWbWidget(){
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

//Overridden
bool CARLsimCNSWbWidget::checkInputs(){
	//Fix description
	if(descriptionEdit->text().isEmpty())
		descriptionEdit->setText("Undescribed");


	//Check inputs are not empty
	try{
		checkInput(fromCombo, "From neuron group has not been set.");
		checkInput(toCombo, "To neuron group has not been set.");
		checkInput(minWeightRange1Edit, "Min weight range 1 has not been set.");
		checkInput(maxWeightRange1Edit, "Max weight range 1 has not been set.");
		checkInput(weightRange1PercentEdit, "Max weight range 2 has not been set.");
		checkInput(minWeightRange2Edit, "Min weight range 2 has not been set.");
		checkInput(maxWeightRange2Edit, "Max weight range 2 has not been set.");
		checkInput(minDelayEdit, "Min delay has not been set.");
		checkInput(maxDelayEdit, "Max delay has not been set.");
		checkInput(connectionProbabilityEdit, "Density has not been set.");
		checkInput(seedEdit, "Random seed has not been set.");
	}
	catch(SpikeStreamException& ex){
		QMessageBox::warning(this, "CARLsimCNS Webots Connection Group Builder", ex.getMessage(), QMessageBox::Ok);
		return false;
	}

	// dirty
	CARLsimCNSWbBuilderThread* builder = (CARLsimCNSWbBuilderThread*)connectionBuilder;

	builder->tof2vel.weights = tof2vel.weightsSpin->value();
	builder->tof2vel.w_factor = tof2vel.weightFactorSpin->value();
	builder->tof2vel.delays = tof2vel.delaysSpin->value();

	builder->ps2vel.weights = ps2vel.weightsSpin->value();
	builder->ps2vel.w_factor = ps2vel.weightFactorSpin->value();
	builder->ps2vel.delays = ps2vel.delaysSpin->value();



	//Inputs are ok
	return true;
}


//Override
ConnectionGroupInfo CARLsimCNSWbWidget::getConnectionGroupInfo(){

	// references to neuron groups
	QHash<QString, NeuronGroup*>& groupsMap = Globals::getNetwork()->getNeuronGroupsMap(); // build only once

	unsigned int synapseTypeID = getSynapseTypeID(synapseTypeCombo->currentText());

	//Store parameters in parameter map
	QHash<QString, double> paramMap;
	paramMap["min_weight_range_1"] = Util::getDouble(minWeightRange1Edit->text());
	paramMap["max_weight_range_1"] = Util::getDouble(maxWeightRange1Edit->text());
	paramMap["percent_weight_range_1"] = Util::getDouble(weightRange1PercentEdit->text());
	paramMap["min_weight_range_2"] = Util::getDouble(minWeightRange2Edit->text());
	paramMap["max_weight_range_2"] = Util::getDouble(maxWeightRange2Edit->text());
	paramMap["min_delay"] = Util::getDouble(minDelayEdit->text());
	paramMap["max_delay"] = Util::getDouble(maxDelayEdit->text());
	paramMap["connection_probability"] = Util::getDouble(connectionProbabilityEdit->text());
	paramMap["random_seed"] = Util::getDouble(seedEdit->text());

	//Use extracted praameters to construct connection group info
	ConnectionGroupInfo info(0, descriptionEdit->text(), 0, 0, paramMap, Globals::getNetworkDao()->getSynapseType(synapseTypeID));
	return info;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the graphical components */
void CARLsimCNSWbWidget::buildGUI(QVBoxLayout* mainVBox){
	QGroupBox* mainGroupBox = new QGroupBox("CARLsimCNSWb Connection Group Builder", this);

	// Template
	templateCombo = new QComboBox();

	//placed in a subdirectory as it does not depend on the central spikestream config
	QFileInfo configFile(ConfigLoader::getConfigFilePath());
	auto configDir = configFile.absoluteDir().absolutePath() + +"/cnswb";
	qDebug() << configDir << endl;

	for (QDirIterator confFileIter(configDir, { "*.config" }, QDir::Files); confFileIter.hasNext(); )
	{
		confFileIter.next();
		auto path = confFileIter.filePath();
		qDebug() << path << endl;
		ConfigLoader* configLoader = new ConfigLoader(path);
		configLoaders.append(configLoader);
		templateCombo->addItem(configLoader->getParameter("name"));
	}
	connect (templateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTemplate(int)));
	templateCombo->setMinimumSize(50, 20);
	mainVBox->addWidget(templateCombo);


	// Neuron Parameter Groups		
	auto groupBox2 = new QGroupBox("Connection parameter", this);
	QGridLayout* gridLayout2 = new QGridLayout();
	gridLayout2->setMargin(10);
	auto column = 1;
	
	tof2vel.addGroup("ToF-> VEL", gridLayout2, configLoader);

	ps2vel.addGroup("PS -> VEL", gridLayout2, configLoader);

	
	groupBox2->setLayout(gridLayout2);
	mainVBox->addWidget(groupBox2);


	//Validators for double and integer parameters
	QDoubleValidator* doubleValidator = new QDoubleValidator(-1.0, 1000000.0, 5, this);
	QDoubleValidator* weightValidator = new QDoubleValidator(-1.0, 1.0, 5, this);
	QIntValidator* delayValidator = new QIntValidator(0, 10000, this);
	QIntValidator* seedValidator = new QIntValidator(0, 1000000000, this);
	QIntValidator* percentValidator = new QIntValidator(0, 100, this);


	//Add description widget
	descriptionEdit = new QLineEdit("Undescribed");
	QHBoxLayout* descLayout = new QHBoxLayout();
	descLayout->addWidget(new QLabel("Description: "));
	descLayout->addWidget(descriptionEdit);
	mainVBox->addLayout(descLayout);
	mainVBox->addSpacing(10);

	//Add from and to combos
	fromCombo = new QComboBox();
	addNeuronGroups(fromCombo);
	toCombo = new QComboBox();
	addNeuronGroups(toCombo);
	QHBoxLayout* fromToLayout = new QHBoxLayout();
	fromToLayout->addWidget(new QLabel("From: "));
	fromToLayout->addWidget(fromCombo);
	fromToLayout->addWidget(new QLabel(" To: "));
	fromToLayout->addWidget(toCombo);
	fromToLayout->addStretch(10);
	mainVBox->addLayout(fromToLayout);
	mainVBox->addSpacing(10);

	//Weight range 1
	minWeightRange1Edit = new QLineEdit("0.0");
	minWeightRange1Edit->setValidator(weightValidator);
	maxWeightRange1Edit = new QLineEdit("0.5");
	maxWeightRange1Edit->setValidator(weightValidator);
	weightRange1PercentEdit = new QLineEdit("100");
	weightRange1PercentEdit->setValidator(percentValidator);
	QHBoxLayout* weightRange1Box = new QHBoxLayout();
	weightRange1Box->addWidget(new QLabel("Weight range 1 from: "));
	weightRange1Box->addWidget(minWeightRange1Edit);
	weightRange1Box->addWidget(new QLabel(" to: "));
	weightRange1Box->addWidget(maxWeightRange1Edit);
	weightRange1Box->addWidget(new QLabel(" Proportion weight range 1: "));
	weightRange1Box->addWidget(weightRange1PercentEdit);
	weightRange1Box->addWidget(new QLabel(" %"));
	weightRange1Box->addStretch(10);
	mainVBox->addLayout(weightRange1Box);
	mainVBox->addSpacing(10);

	//Weight range 2
	minWeightRange2Edit = new QLineEdit("0");
	minWeightRange2Edit->setValidator(weightValidator);
	maxWeightRange2Edit = new QLineEdit("1.0");
	maxWeightRange2Edit->setValidator(weightValidator);
	QHBoxLayout* weightRange2Box = new QHBoxLayout();
	weightRange2Box->addWidget(new QLabel("Weight range 2 from: "));
	weightRange2Box->addWidget(minWeightRange2Edit);
	weightRange2Box->addWidget(new QLabel(" to: "));
	weightRange2Box->addWidget(maxWeightRange2Edit);
	weightRange2Box->addStretch(10);
	mainVBox->addLayout(weightRange2Box);
	mainVBox->addSpacing(10);

	//Delay
	minDelayEdit = new QLineEdit("1");
	minDelayEdit->setValidator(delayValidator);
	maxDelayEdit = new QLineEdit("1");
	maxDelayEdit->setValidator(delayValidator);
	QHBoxLayout* delayBox = new QHBoxLayout();
	delayBox->addWidget(new QLabel("Delay (ms) from: "));
	delayBox->addWidget(minDelayEdit);
	delayBox->addWidget(new QLabel(" to: "));
	delayBox->addWidget(maxDelayEdit);
	delayBox->addStretch(10);
	mainVBox->addLayout(delayBox);
	mainVBox->addSpacing(10);

	//Add connection probability and synapse type
	connectionProbabilityEdit = new QLineEdit("1.0");
	connectionProbabilityEdit->setMaximumSize(100, 30);
	connectionProbabilityEdit->setValidator(doubleValidator);
	synapseTypeCombo = new QComboBox();
	addSynapseTypes(synapseTypeCombo);
	QHBoxLayout* miscLayout = new QHBoxLayout();
	miscLayout->addWidget(new QLabel(" Connection probability (0-1): "));
	miscLayout->addWidget(connectionProbabilityEdit);
	miscLayout->addWidget(new QLabel(" Synapse type: "));
	miscLayout->addWidget(synapseTypeCombo);
	miscLayout->addStretch(5);
	mainVBox->addLayout(miscLayout);
	mainVBox->addSpacing(10);

	//Random Seed
	seedEdit = new QLineEdit("1234");
	seedEdit->setMaximumSize(100, 30);
	seedEdit->setValidator(seedValidator);
	QHBoxLayout* seedLayout = new QHBoxLayout();
	seedLayout->addWidget(new QLabel("Random seed: "));
	seedLayout->addWidget(seedEdit);
	seedLayout->addStretch(5);
	mainVBox->addLayout(seedLayout);
	mainVBox->addSpacing(10);

	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 600);
}




/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/



void CARLsimCNSWbWidget::updateTemplate(int i) {

	configLoader = configLoaders[i];

	tof2vel.weightsSpin->setValue(Util::getFloat(configLoader->getParameter("tof2vel_weights", defaults["tof2vel_weights"])));
	tof2vel.weightFactorSpin->setValue(Util::getFloat(configLoader->getParameter("tof2vel_w_factor", defaults["tof2vel_w_factor"])));
	tof2vel.delaysSpin->setValue(Util::getInt(configLoader->getParameter("tof2vel_delays", defaults["tof2vel_delays"])));

	ps2vel.weightsSpin->setValue(Util::getFloat(configLoader->getParameter("ps2vel_weights", defaults["ps2vel_weights"])));
	ps2vel.weightFactorSpin->setValue(Util::getFloat(configLoader->getParameter("ps2vel_w_factor", defaults["ps2vel_w_factor"])));
	ps2vel.delaysSpin->setValue(Util::getInt(configLoader->getParameter("ps2vel_delays", defaults["ps2vel_delays"])));



};

//Define the principal cells of CA1
void CARLsimCNSWbWidget::ConnectionParam_t::addGroup(QString name, QGridLayout* gridLayout, ConfigLoader* configLoader, bool inhib) {

	int row;

	//Define Pyramidal Neurons by Izhikevich 4-parameter model
	row = gridLayout->rowCount();
	gridLayout->addWidget(new QLabel(name), row, 0);

	//Add EditBox for the Weights between the PC
	QHBoxLayout* weightsLayout = new QHBoxLayout();
	weightsLayout->addSpacing(20);
	weightsLayout->addWidget(new QLabel("weights:"));
	weightsSpin = new QDoubleSpinBox();
	if (inhib) {
		weightsSpin->setMinimum(-1.0);
		weightsSpin->setMaximum(0.0);
		weightsSpin->setDecimals(5);
		weightsSpin->setSingleStep(0.01);
	}
	else {
		weightsSpin->setMinimum(0.0);
		weightsSpin->setMaximum(4.0);
		weightsSpin->setDecimals(4);
		weightsSpin->setSingleStep(0.1);		
	}
	weightsSpin->setFixedWidth(70);
	weightsLayout->addWidget(weightsSpin);
	weightsLayout->addWidget(new QLabel("x"));
	weightFactorSpin = new QDoubleSpinBox();
	weightFactorSpin->setMinimum(0.0);
	if(inhib)
		weightFactorSpin->setMaximum(1000.);
	else
		weightFactorSpin->setMaximum(100.);
	weightFactorSpin->setDecimals(0);
	weightFactorSpin->setSingleStep(10);
	weightFactorSpin->setFixedWidth(60);
	weightsLayout->addWidget(weightFactorSpin);

	weightsLayout->addSpacing(10);

	weightsLayout->addWidget(new QLabel("delays:"));
	delaysSpin = new QSpinBox();
	delaysSpin->setMinimum(0);
	delaysSpin->setMaximum(20);
	delaysSpin->setSingleStep(1);
	weightsLayout->addWidget(delaysSpin);
	weightsLayout->addWidget(new QLabel("(ms)"));

	weightsLayout->addStretch(1);
	gridLayout->addLayout(weightsLayout, row, 1);
}



