#pragma once

#include <QObject>
#include <QPainter>
#include <QPrinter>

class Cbz2pdf;

class Cbz2pdfWorker : public QObject {
    Q_OBJECT
private:
    friend Cbz2pdf;
    explicit Cbz2pdfWorker();
    virtual ~Cbz2pdfWorker() = default;
    void addStep(QString currentDir);
    int createPdf(QString dirOrZipName);
    int addPage(QPrinter &printer, QPainter &painter, QString fileName);
public slots:
    void setK(double K);
    void setDirOrZipName(QString dirOrZipName);
    void start();
    void stop();
signals:
    void started();
    void finished(int codeError);
    void stoped();
    void process(double process);
    void logMsg(QString msg);
private:
    int checkStop();
    double _k = .75;
    QString _inPath;
    int _numPage = 0;
    int _step = 0;
    int _stepCount = 0;
    bool _toStop = false;
    bool _isStart = false;
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
    void stop()                                 { emit sigStop(); }
signals:
    void sigSetK(double k);
    void sigSetDirOrZipName(QString dirOrZipName);
    void sigStart();
    void sigStop();
};
