#ifndef ANALYSESTABLEVIEW_H
#define ANALYSESTABLEVIEW_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! View of model listing the different types of analysis. Used for loading and deleting analyses. */
    class SPIKESTREAM_APP_LIB_EXPORT AnalysesTableView : public QTableView {
		Q_OBJECT

	public:
		AnalysesTableView(QWidget* parent, QAbstractTableModel* model);
	    ~AnalysesTableView();
	    void resizeHeaders();

	private slots:
	    void tableClicked(QModelIndex index);

    };

}

#endif//ANALYSESTABLEVIEW_H
