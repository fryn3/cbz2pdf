#pragma once

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

private slots:
    void on_btnSelectDir_clicked();

    void on_btnStart_clicked();

    void on_btnSelectFile_clicked();

    void on_pushButton_clicked();

private:
    void createPdf(QString dirOrZipName, QString pdfFullName = nullptr);
    void addPage(QPrinter &printer, QPainter &painter, QString fileName);
    Ui::MainWindow *ui;
    int _numPage = 0;
};
