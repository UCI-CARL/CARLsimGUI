#ifndef CONNECTIONSTABLEVIEW_H
#define CONNECTIONSTABLEVIEW_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! View of model that holds information about the connection groups available for the
		current network. */
	class SPIKESTREAM_APP_LIB_EXPORT ConnectionsTableView : public QTableView {
		Q_OBJECT

		public:
			ConnectionsTableView(QWidget* parent, QAbstractTableModel* model);
			~ConnectionsTableView();
			void resizeHeaders();

		private slots:

	};

}

#endif//CONNECTIONSTABLEVIEW_H
