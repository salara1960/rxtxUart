#include "mainwindow.h"
#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>
#include <QString>


int main(int argc, char *argv[])
{

    setlocale(LC_ALL,"UTF8");

    try {

        QLocale loc(QLocale::Russian, QLocale::RussianFederation);
        QLocale::setDefault(loc);

        QApplication app(argc, argv);

        QLockFile lockFile(QDir::temp().absoluteFilePath("hexSerial.lock"));
        if (!lockFile.tryLock(100)) {
            QMessageBox::warning(nullptr, "Attention !!!", "Program is already running");

            return 1;
        }

        MainWindow wnd;

        wnd.show();

        app.exec();
    }

    catch (MainWindow::TheError(er)) {
        int cerr = er.code;
        QString errStr = "";
        QString cerrStr = "";
        cerrStr.asprintf("%d", cerr);
        if (cerr > 0) {
            if (cerr & 1) errStr.append("Error create serial port object (" + cerrStr + ")\n");
            if (cerr & 2) errStr.append("Error starting timer_wait_data (" + cerrStr + ")\n");
            if (cerr & 4) errStr.append("Error create settings object\n");
            if (cerr & 8) errStr.append("Error reading from serial port\n");
        } else errStr.append("Unknown Error (" + cerrStr + ")\n");
        if (errStr.length() > 0) perror(reinterpret_cast<char *>(cerrStr.data()));

        return cerr;
    }
    catch (std::bad_alloc&) {
        perror("Error while alloc memory via call function new\n");

        return -1;
    }

    return 0;
}
