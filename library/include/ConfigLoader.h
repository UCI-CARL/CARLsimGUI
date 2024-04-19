#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

//Qt includes
#include <QString>
#include <QHash>

// Spikestream 0.3
#include "api.h" 

namespace spikestream{

	/*! Loads the configuration from the specified file. */
	class SPIKESTREAM_LIB_EXPORT ConfigLoader{
		public:
			ConfigLoader();
				
			ConfigLoader(const QString& configFilePath); 
			~ConfigLoader();
			QString getParameter(const QString& paramName);
			QString getParameter(const QString& paramName, const QString& paramDefault);  

			bool hasParameter(const QString& paramName) { 
				return configMap.contains(paramName);
			};

			static void setConfigFilePath(const QString& path); 
			static const QString& getConfigFilePath();

		private:
			//=========================== VARIABLES =======================================
			/*! Holds the configuration parameters loaded from the file.*/
			QHash<QString, QString> configMap;


			//============================ METHODS ========================================
			/*! Declare copy constructor private so it cannot be used inadvertently.*/
			ConfigLoader(const ConfigLoader&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			ConfigLoader operator = (const ConfigLoader&);

			void printConfig();
			void setParameter(const QString& configFileLine);
			void ConfigLoader::setParameter(const QString& configFileLine, QString& prevParam); 

			/*! Relative Path of the config file passed as first command line param */
			static QString configFilePath;

	};

}

#endif//CONFIGLOADER_H
