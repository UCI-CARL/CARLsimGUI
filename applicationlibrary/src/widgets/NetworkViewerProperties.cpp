//SpikeStream includes
#include "ConnectionsModel.h"
#include "ConnectionsTableView.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkDisplay.h"
#include "EventRouter.h"
#include "NetworkViewerProperties.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QButtonGroup>
#include <QLayout>


/*! Constructor */
NetworkViewerProperties::NetworkViewerProperties(QWidget* parent) : QWidget(parent){
	//Main vertical layout
	QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);

	//Set up check box to toggle full rendering of scene
	QHBoxLayout *renderBox = new QHBoxLayout();
	renderCheckBox = new QCheckBox("Render high quality view", this);
	connect(renderCheckBox, SIGNAL(clicked()), this, SLOT(setRenderMode()));
	renderBox->addWidget(renderCheckBox);
	renderBox->addStretch(5);
	mainVerticalBox->addLayout(renderBox);

	//Widget for full render controls
	fullRenderControlsWidget = new QWidget();
	fullRenderControlsWidget->setEnabled(false);
	QVBoxLayout* fullRenConVBox = new QVBoxLayout(fullRenderControlsWidget);

	//Slider for neuron transparency
	QHBoxLayout* transparencyBox = new QHBoxLayout();
	QSlider* transparencySlider = new QSlider(Qt::Horizontal);
	transparencySlider->setRange(0, 100);
	transparencySlider->setSliderPosition(100);
	connect(transparencySlider, SIGNAL(sliderMoved(int)), this, SLOT(neuronTransparencyChanged(int)));
	transparencyBox->addWidget(new QLabel("Neuron opacity      "));
	transparencyBox->addWidget(transparencySlider);
	transparencyBox->addWidget(new QLabel("max"));
	transparencyBox->addStretch(3);
	fullRenConVBox->addLayout(transparencyBox);

	//Slider for synaptic connection transparency
	QHBoxLayout* transparencyBox2 = new QHBoxLayout();
	QSlider* transparencySlider2 = new QSlider(Qt::Horizontal);
	transparencySlider2->setRange(0, 100);
	transparencySlider2->setSliderPosition(100);
	connect(transparencySlider2, SIGNAL(sliderMoved(int)), this, SLOT(connectionTransparencyChanged(int)));
	transparencyBox2->addWidget(new QLabel("Connection opacity "));
	transparencyBox2->addWidget(transparencySlider2);
	transparencyBox2->addWidget(new QLabel("max"));
	transparencyBox2->addStretch(3);
	fullRenConVBox->addLayout(transparencyBox2);

	//Weights
	QHBoxLayout* weightRenderLayout = new QHBoxLayout();
	weightsCheckBox = new QCheckBox("Weights:", this);
	connect(weightsCheckBox, SIGNAL(clicked()), this, SLOT(setRenderWeights()));
	weightRenderLayout->addWidget(weightsCheckBox);

	weightButtonGroup = new QButtonGroup();
	connect(weightButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(weightDisplayChanged(int)));	
	QRadioButton* tempWeightsButton = new QRadioButton("learned");
	tempWeightsButton->setChecked(true);
	weightButtonGroup->addButton(tempWeightsButton, 1);
	weightRenderLayout->addWidget(tempWeightsButton);
	QRadioButton* currentWeightsButton = new QRadioButton("initial");
	weightButtonGroup->addButton(currentWeightsButton, 0);  
	weightRenderLayout->addWidget(currentWeightsButton);
	fullRenConVBox->addLayout(weightRenderLayout);

	QHBoxLayout* styleRenderLayout = new QHBoxLayout();
	styleButtonGroup = new QButtonGroup(); // connection style
	connect(styleButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(styleDisplayChanged(int)));
	synapseStyleButton = new QRadioButton("Synapse");  // end
	synapseStyleButton->setChecked(true);
	synapseStyleButton->setDisabled(true);
	styleButtonGroup->addButton(synapseStyleButton, 0);
	styleRenderLayout->addWidget(synapseStyleButton);
	middleStyleButton = new QRadioButton("middle"); // as before
	middleStyleButton->setDisabled(true);
	styleButtonGroup->addButton(middleStyleButton, 1);
	styleRenderLayout->addWidget(middleStyleButton);
	evenStyleButton = new QRadioButton("even");  // as a cylinder
	evenStyleButton->setDisabled(true);
	styleButtonGroup->addButton(evenStyleButton, 2);
	styleRenderLayout->addWidget(evenStyleButton);
	fullRenConVBox->addLayout(styleRenderLayout);

	//Delays
	QHBoxLayout* delayRenderLayout = new QHBoxLayout();
	delaysCheckBox = new QCheckBox("Delays:", this);
	connect(delaysCheckBox, SIGNAL(clicked()), this, SLOT(setRenderDelays()));
	delayRenderLayout->addWidget(delaysCheckBox);
	delayButtonGroup = new QButtonGroup();
	connect(delayButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(delayDisplayChanged(int)));
	QRadioButton* tempDelaysButton = new QRadioButton("learned");
	//tempDelaysButton->setDisabled(true);
	tempDelaysButton->setChecked(true);
	delayButtonGroup->addButton(tempDelaysButton, 1);
	delayRenderLayout->addWidget(tempDelaysButton);
	QRadioButton* currentDelaysButton = new QRadioButton("initial");
	//currentDelaysButton->setChecked(true);
	delayButtonGroup->addButton(currentDelaysButton, 0);
	delayRenderLayout->addWidget(currentDelaysButton);
	fullRenConVBox->addLayout(delayRenderLayout);


	//Add full render controls widget to main layout
	QHBoxLayout* fullRenderHolderBox = new QHBoxLayout();
	fullRenderHolderBox->addSpacing(30);
	fullRenderHolderBox->addWidget(fullRenderControlsWidget);
	fullRenderHolderBox->addStretch(5);
	mainVerticalBox->addLayout(fullRenderHolderBox);

	//Button group to set connection mode
	QButtonGroup* conButGroup = new QButtonGroup();

	//All connection widgets
	allConsButt = new QRadioButton("All selected connections");
	conButGroup->addButton(allConsButt);
	mainVerticalBox->addWidget(allConsButt);

	//Single neuron widgets
	conSingleNeurButt = new QRadioButton("Neuron");
	conButGroup->addButton(conSingleNeurButt);
	singleNeuronIDLabel = new QLabel("");
	QHBoxLayout* singleNeuronBox = new QHBoxLayout();
	singleNeuronBox->addWidget(conSingleNeurButt);
	singleNeuronBox->addWidget(singleNeuronIDLabel);
	singleNeuronBox->addWidget(new QLabel ("connections"));
	singleNeuronBox->addStretch(5);
	mainVerticalBox->addLayout(singleNeuronBox);

	fromToCombo = new QComboBox();
	fromToCombo->addItem("From and To");
	fromToCombo->addItem("From");
	fromToCombo->addItem("To");
	connect(fromToCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fromToSelectionChanged(int)));
	QHBoxLayout* fromToSingleBox = new QHBoxLayout();
	fromToSingleBox->addSpacing(20);
	fromToSingleBox->addWidget(fromToCombo);
	truthTableButton = new QPushButton("Truth Table");
	truthTableButton->setVisible(false);
	connect(truthTableButton, SIGNAL(clicked()), this, SLOT(showTruthTable()));
	fromToSingleBox->addWidget(truthTableButton);
	fromToSingleBox->addStretch(5);
	mainVerticalBox->addLayout(fromToSingleBox);

	//Between neuron widgets
	conBetweenNeurButt = new QRadioButton("Connection ");
	conButGroup->addButton(conBetweenNeurButt);
	QHBoxLayout* betweenBox = new QHBoxLayout();
	betweenBox->addWidget(conBetweenNeurButt);
	fromLabel = new QLabel("From: ");
	betweenBox->addWidget(fromLabel);
	fromNeuronIDLabel = new QLabel("");
	betweenBox->addWidget(fromNeuronIDLabel);
	toLabel = new QLabel("to: ");
	betweenBox->addWidget(toLabel);
	toNeuronIDLabel = new QLabel("");
	betweenBox->addWidget(toNeuronIDLabel);
	betweenBox->addStretch(5);
	mainVerticalBox->addLayout(betweenBox);


	//Positive and negative filtering
	QHBoxLayout* posNegBox = new QHBoxLayout();
	posNegBox->addWidget(new QLabel("Filter by connection weight: "));
	posNegCombo = new QComboBox();
	posNegCombo->addItem("All connections");
	posNegCombo->addItem("Positive connections");
	posNegCombo->addItem("Negative connections");
	connect(posNegCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(posNegSelectionChanged(int)));
	posNegBox->addWidget(posNegCombo);
	posNegBox->addStretch(5);
	mainVerticalBox->addLayout(posNegBox);

	//Add table view and model
	QAbstractTableModel* connectionsModel = new ConnectionsModel();
	QTableView* connectionsView = new ConnectionsTableView(this, connectionsModel);
	connectionsView->setMinimumWidth(500);
	connectionsView->setMinimumHeight(500);
	mainVerticalBox->addWidget(connectionsView);

	//Label listing number of connections in the table
	QHBoxLayout* numConLabBox = new QHBoxLayout();
	numConLabBox->addSpacing(5);
	numberOfConnectionsLabel = new QLabel("Number of connections: 0");
	numConLabBox->addWidget(numberOfConnectionsLabel);
	numConLabBox->addStretch(5);
	mainVerticalBox->addLayout(numConLabBox);
	mainVerticalBox->addStretch(5);

	//Listen for changes in the network display
	connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(networkDisplayChanged()));
	connect(Globals::getEventRouter(), SIGNAL(visibleConnectionsChangedSignal()), this, SLOT(updateConnectionCount()));

	/* Initialize truth table dialog to NULL
		Otherwise it appears as an annoying flash up during boot up of SpikeStream */
	truthTableDialog = NULL;

	//Initial state is to show all connections
	showAllConnections();

}


