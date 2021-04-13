#include "cbz2pdf.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>


Cbz2pdfWorker::Cbz2pdfWorker() : QObject(nullptr) { }

void Cbz2pdfWorker::setK(double K) {
    _k = K;
}

void Cbz2pdfWorker::setDirOrZipName(QString dirOrZipName) {
    _inPath = dirOrZipName;
}

void Cbz2pdfWorker::start() {
    emit started();
    createPdf(_inPath);
}

void Cbz2pdfWorker::addStep(QString currentDir) {
    if (currentDir == _inPath) {
        ++_step;
        emit process(1. * _step / _stepCount);
    }
}

void Cbz2pdfWorker::createPdf(QString dirOrZipName) {
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
            _step = 0;
            qDebug() << __PRETTY_FUNCTION__ << _stepCount;
        }
        QFileInfoList listDirs = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::Name);
        haveSubDirs = !listDirs.isEmpty();
        for (auto &dir: listDirs) {
            createPdf(dir.absoluteFilePath());
            addStep(dirOrZipName);
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
        return;
    }

    if (selectFiles.isEmpty() && !haveSubDirs) {
        emit logMsg("dir is empty");
        return;
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
        emit logMsg(QString("image[%1]: %2").arg(_numPage + 1, 3, 10, QLatin1Char('0')).arg(str));
        addPage(printer, painter, str);
        addStep(dirOrZipName);
    }
    painter.end();
    if (_inPath == dirOrZipName) {
        emit finished();
    }
}

void Cbz2pdfWorker::addPage(QPrinter &printer, QPainter &painter, QString fileName) {
    ++_numPage;
    QPixmap pixmap;
    if (!pixmap.load(fileName)) {
        emit logMsg(QString("can't load file: %1").arg(fileName));
        return;
    }
    printer.setPageSize(QPageSize(pixmap.size() * _k, QPageSize::Point));
    if (_numPage == 1) {
        if (! painter.begin(&printer)) {
            emit logMsg("failed to open file, is it writable?");
            return;
        }
    } else {
        printer.newPage();
    }
    painter.drawPixmap(0, 0, pixmap);
}

Cbz2pdfPrivate::Cbz2pdfPrivate(Cbz2pdfWorker *worker, Cbz2pdf *controller) {
    connect(this, &Cbz2pdfPrivate::sigSetK           , worker, &Cbz2pdfWorker::setK           );
    connect(this, &Cbz2pdfPrivate::sigSetDirOrZipName, worker, &Cbz2pdfWorker::setDirOrZipName);
    connect(this, &Cbz2pdfPrivate::sigStart          , worker, &Cbz2pdfWorker::start          );
    connect(worker, &Cbz2pdfWorker::started, this, [controller] {
        emit controller->started();
    });
    connect(worker, &Cbz2pdfWorker::finished, this, [controller] {
        emit controller->finished();
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
    _thread.quit();
    _thread.wait();
}

