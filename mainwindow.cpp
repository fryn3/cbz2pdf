#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }


void MainWindow::createPdf(QString dirOrZipName, QString pdfFullName) {
    QFileInfo fileInfo(dirOrZipName);
    QStringList selectFiles;
    bool haveSubDirs = false;
    if (fileInfo.isDir()) {
        QDir dir(dirOrZipName);
        QFileInfoList listDirs = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::Name);
        haveSubDirs = !listDirs.isEmpty();
        for (auto &dir: listDirs) {
            createPdf(dir.absoluteFilePath());
        }
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        QFileInfoList list = dir.entryInfoList();
        for (auto &fileInfo: list) {
            if (!fileInfo.absoluteFilePath().endsWith(".pdf", Qt::CaseInsensitive)
                    && !fileInfo.absoluteFilePath().endsWith(".zip", Qt::CaseInsensitive)) {
                /// В будущем обрабатывать зип архивы!
                selectFiles.append(fileInfo.absoluteFilePath());
            }
        }
    } else {
        QMessageBox::warning(this, "amm", "I must todo working with zip");
        return;
    }


    QPrinter printer;
    _numPage = 0;
    if (selectFiles.isEmpty() && !haveSubDirs) {
        ui->textBrowser->append("dir is empty");
        return;
    }
    printer.setOutputFormat(QPrinter::PdfFormat);
    if (pdfFullName.isEmpty()) {
        pdfFullName = QFileInfo(dirOrZipName).absoluteFilePath().append(".pdf");
    }
    printer.setOutputFileName(pdfFullName);
    printer.setFullPage(true);
    QPainter painter;
    ui->textBrowser->append(QString("generation %1 file").arg(QFileInfo(pdfFullName).fileName()));
    for (const auto &str: selectFiles) {
        ui->textBrowser->append(QString("image[%1]: %2").arg(_numPage + 1, 3, 10, QLatin1Char('0')).arg(str));
        addPage(printer, painter, str);
    }
    painter.end();
}

void MainWindow::addPage(QPrinter &printer, QPainter &painter, QString fileName) {
    ++_numPage;
    qDebug() << fileName;
    QPixmap pixmap;
    if (!pixmap.load(fileName)) {
        qWarning() << "can't load file:" << fileName;
        return;
    }
    qDebug() << pixmap.size();
    qDebug() << printer.setPageSize(QPageSize(pixmap.size() * ui->sbK->value(), QPageSize::Point));
    if (_numPage == 1) {
        if (! painter.begin(&printer)) {
            qWarning("failed to open file, is it writable?");
            return;
        }
    } else {
        qDebug() << printer.newPage();
    }
    painter.drawPixmap(0, 0, pixmap);
}

void MainWindow::on_btnSelectDir_clicked() {
    QString dirStr = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui->leIn->setText(dirStr);
}

void MainWindow::on_btnSelectFile_clicked() {
    QString fileStr = QFileDialog::getOpenFileName(this, tr("Select file"), "",
                                                  tr("Zip (*.zip *.cbz);;Any (*.*)"));
    ui->leIn->setText(fileStr);
}

void MainWindow::on_pushButton_clicked() {
    QString fileStr = QFileDialog::getSaveFileName(this, tr("Select file"), "",
                                                  tr("PDF (*.pdf)"));
    ui->leOutFile->setText(fileStr);
}

void MainWindow::on_btnStart_clicked() {
    ui->textBrowser->clear();
    if (ui->leIn->text().isEmpty()) {
        QMessageBox::warning(this, "Select file", "Select dir or file for reading");
        return;
    }
    createPdf(ui->leIn->text(), ui->leOutFile->text());
    ui->statusbar->showMessage(QString("File %1 created!")
                               .arg(QFileInfo(ui->leOutFile->text()).fileName()), 15);
}


