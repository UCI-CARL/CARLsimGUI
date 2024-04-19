//SpikeStream includes
#include "Globals.h"
#include "NoiseInjectorWidget.h"
#include "NoiseInjectorTableView.h"
#include "NoiseInjectorXmlLoader.h"
#include "NoiseInjectorModel.h"
#include "CarlsimWrapper.h"
#include "Util.h"

using namespace spikestream::carlsim_injectors;

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QGroupBox>

 
//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new NoiseInjectorWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		return QString("Noise Injector");  
	}
}


/*! Constructor */
NoiseInjectorWidget::NoiseInjectorWidget(QWidget* parent) : AbstractInjectorWidget(parent){

	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//placeholder the for adhoc feed, from spike stream main widget
	// is refreshed by the current selected injector
	// allows Noise Injection (Button) and modification of the parameter (group is key -> new/update)


	//Group box for injection controls
	injectGroupBox = new QGroupBox("Noise");
	QVBoxLayout* injectVBox = new QVBoxLayout();

	//Add widgets to inject noise into specified layers
	QHBoxLayout* injectNoiseBox = new QHBoxLayout();

	injectNoiseButton = new QPushButton("Inject");
	injectNoiseButton->setMinimumHeight(20);
	connect(injectNoiseButton, SIGNAL(clicked()), this, SLOT(injectButtonClicked()));
	injectNoiseBox->addWidget(injectNoiseButton);

	
	injectNoiseNeuronGroupCombo = new QComboBox();
	injectNoiseNeuronGroupCombo->setMinimumSize(190, 20);
	injectNoiseBox->addWidget(injectNoiseNeuronGroupCombo);
	connect (injectNoiseNeuronGroupCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateNeuronGroup(const QString &)));

	injectNoisePercentCombo = new QComboBox();
	injectNoisePercentCombo->addItem("0.1 %");
	injectNoisePercentCombo->addItem("1 %");
	injectNoisePercentCombo->addItem("5 %");
	for(int i=10; i<=100; i += 10)
		injectNoisePercentCombo->addItem(QString::number(i) + " %");
	injectNoisePercentCombo->setMinimumSize(60, 20);
	injectNoiseBox->addWidget(injectNoisePercentCombo);

	injectNoiseCurrentCombo = new QComboBox();
	injectNoiseCurrentCombo->addItem("1 mA");
	injectNoiseCurrentCombo->addItem("5 mA");
	for(int i=10; i<=100; i += 10)
		injectNoiseCurrentCombo->addItem(QString::number(i)+" mA");
	injectNoiseCurrentCombo->setMinimumSize(50, 20);
	injectNoiseBox->addWidget(injectNoiseCurrentCombo);


	////Add Button to add injectors to the list defined above or delete injectors from the list
	addButton = new QPushButton("Upsert");  // Add or Update
	addButton->setEnabled(true);
	addButton->setVisible(false); 
	connect (addButton, SIGNAL(clicked()), this, SLOT(addInjector()));
	injectNoiseBox->addWidget(addButton);
	connect (addButton, SIGNAL(clicked()), this, SLOT(addInjector()));

	injectNoiseBox->addStretch(5); 


	//Add inject group box to layout
	injectGroupBox->setLayout(injectVBox);

	injectVBox->addSpacing(2);
	injectVBox->addLayout(injectNoiseBox);

	verticalBox->addWidget(injectGroupBox);

		
	//Construct table view and model
	currentInjectorModel = new NoiseInjectorModel();
	QTableView* currentInjectorView = new NoiseInjectorTableView(this, currentInjectorModel);
	verticalBox->addWidget(currentInjectorView);
	connect (currentInjectorView, SIGNAL(injectorSelected(int)), this, SLOT(updateInjection(int)) ); 

	
}


/*! Destructor */
NoiseInjectorWidget::~NoiseInjectorWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class CarlsimWrapper */
void NoiseInjectorWidget::setWrapper(void *wrapper){
	this->carlsimWrapper = (CarlsimWrapper*)wrapper;

	// crashes in the Constructor (data is NULL)
	injectNoiseCurrentCombo->setEditable(true);  
	injectNoisePercentCombo->setEditable(true);  

	// new Entries are added to the list for convenience
	injectNoiseCurrentCombo->setInsertPolicy(QComboBox::InsertAlphabetically); 
	injectNoisePercentCombo->setInsertPolicy(QComboBox::InsertAlphabetically); 

}


void NoiseInjectorWidget::inject(){
	currentInjectorModel->injectFor(carlsimWrapper); 
}

