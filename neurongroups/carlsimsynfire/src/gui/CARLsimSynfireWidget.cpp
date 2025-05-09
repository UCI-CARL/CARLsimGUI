//SpikeStream includes
#include "CARLsimSynfireWidget.h"
#include "Globals.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
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
	/*! Creates class when library is dynamically loaded. */
	_MSC_DLL_API QWidget* getClass(){
		return new CARLsimSynfireWidget();
	}

	/*! Returns a descriptive name for this widget */
	_MSC_DLL_API QString getName(){
		
		return QString("CARLsim Synfire Builder"); // Both for Inbound and Outbound
	}
}


//Define the principal cells of CA1
void CARLsimSynfireWidget::NeuronParam_t::addGroup(QString name, QGridLayout* gridLayout, ConfigLoader* configLoader) {

	int row;

	//Define Pyramidal Neurons by Izhikevich 4-parameter model
	row = gridLayout->rowCount();
	gridLayout->addWidget(new QLabel(name), row, 0);
	QHBoxLayout* abcdLayout = new QHBoxLayout();
	abcdLayout->addSpacing(10);

	abcdLayout->addWidget(new QLabel("a:"));
	aSpin = new QDoubleSpinBox();
	abcdLayout->addWidget(aSpin);
	aSpin->setMinimum(-9.9);
	aSpin->setMaximum(9.9);
	aSpin->setDecimals(4);
	aSpin->setSingleStep(0.01);

	abcdLayout->addWidget(new QLabel("b:"));
	bSpin = new QDoubleSpinBox();
	abcdLayout->addWidget(bSpin);
	bSpin->setMinimum(-9.9);
	bSpin->setMaximum(9.9);
	bSpin->setDecimals(4);
	bSpin->setSingleStep(0.02);

	abcdLayout->addWidget(new QLabel("c:"));
	cSpin = new QDoubleSpinBox();
	abcdLayout->addWidget(cSpin);
	cSpin->setMinimum(-70.0);
	cSpin->setMaximum(0.0);
	cSpin->setDecimals(2);
	cSpin->setSingleStep(1.0);

	abcdLayout->addWidget(new QLabel("d:"));
	dSpin = new QDoubleSpinBox();
	dSpin->setMinimum(-25.0);   // DAB
	dSpin->setMaximum(9.9);
	dSpin->setDecimals(3);
	dSpin->setSingleStep(1.0);
	abcdLayout->addWidget(dSpin);

	abcdLayout->addStretch(1);
	gridLayout->addLayout(abcdLayout, row, 1);
}



