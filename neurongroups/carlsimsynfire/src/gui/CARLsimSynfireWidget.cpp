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

	{
		QHBoxLayout* abcdLayout = new QHBoxLayout();
		abcdLayout->addSpacing(10);

		prefixEdit = new QLineEdit("syn");   // Gexe  Ginh   Group Prefix   syn is the project 
		abcdLayout->addWidget(new QLabel("Prefix:"));
		abcdLayout->addWidget(prefixEdit);

		abcdLayout->addSpacing(10);

		segmentsSpin = new QSpinBox();
		segmentsSpin->setMinimum(1);
		segmentsSpin->setMaximum(10);
		segmentsSpin->setValue(4);
		abcdLayout->addWidget(new QLabel("Segments:"));
		abcdLayout->addWidget(segmentsSpin);

		abcdLayout->addStretch(1);

		mainVBox->addLayout(abcdLayout);
		mainVBox->addSpacing(5);
	}


	{
		auto groupBox = new QGroupBox("Segment groups layout", parent);
		QGridLayout* gridLayout = new QGridLayout();
		gridLayout->setMargin(10);

		{
			int row = gridLayout->rowCount();
			gridLayout->addWidget(new QLabel("Stimulus"), row, 0);
			QHBoxLayout* abcdLayout = new QHBoxLayout();
			abcdLayout->addSpacing(10);

			stimWidthSpin = new QSpinBox();
			stimWidthSpin->setMinimum(1);
			stimWidthSpin->setMaximum(1000);
			stimWidthSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("width:"));
			abcdLayout->addWidget(stimWidthSpin);

			stimHeightSpin = new QSpinBox();
			stimHeightSpin->setMinimum(1);
			stimHeightSpin->setMaximum(1000);
			stimHeightSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("height:"));
			abcdLayout->addWidget(stimHeightSpin);
			abcdLayout->addStretch(1);

			gridLayout->addLayout(abcdLayout, row, 1);
		}


		{
			int row = gridLayout->rowCount();
			gridLayout->addWidget(new QLabel("Excitatory group"), row, 0);
			QHBoxLayout* abcdLayout = new QHBoxLayout();
			abcdLayout->addSpacing(10);

			excWidthSpin = new QSpinBox();
			excWidthSpin->setMinimum(1);
			excWidthSpin->setMaximum(1000);
			excWidthSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("width:"));
			abcdLayout->addWidget(excWidthSpin);

			excHeightSpin = new QSpinBox();
			excHeightSpin->setMinimum(1);
			excHeightSpin->setMaximum(1000);
			excHeightSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("height:"));
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
			abcdLayout->addWidget(new QLabel("width"));
			abcdLayout->addWidget(inhWidthSpin);

			inhHeightSpin = new QSpinBox();
			inhHeightSpin->setMinimum(1);
			inhHeightSpin->setMaximum(1000);
			inhHeightSpin->setValue(50);
			abcdLayout->addWidget(new QLabel("height:"));
			abcdLayout->addWidget(inhHeightSpin);
			abcdLayout->addStretch(1);

			gridLayout->addLayout(abcdLayout, row, 1);

			groupBox->setLayout(gridLayout);
			mainVBox->addWidget(groupBox);
		}
	}

	// Neuron Parameter Groups		
	auto groupBox = new QGroupBox("Neuron parameter", parent);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);
	exc.addGroup("Excitatory Groups (RS)", gridLayout, configLoader);
	inh.addGroup("Inhibitory Groups (FS)", gridLayout, configLoader);
	groupBox->setLayout(gridLayout);
	mainVBox->addWidget(groupBox);


	{
		auto groupBox = new QGroupBox("Synapses", parent);
		QGridLayout* gridLayout = new QGridLayout();
		gridLayout->setMargin(10);

		{
			int row = gridLayout->rowCount();

			QHBoxLayout* abcdLayout = new QHBoxLayout();

			cobaCheck = new QCheckBox();
			cobaCheck->setChecked(false);
			abcdLayout->addWidget(new QLabel("COBA:"));
			abcdLayout->addWidget(cobaCheck);

			abcdLayout->addSpacing(10);

			cobaAmpaSpin = new QSpinBox();
			cobaAmpaSpin->setMinimum(1);
			cobaAmpaSpin->setMaximum(10);
			segmentsSpin->setValue(4);
			abcdLayout->addWidget(new QLabel("AMPA:"));
			abcdLayout->addWidget(cobaAmpaSpin);

			cobaNmdaSpin = new QSpinBox();
			cobaNmdaSpin->setMinimum(1);
			cobaNmdaSpin->setMaximum(10);
			segmentsSpin->setValue(4);
			abcdLayout->addWidget(new QLabel("NMDA:"));
			abcdLayout->addWidget(cobaNmdaSpin);

			cobaGabaASpin = new QSpinBox();
			cobaGabaASpin->setMinimum(1);
			cobaGabaASpin->setMaximum(10);
			segmentsSpin->setValue(4);
			abcdLayout->addWidget(new QLabel("GABAa:"));
			abcdLayout->addWidget(cobaGabaASpin);

			cobaGabaBSpin = new QSpinBox();
			cobaGabaBSpin->setMinimum(1);
			cobaGabaBSpin->setMaximum(10);
			segmentsSpin->setValue(4);
			abcdLayout->addWidget(new QLabel("GABAb:"));
			abcdLayout->addWidget(cobaGabaBSpin);

			abcdLayout->addStretch(1);

			gridLayout->addLayout(abcdLayout, row, 1);

		}
		groupBox->setLayout(gridLayout);
		mainVBox->addWidget(groupBox);
	}







	//Validators for double and integer parameters
	QDoubleValidator* doubleValidator = new QDoubleValidator(0.0, 1.0, 2, this);
	QDoubleValidator* percentValidator = new QDoubleValidator(0.0, 100.0, 2, this);
	QIntValidator* posValidator = new QIntValidator(-1000000, 1000000, this);
	QIntValidator* positiveIntValidator = new QIntValidator(0, 1000000, this);


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

	defaults["stim_columns"] = "10";
	defaults["stim_rows"] = "20";

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

	// coba
	defaults["coba"] = "true";
	defaults["ampa"] = "1";
	defaults["nmda"] = "2";
	defaults["gaba_a"] = "10";
	defaults["gaba_b"] = "10";

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
			stimWidthSpin->value(),
			stimHeightSpin->value(),
			excWidthSpin->value(),
			excHeightSpin->value(),
			inhWidthSpin->value(),
			inhHeightSpin->value(),
			cobaCheck->isChecked(),
			cobaAmpaSpin->value(),
			cobaNmdaSpin->value(),
			cobaGabaASpin->value(),
			cobaGabaBSpin->value(),
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

	stimWidthSpin->setValue(Util::getInt(configLoader->getParameter("stim_columns", defaults["stim_columns"])));
	stimHeightSpin->setValue(Util::getInt(configLoader->getParameter("stim_rows", defaults["stim_rows"])));

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

	cobaCheck->setChecked(Util::getBool(configLoader->getParameter("coba", defaults["coba"])));
	cobaAmpaSpin->setValue(Util::getInt(configLoader->getParameter("ampa", defaults["ampa"])));
	cobaNmdaSpin->setValue(Util::getInt(configLoader->getParameter("nmda", defaults["nmda"])));
	cobaGabaASpin->setValue(Util::getInt(configLoader->getParameter("gaba_a", defaults["gaba_a"])));
	cobaGabaBSpin->setValue(Util::getInt(configLoader->getParameter("gaba_b", defaults["gaba_b"])));



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


