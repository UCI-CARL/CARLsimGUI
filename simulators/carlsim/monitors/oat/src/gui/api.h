
#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN) 
#    if defined(carlsimoat_EXPORTS)
#      define CARLSIMOAT_LIB_EXPORT Q_DECL_EXPORT		
#    else
#      define CARLSIMOAT_LIB_EXPORT Q_DECL_IMPORT
#    endif
#endif

