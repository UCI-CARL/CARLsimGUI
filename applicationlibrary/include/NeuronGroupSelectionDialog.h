#ifndef NEURONGROUPSELECTIONDIALOG_H
#define NEURONGROUPSELECTIONDIALOG_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "Network.h"
#include "NeuronGroupSelectionModel.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays a checkable list of the neuron groups in the supplied network.
		Enables user to select all or a subset of these neuron groups. */
	class SPIKESTREAM_APP_LIB_EXPORT NeuronGroupSelectionDialog : public QDialog {
		Q_OBJECT

		public:
			NeuronGroupSelectionDialog(Network* network, QWidget* parent = 0);
			~NeuronGroupSelectionDialog();
			QList<NeuronGroup*> getNeuronGroups();

		private:
			//=================  VARIABLES  ===================
			NeuronGroupSelectionModel* neurGrpSelectionModel;

	};

}


#endif//NEURONGROUPSELECTIONDIALOG_H
