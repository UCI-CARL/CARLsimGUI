#ifndef CARLSIMOATNEURONREPORTDIALOG_H
#define CARLSIMOATNEURONREPORTDIALOG_H

#include "api.h"

#include "CarlsimOatReportDialog.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		/*!  */
		class CarlsimOatNeuronReportDialog : public CarlsimOatReportDialog {
			Q_OBJECT
		public:
			CarlsimOatNeuronReportDialog(QWidget* parent, OatMonitor* monitor);
			OatNeuronMonitor* getMonitor() { return (OatNeuronMonitor*)monitor; };  //qt meta cast <OatGroupMonitor*>
		protected:
			virtual void updateReport();
			virtual void addFields(QHBoxLayout *checkBoxes, int space);
			virtual void addParts(QHBoxLayout *checkBoxes);
		private slots:
			void updateStats(const int checked);
			void updateVUI(const int checked);
		private: 
			bool bStats, bVUI;
		};

	}
}

#endif//CARLSIMOATNEURONREPORTDIALOG_H