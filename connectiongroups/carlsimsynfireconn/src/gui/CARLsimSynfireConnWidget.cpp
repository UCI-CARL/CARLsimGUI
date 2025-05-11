//SpikeStream includes
#include "CARLsimSynfireConnWidget.h"
#include "Globals.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "CARLsimSynfireConnBuilderThread.h"
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
	/*! Creates a CARLsimSynfireConnWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CARLsimSynfireConnWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("CARLsimSynfireConn connection group builder");
	}
}


/*! Constructor */
CARLsimSynfireConnWidget::CARLsimSynfireConnWidget(QWidget* parent) : AbstractConnectionWidget(parent) {
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
	connectionBuilder = new CARLsimSynfireConnBuilderThread();
	connect (connectionBuilder, SIGNAL(finished()), this, SLOT(builderThreadFinished()));
	connect(connectionBuilder, SIGNAL( progress(int, int, QString) ), this, SLOT( updateProgress(int, int, QString) ), Qt::QueuedConnection);


	//Set Defaults for Config File applying the same names	

	defaults["prefix"] = "C";     // Gexc  Ginh, Gstim  -> G  depends on paper and .. 

	defaults["segments"] = "4";

	defaults["loop"] = "true";

	// decouple, state requires phasic bursting to be activated reliable
	defaults["exc2exc_weights"] = "0.01";
	defaults["exc2exc_w_factor"] = "100.0";
	defaults["exc2exc_delays"] = "10";
	defaults["exc2exc_syn_per_neuron"] = "60";  // Höppner2022

	// decouple, state requires phasic bursting to be activated reliable
	defaults["exc2inh_weights"] = "0.035";  // effective inhibition, ineffective with 0.01
	defaults["exc2inh_w_factor"] = "100.0";
	defaults["exc2inh_delays"] = "10";
	defaults["exc2inh_syn_per_neuron"] = "60";  // Höppner2022

	// decouple, state requires phasic bursting to be activated reliable
	defaults["inh2exc_weights"] = "-0.02";
	defaults["inh2exc_w_factor"] = "100.0";
	//defaults["inh2exc_delays"] = "2";  // + 1ms of the FS = 10ms
	defaults["inh2exc_delays"] = "8";  // Höppner2022
	defaults["inh2exc_syn_per_neuron"] = "25";  // Höppner2022

	updateTemplate(0);
}


/*! Destructor */
CARLsimSynfireConnWidget::~CARLsimSynfireConnWidget(){
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

//Overridden
bool CARLsimSynfireConnWidget::checkInputs(){
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
		QMessageBox::warning(this, "CARLsim Synfire Connection Group Builder", ex.getMessage(), QMessageBox::Ok);
		return false;
	}

	// dirty
	CARLsimSynfireConnBuilderThread* builder = (CARLsimSynfireConnBuilderThread*)connectionBuilder;

	builder->exc2exc.weights = exc2exc.weightsSpin->value();
	builder->exc2exc.w_factor = exc2exc.weightFactorSpin->value();
	builder->exc2exc.delays = exc2exc.delaysSpin->value();
	builder->exc2exc.syn_per_neuron = exc2exc.synPerNeuronSpin->value();

	builder->exc2inh.weights = exc2inh.weightsSpin->value();
	builder->exc2inh.w_factor = exc2inh.weightFactorSpin->value();
	builder->exc2inh.delays = exc2inh.delaysSpin->value();
	builder->exc2inh.syn_per_neuron = exc2inh.synPerNeuronSpin->value();

	builder->inh2exc.weights = inh2exc.weightsSpin->value();
	builder->inh2exc.w_factor = inh2exc.weightFactorSpin->value();
	builder->inh2exc.delays = inh2exc.delaysSpin->value();
	builder->inh2exc.syn_per_neuron = inh2exc.synPerNeuronSpin->value();

	// Höppner2022
	builder->prefix = prefixEdit->text();
	builder->segments = segmentsSpin->value();
	builder->loop = loopCheck->isChecked();

	//builder->n = 200;  // TODO get nbetter name 

	//Inputs are ok
	return true;
}


