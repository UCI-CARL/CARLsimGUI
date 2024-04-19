#ifndef NEURONGROUPTABLEVIEW_H
#define NEURONGROUPTABLEVIEW_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "NeuronGroupModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A view of the NeuronGroupModel displaying information about the
		neuron groups in the current network. */
	class SPIKESTREAM_APP_LIB_EXPORT NeuronGroupTableView : public QTableView {
		Q_OBJECT

		public:
			NeuronGroupTableView(QWidget* parent, NeuronGroupModel* model);
			~NeuronGroupTableView();
			void resizeHeaders();

		private slots:
			void headerClicked(int column);
			void tableClicked(QModelIndex index);
			void tableDoubleClicked(QModelIndex index);

		private:
			//====================  VARIABLES  =====================
			/*! Model associated with this view */
			NeuronGroupModel* neuronGroupModel;

	};

}

#endif//NEURONGROUPTABLEVIEW_H
