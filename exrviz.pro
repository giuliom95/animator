TEMPLATE = app
TARGET = bin/animator
HEADERS +=  src/mainwin.hpp     \
            src/oglWidget.hpp   \
            src/utilities.hpp
SOURCES +=  src/main.cpp        \
            src/mainwin.cpp     \
            src/oglWidget.cpp   \
            src/utilities.cpp
QT += widgets
INCLUDEPATH +=
LIBS +=