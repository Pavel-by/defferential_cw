QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cube.cpp \
    drawer.cpp \
    edit/editwindow.cpp \
    figure.cpp \
    figurewrapper.cpp \
    figureloader.cpp \
    light/lightconfig.cpp \
    light/materialconfig.cpp \
    main.cpp \
    mainwindow.cpp \
    simulation/iceberg.cpp \
    simulation/integralcalculator.cpp \
    simulation/polyhedron.cpp \
    simulation/quaternion.cpp \
    simulation/simulationwindow.cpp \
    simulation/state.cpp \
    simulation/water.cpp \
    utils/figureserializer.cpp \
    utils/verticesstretcher.cpp \
    viewmatrixwrapper.cpp \
    simulation/underwater.cpp

HEADERS += \
    cube.h \
    drawer.h \
    edit/editwindow.h \
    figure.h \
    figurewrapper.h \
    light/lightconfig.h \
    light/materialconfig.h \
    log.h \
    figureloader.h \
    mainwindow.h \
    simulation/iceberg.h \
    simulation/integralcalculator.h \
    simulation/polyhedron.h \
    simulation/quaternion.h \
    simulation/simulationwindow.h \
    simulation/state.h \
    simulation/water.h \
    utils/figureserializer.h \
    utils/verticesstretcher.h \
    viewmatrixwrapper.h \
    glutils.h \
    simulation/underwater.h

FORMS += \
    editwindow.ui \
    figureloader.ui \
    mainwindow.ui \
    simulationwindow.ui

LIBS += -lopengl32 -lglu32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    shaders.qrc
