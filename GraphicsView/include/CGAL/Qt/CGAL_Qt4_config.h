#ifndef CGAL_QT4_CONFIG_H
#define CGAL_QT4_CONFIG_H

#include <QtCore/qglobal.h>

#ifdef CGAL_Qt4_EXPORTS
#  define CGAL_QT4_EXPORT Q_DECL_EXPORT
#else
#  define CGAL_QT4_EXPORT Q_DECL_IMPORT
#endif

#endif // CGAL_QT4_CONFIG_H