/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/
void NoiseInjectorWidget::addInjector() {

	auto name = injectNoiseNeuronGroupCombo->currentText();

	auto map = Globals::getNetwork()->getNeuronGroupsMap(); 
	auto g = map[name]; 
	auto type= currentInjectorModel->currentInjection(g)?
		NoiseInjectorModel::CURRENT:NoiseInjectorModel::FIRE;

	auto percentText = injectNoisePercentCombo->currentText().split(" ").first(); 
	auto currentText = injectNoiseCurrentCombo->currentText().split(" ").first(); 

	auto percent = percentText.toFloat();
	auto current = currentText.toFloat(); 

	if(currentInjectorModel->neurGrpList.contains(g)) {
		currentInjectorModel->updateInjector(g->getID(), percent, current); 
	} else {
		currentInjectorModel->appendInjector(g->getID(), type, percent, current, false, carlsimWrapper);  // do not sustain firing immediately after adding
	}

}

void NoiseInjectorWidget::deleteInjectors() {
}


void NoiseInjectorWidget::updateInjection(int i) {
	auto m = currentInjectorModel; 

	auto group = m->neurGrpList[i]; 
	//auto entry = group->getInfo().getName()+"("+QString::number(group->getID())+")"; 
	auto entry = group->getInfo().getName(); 
	
	auto index = injectNoiseNeuronGroupCombo->findText(entry);  
	if(index<0)
		return; 
	injectNoiseNeuronGroupCombo->setCurrentIndex(index); 

	//auto percent = QString::vasprintf("%f %%", m->percentageList[i]); 
	auto percent = QString::number(m->percentageList[i]) + " %"; 
	auto current = QString::number(m->currentList[i]) + " mA"; 

	injectNoisePercentCombo->setEditText(percent); 
	injectNoiseCurrentCombo->setEditText(m->typeList[i]==NoiseInjectorModel::CURRENT?current:""); 

}

void NoiseInjectorWidget::updateNeuronGroup(const QString &name) {
	
	auto map = Globals::getNetwork()->getNeuronGroupsMap(); 
	auto g = map[name]; 
	if(currentInjectorModel->neurGrpList.contains(g)) {
		addButton->setVisible(true); 
		addButton->setText("Update Injector"); 
	} else {
		addButton->setVisible(true); 
		addButton->setText("Add Injector"); 
	}	

	if(currentInjectorModel->currentInjection(g))
		injectNoiseCurrentCombo->setEnabled(true); 
	else {
		injectNoiseCurrentCombo->setEnabled(false); 
		injectNoiseCurrentCombo->setEditText(""); 
	}

		
}



void NoiseInjectorWidget::injectButtonClicked() {

	auto groupName = injectNoiseNeuronGroupCombo->currentText(); 
	auto map = Globals::getNetwork()->getNeuronGroupsMap();
	auto group = map[groupName];   

	if (group == NULL) {
		qDebug() << "inject button without selecting a group " << endl;
		return;
	}

	auto percentText = injectNoisePercentCombo->currentText().split(" ").first(); 
	auto percent = percentText.toFloat();

	if(currentInjectorModel->currentInjection(group)) {
		auto currentText = injectNoiseCurrentCombo->currentText().split(" ").first(); 
		auto current = currentText.toFloat(); 
		currentInjectorModel->injectCurrentVector(group, percent, current); 
	} else {
		currentInjectorModel->injectSpikeVector(group, percent);
	}


}


void NoiseInjectorWidget::loadInjectors() {
	qDebug("loadInjectors"); 

	loadNeuronGroups();

	ConfigLoader* configLoader = new ConfigLoader();

	if (!Util::getBool(configLoader->getParameter("carlsimWidget_autoloadInjectors", "false")))
		return; 

	auto xmlConfigPath = configLoader->getParameter("carlsimWidget_xml_config", "CarlsimWidget.xml");

	qDebug() << " xmlConfigPath: " << xmlConfigPath;

	qDebug() << "move NoiseInjectorXmlLoader from Wrapper to NoiseInjectorWidget";
	
	qDebug() << "loadInjectors from " << xmlConfigPath;

	NoiseInjectorXmlLoader xmlLoader(carlsimWrapper, currentInjectorModel);

	QFile configFile(xmlConfigPath);
	if (!configFile.exists()) {
		QString rootDirectory = Util::getRootDirectory();
		configFile.setFileName(rootDirectory + "/" + xmlConfigPath);
		if (!configFile.exists()) {
			throw SpikeStreamException("Cannot find xml config file " + xmlConfigPath + " in directory " + rootDirectory + " or as absolute path.");
		}
	}
	qDebug() << "Load Injectors from " << configFile.fileName() << endl;
	xmlLoader.loadFromPath(configFile.fileName());

	emit injectorsLoaded();

}

/*! Loads the current neuron groups into the control widgets */
void NoiseInjectorWidget::loadNeuronGroups(){
	injectNoiseNeuronGroupCombo->clear();

	QList<NeuronGroupInfo> neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();
	foreach(NeuronGroupInfo info, neurGrpInfoList){		
		// display with id
		//injectNoiseNeuronGroupCombo->addItem(info.getName() + "(" + QString::number(info.getID()) + ")");
		// consistent format with table column
		injectNoiseNeuronGroupCombo->addItem(info.getName());
	}
}



/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/



/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/


