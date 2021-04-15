INCLUDEPATH += $$PWD

QT += gui
QT += printsupport

HEADERS += \
    $$PWD/cbz2pdf.h

# Если pri & pro файлы имеют одинаковый путь
!equals(PWD, $$_PRO_FILE_PWD_) {
    LIBS += -L$$DESTDIR -lcbz2pdf
}
