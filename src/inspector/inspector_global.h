#ifndef INSPECTOR_GLOBAL_H
#define INSPECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#include "config_inspector.h"

#if defined(INSPECTOR_EXPORTS)
#  define INSPECTOR_EXPORT Q_DECL_EXPORT
#else
#  define INSPECTOR_EXPORT
#endif

#if __cplusplus >= 201103L
#  define INSPECTOR_NOEXCEPT noexcept
#  define INSPECTOR_NOEXCEPT_OP(x) noexcept((x))
#else
#  define INSPECTOR_NOEXCEPT throw ()
#  define INSPECTOR_NOEXCEPT_OP(x)
#endif

#endif // INSPECTOR_GLOBAL_H
