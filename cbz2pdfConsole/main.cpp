#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QGuiApplication>
#include <iostream>

#include "cbz2pdf.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QCommandLineOption coefOption({"k", "coef"}, "Set coefficient", "coef");
    QCommandLineOption pathOption({"d", "dir", "f", "file", "p", "path"}, "Input path", "path");
    QCommandLineOption noLogOption({"no-log", "nl"}, "Without logs");

    QCommandLineParser parser;
    parser.addOptions({coefOption, pathOption, noLogOption});
    parser.process(a.arguments());
    Cbz2pdf c;
    if (!parser.isSet(noLogOption)) {
        QObject::connect(&c, &Cbz2pdf::started, [] {
            std::cout << "started" << std::endl;
        });
        QObject::connect(&c, &Cbz2pdf::stoped, [] {
            std::cout << "stoped!" << std::endl;
        });

        QObject::connect(&c, &Cbz2pdf::logMsg, [] (QString msg) {
            std::cout << "msg: " << qPrintable(msg) << std::endl;
        });
    }

    QObject::connect(&c, &Cbz2pdf::finished, &a, &QCoreApplication::exit);
    if (parser.isSet(pathOption)) {
        c.setDirOrZipName(parser.value(pathOption));
        if (parser.isSet(coefOption)) {
            c.setK(parser.value(coefOption).toDouble());
        }
    } else {
        std::cout << "input dir or zip file: ";
        std::string s;
        std::cin >> s;
        c.setDirOrZipName(QString::fromStdString(s));
        std::cout << "input coefficient (default 0.75): ";
        double k;
        std::cin >> k;
        c.setK(k);
    }
    c.start();
    return a.exec();
}