/*! Destructor */
NetworkViewerProperties::~NetworkViewerProperties(){
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! User has selected or deselected a from neuron. Settings are adjusted to take account of this. */
void NetworkViewerProperties::fromToSelectionChanged(int index){
	if(index == 0){
		Globals::getNetworkDisplay()->clearDirectionFiltering();
	}
	else if (index == 1){
		Globals::getNetworkDisplay()->showFromConnections();
	}
	else if (index == 2){
		Globals::getNetworkDisplay()->showToConnections();
	}
}


/*! The network display settings have changed. */
void NetworkViewerProperties::networkDisplayChanged(){
	unsigned int connectionMode = Globals::getNetworkDisplay()->getConnectionMode();
	if(connectionMode & CONNECTION_MODE_ENABLED){
		if(connectionMode & SHOW_BETWEEN_CONNECTIONS)
			showBetweenConnections();
		else {
			showSingleConnections();			
		}
	}
	else{
		showAllConnections();
	}
}

/*! Called when the slider controlling neuron transparency is changed. */
void NetworkViewerProperties::neuronTransparencyChanged(int sliderValue){
	Globals::getNetworkDisplay()->setNeuronTransparency((float)sliderValue/100.0f);
}

/*! Called when the slider controlling neuron transparency is changed. */
void NetworkViewerProperties::connectionTransparencyChanged(int sliderValue) {
	Globals::getNetworkDisplay()->setConnectionTransparency((float)sliderValue / 100.0f);
}

/*! Filtering for showing/hiding positive/negative connections has been changed. */
void NetworkViewerProperties::posNegSelectionChanged(int index){
	if(index == 0){
		Globals::getNetworkDisplay()->clearWeightFiltering();
	}
	else if (index == 1){
		Globals::getNetworkDisplay()->showPositiveConnections();
	}
	else if (index == 2){
		Globals::getNetworkDisplay()->showNegativeConnections();
	}
}


/*! Switches between full render mode and fast render mode */
void NetworkViewerProperties::setRenderMode(){
	if(renderCheckBox->isChecked()){//Full render mode
		Globals::getNetworkDisplay()->setFullRenderMode(true);
		fullRenderControlsWidget->setEnabled(true);
	}
	else{
		Globals::getNetworkDisplay()->setFullRenderMode(false);
		fullRenderControlsWidget->setEnabled(false);
	}
}


/*! Shows a dialog with the truth table for the selected neuron */
void NetworkViewerProperties::showTruthTable(){
	//Get neuron id.
	unsigned int tmpNeurID = Globals::getNetworkDisplay()->getSingleNeuronID();
	if(tmpNeurID == 0)
		throw SpikeStreamException("Truth table cannot be displayed for an invalid neuron ID");

	//Show non modal dialog
	showTruthTableDialog(tmpNeurID);
}


/*! Updates the field showing the number of connections in the table. */
void NetworkViewerProperties::updateConnectionCount(){
	numberOfConnectionsLabel->setText("Number of connections: " + QString::number( Globals::getNetworkDisplay()->getVisibleConnectionsList().size() ) );
}


/*!  */
void NetworkViewerProperties::setRenderWeights() {
	if (weightsCheckBox->isChecked()) {
		synapseStyleButton->setDisabled(false);
		middleStyleButton->setDisabled(false);
		evenStyleButton->setDisabled(false);
		this->weightDisplayChanged(weightButtonGroup->checkedId());
	}
	else {		
		synapseStyleButton->setDisabled(true);
		middleStyleButton->setDisabled(true);
		evenStyleButton->setDisabled(true);
		delaysCheckBox->setDisabled(false);
		Globals::getNetworkDisplay()->disableWeightRender();
	}
}


/*! Called when the weight display mode is changed */
void NetworkViewerProperties::weightDisplayChanged(int buttonID){
	if(buttonID == 0)
		Globals::getNetworkDisplay()->renderCurrentWeights();
	else if(buttonID == 1)
		Globals::getNetworkDisplay()->renderTempWeights();
	else
		qCritical()<<"Button ID not recognized: "<<buttonID;
}

/*! Called when the weight display mode is changed */
void NetworkViewerProperties::styleDisplayChanged(int buttonID) {
	if (buttonID == 0) {
		delaysCheckBox->setDisabled(false);
		Globals::getNetworkDisplay()->renderWeightSynapse();
	}
	else if (buttonID == 1) {
		delaysCheckBox->setChecked(false);
		delaysCheckBox->setDisabled(true);
		Globals::getNetworkDisplay()->renderWeightMiddle();			
	}
	else if (buttonID == 2) {
		delaysCheckBox->setChecked(false);
		delaysCheckBox->setDisabled(true);
		Globals::getNetworkDisplay()->renderWeightEven();
	}
	else
		qCritical() << "Button ID not recognized: " << buttonID;
}


/*!  */
void NetworkViewerProperties::setRenderDelays() {
	if (delaysCheckBox->isChecked())
		this->delayDisplayChanged(delayButtonGroup->checkedId());
	else
		Globals::getNetworkDisplay()->disableDelayRender();	
}

/*! Called when the weight display mode is changed */
void NetworkViewerProperties::delayDisplayChanged(int buttonID) {
	if (buttonID == 0)
		Globals::getNetworkDisplay()->renderCurrentDelays();
	else if (buttonID == 1)
		Globals::getNetworkDisplay()->renderTempDelays();
	else
		qCritical() << "Button ID not recognized: " << buttonID;
}



/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Sets widget state to match situation in which all connections are visible. */
void NetworkViewerProperties::showAllConnections(){
	allConsButt->setChecked(true);
	allConsButt->setEnabled(true);
	conBetweenNeurButt->setEnabled(false);
	conSingleNeurButt->setEnabled(false);
	singleNeuronIDLabel->setEnabled(false);
	singleNeuronIDLabel->setText("");
	fromToCombo->setEnabled(false);
	fromNeuronIDLabel->setEnabled(false);
	fromNeuronIDLabel->setText("");
	toNeuronIDLabel->setEnabled(false);
	toNeuronIDLabel->setText("");
	fromLabel->setEnabled(false);
	toLabel->setEnabled(false);
	truthTableButton->setVisible(false);
	hideTruthTableDialog();
}


/*! Sets widget state to match situation in which the connections between two neurons are visible. */
void NetworkViewerProperties::showBetweenConnections(){
	conBetweenNeurButt->setChecked(true);
	conBetweenNeurButt->setEnabled(true);
	fromNeuronIDLabel->setEnabled(true);
	fromNeuronIDLabel->setText(QString::number(Globals::getNetworkDisplay()->getSingleNeuronID()));
	toNeuronIDLabel->setEnabled(true);
	toNeuronIDLabel->setText(QString::number(Globals::getNetworkDisplay()->getToNeuronID()));
	fromToCombo->setEnabled(false);
	allConsButt->setEnabled(false);
	conSingleNeurButt->setEnabled(false);
	singleNeuronIDLabel->setEnabled(false);
	singleNeuronIDLabel->setText("");
	fromLabel->setEnabled(true);
	toLabel->setEnabled(true);
	truthTableButton->setVisible(false);
	hideTruthTableDialog();
}


/*! Sets widget state to match situation in which the connections to/from a single neuron are shown. */
void NetworkViewerProperties::showSingleConnections(){
	//Id of the single neuron
	unsigned int singleNeuronID = Globals::getNetworkDisplay()->getSingleNeuronID();

	//Set up graphical components appropriately
	conSingleNeurButt->setChecked(true);
	conSingleNeurButt->setEnabled(true);
	singleNeuronIDLabel->setEnabled(true);
	singleNeuronIDLabel->setText(QString::number(singleNeuronID));
	fromToCombo->setEnabled(true);
	fromNeuronIDLabel->setEnabled(false);
	fromNeuronIDLabel->setText("");
	toNeuronIDLabel->setEnabled(false);
	toNeuronIDLabel->setText("");
	allConsButt->setEnabled(false);
	conBetweenNeurButt->setEnabled(false);
	fromLabel->setEnabled(false);
	toLabel->setEnabled(false);

	//Show button to launch truth table dialog if to neuron connections are shown and it is a weightless neuron
	if(Globals::getNetwork()->getNeuronGroupFromNeuronID(singleNeuronID)->isWeightless()){
		//Update neuron in truth table dialog if it is visible
		if(truthTableDialog != NULL && truthTableDialog->isVisible())
			showTruthTableDialog(singleNeuronID);

		//Show button to display dialog if it is not already visible
		if(fromToCombo->currentText() == "To"){
			truthTableButton->setVisible(true);
		}
		else{
			truthTableButton->setVisible(false);
			hideTruthTableDialog();
		}
	}
	else{
		truthTableButton->setVisible(false);
		hideTruthTableDialog();
	}
}


/*! Shows the dialog with the specified neuron's truth table */
void NetworkViewerProperties::showTruthTableDialog(unsigned int neuronID){
	if(truthTableDialog == NULL)
		truthTableDialog = new TruthTableDialog();
	truthTableDialog->show(neuronID);
}


/*! Hides the dialog with the neuron's truth table */
void NetworkViewerProperties::hideTruthTableDialog(){
	if(truthTableDialog != NULL)
		truthTableDialog->hide();
}

