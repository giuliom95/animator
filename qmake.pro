TEMPLATE = app
TARGET = bin/animator
HEADERS +=  src/mainwin.hpp     \
            src/oglWidget.hpp   \
            src/timebar.hpp     \
            src/utilities.hpp   \
            src/appstate.hpp
SOURCES +=  src/main.cpp        \
            src/mainwin.cpp     \
            src/oglWidget.cpp   \
            src/timebar.cpp     \
            src/utilities.cpp
QT += widgets
INCLUDEPATH +=
LIBS +=