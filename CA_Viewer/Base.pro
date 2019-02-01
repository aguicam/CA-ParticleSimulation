
TEMPLATE = app
TARGET = Base

QT += gui widgets opengl

CONFIG += qt debug_and_release c++11

Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR     = release/.moc
Debug:OBJECTS_DIR   = debug/.obj
Debug:MOC_DIR       = debug/.moc

INCLUDEPATH += .

win32-msvc2015{
        LIBS           += -lopengl32
        QMAKE_CXXFLAGS += /MP /Zi
        QMAKE_LFLAGS   += /MACHINE:X64
}

macx{
        QMAKE_SONAME_PREFIX = @executable_path/../Frameworks
        QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Frameworks
        QMAKE_LFLAGS   -= -mmacosx-version-min=10.8
        QMAKE_LFLAGS   += -mmacosx-version-min=10.9
        QMAKE_CXXFLAGS -= -mmacosx-version-min=10.8
        QMAKE_CXXFLAGS += -mmacosx-version-min=10.9
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}

unix:!macx{
        CONFIG += C++11
}

mingw{
        LIBS += -lopengl32
}

# Input
HEADERS += glwidget.h mainwindow.h \
    trianglemesh.h \
    plyreader.h \
    GeometryP.h \
    Particle.h
FORMS += mainwindow.ui
SOURCES += glwidget.cpp main.cpp mainwindow.cpp \
    trianglemesh.cpp \
    plyreader.cpp \
    GeometryP.cpp \
    Particle.cpp

DISTFILES += \
    shaders/simpleshader.vert \
    shaders/simpleshader.frag

RESOURCES += \
    resources.qrc

LIBS += -lopengl32
