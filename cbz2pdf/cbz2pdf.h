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

/*!
 * \brief The Cbz2pdf class переводит картинки в формат PDF.
 */
class CBZ2PDF_EXPORT Cbz2pdf : public QObject {
    Q_OBJECT
public:

    /*!
     * \brief The CodeError enum возвращается вместе с сигналом finished.
     */
    enum CodeError {
        NoError,        ///< Ошибок нет.
        ErrStoped,      ///< Процесс остановлен (устаревший).
        ErrFileIn,      ///< Ошибка с входными файлами.
        ErrFileOut,     ///< Ошибка с выходными файлами.

        CodeErrorCount
    };

    explicit Cbz2pdf(QObject *parent = nullptr);
    virtual ~Cbz2pdf();

public slots:

    /*!
     * \brief Устанавливает коэффициент для размера страницы.
     *
     * Не знаю почему, при вызове QPrinter::setPageSize необходимо передавать
     * размер QPixmap::size() * K. Причем если разрешение экрана fullHD, то
     * K = 0.75 (значение по умолчанию), если разрешение экрана 4K, то нужно
     * устанавливать значение K = 0.5.
     * \param k - коэффициент размера страницы.
     */
    void setK(double k);

    /*!
     * \brief Устанавливает путь для парсинга.
     * \param dirOrZipName - путь к архиву или к папке, где будет поиск
     * картинок.
     */
    void setDirOrZipName(QString dirOrZipName);

    /*!
     * \brief Запускает процесс создания PDF файлов.
     */
    void start();

    /*!
     * \brief Останавливает процесс.
     */
    void stop();

signals:

    /*!
     * \brief Испускается после вызова start().
     */
    void started();

    /*!
     * \brief Испускается при завершении создании файлов.
     * \param codeError - возвращает код ошибки enum CodeError.
     */
    void finished(int codeError);

    /*!
     * \brief Испускается после вызова stop().
     *
     * После этого сигнала не вызывается сигнал finished().
     */
    void stoped();

    /*!
     * \brief Процесс выполнения.
     * \param process - число в пределах от 0 до 1.
     */
    void process(double process);

    /*!
     * \brief Выводит сообщения с какими файлами работает.
     * \param msg - текст сообщения.
     */
    void logMsg(QString msg);
private:
    Cbz2pdfWorker *_worker;
    Cbz2pdfPrivate *_private;
    QThread _thread;
};
