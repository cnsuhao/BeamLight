TARGET = beamlight

LIBS += ../BeamLight/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

QMAKE_CXXFLAGS += -std=c++14 -O2

INCLUDEPATH += \
    ../BeamLight/Urho3D/include \
    ../BeamLight/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    luckey.h \
    mastercontrol.h \
    beamlight.h

SOURCES += \
    luckey.cpp \
    mastercontrol.cpp \
    beamlight.cpp

DISTFILES += \
    LICENSE_TEMPLATE

