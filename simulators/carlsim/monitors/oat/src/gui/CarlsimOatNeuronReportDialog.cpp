
//SpikeStream includes
#include "CarlsimOatNeuronReportDialog.h"
#include "CarlsimOatNeuronMonitor.h"
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

CarlsimOatNeuronReportDialog::CarlsimOatNeuronReportDialog(QWidget* parent, OatMonitor* monitor) :
	CarlsimOatReportDialog(parent, monitor) {

	bStats = true;
	bVUI = false;

	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addHeader(mainVBox);
	addBody(mainVBox);

}


void CarlsimOatNeuronReportDialog::updateReport() {

	QTextTableFormat tableFormat;
	tableFormat.setCellPadding(2);
	tableFormat.setCellSpacing(0);
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Inset);

	auto m = this->getMonitor();

	report->clear(); 

	if(bStats)
		m->printStats(report, tableFormat);
	if(bVUI)
		m->printVUI(report, tableFormat);

	CarlsimOatReportDialog::updateReport();  
}

void CarlsimOatNeuronReportDialog::addFields(QHBoxLayout *, int ) {
	// none
}

void CarlsimOatNeuronReportDialog::addParts(QHBoxLayout *checkBoxes) {

	QCheckBox *statsCheckBox = new QCheckBox("Statistics");
	statsCheckBox->setChecked(bStats);
	checkBoxes->addWidget(statsCheckBox);
	connect (statsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateStats(int)));

	QCheckBox *vuiCheckBox = new QCheckBox("VUI");
	vuiCheckBox->setChecked(bVUI);
	checkBoxes->addWidget(vuiCheckBox);
	connect (vuiCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateVUI(int)));
}

void CarlsimOatNeuronReportDialog::updateStats(const int checked) {
	bStats = checked;
}


void CarlsimOatNeuronReportDialog::updateVUI(const int checked) {
	bVUI = checked;
}


