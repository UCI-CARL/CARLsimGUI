#ifndef CARLSIMOATPERFREPORTDIALOG_H
#define CARLSIMOATPERFREPORTDIALOG_H

#include "api.h"

#include "CarlsimOatReportDialog.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		/*!  */
		class CarlsimOatPerformanceReportDialog : public CarlsimOatReportDialog {
			Q_OBJECT
		public:
			CarlsimOatPerformanceReportDialog(QWidget* parent, OatMonitor* monitor);
			OatPerformanceMonitor* getMonitor() { return (OatPerformanceMonitor*)monitor; };  //qt meta cast <OatGroupMonitor*>
		protected:
			virtual void updateReport();
			virtual void addFields(QHBoxLayout *checkBoxes, int space);
			virtual void addParts(QHBoxLayout *checkBoxes);
		private slots:
			void updateStats(const int checked);
			void updatePerformance(const int checked);
		private: 
			bool bStats, bPerformance;
		};

	}
}

#endif//CARLSIMOATPERFREPORTDIALOG_H