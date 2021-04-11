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


void MainWindow::createPdf(QString pdfFullName, QStringList filesName) {
    QPrinter printer;
    if (filesName.isEmpty()) {
        return;
    }
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(pdfFullName);
    printer.setFullPage(true);
    QPainter painter;

    for (const auto &str: filesName) {
        addPage(printer, painter, str);
    }
    painter.end();
}

void MainWindow::addPage(QPrinter &printer, QPainter &painter, QString fileName) {
    static int numPage = 0;
    ++numPage;
    qDebug() << fileName;
    QPixmap pixmap;
    if (!pixmap.load(fileName)) {
        qWarning() << "can't load file:" << fileName;
        return;
    }
    qDebug() << pixmap.size();
    qDebug() << printer.setPageSize(QPageSize(pixmap.size() * .75, QPageSize::Point));
    if (numPage == 1) {
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
    QDir dir(dirStr);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    qDebug() << "     Bytes Filename";
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        qDebug() << QString("%1 %2").arg(fileInfo.size(), 10)
                                    .arg(fileInfo.fileName());
    }
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
    if (ui->leIn->text().isEmpty()) {
        QMessageBox::warning(this, "Select file", "Select dir or file for reading");
        return;
    }
    if (ui->leOutFile->text().isEmpty()) {
        QMessageBox::warning(this, "Select file", "Select out file");
        return;
    }
    QFileInfo fileInfo(ui->leIn->text());
    QStringList selectFiles;
    if (fileInfo.isDir()) {
        QDir dir(ui->leIn->text());
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            selectFiles.append(list.at(i).absoluteFilePath());
        }
    } else {
        QMessageBox::warning(this, "amm", "I must todo working with zip");
        return;
    }
    if (selectFiles.isEmpty()) {
        ui->statusbar->showMessage("Files not selected", 5);
        return;
    }
    createPdf(ui->leOutFile->text(), selectFiles);
    ui->statusbar->showMessage(QString("File %1 created!")
                               .arg(QFileInfo(ui->leOutFile->text()).fileName()), 5);
}


