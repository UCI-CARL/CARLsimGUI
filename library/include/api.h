#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) 
#    if defined(carlsimguilib_EXPORTS)
#      define SPIKESTREAM_LIB_EXPORT Q_DECL_EXPORT		
#    else
#      define SPIKESTREAM_LIB_EXPORT Q_DECL_IMPORT
#    endif
#else
#	define SPIKESTREAM_LIB_EXPORT
#endif
