
//SpikeStream includes
#include "CarlsimOatSpikeReportDialog.h"
#include "CarlsimOatSpikeMonitor.h"
#include "SpikeStreamException.h"

//#include "Util.h"
//#include "Globals.h"


using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextDocumentWriter>
#include <QFileDialog>


CarlsimOatSpikeReportDialog::CarlsimOatSpikeReportDialog(QWidget* parent, OatMonitor* monitor) :
	CarlsimOatReportDialog(parent, monitor) {

	setMinimumSize(250, 30);

	// values set in the dialog as query params
	minFiringRate = 0.001f;
	maxFiringRate = 50.f;
	neuronId = 0;  


	maxRatesNeurons = 10;  // size of vect of Rates/Sorted   

	bMetrics = false;
	bStats = false;
	bTimes = true; 

	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addHeader(mainVBox);
	addBody(mainVBox);

	
}


void CarlsimOatSpikeReportDialog::updateReport() {

	QTextTableFormat tableFormat;
	tableFormat.setCellPadding(0);
	tableFormat.setCellSpacing(0);
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Inset);

	auto m = this->getMonitor();

	report->clear();

	if (bMetrics) {
		m->printNeuron(report, tableFormat, neuronId);
		m->printNeurons(report, tableFormat, minFiringRate, maxFiringRate);
	}
	if (bStats)
		m->printStats(report, tableFormat);
	if (bTimes)
		m->printSpikeTimes(report, tableFormat);

	// format MarkDown
	//reportWidget->setMarkdown(report);  // This property was introduced in Qt 5.14.
	// => Port to Qt 5 !!!
	//reportWidget->setDocument(report); 

	CarlsimOatReportDialog::updateReport();  // super
}




//void CarlsimOatSpikeReportDialog::addFields(QVBoxLayout* mainVLayout) {
void CarlsimOatSpikeReportDialog::addFields(QHBoxLayout *buttonBox, int space) {

	QLabel* neurIdLabel = new QLabel("Neuron Id:"); 
	buttonBox->addWidget(neurIdLabel);

	QLineEdit* neurIdEdit = new QLineEdit(QString::number(neuronId));
	neurIdEdit->setFixedWidth(30); 
	buttonBox->addWidget(neurIdEdit);
	connect(neurIdEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateNeuronId(const QString &)));

	buttonBox->addSpacing(space);

	QLabel* firingRateLabel = new QLabel("Min/Max Firing Rate:");
	buttonBox->addWidget(firingRateLabel);

	QLineEdit* minFiringRateEdit = new QLineEdit(QString::number(minFiringRate));
	minFiringRateEdit->setFixedWidth(50);
	buttonBox->addWidget(minFiringRateEdit);
	connect(minFiringRateEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMinFiringRate(const QString &)));

	QLineEdit* maxFiringRateEdit = new QLineEdit(QString::number(maxFiringRate));
	maxFiringRateEdit->setFixedWidth(50);
	buttonBox->addWidget(maxFiringRateEdit);
	connect(maxFiringRateEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMaxFiringRate(const QString &)));
}


void CarlsimOatSpikeReportDialog::addParts(QHBoxLayout *checkBoxes) {

	QCheckBox *metricsCheckBox = new QCheckBox("Metrices");
	metricsCheckBox->setChecked(bMetrics);
	checkBoxes->addWidget(metricsCheckBox);
	connect (metricsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateMetrics(int)));

	QCheckBox *statsCheckBox = new QCheckBox("Statistics");
	statsCheckBox->setChecked(bStats);
	checkBoxes->addWidget(statsCheckBox);
	connect (statsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateStats(int)));

	QCheckBox *timesCheckBox = new QCheckBox("Spike-Times");
	timesCheckBox->setChecked(bTimes);
	checkBoxes->addWidget(timesCheckBox);
	connect (timesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateTimes(int)));
}


void CarlsimOatSpikeReportDialog::updateNeuronId(const QString &text) {
	neuronId = text.toInt();
}

void CarlsimOatSpikeReportDialog::updateMinFiringRate(const QString &text) {
	minFiringRate = text.toFloat();
}

void CarlsimOatSpikeReportDialog::updateMaxFiringRate(const QString &text) {
	maxFiringRate = text.toFloat();
}

void CarlsimOatSpikeReportDialog::updateMetrics(const int checked) {
	bMetrics = checked;
}

void CarlsimOatSpikeReportDialog::updateStats(const int checked) {
	bStats = checked;
}


void CarlsimOatSpikeReportDialog::updateTimes(const int checked) {
	bTimes = checked;
}
