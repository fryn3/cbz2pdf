#include "cbz2pdf.h"

#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include <cbz2pdfprivate.h>

Cbz2pdf::Cbz2pdf(QObject *parent)
    : QObject(parent),
      _worker(new Cbz2pdfWorker()),
      _private(new Cbz2pdfPrivate(_worker, this)) {
    _worker->moveToThread(&_thread);
    connect(&_thread, &QThread::finished, _worker, &QObject::deleteLater);
    _thread.start();
}

Cbz2pdf::~Cbz2pdf() {
    stop();
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect( _worker, &Cbz2pdfWorker::stoped, &loop, &QEventLoop::quit );
    connect( &timer, &QTimer::timeout, &loop, &QEventLoop::quit );
    timer.start(2000);
    loop.exec();
    if (timer.isActive()) {
        _thread.quit();
        _thread.wait();
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "timeout";
        _thread.terminate();
    }
}

void Cbz2pdf::setK(double k)                         { _private->setK(k); }

void Cbz2pdf::setDirOrZipName(QString dirOrZipName)  { _private->setDirOrZipName(dirOrZipName); }

void Cbz2pdf::start()                                { _private->start(); }

void Cbz2pdf::stop()                                 { _private->stop(); }

