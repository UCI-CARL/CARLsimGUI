#ifndef SYNAPSEPARAMETERSVIEW_H
#define SYNAPSEPARAMETERSVIEW_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "SynapseParametersModel.h"

//Qt includes
#include <QTableView>

namespace spikestream {

	/*! A table of synapse parameters for the current network.
		NOTE: This only displays synapse parameters for a single type of synapse. */
	class SPIKESTREAM_APP_LIB_EXPORT SynapseParametersView : public QTableView {
		Q_OBJECT

		public:
			SynapseParametersView(QWidget* parent, SynapseParametersModel* model);
			~SynapseParametersView();

		private slots:
			void resizeHeaders();
			void tableClicked(QModelIndex index);

		private:
			//======================  VARIABLES  =====================
			/*! The model associated with the view */
			SynapseParametersModel* model;
	};

}

#endif//SYNAPSEPARAMETERSVIEW_H