//Override
ConnectionGroupInfo CARLsimSynfireConnWidget::getConnectionGroupInfo(){

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
void CARLsimSynfireConnWidget::buildGUI(QVBoxLayout* mainVBox){
	QGroupBox* mainGroupBox = new QGroupBox("CARLsimSynfireConn Connection Group Builder", this);

	// Template
	templateCombo = new QComboBox();

	//placed in a subdirectory as it does not depend on the central spikestream config
	QFileInfo configFile(ConfigLoader::getConfigFilePath());
	auto configDir = configFile.absoluteDir().absolutePath() + +"/synfire";  // CAUTION: shared file, as must match by name e.g. prefix or column size
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

	{
		QHBoxLayout* abcdLayout = new QHBoxLayout();
		abcdLayout->addSpacing(10);

		prefixEdit = new QLineEdit("syn");   // Gexe  Ginh   Group Prefix   syn is the project 
		abcdLayout->addWidget(new QLabel("Prefix:"));
		abcdLayout->addWidget(prefixEdit);

		segmentsSpin = new QSpinBox();
		segmentsSpin->setMinimum(1);
		segmentsSpin->setMaximum(10);
		segmentsSpin->setValue(4);
		abcdLayout->addWidget(new QLabel("Segments:"));
		abcdLayout->addWidget(segmentsSpin);

		loopCheck = new QCheckBox();
		loopCheck->setChecked(false);
		abcdLayout->addWidget(new QLabel("Loop:"));
		abcdLayout->addWidget(loopCheck);
		abcdLayout->addStretch(1);

		mainVBox->addLayout(abcdLayout);
		mainVBox->addSpacing(5);
	}


	{

		// Neuron Parameter Groups		
		auto groupBox2 = new QGroupBox("Connection parameter", this);
		QGridLayout* gridLayout2 = new QGridLayout();
		gridLayout2->setMargin(10);
		auto column = 1;

		exc2exc.addGroup("Exc[i-1] -> Exc[i]", gridLayout2, configLoader);

		exc2inh.addGroup("Exc[i-1] -> Inh[i]", gridLayout2, configLoader);

		inh2exc.addGroup("Inh[i] -> Exc[i]", gridLayout2, configLoader, true);


		groupBox2->setLayout(gridLayout2);
		mainVBox->addWidget(groupBox2);
	}

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



void CARLsimSynfireConnWidget::updateTemplate(int i) {

	configLoader = configLoaders[i];

	prefixEdit->setText(configLoader->getParameter("prefix", defaults["prefix"]));

	segmentsSpin->setValue(Util::getInt(configLoader->getParameter("segments", defaults["segments"])));

	loopCheck->setChecked(Util::getBool(configLoader->getParameter("loop", defaults["loop"])));

	exc2exc.weightsSpin->setValue(Util::getFloat(configLoader->getParameter("exc2exc_weights", defaults["exc2exc_weights"])));
	exc2exc.weightFactorSpin->setValue(Util::getFloat(configLoader->getParameter("exc2exc_w_factor", defaults["exc2exc_w_factor"])));
	exc2exc.delaysSpin->setValue(Util::getInt(configLoader->getParameter("exc2exc_delays", defaults["exc2exc_delays"])));
	exc2exc.synPerNeuronSpin->setValue(Util::getInt(configLoader->getParameter("exc2exc_syn_per_neuron", defaults["exc2exc_syn_per_neuron"])));

	exc2inh.weightsSpin->setValue(Util::getFloat(configLoader->getParameter("exc2inh_weights", defaults["exc2inh_weights"])));
	exc2inh.weightFactorSpin->setValue(Util::getFloat(configLoader->getParameter("exc2inh_w_factor", defaults["exc2inh_w_factor"])));
	exc2inh.delaysSpin->setValue(Util::getInt(configLoader->getParameter("exc2inh_delays", defaults["exc2inh_delays"])));
	exc2inh.synPerNeuronSpin->setValue(Util::getInt(configLoader->getParameter("exc2inh_syn_per_neuron", defaults["exc2inh_syn_per_neuron"])));

	inh2exc.weightsSpin->setValue(Util::getFloat(configLoader->getParameter("inh2exc_weights", defaults["inh2exc_weights"])));
	inh2exc.weightFactorSpin->setValue(Util::getFloat(configLoader->getParameter("inh2exc_w_factor", defaults["inh2exc_w_factor"])));
	inh2exc.delaysSpin->setValue(Util::getInt(configLoader->getParameter("inh2exc_delays", defaults["inh2exc_delays"])));
	inh2exc.synPerNeuronSpin->setValue(Util::getInt(configLoader->getParameter("inh2exc_syn_per_neuron", defaults["inh2exc_syn_per_neuron"])));
};

//Define the principal cells of CA1
void CARLsimSynfireConnWidget::ConnectionParam_t::addGroup(QString name, QGridLayout* gridLayout, ConfigLoader* configLoader, bool inhib) {

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

	weightsLayout->addWidget(new QLabel("synapses:"));
	synPerNeuronSpin = new QSpinBox();
	delaysSpin->setMinimum(0);
	delaysSpin->setMaximum(1000);
	delaysSpin->setSingleStep(1);
	weightsLayout->addWidget(synPerNeuronSpin);
	weightsLayout->addWidget(new QLabel("(per neuron)"));

	weightsLayout->addStretch(1);
	gridLayout->addLayout(weightsLayout, row, 1);
}



