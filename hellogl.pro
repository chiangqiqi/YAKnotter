VPATH += ../shared
INCLUDEPATH += ../shared

HEADERS       = glwidget.h \
                window.h \
                qtlogo.h \
    knot.h \
    segment.h \
    paintwidget.h \
    BezierCurve.h
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                knot.cpp \
                segment.cpp \
    paintwidget.cpp \
    BezierCurve.inl
QT           += opengl widgets

QMAKE_CXXFLAGS += -std=c++11
# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
INSTALLS += target

contains(QT_CONFIG, opengles.) {
    contains(QT_CONFIG, angle): \
        warning("Qt was built with ANGLE, which provides only OpenGL ES 2.0 on top of DirectX 9.0c")
    error("This example requires Qt to be configured with -opengl desktop")
}

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl
