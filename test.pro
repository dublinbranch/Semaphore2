CONFIG += c++17 console
CONFIG -= app_bundle
CONFIG -= qt


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#        boo.cpp \
        main.cpp \
        semaphore2.cpp

#QMAKE_CXXFLAGS += -O0 -march=native #-flto -fuse-ld=gold -fuse-linker-plugin

#LIBS += -L$$PWD/mmap_allocator/ -l:libmmap_allocator.a

#QMAKE_CXXFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_CFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_LFLAGS+= -fsanitize=address -fno-omit-frame-pointer

HEADERS += \
    semaphore2.h

LIBS += -lpthread
