#ifndef CARLSIMOATREPORTDIALOG_H
#define CARLSIMOATREPORTDIALOG_H

#include "api.h"


//Qt includes
#include <QDialog>
#include <QLayout>
#include <QTextEdit>
#include <QCheckBox>
#include <QTextDocument>

namespace spikestream {

	class CarlsimWrapper;
	class NeuronGroup;
	class ConnectionGroup;

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class OatMonitor;
		class OatSpikeMonitor;
		class OatConnectionMonitor;
		class OatGroupMonitor;
		class OatNeuronMonitor;


			/*! generic behavior of report dialog, clicked on the print button
		   */
		class CarlsimOatReportDialog : public QDialog {
			Q_OBJECT
		public:

			CarlsimOatReportDialog(QWidget* parent, OatMonitor* monitor);

			~CarlsimOatReportDialog();


		public slots:
			void update();   // the underlying model can connect changed signals to the update slot
			void save();    // HTML or ODF
			void copy();	// Copy report in HTML rich text to the buffer
			void autoupdate();    // connect/disconnect change/update 

		protected:
			virtual void updateReport();

			virtual void addHeader(QVBoxLayout* mainVBox); // Header consists of the fields and parts 
			virtual void addBody(QVBoxLayout* mainVBox);	  // The Report Document itself
			virtual void addFields(QHBoxLayout *checkBoxes, int space) = 0; // Add the class specific buttons 
			virtual void addParts(QHBoxLayout *checkBoxes) = 0; // Add the Checkboxes, which parts the report consists of

			void addButtons(QVBoxLayout* mainVLayout);

			QTextDocument* report;
			QTextEdit* reportWidget;  
			OatMonitor *monitor;

			QCheckBox* autoupdateCheckBox; 

		};


		


	}
}

#endif//CARLSIMOATREPORTDIALOG_H