/*! Constructor */
CARLsimSynfireWidget::CARLsimSynfireWidget(QWidget* parent) : QWidget(parent) {
	//Main vertical box
	QVBoxLayout* mainVBox = new QVBoxLayout();

	QGroupBox* mainGroupBox = new QGroupBox("CARLsim Synfire Neuron Groups", this);


	// Template
	templateCombo = new QComboBox();


	// vte is placed in a subdirectory as it does not depend on the central spikestream config
	QFileInfo configFile(ConfigLoader::getConfigFilePath());
	auto configDir = configFile.absoluteDir().absolutePath() + +"/synfire";
	qDebug() << configDir << endl;

	for (QDirIterator confFileIter(configDir, { "*.config" }, QDir::Files); confFileIter.hasNext(); )
	{
		confFileIter.next();
		auto path = confFileIter.filePath();
		qDebug() << path << endl;
		ConfigLoader* configLoader = new ConfigLoader(path);
		wmConfigLoaders.append(configLoader);
		templateCombo->addItem(configLoader->getParameter("name"));
	}
	connect (templateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTemplate(int)));
	templateCombo->setMinimumSize(50, 20);
	mainVBox->addWidget(templateCombo);


	prefixEdit = new QLineEdit("syn");   // Gexe  Ginh   Group Prefix   syn is the project 
	QHBoxLayout* prefixLayout = new QHBoxLayout();
	prefixLayout->addWidget(new QLabel("Prefix:"));
	prefixLayout->addWidget(prefixEdit);
	prefixLayout->addStretch(1);
	mainVBox->addLayout(prefixLayout);
	mainVBox->addSpacing(10);


	// Neuron Parameter Groups		
	auto groupBox = new QGroupBox("Neuron parameter", parent);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);
	exc.addGroup("Excitatory Groups (RS)", gridLayout, configLoader);
	inh.addGroup("Inhibitory Groups (FS)", gridLayout, configLoader);
	groupBox->setLayout(gridLayout);
	mainVBox->addWidget(groupBox);


	segmentsSpin = new QSpinBox();
	segmentsSpin->setMinimum(1);
	segmentsSpin->setMaximum(10);
	segmentsSpin->setValue(4); 
	QHBoxLayout* segmentsLayout = new QHBoxLayout();
	segmentsLayout->addWidget(new QLabel("Segments"));
	segmentsLayout->addWidget(segmentsSpin);
	segmentsLayout->addStretch(1);
	mainVBox->addLayout(segmentsLayout);
	mainVBox->addSpacing(5);
	
	{
		auto groupBox = new QGroupBox("Segment groups layout", parent);
		QGridLayout* gridLayout = new QGridLayout();
		gridLayout->setMargin(10);

		{
			int row = gridLayout->rowCount();
			gridLayout->addWidget(new QLabel("Excitatory group"), row, 0);
			QHBoxLayout* abcdLayout = new QHBoxLayout();
			abcdLayout->addSpacing(10);

			excWidthSpin = new QSpinBox();
			excWidthSpin->setMinimum(1);
			excWidthSpin->setMaximum(1000);
			excWidthSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("Width:"));
			abcdLayout->addWidget(excWidthSpin);

			excHeightSpin = new QSpinBox();
			excHeightSpin->setMinimum(1);
			excHeightSpin->setMaximum(1000);
			excHeightSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("Height:"));
			abcdLayout->addWidget(excHeightSpin);
			abcdLayout->addStretch(1);

			gridLayout->addLayout(abcdLayout, row, 1);
		}

		{
			int row = gridLayout->rowCount();
			gridLayout->addWidget(new QLabel("Inhibitory group"), row, 0);
			QHBoxLayout* abcdLayout = new QHBoxLayout();
			abcdLayout->addSpacing(10);

			inhWidthSpin = new QSpinBox();
			inhWidthSpin->setMinimum(1);
			inhWidthSpin->setMaximum(1000);
			inhWidthSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("Width"));
			abcdLayout->addWidget(inhWidthSpin);

			inhHeightSpin = new QSpinBox();
			inhHeightSpin->setMinimum(1);
			inhHeightSpin->setMaximum(1000);
			inhHeightSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("Height:"));
			abcdLayout->addWidget(inhHeightSpin);
			abcdLayout->addStretch(1);

			gridLayout->addLayout(abcdLayout, row, 1);

			groupBox->setLayout(gridLayout);
			mainVBox->addWidget(groupBox);
		}
	}



	//statesSpin = new QSpinBox();
	//statesSpin->setMinimum(2);  // Idle, Filled
	//statesSpin->setMaximum(5);  // Experimental
	//statesSpin->setValue(3);	// Idle, Filled, Resting
	//QHBoxLayout* indicatorsLayout = new QHBoxLayout();
	//indicatorsLayout->addWidget(new QLabel("States:"));
	//indicatorsLayout->addWidget(statesSpin);
	//indicatorsLayout->addWidget(new QLabel("(internal state neurons)"));
	//indicatorsLayout->addStretch(1);
	//mainVBox->addLayout(indicatorsLayout);
	//mainVBox->addSpacing(10);




	//policyCombo = new QComboBox();
	//policyCombo->addItem("Standard"); // Rows 
	//QHBoxLayout* policyLayout = new QHBoxLayout();
	//policyLayout->addWidget(new QLabel("Layout Policy"));
	//policyLayout->addWidget(policyCombo);
	//policyLayout->addStretch(1);
	//mainVBox->addLayout(policyLayout);
	//mainVBox->addSpacing(10);

	//spaceSpin = new QSpinBox();
	//spaceSpin->setMinimum(0);
	//spaceSpin->setMaximum(10);
	//spaceSpin->setValue(1);
	//QHBoxLayout* spaceLayout = new QHBoxLayout();
	//spaceLayout->addWidget(new QLabel("Space between Channels:"));
	//spaceLayout->addWidget(spaceSpin);
	//spaceLayout->addWidget(new QLabel("grid points"));
	//spaceLayout->addStretch(1);
	//mainVBox->addLayout(spaceLayout);
	//mainVBox->addSpacing(10);


	//Validators for double and integer parameters
	QDoubleValidator* doubleValidator = new QDoubleValidator(0.0, 1.0, 2, this);
	QDoubleValidator* percentValidator = new QDoubleValidator(0.0, 100.0, 2, this);
	QIntValidator* posValidator = new QIntValidator(-1000000, 1000000, this);
	QIntValidator* positiveIntValidator = new QIntValidator(0, 1000000, this);

	////Add name and description widgets
	//nameEdit = new QLineEdit("Unnamed");
	//descriptionEdit = new QLineEdit("Undescribed");
	//QHBoxLayout* nameDescLayout = new QHBoxLayout();
	//nameDescLayout->addWidget(new QLabel("Name: "));
	//nameDescLayout->addWidget(nameEdit);
	//nameDescLayout->addWidget(new QLabel("Description: "));
	//nameDescLayout->addWidget(descriptionEdit);
	//mainVBox->addLayout(nameDescLayout);
	//mainVBox->addSpacing(10);

	////Add position input widgets
	//xPosEdit = new QLineEdit("1");
	//xPosEdit->setMaximumSize(100 , 30);
	//xPosEdit->setValidator(posValidator);
	//yPosEdit = new QLineEdit("1");
	//yPosEdit->setMaximumSize(100 , 30);
	//yPosEdit->setValidator(posValidator);
	//zPosEdit = new QLineEdit("2");
	//zPosEdit->setMaximumSize(100 , 30);
	//zPosEdit->setValidator(posValidator);
	//QHBoxLayout* positionLayout = new QHBoxLayout();
	//positionLayout->addWidget(new QLabel("Position. x: "));
	//positionLayout->addWidget(xPosEdit);
	//positionLayout->addWidget(new QLabel(" y: "));
	//positionLayout->addWidget(yPosEdit);
	//positionLayout->addWidget(new QLabel(" z: "));
	//positionLayout->addWidget(zPosEdit);
	//positionLayout->addStretch(5);
	//mainVBox->addLayout(positionLayout);
	//mainVBox->addSpacing(10);

	////Add width, length and height
	//widthEdit = new QLineEdit("10");
	//widthEdit->setMaximumSize(100, 30);
	//widthEdit->setValidator(positiveIntValidator);
	//lengthEdit = new QLineEdit("10");
	//lengthEdit->setMaximumSize(100, 30);
	//lengthEdit->setValidator(positiveIntValidator);
	//heightEdit = new QLineEdit("10");
	//heightEdit->setMaximumSize(100, 30);
	//heightEdit->setValidator(positiveIntValidator);
	//QHBoxLayout* sizeLayout = new QHBoxLayout();
	//sizeLayout->addWidget(new QLabel("Width (X axis): "));
	//sizeLayout->addWidget(widthEdit);
	//sizeLayout->addWidget(new QLabel(" Length (Y axis): "));
	//sizeLayout->addWidget(lengthEdit);
	//sizeLayout->addWidget(new QLabel(" Height (Z axis): "));
	//sizeLayout->addWidget(heightEdit);
	//sizeLayout->addStretch(5);
	//mainVBox->addLayout(sizeLayout);
	//mainVBox->addSpacing(10);


	//Add button
	QHBoxLayout *addButtonBox = new QHBoxLayout();
	QPushButton *addPushButton = new QPushButton("Add");
	addPushButton->setMaximumSize(100, 30);
	connect(addPushButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
	addButtonBox->addStretch(10);
	addButtonBox->addWidget(addPushButton);
	mainVBox->addLayout(addButtonBox);

	mainGroupBox->setLayout(mainVBox);

	this->setMinimumSize(547, 339);


	//Create builder thread class
	builderThread = new CARLsimSynfireBuilderThread();
	connect (builderThread, SIGNAL(finished()), this, SLOT(builderThreadFinished()));
	connect(builderThread, SIGNAL( progress(int, int, QString) ), this, SLOT( updateProgress(int, int, QString) ) );


	//Set Defaults for Config File applying the same names	

	defaults["prefix"] = "C";     // Gexc  Ginh, Gstim  -> G  depends on paper and .. 

	defaults["segments"] = "4";

	defaults["exc_columns"] = "10";
	defaults["exc_rows"] = "20";

	defaults["inh_columns"] = "10";
	defaults["inh_rows"] = "5";


	// stim: Custom Excitatory
	// configured by NormalSpikeGen mean, sd, spikes

	// exc:  RS
	defaults["exc_a"] = "0.02";
	defaults["exc_b"] = "0.2";
	defaults["exc_c"] = "-65.0";
	defaults["exc_d"] = "8.0";
	
	// inh:  FS
	defaults["inh_a"] = "0.1";
	defaults["inh_b"] = "0.2";
	defaults["inh_c"] = "-65.0";
	defaults["inh_d"] = "2.0";


	updateTemplate(0);

}


