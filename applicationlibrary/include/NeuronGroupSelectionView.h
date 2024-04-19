#ifndef NEURONGROUPSELECTIONVIEW_H
#define NEURONGROUPSELECTIONVIEW_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "NeuronGroupSelectionModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A view of the NeuronGroupSelectionModel displaying a selectable list of neuron groups. */
	class SPIKESTREAM_APP_LIB_EXPORT NeuronGroupSelectionView : public QTableView {
		Q_OBJECT

		public:
			NeuronGroupSelectionView(QWidget* parent, NeuronGroupSelectionModel* model);
			~NeuronGroupSelectionView();
			void resizeHeaders();

		private slots:
			void headerClicked(int column);
			void tableClicked(QModelIndex index);

		private:
			//====================  VARIABLES  =====================
			/*! Model associated with this view */
			NeuronGroupSelectionModel* neuronGroupSelectionModel;

	};

}

#endif//NEURONGROUPSELECTIONVIEW_H
