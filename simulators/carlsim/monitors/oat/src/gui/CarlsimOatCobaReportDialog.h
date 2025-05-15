#ifndef CARLSIMOATCOBAREPORTDIALOG_H
#define CARLSIMOATCOBAREPORTDIALOG_H

#include "api.h"

#include "CarlsimOatReportDialog.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		/*!  */
		class CarlsimOatCobaReportDialog : public CarlsimOatReportDialog {
			Q_OBJECT
		public:
			CarlsimOatCobaReportDialog(QWidget* parent, OatMonitor* monitor);
			OatCobaMonitor* getMonitor() { return (OatCobaMonitor*)monitor; };  //qt meta cast <OatGroupMonitor*>
		protected:
			virtual void updateReport();
			virtual void addFields(QHBoxLayout *checkBoxes, int space);
			virtual void addParts(QHBoxLayout *checkBoxes);
		private slots:
			void updateStats(const int checked);
			void updateCOBA(const int checked);
		private: 
			bool bStats, bCOBA;
		};

	}
}

#endif//CARLSIMOATCOBAREPORTDIALOG_H