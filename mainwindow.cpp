#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(&_controller, &Cbz2pdf::started, [this] {
        ui->textBrowser->append("process started");
    });
    connect(&_controller, &Cbz2pdf::finished, [this] {
        ui->textBrowser->append("process finished");
    });
    connect(&_controller, &Cbz2pdf::process, [this] (double process) {
        ui->progressBar->setValue(process * 1000);
    });
    connect(&_controller, &Cbz2pdf::logMsg, [this] (QString msg) {
        ui->textBrowser->append(msg);
    });

    QDesktopWidget *desk = QApplication::desktop();
    qDebug() << desk->width()<< desk->height();
    qDebug() << desk;
}

MainWindow::~MainWindow() { delete ui; }

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

void MainWindow::on_btnStart_clicked() {
    ui->textBrowser->clear();
    if (ui->leIn->text().isEmpty()) {
        QMessageBox::warning(this, "Select file", "Select dir or file for reading");
        return;
    }
    _controller.setK(ui->sbK->value());
    _controller.setDirOrZipName(ui->leIn->text());
    _controller.start();
}


