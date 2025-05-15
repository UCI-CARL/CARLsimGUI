
//SpikeStream includes
#include "CarlsimOatCobaReportDialog.h"
#include "CarlsimOatCobaMonitor.h"
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

CarlsimOatCobaReportDialog::CarlsimOatCobaReportDialog(QWidget* parent, OatMonitor* monitor) :
	CarlsimOatReportDialog(parent, monitor) {

	bStats = true;
	bCOBA = false;

	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addHeader(mainVBox);
	addBody(mainVBox);

}


void CarlsimOatCobaReportDialog::updateReport() {

	QTextTableFormat tableFormat;
	tableFormat.setCellPadding(2);
	tableFormat.setCellSpacing(0);
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Inset);

	auto m = this->getMonitor();

	report->clear(); 

	if(bStats)
		m->printStats(report, tableFormat);
	if(bCOBA)
		m->printCOBA(report, tableFormat);

	CarlsimOatReportDialog::updateReport();  
}

void CarlsimOatCobaReportDialog::addFields(QHBoxLayout *, int ) {
	// none
}

void CarlsimOatCobaReportDialog::addParts(QHBoxLayout *checkBoxes) {

	QCheckBox *statsCheckBox = new QCheckBox("Statistics");
	statsCheckBox->setChecked(bStats);
	checkBoxes->addWidget(statsCheckBox);
	connect (statsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateStats(int)));

	QCheckBox *cobaCheckBox = new QCheckBox("COBA");
	cobaCheckBox->setChecked(bCOBA);
	checkBoxes->addWidget(cobaCheckBox);
	connect (cobaCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateCOBA(int)));
}

void CarlsimOatCobaReportDialog::updateStats(const int checked) {
	bStats = checked;
}


void CarlsimOatCobaReportDialog::updateCOBA(const int checked) {
	bCOBA = checked;
}


