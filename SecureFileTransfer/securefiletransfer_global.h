#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SECUREFILETRANSFER_LIB)
#  define SECUREFILETRANSFER_EXPORT Q_DECL_EXPORT
# else
#  define SECUREFILETRANSFER_EXPORT Q_DECL_IMPORT
# endif
#else
# define SECUREFILETRANSFER_EXPORT
#endif
