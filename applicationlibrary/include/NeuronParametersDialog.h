#ifndef NEURONPARAMETERSDIALOG_H
#define NEURONPARAMETERSDIALOG_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays the current neuron parameters for the currently loaded network.
		Clicking on a particular row launches a dialog to edit parameters for the corresponding neuron group
		NOTE: Not thread safe. */
	class SPIKESTREAM_APP_LIB_EXPORT NeuronParametersDialog : public QDialog {
		Q_OBJECT

		public:
			NeuronParametersDialog(QWidget* parent=0);
			~NeuronParametersDialog();
	};

}

#endif//NEURONPARAMETERSDIALOG_H

