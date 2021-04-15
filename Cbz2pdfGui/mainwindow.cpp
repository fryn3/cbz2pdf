#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(this, &MainWindow::isStartedChanged, this, [this] {
        if (isStarted()) {
            ui->btnStartStop->setText("Stop");
        } else {
            ui->btnStartStop->setText("Start convert to PDF");
            ui->btnStartStop->setEnabled(true);
        }
    });
    connect(ui->btnStartStop, &QPushButton::clicked, this, &MainWindow::start);
    connect(&_controller, &Cbz2pdf::started, this, [this] {
        ui->textBrowser->append("process started");
    });
    connect(&_controller, &Cbz2pdf::finished, this, [this] (int codeError) {
        setIsStarted(false);
        if (codeError) {
            ui->textBrowser->append(QString("process finished with errorcode %1").arg(codeError));
        } else {
            ui->textBrowser->append("process finished");
        }
    });
    connect(&_controller, &Cbz2pdf::stoped, this, [this] {
        setIsStarted(false);
        ui->textBrowser->append("process stoped!");
        ui->progressBar->setValue(0);
    });
    connect(&_controller, &Cbz2pdf::process, this, [this] (double process) {
        ui->progressBar->setValue(process * 1000);
    });
    connect(&_controller, &Cbz2pdf::logMsg, this, [this] (QString msg) {
        ui->textBrowser->append(msg);
    });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setPath(QString path) {
    ui->leIn->setText(path);
}

void MainWindow::setK(double coef) {
    ui->sbK->setValue(coef);
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

bool MainWindow::isStarted() const {
    return _isStarted;
}

void MainWindow::setIsStarted(bool value) {
    if (_isStarted != value) {
        _isStarted = value;
        emit isStartedChanged();
    }
}

void MainWindow::start() {
    if (isStarted()) {
        _controller.stop();
        ui->btnStartStop->setEnabled(false);
        ui->btnStartStop->setText("Wait please");
    } else {
        ui->textBrowser->clear();
        ui->progressBar->setValue(0);
        if (ui->leIn->text().isEmpty()) {
            QMessageBox::warning(this, "Select file", "Select dir or file for reading");
            return;
        }
        _controller.setK(ui->sbK->value());
        _controller.setDirOrZipName(ui->leIn->text());
        _controller.start();
        setIsStarted(true);
    }
}
