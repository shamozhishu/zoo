#ifndef OSGQT_VERSION
#define OSGQT_VERSION

#define OSGQT_QT_VERSION 5

#include <QtGlobal>
#if (QT_VERSION < QT_VERSION_CHECK(OSGQT_QT_VERSION,0,0))
#error "Qt version mismatch detected! Make sure to compile applications using osgQt with the same major Qt version that osgQt has been compiled against."
#endif

#endif
