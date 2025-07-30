
//SpikeStream includes
#include "CarlsimOatPerformanceReportDialog.h"
#include "CarlsimOatPerformanceMonitor.h"
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

// -----------------------------------------------------------------

CarlsimOatPerformanceReportDialog::CarlsimOatPerformanceReportDialog(QWidget* parent, OatMonitor* monitor) :
	CarlsimOatReportDialog(parent, monitor) {

	bStats = true;
	bPerformance = false;

	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addHeader(mainVBox);
	addBody(mainVBox);

}


void CarlsimOatPerformanceReportDialog::updateReport() {

	QTextTableFormat tableFormat;
	tableFormat.setCellPadding(2);
	tableFormat.setCellSpacing(0);
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Inset);

	auto m = this->getMonitor();

	report->clear(); 

	if(bStats)
		m->printStats(report, tableFormat);
	if(bPerformance)
		m->printPerformance(report, tableFormat);

	CarlsimOatReportDialog::updateReport();  
}

void CarlsimOatPerformanceReportDialog::addFields(QHBoxLayout *, int ) {
	// none
}

void CarlsimOatPerformanceReportDialog::addParts(QHBoxLayout *checkBoxes) {

	QCheckBox *statsCheckBox = new QCheckBox("Statistics");
	statsCheckBox->setChecked(bStats);
	checkBoxes->addWidget(statsCheckBox);
	connect (statsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateStats(int)));

	QCheckBox *cobaCheckBox = new QCheckBox("COBA");
	cobaCheckBox->setChecked(bPerformance);
	checkBoxes->addWidget(cobaCheckBox);
	connect (cobaCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateCOBA(int)));
}

void CarlsimOatPerformanceReportDialog::updateStats(const int checked) {
	bStats = checked;
}


void CarlsimOatPerformanceReportDialog::updatePerformance(const int checked) {
	bPerformance = checked;
}


