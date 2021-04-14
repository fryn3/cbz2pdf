#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineOption coefOption({"k", "coef"}, "Set coefficient", "coef");
    QCommandLineOption pathOption({"d", "dir", "f", "file", "p", "path"}, "Input path", "path");
    QCommandLineOption miniOption({"m", "mini"}, "Without window and auto start");
    QCommandLineOption startOption({"s", "start"}, "Auto start, if not mini");

    QCommandLineParser parser;
    parser.addOptions({coefOption, pathOption, miniOption, startOption});
    parser.process(a.arguments());

    if (parser.isSet(miniOption)) {
        if (!parser.isSet(pathOption)) {
            Q_ASSERT(false && "must be path");
            return 1;
        }
        Cbz2pdf c;
        c.setDirOrZipName(parser.value(pathOption));
        if (parser.isSet(coefOption)) {
            c.setK(parser.value(coefOption).toDouble());
        }
        QObject::connect(&c, &Cbz2pdf::finished, &a, &QApplication::quit);
        c.start();
        return a.exec();
    } else {
        MainWindow w;
        if (parser.isSet(coefOption)) {
            w.setK(parser.value(coefOption).toDouble());
        }
        if (parser.isSet(pathOption)) {
            w.setPath(parser.value(pathOption));
        }
        if (parser.isSet(startOption)) {
            w.start();
        }
        w.show();
        return a.exec();
    }
}