/*! Destructor */
CARLsimSynfireWidget::~CARLsimSynfireWidget(){


}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Called when add button is clicked.
	Checks the input and adds the specified network to the database */
void CARLsimSynfireWidget::addButtonClicked(){
	//Double check network is loaded
	if(!Globals::networkLoaded()){
		QMessageBox::critical(this, "CARLsimCNS Neuron Group Builder Error", "No network loaded.", QMessageBox::Ok);
		return;
	}

	
	////Check inputs are not empty
	//try{
	//	checkInput(xPosEdit, "X position has not been set.");
	//	checkInput(yPosEdit, "Y position has not been set.");
	//	checkInput(zPosEdit, "Z position has not been set.");
	//}
	//catch(SpikeStreamException& ex){
	//	QMessageBox::warning(this, "CARLsimCNS Neuron Group Builder", ex.getMessage(), QMessageBox::Ok);
	//	return;
	//}

	////Extract variables
	//int xPos = Util::getInt(xPosEdit->text());
	//int yPos = Util::getInt(yPosEdit->text());
	//int zPos = Util::getInt(zPosEdit->text());

	////Store parameters in parameter map
	//QHash<QString, double> paramMap;
	//paramMap["x"] = xPos;
	//paramMap["y"] = yPos;
	//paramMap["z"] = zPos;




	//Start thread to add neuron group
	try{
		//builderThread->prepareAddNeuronGroups(nameEdit->text(), descriptionEdit->text(), paramMap);
		builderThread->prepareAddNeuronGroups(	
			prefixEdit->text(),
			segmentsSpin->value(),
			excWidthSpin->value(),
			excHeightSpin->value(),
			inhWidthSpin->value(),
			inhHeightSpin->value(),
			CARLsimSynfireBuilderThread::NeuronParam_t( 
				exc.aSpin->value(), exc.bSpin->value(), exc.cSpin->value(), exc.dSpin->value()),
			CARLsimSynfireBuilderThread::NeuronParam_t(
				inh.aSpin->value(), inh.bSpin->value(), inh.cSpin->value(), inh.dSpin->value())
		);
		progressDialog = new QProgressDialog("Building Synfire neuron groups", "Cancel", 0, 100, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(2000);
		builderThread->start();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred";
	}
}


/*! Called when the builder thread has finished */
void CARLsimSynfireWidget::builderThreadFinished(){
	if(builderThread->isError())
		qCritical()<<builderThread->getErrorMessage();

	progressDialog->close();

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}


/*! Updates user with feedback about progress with the operation */
void CARLsimSynfireWidget::updateProgress(int stepsCompleted, int totalSteps, QString message){
	//Set flag to avoid multiple calls to progress dialog while it is redrawing
	if(updatingProgress)
		return;
	updatingProgress = true;

	//Check for cancellation
	if(progressDialog->wasCanceled()){
		builderThread->stop();
		progressDialog->setLabelText("Cleaning up.");
		progressDialog->setCancelButton(0);//Should not be able to cancel the clean up
		progressDialog->show();
	}
	//Update progress
	else if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
		progressDialog->setLabelText(message);
	}
	else{
		progressDialog->close();
	}

	//Clear flag to indicate that update of progress is complete
	updatingProgress = false;
}


