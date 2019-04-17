TEMPLATE = app
TARGET = bin/animator
HEADERS +=  src/mainwin.hpp     \
            src/oglWidget.hpp
SOURCES +=  src/main.cpp        \
            src/mainwin.cpp     \
            src/oglWidget.cpp
QT += widgets
INCLUDEPATH +=
LIBS +=