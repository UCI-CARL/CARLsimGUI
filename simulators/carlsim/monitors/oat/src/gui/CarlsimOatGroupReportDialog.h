#ifndef CARLSIMOATGROUPREPORTDIALOG_H
#define CARLSIMOATGROUPREPORTDIALOG_H

#include "api.h"

#include "CarlsimOatReportDialog.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		/*!  */
		class CarlsimOatGroupReportDialog : public CarlsimOatReportDialog {
			Q_OBJECT
		public:
			CarlsimOatGroupReportDialog(QWidget* parent, OatMonitor* monitor);  
			OatGroupMonitor* getMonitor() { return (OatGroupMonitor*)monitor; };  //qt meta cast <OatGroupMonitor*> 
		protected:
			virtual void updateReport();
			virtual void addFields(QHBoxLayout *checkBoxes, int space);
			virtual void addParts(QHBoxLayout *checkBoxes);
		private slots: 
			void updateMaxPeaks(const QString &text);
			void updateMaxSorted(const QString &text);
			void updateMetrics(const int checked);
			void updateStats(const int checked);
			void updateData(const int checked);
		private:
			bool allNM;  // all neuro modulators , DA only 
			int maxSorted;  // size of vect of Peak Time Sorted
			int maxPeaks;  // size of vect of Peak Time Sorted
			bool bStats, bMetrics, bData;
		};

	}
}

#endif//CARLSIMOATGROUPREPORTDIALOG_H