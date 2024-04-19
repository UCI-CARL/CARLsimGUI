
#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) 
#    if defined(carlsimwrapper_EXPORTS)
#      define CARLSIMWRAPPER_LIB_EXPORT Q_DECL_EXPORT		
#    else
#      define CARLSIMWRAPPER_LIB_EXPORT Q_DECL_IMPORT
#    endif
#endif

