#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DATABASEACCESS_LIB)
#  define DATABASEACCESS_EXPORT Q_DECL_EXPORT
# else
#  define DATABASEACCESS_EXPORT Q_DECL_IMPORT
# endif
#else
# define DATABASEACCESS_EXPORT
#endif
