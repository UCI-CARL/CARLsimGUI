
#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) 
#    if defined(carlsimguiapplication_EXPORTS)
#      define SPIKESTREAM_APP_LIB_EXPORT Q_DECL_EXPORT		
#    else
#      define SPIKESTREAM_APP_LIB_EXPORT Q_DECL_IMPORT
#    endif
#else
#	define SPIKESTREAM_APP_LIB_EXPORT 
#endif
