#ifndef GRAPHOS_GLOBAL_H
#define GRAPHOS_GLOBAL_H

#include <QtCore/qglobal.h>

#include "config_graphos.h"

#if defined(GRAPHOS_EXPORTS)
#  define GRAPHOS_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHOS_EXPORT
#endif

#if __cplusplus >= 201103L
#  define GRAPHOS_NOEXCEPT noexcept
#  define GRAPHOS_NOEXCEPT_OP(x) noexcept((x))
#else
#  define GRAPHOS_NOEXCEPT throw ()
#  define GRAPHOS_NOEXCEPT_OP(x)
#endif

#endif // GRAPHOS_GLOBAL_H
