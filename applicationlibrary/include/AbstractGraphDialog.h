#ifndef ABSTRACTGRAPHDIALOG_H
#define ABSTRACTGRAPHDIALOG_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "AnalysisInfo.h"
#include "AnalysisSpectrogram.h"
#include "AbstractSpectrogramData.h"
using namespace spikestream;

//Qt includes
#include <QDialog>


namespace spikestream {

	/*! Plots graphs and heat maps to visualise the state based phi data. */
	class SPIKESTREAM_APP_LIB_EXPORT AbstractGraphDialog : public QDialog {
		Q_OBJECT

		public:
			AbstractGraphDialog(QWidget* parent, const AnalysisInfo& info);
			~AbstractGraphDialog();

		protected slots:
			void showNeuronIDS(int state);
			void timeStepChanged(QString timeStepStr);

		protected:
			//=======================  VARIABLES  ======================
			/*! Information about the analysis */
			AnalysisInfo analysisInfo;

			/*! The graph that is being displayed */
			AnalysisSpectrogram* spectrogram;

			/*! The raster data that is being displayed */
			AbstractSpectrogramData* spectrogramData;

	};

}

#endif//ABSTRACTGRAPHDIALOG_H
