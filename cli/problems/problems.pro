TEMPLATE = app
CONFIG += c++14 thread silent
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../lib ../../../external/Catch/include
DEPENDPATH += . ../../lib
PRE_TARGETDEPS += ../../lib/liblib.a
LIBS += ../../lib/liblib.a

include(../../lib/inc.pro)

SOURCES += \
    main.cpp \
    sod/Sod.cpp \
    wave/SoundWave.cpp \
    elastic_bands/ElasticBands.cpp \
    cliff_collapse/CliffCollapse.cpp \
    collision/Collision.cpp \
    Common.cpp

HEADERS += \  
    sod/solution.h \
    Common.h
