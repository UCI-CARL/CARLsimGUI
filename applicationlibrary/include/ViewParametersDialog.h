#ifndef VIEWPARAMETERSDIALOG_H
#define VIEWPARAMETERSDIALOG_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QDialog>
#include <QHash>

namespace spikestream {

	/*! Displays supplied parametes in a non-editable format */
	class SPIKESTREAM_APP_LIB_EXPORT ViewParametersDialog : public QDialog {
		Q_OBJECT

		public:
			ViewParametersDialog(QHash<QString, double> parameterMap, QWidget* parent);
			~ViewParametersDialog();
	};

}

#endif//VIEWPARAMETERSDIALOG
