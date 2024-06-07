
#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) 
#    if defined(carlsimguitest_EXPORTS)
#      define SPIKESTREAM_TEST_LIB_EXPORT Q_DECL_EXPORT		
#    else
#      define SPIKESTREAM_TEST_LIB_EXPORT Q_DECL_IMPORT
#    endif
#else
#	 define SPIKESTREAM_TEST_LIB_EXPORT
#endif
