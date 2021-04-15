QT -= gui

QT += printsupport

TEMPLATE = lib
DEFINES += CBZ2PDF_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cbz2pdf.cpp \
    cbz2pdfprivate.cpp

include(cbz2pdf.pri)

DESTDIR = ../Bin

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    cbz2pdfprivate.h
