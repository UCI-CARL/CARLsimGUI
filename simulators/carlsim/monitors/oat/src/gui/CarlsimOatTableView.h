#ifndef CARLSIMOATTABLEVIEW_H
#define CARLSIMOATTABLEVIEW_H

#include "api.h"

//SpikeStream includes
#include "CarlsimOatModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	namespace carlsim_monitors {
	

		/*! A view of the NeuronGroupModel displaying information about the
			neuron groups in the current network. */
		class CarlsimOatTableView : public QTableView {
			Q_OBJECT

			public:
				CarlsimOatTableView(QWidget* parent, CarlsimOatModel* model);
				~CarlsimOatTableView();
				void resizeHeaders();


			signals: 
				//void injectorSelected(int); 
				void monitorPrint(int index);

			private slots:
				void headerClicked(int column);
				void tableClicked(QModelIndex index);
				void tableDoubleClicked(QModelIndex index);

			private:
				//====================  VARIABLES  =====================
				/*! Model associated with this view */
				CarlsimOatModel* oatModel;

		};

	}

}

#endif//CARLSIMOATTABLEVIEW_H
