
//SpikeStream includes
#include "CarlsimOatReportDialog.h"
#include "CarlsimOatMonitor.h"
#include "SpikeStreamException.h"


using namespace spikestream::carlsim_monitors;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextDocumentWriter>
#include <QFileDialog>

CarlsimOatReportDialog::CarlsimOatReportDialog(QWidget* parent, OatMonitor* monitor) :
  monitor(monitor), QDialog(parent) {
}


CarlsimOatReportDialog::~CarlsimOatReportDialog() {
	if (report)
		delete report;  
}




/*----------------------------------------------------------*/
/*-----                 PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/



/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void CarlsimOatReportDialog::addHeader(QVBoxLayout* mainVLayout) {

	QHBoxLayout *buttonBox = new QHBoxLayout();
	auto space = 10; // pixel

	addFields(buttonBox, space);

	buttonBox->addSpacing(space * 2);

	// align buttons right
	buttonBox->addStretch(1);

	autoupdateCheckBox = new QCheckBox("refresh");
	buttonBox->addWidget(autoupdateCheckBox);
	connect (autoupdateCheckBox, SIGNAL(clicked()), this, SLOT(autoupdate()));

	QPushButton* defaultsButton = new QPushButton("Update");
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(update()));

	// Ceckboxes, see featMonitors 
	QHBoxLayout *checkBoxes = new QHBoxLayout();

	addParts(checkBoxes);

	// align checkboxes left
	checkBoxes->addStretch(1);

	QPushButton* copyButton = new QPushButton("Copy"); // Copy HTML into buffer (compared to plain text) 
	checkBoxes->addWidget(copyButton);
	connect (copyButton, SIGNAL(clicked()), this, SLOT(copy()));

	QPushButton* saveButton = new QPushButton("Save...");
	checkBoxes->addWidget(saveButton);
	connect (saveButton, SIGNAL(clicked()), this, SLOT(save()));

	mainVLayout->addLayout(buttonBox);
	mainVLayout->addLayout(checkBoxes);
}


void CarlsimOatReportDialog::addBody(QVBoxLayout* mainVBox) {
	reportWidget = new QTextEdit(this);
	report = reportWidget->document();
	reportWidget->setReadOnly(true);
	mainVBox->addWidget(reportWidget);
	reportWidget->setDocumentTitle(QString(monitor->type()) + monitor->object);
	updateReport();
}


/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void CarlsimOatReportDialog::addButtons(QVBoxLayout* mainVLayout){

	QHBoxLayout *buttonBox = new QHBoxLayout();

	// align right
	buttonBox->addStretch(1);

	autoupdateCheckBox = new QCheckBox("auto update");
	buttonBox->addWidget(autoupdateCheckBox);
	connect (autoupdateCheckBox, SIGNAL(clicked()), this, SLOT(autoupdate()));

	QPushButton* defaultsButton = new QPushButton("Update");
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(update()));

	QPushButton* saveButton = new QPushButton("Save...");
	buttonBox->addWidget(saveButton);
	connect (saveButton, SIGNAL(clicked()), this, SLOT(save()));

	mainVLayout->addLayout(buttonBox);
}


void CarlsimOatReportDialog::updateReport() {
	reportWidget->update();
}



void CarlsimOatReportDialog::update() {
	updateReport();
}

void CarlsimOatReportDialog::save() {
	auto fileName = QFileDialog::getSaveFileName(this,
		tr("Save Report"), ".", tr("HTML (*.html);;Open Document Files (*.odf)"));
	if (fileName.isEmpty())
		return;
	QTextDocumentWriter writer;
	writer.setFormat("html"); 
	writer.setFileName(fileName); 
	writer.write(report); 
}

void CarlsimOatReportDialog::copy() {
	//auto html = report-> get html;
	//QClipboard::setContent(htlm); 
}


void CarlsimOatReportDialog::autoupdate() {
	if (autoupdateCheckBox->isChecked()) {
		connect(parent(), SIGNAL(timeStepChanged(unsigned int)), this, SLOT(update()));
	}
	else {
		disconnect(parent(), SIGNAL(timeStepChanged(unsigned int)), this, SLOT(update()));
	}
}

