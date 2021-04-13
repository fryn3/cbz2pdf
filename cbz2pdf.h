#pragma once

#include <QObject>
#include <QPainter>
#include <QPrinter>
#include <QThread>

class Cbz2pdf;

class Cbz2pdfWorker : public QObject {
    Q_OBJECT
private:
    friend Cbz2pdf;
    explicit Cbz2pdfWorker();
    virtual ~Cbz2pdfWorker() = default;
    void addStep(QString currentDir);
    void createPdf(QString dirOrZipName);
    void addPage(QPrinter &printer, QPainter &painter, QString fileName);
public slots:
    void setK(double K);
    void setDirOrZipName(QString dirOrZipName);
    void start();
signals:
    void started();
    void finished();
    void process(double process);
    void logMsg(QString msg);
private:
    double _k = .75;
    QString _inPath;
    int _numPage = 0;
    int _step = 0;
    int _stepCount = 0;
};

class Cbz2pdfPrivate : public QObject {
    Q_OBJECT
private:
    friend Cbz2pdf;
    explicit Cbz2pdfPrivate(Cbz2pdfWorker *worker, Cbz2pdf *controller = nullptr);
    virtual ~Cbz2pdfPrivate() = default;
public:
    void setK(double k)                         { emit sigSetK(k); }
    void setDirOrZipName(QString dirOrZipName)  { emit sigSetDirOrZipName(dirOrZipName); }
    void start()                                { emit sigStart(); }
signals:
    void sigSetK(double k);
    void sigSetDirOrZipName(QString dirOrZipName);
    void sigStart();
};

class Cbz2pdf : public QObject {
    Q_OBJECT
public:
    explicit Cbz2pdf(QObject *parent = nullptr);
    virtual ~Cbz2pdf();
public slots:
    void setK(double k)                         { _private->setK(k); }
    void setDirOrZipName(QString dirOrZipName)  { _private->setDirOrZipName(dirOrZipName); }
    void start()                                { _private->start(); }
signals:
    void started();
    void finished();
    void process(double process);
    void logMsg(QString msg);
private:
    Cbz2pdfWorker *_worker;
    Cbz2pdfPrivate *_private;
    QThread _thread;
};

