TEMPLATE = app
CONFIG += c++14 thread silent
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../lib ../.. /usr/include/wx-3.0
DEPENDPATH += .. ../../lib ../../gui
PRE_TARGETDEPS += ../../lib/liblib.a ../../gui/libgui.a
LIBS += ../../gui/libgui.a
LIBS += ../../lib/liblib.a # must be used after libgui
LIBS += `wx-config --libs --gl-libs`

DEFINES += SPH_USE_EIGEN

QMAKE_CXXFLAGS += -Wall -Werror -msse4.1 -std=c++14 -pthread `wx-config --libs all --cxxflags --gl-libs`

CONFIG(release, debug|profile|assert|release) {
  message( "SPH ROTATION --- Building for Release" )
}

CONFIG(profile, debug|profile|assert|release) {
  message( "SPH ROTATION --- Building for Profile" )
  DEFINES += SPH_PROFILE
}

CONFIG(assert, debug|profile|assert|release) {
  message( "SPH ROTATION --- Building for Assert" )
  DEFINES += SPH_DEBUG SPH_PROFILE
  QMAKE_CXXFLAGS += -O2
}

CONFIG(debug, debug|profile|assert|release) {
  message( "SPH ROTATION --- Building for Debug" )
  DEFINES += SPH_DEBUG SPH_PROFILE
}

SOURCES += \
    Rotation.cpp

HEADERS += \
    Rotation.h

