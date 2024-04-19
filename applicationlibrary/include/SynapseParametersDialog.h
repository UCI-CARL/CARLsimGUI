#ifndef SYNAPSEPARAMETERSDIALOG_H
#define SYNAPSEPARAMETERSDIALOG_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays the current synapse parameters for the currently loaded network.
		Clicking on a particular row launches a dialog to edit parameters for the corresponding connection group
		NOTE: Not thread safe. */
	class SPIKESTREAM_APP_LIB_EXPORT SynapseParametersDialog : public QDialog {
		Q_OBJECT

		public:
			SynapseParametersDialog(QWidget* parent = 0);
			~SynapseParametersDialog();
	};

}
#endif//SYNAPSEPARAMETERSDIALOG_H
