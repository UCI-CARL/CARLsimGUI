#ifndef CARLSIMOATSPIKEREPORTDIALOG_H
#define CARLSIMOATSPIKEREPORTDIALOG_H

#include "api.h"

#include "CarlsimOatReportDialog.h"


namespace spikestream {

	namespace carlsim_monitors {  // not hierarchically  carlsim::monitors

		/*!  */
		class CarlsimOatSpikeReportDialog : public CarlsimOatReportDialog {
			Q_OBJECT
		public:

			CarlsimOatSpikeReportDialog(QWidget* parent, OatMonitor* monitor);  // min,max,NId,maxNs, ... 

			OatSpikeMonitor* getMonitor() { return (OatSpikeMonitor*) monitor; }; 

		protected:
			virtual void updateReport(); 
			virtual void addFields(QHBoxLayout *checkBoxes, int space);
			virtual void addParts(QHBoxLayout *checkBoxes);

		private slots:
			void updateNeuronId(const QString &text);
			void updateMinFiringRate(const QString &text);
			void updateMaxFiringRate(const QString &text);
			void updateMetrics(const int checked);
			void updateStats(const int checked);
			void updateTimes(const int checked);
		private:
			float minFiringRate, maxFiringRate;
			int neuronId;
			int maxRatesNeurons;  // size of vect of Rates/Sorted
			bool bStats, bMetrics, bTimes;
		};
		
	}
}

#endif//CARLSIMOATSPIKEREPORTDIALOG_H