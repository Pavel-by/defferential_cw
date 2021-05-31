#include "mainwindow.h"

#include <QApplication>
#include <QOpenGLContext>
#include <QSurfaceFormat>

#include <simulation/simulationwindow.h>

#include <edit/editwindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat *format = new QSurfaceFormat();
    format->setDepthBufferSize(24);
    format->setMajorVersion(1);
    format->setMinorVersion(4);
    format->setSamples(4);
    format->setProfile(QSurfaceFormat::NoProfile);
    QSurfaceFormat::setDefaultFormat(*format);

    // Just for test
    // MainWindow w;
    EditWindow w;
    w.show();
    return a.exec();
}
