#ifndef GUIDLL_GLOBAL_H
#define GUIDLL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GUIDLL_LIBRARY)
#  define GUIDLLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GUIDLLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GUIDLL_GLOBAL_H
