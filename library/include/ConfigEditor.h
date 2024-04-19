#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

//Qt includes
#include <QFile>
#include <QHash>
#include <QString>

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! Sets parameters in the config file and creates a new config file based on the template if one
		does not exist */
	class SPIKESTREAM_LIB_EXPORT ConfigEditor {
		public:
			ConfigEditor();
			~ConfigEditor();

			void setConfigParameters(QHash<QString, QString> newParamMap);

		private:
			//========================  VARIABLES  ========================
			QString rootDirectory;

			//=========================  METHODS  =========================
			void setParameter(QHash<QString, QString>& paramMap, QString& configFileLine);
	};

}

#endif//CONFIGEDITOR_H
