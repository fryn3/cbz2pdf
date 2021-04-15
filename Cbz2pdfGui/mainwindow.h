#pragma once

#include "cbz2pdf.h"

#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <qprinter.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void setPath(QString path);
    void setK(double coef);
    void start();

private slots:
    void on_btnSelectDir_clicked();
    void on_btnSelectFile_clicked();
signals:
    void isStartedChanged();

private:
    bool isStarted() const;
    void setIsStarted(bool value);
    void createPdf(QString dirOrZipName, QString pdfFullName = nullptr);
    void addPage(QPrinter &printer, QPainter &painter, QString fileName);
    Ui::MainWindow *ui;
    int _numPage = 0;
    Cbz2pdf _controller;
    bool _isStarted = false;
};
