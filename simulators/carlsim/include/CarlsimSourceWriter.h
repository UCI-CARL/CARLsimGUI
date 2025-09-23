#ifndef CARLSIMSOURCEWRITER_H
#define CARLSIMSOURCEWRITER_H


#include <QString>
#include <QDir>


// transient classes for openening and closing files etc. 
// 
#include "api.h" 

namespace spikestream {
	class CarlsimWrapper;

	class CARLSIMWRAPPER_LIB_EXPORT CarlsimSourceWriter    {

	public:

		enum Content {Groups, Connections, Generators, Deletes, Main, Events, Monitors}; 
		CarlsimSourceWriter(Content content, bool append = true);  // type ?  aspect_t type
		~CarlsimSourceWriter(); 
	
		static void SetWrapper(spikestream::CarlsimWrapper* w);
		static void TouchFiles();

		static const QString ContainerPath();

		static const QString NamePath() { return Name; }

		// read only
		static bool Generate;  // cached flag, if to generate (and write) source
		FILE* file;

	private: 
		static spikestream::CarlsimWrapper* Wrapper;
		static QString Dir;  // containing directory of source and data files
		static QString Name;  // containing directory of source and data files

		QDir dir; 
		Content content; 
	};

}

#endif // CARLSIMSOURCEWRITER_H