
//SpikeStream includes
#include "CarlsimOatGroupReportDialog.h"
#include "CarlsimOatGroupMonitor.h"
#include "SpikeStreamException.h"


using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextDocumentWriter>
#include <QFileDialog>


CarlsimOatGroupReportDialog::CarlsimOatGroupReportDialog(QWidget* parent, OatMonitor* monitor) :
	CarlsimOatReportDialog(parent, monitor) {

	// values set in the dialog as query params
	allNM = false;
	maxPeaks = 100; 
	maxSorted = 10; // top ten

	bStats = true; 
	bMetrics = false; 
	bData = true; 

	setMinimumSize(250, 30);

	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addHeader(mainVBox);
	addBody(mainVBox);
}



void CarlsimOatGroupReportDialog::addFields(QHBoxLayout *buttonBox, int space) {

	QLabel* maxSortedLabel = new QLabel("Max Sorted:");
	buttonBox->addWidget(maxSortedLabel);
	QLineEdit* maxSortedEdit = new QLineEdit(QString::number(maxSorted));
	maxSortedEdit->setFixedWidth(50);
	buttonBox->addWidget(maxSortedEdit);
	connect(maxSortedEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMaxSorted(const QString &)));

	buttonBox->addSpacing(space);

	QLabel* maxPeaksLabel = new QLabel("Max Peaks:");
	buttonBox->addWidget(maxPeaksLabel);
	QLineEdit* maxPeaksEdit = new QLineEdit(QString::number(maxPeaks));
	maxPeaksEdit->setFixedWidth(50);
	buttonBox->addWidget(maxPeaksEdit);
	connect(maxPeaksEdit, SIGNAL(textEdited(const QString &)), this, SLOT(updateMaxPeaks(const QString &)));

}


void CarlsimOatGroupReportDialog::addParts(QHBoxLayout *checkBoxes) {

	QCheckBox *metricsCheckBox = new QCheckBox("Metrices");
	metricsCheckBox->setChecked(bMetrics);
	checkBoxes->addWidget(metricsCheckBox);
	connect (metricsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateMetrics(int)));

	QCheckBox *statsCheckBox = new QCheckBox("Statistics");
	statsCheckBox->setChecked(bStats);
	checkBoxes->addWidget(statsCheckBox);
	connect (statsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateStats(int)));

	QCheckBox *dataCheckBox = new QCheckBox("Data");
	dataCheckBox->setChecked(bData);
	checkBoxes->addWidget(dataCheckBox);
	connect (dataCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateData(int)));
}


void CarlsimOatGroupReportDialog::updateMaxPeaks(const QString &text) {
	maxPeaks= text.toInt();
}

void CarlsimOatGroupReportDialog::updateMaxSorted(const QString &text) {
	maxSorted = text.toInt();
}


void CarlsimOatGroupReportDialog::updateMetrics(const int checked) {
	bMetrics = checked;
}

void CarlsimOatGroupReportDialog::updateStats(const int checked) {
	bStats = checked;
}


void CarlsimOatGroupReportDialog::updateData(const int checked) {
	bData = checked;
}



void CarlsimOatGroupReportDialog::updateReport() {

	QTextTableFormat tableFormat;
	tableFormat.setCellPadding(0);
	tableFormat.setCellSpacing(0);
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Inset);

	auto m = this->getMonitor();

	report->clear();   

	if(bStats)
		m->printStats(report, tableFormat);
	if (bMetrics) {
		m->printPeaksSorted(report, tableFormat, maxSorted);
		m->printPeaks(report, tableFormat, maxPeaks);
	}
	if(bData)
		m->printData(report, tableFormat);
	
	CarlsimOatReportDialog::updateReport();
}

