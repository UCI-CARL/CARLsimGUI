#ifndef PLUGINSDIALOG_H
#define PLUGINSDIALOG_H

// SpikeStream 0.3
#include "api.h"

//Qt includes
#include <QDialog>

namespace spikestream {

    /*! Scans the specified plugins folder and displays a list of available plugins in
		a combo box, which enables the user to select one and interact with its GUI */
    class SPIKESTREAM_APP_LIB_EXPORT PluginsDialog : public QDialog {
		Q_OBJECT

		public:
			PluginsDialog(QWidget* parent, const QString pluginFolder, const QString title);
			~PluginsDialog();
	};

}

#endif//PLUGINSDIALOG_H

