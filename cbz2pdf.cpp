#include "cbz2pdf.h"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QTimer>


Cbz2pdfWorker::Cbz2pdfWorker() : QObject(nullptr) { }

void Cbz2pdfWorker::setK(double K) {
    _k = K;
}

void Cbz2pdfWorker::setDirOrZipName(QString dirOrZipName) {
    _inPath = dirOrZipName;
}

void Cbz2pdfWorker::start() {
    _isStart = true;
    _toStop = false;
    _step = 0;
    emit started();
    auto err = createPdf(_inPath);
    if (_toStop) {
        _toStop = false;
        emit stoped();
        _isStart = false;
        return;
    }
    emit process(1);
    emit finished(err);
    _isStart = false;
}

void Cbz2pdfWorker::stop() {
    if (!_isStart) {
        emit stoped();
        return;
    }
    _toStop = true;
}

void Cbz2pdfWorker::addStep(QString currentDir) {
    if (currentDir == _inPath) {
        ++_step;
        emit process(1. * _step / _stepCount);
    }
}

int Cbz2pdfWorker::checkStop() {
    QEventLoop loop;
    loop.processEvents();
    if (_toStop) {
        return Cbz2pdf::ErrStoped;
    }
    return Cbz2pdf::NoError;
}

int Cbz2pdfWorker::createPdf(QString dirOrZipName) {
    QFileInfo fileInfo(dirOrZipName);
    QStringList selectFiles;
    bool haveSubDirs = false;
    if (fileInfo.isDir()) {
        QDir dir(dirOrZipName);
        if (dirOrZipName == _inPath) {
            // Тут можно вычесть пдф файлы, тк я их не учитываю при прогрессе.
            _stepCount = dir.entryInfoList(QDir::Files | QDir::Dirs
                                           | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                           QDir::Name).count();
        }
        QFileInfoList listDirs = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::Name);
        haveSubDirs = !listDirs.isEmpty();
        for (auto &dir: listDirs) {
            createPdf(dir.absoluteFilePath());
            addStep(dirOrZipName);
            if (checkStop()) {
                return Cbz2pdf::ErrStoped;
            }
        }
        QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoSymLinks, QDir::Name);
        for (auto &fileInfo: list) {
            if (!fileInfo.absoluteFilePath().endsWith(".pdf", Qt::CaseInsensitive)) {
                if (fileInfo.absoluteFilePath().endsWith(".zip", Qt::CaseInsensitive)) {
                    /// В будущем обрабатывать зип архивы!
                    addStep(dirOrZipName);
                } else {
                    selectFiles.append(fileInfo.absoluteFilePath());
                }
            }
        }
    } else {
        emit logMsg("I must todo working with zip");
        return Cbz2pdf::NoError;
    }

    if (selectFiles.isEmpty() && !haveSubDirs) {
        emit logMsg("dir is empty");
        return Cbz2pdf::NoError;
    }

    if (checkStop()) {
        return Cbz2pdf::ErrStoped;
    }
    _numPage = 0;
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    QString pdfFullName = fileInfo.absoluteFilePath().append(".pdf");
    printer.setOutputFileName(pdfFullName);
    printer.setFullPage(true);
    QPainter painter;
    emit logMsg(QString("generation %1 file").arg(QFileInfo(pdfFullName).fileName()));
    for (const auto &str: selectFiles) {
        if (checkStop()) {
            painter.end();
            return Cbz2pdf::ErrStoped;
        }
        emit logMsg(QString("image[%1]: %2").arg(_numPage + 1, 3, 10, QLatin1Char('0')).arg(str));
        if (int err = addPage(printer, painter, str) != Cbz2pdf::NoError) {
            return err;
        }
        addStep(dirOrZipName);
    }
    painter.end();
    return Cbz2pdf::NoError;
}

int Cbz2pdfWorker::addPage(QPrinter &printer, QPainter &painter, QString fileName) {
    ++_numPage;
    QPixmap pixmap;
    if (!pixmap.load(fileName)) {
        emit logMsg(QString("can't load file: %1").arg(fileName));
        return Cbz2pdf::ErrFileIn;
    }
    printer.setPageSize(QPageSize(pixmap.size() * _k, QPageSize::Point));
    if (_numPage == 1) {
        if (! painter.begin(&printer)) {
            emit logMsg("failed to open file, is it writable?");
            return Cbz2pdf::ErrFileOut;
        }
    } else {
        printer.newPage();
    }
    painter.drawPixmap(0, 0, pixmap);
    return Cbz2pdf::NoError;
}

Cbz2pdfPrivate::Cbz2pdfPrivate(Cbz2pdfWorker *worker, Cbz2pdf *controller)
    : QObject(controller) {
    connect(this, &Cbz2pdfPrivate::sigSetK           , worker, &Cbz2pdfWorker::setK           );
    connect(this, &Cbz2pdfPrivate::sigSetDirOrZipName, worker, &Cbz2pdfWorker::setDirOrZipName);
    connect(this, &Cbz2pdfPrivate::sigStart          , worker, &Cbz2pdfWorker::start          );
    connect(this, &Cbz2pdfPrivate::sigStop           , worker, &Cbz2pdfWorker::stop           );
    connect(worker, &Cbz2pdfWorker::started, this, [controller] {
        emit controller->started();
    });
    connect(worker, &Cbz2pdfWorker::finished, this, [controller] (int codeError) {
        emit controller->finished(codeError);
    });
    connect(worker, &Cbz2pdfWorker::stoped, this, [controller] {
        emit controller->stoped();
    });
    connect(worker, &Cbz2pdfWorker::process, this, [controller] (double process) {
        emit controller->process(process);
    });
    connect(worker, &Cbz2pdfWorker::logMsg, this, [controller] (QString msg) {
        emit controller->logMsg(msg);
    });
}

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

