#ifndef NOISEINJECTORTABLEVIEW_H
#define NOISEINJECTORTABLEVIEW_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "../models/NoiseInjectorModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	namespace carlsim_injectors { 
	

		/*! A view of the NeuronGroupModel displaying information about the
			neuron groups in the current network. */
		class NoiseInjectorTableView : public QTableView {
			Q_OBJECT

			public:
				NoiseInjectorTableView(QWidget* parent, NoiseInjectorModel* model);
				~NoiseInjectorTableView();
				void resizeHeaders();


			signals: 
				void injectorSelected(int); 

			private slots:
				void headerClicked(int column);
				void tableClicked(QModelIndex index);
				void tableDoubleClicked(QModelIndex index);

			private:
				//====================  VARIABLES  =====================
				/*! Model associated with this view */
				NoiseInjectorModel* currentInjectorModel;

		};

	}

}

#endif//NOISEINJECTORTABLEVIEW_H
