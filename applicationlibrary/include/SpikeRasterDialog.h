#ifndef SPIKERASTERDIALOG_H
#define SPIKERASTERDIALOG_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "NeuronGroup.h"
#include "SpikeRasterWidget.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	class SPIKESTREAM_APP_LIB_EXPORT SpikeRasterDialog : public QDialog {
		Q_OBJECT

		public:
			SpikeRasterDialog(QList<NeuronGroup*> neuronGroupList, QWidget* parent = 0);
			~SpikeRasterDialog();
			void addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep);

		private slots:
			void colorComboChanged(int index);

		private:
			//====================  VARIABLES  ======================
			SpikeRasterWidget* spikeRasterWidget;

	};

}

#endif//SPIKERASTERDIALOG_H
