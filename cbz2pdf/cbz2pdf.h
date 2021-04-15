#pragma once

#include <QtCore/qglobal.h>

#if defined(CBZ2PDF_LIBRARY)
#  define CBZ2PDF_EXPORT Q_DECL_EXPORT
#else
#  define CBZ2PDF_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>

#include <QThread>
#include <QDebug>

class Cbz2pdfWorker;
class Cbz2pdfPrivate;

class CBZ2PDF_EXPORT Cbz2pdf : public QObject {
    Q_OBJECT
public:
    enum CodeError {
        NoError,
        ErrStoped,
        ErrFileOut,
        ErrFileIn,

        CodeErrorCount
    };
    explicit Cbz2pdf(QObject *parent = nullptr);
    virtual ~Cbz2pdf();
public slots:
    void setK(double k);
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
    Cbz2pdfWorker *_worker;
    Cbz2pdfPrivate *_private;
    QThread _thread;
};