void CARLsimSynfireWidget::updateTemplate(int i) {

	configLoader = wmConfigLoaders[i];
	
	prefixEdit->setText(configLoader->getParameter("prefix", defaults["prefix"]));

	segmentsSpin->setValue(Util::getInt(configLoader->getParameter("segments", defaults["segments"])));

	excWidthSpin->setValue(Util::getInt(configLoader->getParameter("exc_columns", defaults["exc_columns"])));
	excHeightSpin->setValue(Util::getInt(configLoader->getParameter("exc_rows", defaults["exc_rows"])));

	inhWidthSpin->setValue(Util::getInt(configLoader->getParameter("inh_columns", defaults["inh_columns"])));
	inhHeightSpin->setValue(Util::getInt(configLoader->getParameter("inh_rows", defaults["inh_rows"])));

	exc.aSpin->setValue(Util::getFloat(configLoader->getParameter("exc_a", defaults["exc_a"])));
	exc.bSpin->setValue(Util::getFloat(configLoader->getParameter("exc_b", defaults["exc_b"])));
	exc.cSpin->setValue(Util::getFloat(configLoader->getParameter("exc_c", defaults["exc_c"])));
	exc.dSpin->setValue(Util::getFloat(configLoader->getParameter("exc_d", defaults["exc_d"])));

	inh.aSpin->setValue(Util::getFloat(configLoader->getParameter("inh_a", defaults["inh_a"])));
	inh.bSpin->setValue(Util::getFloat(configLoader->getParameter("inh_b", defaults["inh_b"])));
	inh.cSpin->setValue(Util::getFloat(configLoader->getParameter("inh_c", defaults["inh_c"])));
	inh.dSpin->setValue(Util::getFloat(configLoader->getParameter("inh_d", defaults["inh_d"])));

};

/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Checks that there is valid input in the specified combo box and throws an exception with the specified error if not. */
void CARLsimSynfireWidget::checkInput(QLineEdit* inputEdit, const QString& errMsg){
	if(inputEdit->text().isEmpty()){
		throw SpikeStreamException(errMsg);
	}
}


