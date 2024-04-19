
//SpikeStream includes
#include "CarlsimOatConnectionReportDialog.h"
#include "CarlsimOatConnectionMonitor.h"
#include "SpikeStreamException.h"

using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextDocumentWriter>
#include <QFileDialog>


// -----------------------------------------------------------------------

CarlsimOatConnectionReportDialog::CarlsimOatConnectionReportDialog(QWidget* parent, OatMonitor* monitor) :
	CarlsimOatReportDialog(parent, monitor) {

	// values set in the dialog as query params
	preId = 0; 
	postId = 0;
	minAbsChanged = 0.001;
	minWeight = 0.001;
	maxWeight = 1.0;
	weight = 0.0;
	bWeights = true;
	bPrevWeights = false;
	bWeightChanges = false; 
	bQueries = false; 
	bStats = true;

	setMinimumSize(250, 30);

	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addHeader(mainVBox);
	addBody(mainVBox);
}


void CarlsimOatConnectionReportDialog::updateReport() {

	QTextTableFormat tableFormat;
	tableFormat.setCellPadding(2);
	tableFormat.setCellSpacing(0);
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Inset);

	auto m = this->getMonitor();

	report->clear();   

	if(bQueries)
		 m->printQueries(report, tableFormat, preId, postId, minAbsChanged, minWeight, maxWeight, weight);
	if(bStats)
		m->printStats(report, tableFormat);
	if(bWeights)
		m->printWeights(report, tableFormat);
	if(bPrevWeights)
		m->printPrevWeights(report, tableFormat);
	if(bWeightChanges)
		m->printWeightChanges(report, tableFormat);

	CarlsimOatReportDialog::updateReport();  
}


void CarlsimOatConnectionReportDialog::addFields(QHBoxLayout *buttonBox, int space) {

	QLabel* neurIdLabel = new QLabel("Pre-/Post-Neuron:");
	buttonBox->addWidget(neurIdLabel);
	QLineEdit* preIdEdit = new QLineEdit(QString::number(preId));
	preIdEdit->setFixedWidth(30);
	buttonBox->addWidget(preIdEdit);
	connect(preIdEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updatePreId(const QString &)));
	QLineEdit* postIdEdit = new QLineEdit(QString::number(postId));
	postIdEdit->setFixedWidth(30);
	buttonBox->addWidget(postIdEdit);
	connect(postIdEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updatePostId(const QString &)));

	buttonBox->addSpacing(space);

	QLabel* weightRangeLabel = new QLabel("Min/Max Weight:");
	buttonBox->addWidget(weightRangeLabel);
	QLineEdit* minWeightEdit = new QLineEdit(QString::number(minWeight));
	minWeightEdit->setFixedWidth(50);
	buttonBox->addWidget(minWeightEdit);
	connect(minWeightEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMinWeight(const QString &)));
	QLineEdit* maxWeightEdit = new QLineEdit(QString::number(maxWeight));
	maxWeightEdit->setFixedWidth(50);
	buttonBox->addWidget(maxWeightEdit);
	connect(maxWeightEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMaxWeight(const QString &)));

	buttonBox->addSpacing(space);

	QLabel* weightLabel = new QLabel("Weight:");
	buttonBox->addWidget(weightLabel);
	QLineEdit* weightEdit = new QLineEdit(QString::number(weight));
	weightEdit->setFixedWidth(50);
	buttonBox->addWidget(weightEdit);
	connect(weightEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateWeight(const QString &)));

	buttonBox->addSpacing(space);

	QLabel* minEpsLabel = new QLabel("Min.Abs.Change:");
	buttonBox->addWidget(minEpsLabel);
	QLineEdit* minEpsEdit = new QLineEdit(QString::number(minAbsChanged));
	minEpsEdit->setFixedWidth(50);
	buttonBox->addWidget(minEpsEdit);
	connect(minEpsEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMinAbsChanged(const QString &)));

}

void CarlsimOatConnectionReportDialog::addParts(QHBoxLayout *checkBoxes) {

	QCheckBox *queriesCheckBox = new QCheckBox("Metrices");
	queriesCheckBox->setChecked(bQueries);
	checkBoxes->addWidget(queriesCheckBox);
	connect (queriesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateQueries(int)));

	QCheckBox *statsCheckBox = new QCheckBox("Statistics");
	statsCheckBox->setChecked(bStats);
	checkBoxes->addWidget(statsCheckBox);
	connect (statsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateStats(int)));

	QCheckBox *weightsCheckBox = new QCheckBox("Weights");
	weightsCheckBox->setChecked(bWeights);
	checkBoxes->addWidget(weightsCheckBox);
	connect (weightsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateWeights(int)));

	QCheckBox *prevWeightsCheckBox = new QCheckBox("Prev-Weights");
	prevWeightsCheckBox->setChecked(bPrevWeights);
	checkBoxes->addWidget(prevWeightsCheckBox);
	connect (prevWeightsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updatePrevWeights(int)));

	QCheckBox *weightChangesCheckBox = new QCheckBox("Weight-Changes");
	weightChangesCheckBox->setChecked(bWeightChanges);
	checkBoxes->addWidget(weightChangesCheckBox);
	connect (weightChangesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateWeightChanges(int)));
}





void CarlsimOatConnectionReportDialog::updatePreId(const QString &text) {
	preId = text.toInt();
}

void CarlsimOatConnectionReportDialog::updatePostId(const QString &text) {
	postId = text.toInt();
}

void CarlsimOatConnectionReportDialog::updateMinAbsChanged(const QString &text) {
	minAbsChanged = text.toFloat(); 
}

void CarlsimOatConnectionReportDialog::updateMinWeight(const QString &text) {
	minWeight = text.toFloat();
}

void CarlsimOatConnectionReportDialog::updateMaxWeight(const QString &text) {
	maxWeight = text.toFloat();
}

void CarlsimOatConnectionReportDialog::updateWeight(const QString &text) {
	weight = text.toFloat();
}


void CarlsimOatConnectionReportDialog::updateWeights(const int checked) {
	bWeights = checked;
}

void CarlsimOatConnectionReportDialog::updatePrevWeights(const int checked) {
	bPrevWeights = checked;
}

void CarlsimOatConnectionReportDialog::updateWeightChanges(const int checked) {
	bWeightChanges = checked;
}

void CarlsimOatConnectionReportDialog::updateQueries(const int checked) {
	bQueries = checked;
}

void CarlsimOatConnectionReportDialog::updateStats(const int checked) {
	bStats = checked;
}

