#ifndef CARLSIMOATCONNECTIONREPORTDIALOG_H
#define CARLSIMOATCONNECTIONREPORTDIALOG_H

#include "api.h"

#include "CarlsimOatReportDialog.h"

namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		class CarlsimOatConnectionReportDialog : public CarlsimOatReportDialog {
			Q_OBJECT
		public:
			CarlsimOatConnectionReportDialog(QWidget* parent, OatMonitor* monitor);
			OatConnectionMonitor* getMonitor() { return (OatConnectionMonitor*)monitor; };  //qt meta cast <OatGroupMonitor*>  
		protected:
			virtual void updateReport();
			virtual void addFields(QHBoxLayout *checkBoxes, int space); // Add the class specific buttons 
			virtual void addParts(QHBoxLayout *checkBoxes); // Add the Checkboxes, which parts the report consists of
		private slots:
			void updatePreId(const QString &text);
			void updatePostId(const QString &text);
			void updateMinAbsChanged(const QString &text);
			void updateMinWeight(const QString &text);
			void updateMaxWeight(const QString &text);
			void updateWeight(const QString &text);
			void updateWeights(const int checked);
			void updatePrevWeights(const int checked);
			void updateWeightChanges(const int checked);
			void updateQueries(const int checked);
			void updateStats(const int checked);
		private:
			int preId, postId;		
			double minAbsChanged;
			double minWeight, maxWeight, weight; 
			bool bWeights, bPrevWeights, bWeightChanges, bQueries, bStats; 
		};

	}
}

#endif//CARLSIMOATCONNECTIONREPORTDIALOG_H