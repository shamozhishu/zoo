#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LIBCTK_WAR_SCENE_LIB)
#  define LIBCTK_WAR_SCENE_EXPORT Q_DECL_EXPORT
# else
#  define LIBCTK_WAR_SCENE_EXPORT Q_DECL_IMPORT
# endif
#else
# define LIBCTK_WAR_SCENE_EXPORT
#endif
