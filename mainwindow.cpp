#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"



//******************************************************************************************************
//
//       Transfer files via serial interface
//
//******************************************************************************************************



//  Макрос для промежуточного подсчета контрольной суммы CRC32
#define CRC32(crc, ch) ((crc >> 8) ^ crc32_tab[(crc ^ (ch)) & 0xff])

//-----------------------------------------------------------------------
// Таблица для подсчета контрольной суммы CRC32 (polynomial 0xedb88320)
//
//CRC-32-IEEE 802.3  (x^{32} + x^{26} + x^{23} + x^{22} + x^{16} + x^{12} + x^{11} + x^{10} + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1)
//
static const uint32_t crc32_tab[256] = {
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};



//******************************************************************************************************
//******************************************************************************************************
//******************************************************************************************************
//         Конструктор класса
//
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), conf(new SettingsDialog)
{
    ui->setupUi(this);

    MyError = 0;

    if (!conf) {
        MyError |= 4;//create settings object error
        throw TheError(MyError);
    }

    this->setWindowIcon(QIcon(main_pic));

    this->setWindowOpacity(0.90);//set the level of transparency

    first  = true;
    con = false;
    sdevName = "";
    sdev = nullptr;
    crcFile = 0;
    fileSize = 0;
    pTmp = nullptr;
    Tik = 0;
    ms10 = 0;
    txData.clear();
    rxData.clear();
    data_ack = 0;

    //-----   for cmds  -------

    fileName.clear();
    tmr_cmd = 0;
    goCmd = false;
    cmd = noneCmd;
    whoNow = noneNow;

    //-------------------------

    tmr_sec = startTimer(10);// 10 msec.
    if (tmr_sec <= 0) {
        MyError |= 2;//start_timer error
        throw TheError(MyError);
    }

    connect(this, SIGNAL(sigWrite(QByteArray&)), this, SLOT(slotWrite(QByteArray&)));

    connect(ui->actionVERSION,    &QAction::triggered, this, &MainWindow::About);
    connect(ui->actionPORT,       &QAction::triggered, conf, &SettingsDialog::show);
    connect(ui->actionCONNECT,    &QAction::triggered, this, &MainWindow::on_connect);
    connect(ui->actionDISCONNECT, &QAction::triggered, this, &MainWindow::on_disconnect);
    connect(ui->actionCLEAR,      &QAction::triggered, this, &MainWindow::clrLog);    

    connect(ui->actionSendFile, &QAction::triggered, this, &MainWindow::txFile);
    connect(ui->actionRecvFile, &QAction::triggered, this, &MainWindow::rxFile);
    connect(ui->actionCRC32,    &QAction::triggered, this, &MainWindow::crcCalc);

    ui->actionVERSION->setToolTip("About " + title);
    ui->actionPORT->setToolTip("Configure Serial Port");
    ui->actionCLEAR->setToolTip("Clear Log window");
    ui->actionSendFile->setToolTip("Send File");
    ui->actionRecvFile->setToolTip("Recv File");
    ui->actionCRC32->setToolTip("Calc CRC32");

    enKey(false);

    ui->actionCONNECT->setEnabled(true);
    ui->actionPORT->setEnabled(true);
    ui->actionDISCONNECT->setEnabled(false);
    ui->actionCLEAR->setEnabled(false);
    ui->status->clear();
    ui->log->setReadOnly(true);

    connect(this, &MainWindow::sigConn, this, &MainWindow::on_connect);
    connect(this, &MainWindow::sigDisc, this, &MainWindow::on_disconnect);
    connect(this, &MainWindow::sig_crcCalc, this, &MainWindow::crcCalc);
    connect(this, &MainWindow::sig_Ready, this, &MainWindow::slot_Ready);
    connect(this, &MainWindow::sig_timeOutAck, this, &MainWindow::slot_timeOutAck);

    connect(this, &MainWindow::sig_txDone, this, &MainWindow::slot_txDone);
    connect(this, &MainWindow::sig_rxDone, this, &MainWindow::slot_rxDone);

    logFileName = "logs.txt";

    setLogs();
}
//-------------------------------------------------------------------------------
//      Деструктор класса
//
MainWindow::~MainWindow()
{
    itProcDone();

    deinitSerial();

    if (conf) delete conf;
    if (pTmp) free(pTmp);
    if (rTmp) delete rTmp;

    if (logFile.isOpen()) logFile.close();

    killTimer(tmr_sec);
    delete ui;
}
//--------------------------------------------------------------------------------
//        Функция инициализации файла протокола (логов)
//
void MainWindow::setLogs()
{
bool lg = logflag;

    logMutex.lock();
    if (!lg) {//запись логов выключена - надо включить (открыть файл)
        if (logFile.isOpen()) {
            logFile.close();
        } else {
            logFile.setFileName(logFileName);
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
                 lg = true;
            }
        }
    } else {
        if (logFile.isOpen()) logFile.close();
        lg = false;
    }
    logMutex.unlock();

    logflag = lg;
}
//--------------------------------------------------------------------------------
//       Функция записи в файл логов текстовой строки
//
void MainWindow::toLogFile(const QString & st)
{
    if (logflag) {
        time_t ict = QDateTime::currentDateTimeUtc().toTime_t();
        struct tm *ct = localtime(&ict);
        QString fw = QString::asprintf("%02d.%02d %02d:%02d:%02d | ",
                    ct->tm_mday, ct->tm_mon+1, ct->tm_hour, ct->tm_min, ct->tm_sec);
        //
        fw.append(st);
        QByteArray data(fw.toUtf8());
        if (data.at(data.length() - 1) != 0x0a) data.append(0x0a);
        logMutex.lock();
            logFile.write(data);
            logFile.flush();
        logMutex.unlock();
    }
}
//--------------------------------------------------------------------------------

MainWindow::TheError::TheError(int err) { code = err; }//error class descriptor

//--------------------------------------------------------------------------------
//      Функция очищает область окна для вывода данных
//
void MainWindow::clrLog()
{
    ui->log->clear();
}
//--------------------------------------------------------------------------------
//   Функция возвращает контрольную сумму CRC32 массива байт
//
uint32_t MainWindow::crc32(const uint32_t crc_origin, const uint8_t *buf, const uint32_t size)
{
uint32_t crc = ~crc_origin;
const uint8_t *p = buf;

    if (!buf || !size) return ~crc;

    for (uint32_t i = 0; i < size; i++) crc = CRC32(crc, *p++);

    return ~crc;

}
//--------------------------------------------------------------------------------
//    Функция выполняет инициализацию последовательного порта
//
int MainWindow::initSerial()
{
    //deinitSerial();

    sdev = new QSerialPort(sdevName);
    if (sdev) {
        SettingsDialog::Settings p = conf->settings();
        sdevName = p.name;   sdev->setPortName(sdevName);
        sdev->setBaudRate(p.baudRate);
        sdev->setDataBits(p.dataBits);
        sdev->setParity(p.parity);
        sdev->setFlowControl(p.flowControl);
        sdev->setStopBits(p.stopBits);

        if (!sdev->open(QIODevice::ReadWrite)) {
            delete sdev;
            sdev = nullptr;
            return 1;
        } else {
            rxData.clear();
            txData.clear();
            while (!sdev->atEnd()) rxData = sdev->readAll();
            rxData.clear();
            txData.clear();

            connect(sdev, &QSerialPort::readyRead, this, &MainWindow::ReadData);
            connect(sdev, &QSerialPort::errorOccurred, this, &MainWindow::slotError);

            return 0;
        }
    } else {
        MyError |= 1;//create serial_port_object error
        throw TheError(MyError);
    }

}
//--------------------------------------------------------------------------------
//    Функция выполняет закрытие последовательного порта
//
void MainWindow::deinitSerial()
{
    if (sdev) {
        if (sdev->isOpen()) sdev->close();
        sdev->disconnect();
        delete sdev;
        sdev = nullptr;
        rxData.clear();
        txData.clear();
    }
}
//--------------------------------------------------------------------------------
//     Функция формирует окно с данными о версии приложения и библиотеки Qt
//
void MainWindow::About()
{
    QString st ="\nTransfer files util\nversion " + vers + "\nused : Qt v.";
    st.append(QT_VERSION_STR);

    QMessageBox box;
    box.setStyleSheet("background-color: rgb(208, 208, 208);");
    box.setIconPixmap(QPixmap(nii_pic));
    box.setText(st);
    box.setWindowTitle("About");
    box.exec();
}
//-----------------------------------------------------------------------
//   Функция вывода данных в область окна приложения
//
void MainWindow::LogSave(const char *func, QByteArray & st, bool pr)
{
    QString fw;
    if (pr) {
        time_t ict = QDateTime::currentDateTimeUtc().toTime_t();
        struct tm *ct = localtime(&ict);
        fw = QString::asprintf("%02d.%02d %02d:%02d:%02d | ",
                ct->tm_mday, ct->tm_mon+1, ct->tm_hour, ct->tm_min, ct->tm_sec);
    }
    if (func) {
        fw.append("[");
        fw.append(func);
        fw.append("] ");
    }
    fw.append(st);

    ui->log->append(fw);//to log screen

    fw.clear();
    fw.append(st);
    toLogFile(fw);
}
//-----------------------------------------------------------------------
//   Функция вывода данных в область окна приложения
//
void MainWindow::LogSave(const char *func, const QString & st, bool pr)
{
    QString fw;
    if (pr) {
        time_t ict = QDateTime::currentDateTimeUtc().toTime_t();
        struct tm *ct = localtime(&ict);
        fw = QString::asprintf("%02d.%02d %02d:%02d:%02d | ",
                ct->tm_mday, ct->tm_mon+1, ct->tm_hour, ct->tm_min, ct->tm_sec);
    }
    if (func) {
        fw.append("[");
        fw.append(func);
        fw.append("] ");
    }
    fw.append(st);

    ui->log->append(fw);//to log screen

    toLogFile(st);
}
//-----------------------------------------------------------------------
//     Функция-слот вызывается для подключения к последовательному порту
//     с выбранными параметрами порта
//
void MainWindow::on_connect()
{
    if (con) return;


    if (!initSerial()) {
        ui->status->clear();
        ui->status->setText("Connected to " + sdevName +
                            ": " + conf->settings().stringBaudRate +
                            " " + conf->settings().stringDataBits +
                            conf->settings().stringParity.at(0) +
                            conf->settings().stringStopBits +
                            " FlowControl " + conf->settings().stringFlowControl);
        con = true;
        ui->actionCONNECT->setEnabled(false);
        ui->actionPORT->setEnabled(false);
        ui->actionDISCONNECT->setEnabled(true);
        ui->actionCLEAR->setEnabled(true);

        enKey(true);
        ui->log->setEnabled(true);
    } else {
        ui->status->clear();
        ui->status->setText("Serial port " + sdevName + " open ERROR");

        deinitSerial();
    }
}
//-----------------------------------------------------------------------
//   Функция-слот вызывается для отключения от последовательного порта
//
void MainWindow::on_disconnect()
{
    if (!con) return;

    deinitSerial();

    ui->status->clear();
    ui->status->setText("Disconnect from serial port " + sdevName);

    ui->actionPORT->setEnabled(true);

    con = false;
    ui->actionCONNECT->setEnabled(true);
    ui->actionDISCONNECT->setEnabled(false);
    ui->actionCLEAR->setEnabled(false);

    ui->log->setEnabled(false);
    enKey(false);
}
//-----------------------------------------------------------------------
//   Функция выполняет запись данных в последовательный порт
//
void MainWindow::slotWrite(QByteArray & mas)
{
    if (!sdev) return;

    if (sdev->isOpen()) {
        QString m(mas);
        sdev->write(m.toLocal8Bit());
        LogSave(nullptr, mas, false);
    }
}
//------------------------------------------------------------------------------------
unsigned char MainWindow::myhextobin(const char *uk)
{
char line[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
unsigned char a = 0, b = 0, c = 0, i;


    for	(i = 0; i < 16; i++) { if (*uk == line[i]) { b = i; break; } else b = 0xff; }
    for	(i = 0; i < 16; i++) { if (*(uk + 1) == line[i]) { c = i; break; } else c = 0xff; }
    if ((b == 0xff) || (c == 0xff)) a = 0xff; else { b <<= 4;   a = b | c; }

    return a;
}
//-----------------------------------------------------------------------
void MainWindow::hexTobin(const char *in, QByteArray *out)
{
size_t i = 0, len = strlen(in) / 2;
const char *uk = in;

    while (i < len) {
        out->append( static_cast<char>(myhextobin(uk)) );
        i++; uk += 2;
    }
}
//-----------------------------------------------------------------------
//   Обработчик собитий от таймера (интервал 10 мсек)
//
void MainWindow::timerEvent(QTimerEvent *event)
{ 
    if (tmr_sec == event->timerId()) {
        ms10++;
        if (!(ms10 % 100)) {
            Tik++;
            time_t it_ct = QDateTime::currentDateTimeUtc().toTime_t();
            struct tm *ctimka = localtime(&it_ct);
            QString dt = QString::asprintf("%02d.%02d.%04d %02d:%02d:%02d",
                    ctimka->tm_mday,
                    ctimka->tm_mon + 1,
                    ctimka->tm_year + 1900,
                    ctimka->tm_hour,
                    ctimka->tm_min,
                    ctimka->tm_sec);
            setWindowTitle(title +" ver. " + vers + "  |  " + dt);
        }
        if (wait_ack) {
            if (check_tmr(wait_ack)) {
                wait_ack = 0;
                goCmd = false;
                emit sig_timeOutAck();
            }
        }
    }
}
//--------------------------------------------------------------------------------
//     Обработчик события "ошибка" на последовательном порту
//
void MainWindow::slotError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        MyError |= 8;//error reading from serial port
        throw TheError(MyError);
    }
}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//  Функции для установки временных интервалов , а также их проверки
//
uint32_t MainWindow::get10ms()
{
    return ms10;
}
//
uint32_t MainWindow::get_tmr(uint32_t tm)
{
    return (get10ms() + tm);
}
//
int MainWindow::check_tmr(uint32_t tm)
{
    return (get10ms() >= tm ? 1 : 0);
}
//**************************************************************************************
//**************************************************************************************
//**************************************************************************************
//   Обработчик события "нет ответа" по последовательному порту на пакет данных
//
void MainWindow::slot_timeOutAck()
{
    ui->status->clear();
    QString st;
    if (cmd == txCmd) {
        QString name(hdr_file.name);
        st = all_mode[mode] + " '" + name + "' : Wait answer timeout !";
    } else {
        st = all_mode[mode] + " : Wait answer timeout !";
    }
    ui->status->setText(st);
    LogSave(nullptr, st, true);
    //
    itProcDone();
    //
    enKey(true);
    //
    cmd = noneCmd;
    goCmd = false;
    QApplication::beep();
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//   Функция обеспечивает подсчет контрольной суммы выбранного файла
//
void MainWindow::crcCalc()
{
    mode = cmd = crcCmd;
    ui->status->clear();
    if (getFile(cmd)) {
        QMessageBox::critical(this, "calc CRC32", "Error select file");
    }
}
//--------------------------------------------------------------------------------
void MainWindow::enKey(bool en)
{
    ui->actionSendFile->setEnabled(en);
    ui->actionRecvFile->setEnabled(en);
}
//--------------------------------------------------------------------------------
//   Функция обеспечивает начало передачи файла по последовательному порту -
//   выбор файла, подсчет его контрольной суммы, пересчет рабочих параметров,
//   запись структуры заголовка в последовательный порт
//
void MainWindow::txFile()
{
    if (!sdev) return;

    mode = cmd = txCmd;
    whoNow = noneNow;
    ui->status->clear();
    enKey(false);
    if (getFile(mode)) {
        QMessageBox::information(this, "tx File", "Error select file");
        enKey(true);
        return;
    }

    rxData.clear();
    txData.clear();

    memset(hdr_file.name, 0, sizeof(hdr_file_t));
    size_t sz = fmName.length();
    if (sz > MAX_LEN_FNAME) sz = MAX_LEN_FNAME;
    memcpy(hdr_file.name, fmName.data(), sz);
    hdr_file.size = fileSize;
    hdr_file.crc = crcFile;
    rxData.clear();
    if (sdev->isOpen()) {
        if (sdev->write((const char *)&hdr_file, sizeof(hdr_file_t)) == sizeof(hdr_file_t)) {
            send_len = seq_num = 0;
            seq_num = 0;
            blk_idx = -1;
            last_len = BUF_SIZE;
            blk_cnt = hdr_file.size / BUF_SIZE;
            if (hdr_file.size % BUF_SIZE) {
                blk_cnt++;
                last_len = hdr_file.size % BUF_SIZE;
            }
            max_cnt = blk_cnt;
            tx_done = 0;
            goCmd = true;
            wait_ack = get_tmr(300);//300 * 10 = 3000 msec = 3 sec
            QString nam(hdr_file.name);
            LogSave(nullptr,
                    "Send header [name:" + nam +
                    ",  size:" + QString::number(hdr_file.size, 10) +
                    ",  crc32:0x" + QString::number(hdr_file.crc, 16).toUpper() + "]",
                    true);
            whoNow = hdrNow;
            ui->actionSendFile->setEnabled(false);
        }
    }
}
//--------------------------------------------------------------------------------
//  Функция выполняет выбор файла, чтение его в буфер и подсчет контрольной суммы
//
int MainWindow::getFile(uint8_t md)
{
int ret = 1;

    QString *nm = new QString();
    ui->status->clear();
    *nm = QFileDialog::getOpenFileName(this, tr("Open file"), nullptr, tr("Files (*)"));
    if (!nm->size()) {
        ui->status->clear();
        ui->status->setText("No file selected");
        delete nm;
        return ret;
    }

    QFile fil(*nm);
    QString stf = fil.fileName();
    QStringList parts = stf.split("/");
    fName = parts.at(parts.size() - 1);
    fmName.clear();
    fmName.append(fName.toLocal8Bit());
    fileSize = 0;
    bool flag = fil.open(QIODevice::ReadOnly);
    if (flag) {
        fileSize = fil.size();
        if (fileSize > 0) {
            if (pTmp) {
                free(pTmp);
                pTmp = nullptr;
            }
            pTmp = (uint8_t *)calloc(1, fileSize);
            qint64 dl = fil.read((char *)pTmp, fileSize);
            if (dl != fileSize) {
                ui->status->setText("Error reading file " + *nm);
            } else {
                fileName.clear();
                fileName = nm->toUtf8();
                crcFile = crc32(0, pTmp, fileSize);
                if (md == crcCmd) {
                    QString stx = "File : " + *nm + cr_lf +
                              "Size: " + QString::number(fileSize, 10) + " bytes" + cr_lf +
                              "crc32: 0x" + QString::number(crcFile, 16).toUpper() + cr_lf;
                    QMessageBox::information(this, "calc crc32", stx);
                }
                ui->status->setText("File : " + *nm + " (" + QString::number(fileSize, 10) + " bytes) crc32 : 0x" + QString::number(crcFile, 16).toUpper());
                ret = 0;
            }
        } else {
            ui->status->setText("File " + *nm + " is empty !");
        }
        fil.close();
    } else {
        ui->status->setText("Error open file " + *nm);
    }
    delete nm;
    nm = nullptr;

    return ret;
}
//-----------------------------------------------------------------------
//     Функция выполняет предварительный анализ принятого ответа
//
int MainWindow::chkDone(QByteArray *buf)
{
int ret = -1;

    switch (cmd) {
        case txCmd:
            if (buf->length() == sizeof(uint32_t)) {
                char *uk = buf->data();
                data_ack = *(uint32_t *)uk;
                ret = sizeof(uint32_t);
            }
        break;
        case rxCmd:
            switch (whoNow) {
                case hdrNow:
                    if (buf->length() == sizeof(hdr_file_t)) {
                        ret = data_ack = sizeof(hdr_file_t);
                    }
                break;
                case blkNow:
                    if (!pack_len) {
                        if (buf->length() >= (int)sizeof(uint32_t)) {
                            char *uk = buf->data();
                            pack_len = *(uint32_t *)uk;
                            if (pack_len & PACK_MASK) {
                                pack_len &= ~PACK_MASK;
                                rx_done = 1;
                            }
                            //
                            if (buf->length() > (int)sizeof(uint32_t)) {
                                QByteArray mas = buf->mid(sizeof(uint32_t));
                                buf->clear();
                                *buf = mas;//rxData = mas;
                            } else {
                                buf->clear();
                            }
                        }
                    } else {
                        if (buf->length() == (int)pack_len) {
                            ret = data_len = pack_len;// + sizeof(uint32_t);
                        }
                    }
                break;
            }
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------
//     Функция выполняет прием и анализ принятых данных
//
void MainWindow::ReadData()
{
int ix = -1;

    while (!sdev->atEnd()) {
        rxData += sdev->readAll();
        if ((ix = chkDone(&rxData)) != -1) break;
    }
    if (ix != -1) {
        wait_ack = 0;
        switch (cmd) {
            case txCmd:
            {
                bool next = true;
                QString st;
                if (whoNow == hdrNow) {
                    st = QString::asprintf("ack: %u", data_ack);
                    st.append(", dump: ");
                    st.append(rxData.toHex());
                    st.append(", blocks: " + QString::number(blk_cnt, 10));
                    toLogFile(st);
                    whoNow = blkNow;
                    crcf = 0;
                    tx_err = 0;
                    //
                    itProcInit(max_cnt);
                    //
                } else {
                    //
                    if (data_ack != crcb) {
                        tx_err = 1;
                        st = " Error ack for pack " + QString::number(seq_num, 10) + " | ACK != CRC : 0x" + QString::number(data_ack, 16).toUpper() + " != 0x" + QString::number(crcb, 16).toUpper();
                        toLogFile(st);
                    }
                    //
                    if (itProc) emit sig_txFile();

                    if (send_len == hdr_file.size) {
                        next = false;
                        enKey(true);
                        emit sig_txDone();
                    }
                }
                //
                if (next) emit sig_Ready();
                //
                rxData.clear();
            }
            break;
            case rxCmd:
                if (whoNow == hdrNow) {
                    QString st = "Get header [";
                    if (data_ack == sizeof(hdr_file_t)) {
                        memcpy((uint8_t *)&hdr_file, rxData.data(), data_ack);
                        if (sdev->write((const char *)&data_ack, sizeof(uint32_t)) == sizeof(uint32_t)) {//send ack
                            QString nm(hdr_file.name);
                            st.append("name:" + nm +
                                      ", size:" + QString::number(hdr_file.size, 10) +
                                      ", crc32:0x" + QString::number(hdr_file.crc, 16).toUpper() + "]");
                            max_cnt = hdr_file.size / BUF_SIZE;
                            if (hdr_file.size % BUF_SIZE) max_cnt++;
                            blk_cnt = max_cnt;
                            ui->status->setText("Recv : " + nm +
                                                " (" + QString::number(hdr_file.size, 10) +
                                                " bytes) crc32 : 0x" + QString::number(hdr_file.crc, 16).toUpper());
                        }
                    }
                    LogSave(nullptr, st, true);
                    if (rTmp) { delete rTmp; rTmp = nullptr; }
                    rTmp = new QByteArray();
                    pack_len = seq_num = 0;
                    recv_len = 0;
                    whoNow = blkNow;
                    rx_done = 0;
                    //
                    itProcInit(max_cnt);
                    //
                    wait_ack = get_tmr(300);//3 sec
                } else {
                    if (pack_len) {
                        //
                        //
                        //
                        if (blk_cnt) blk_cnt--;
                        if (itProc) emit sig_txFile();
                        //
                        int dl = rxData.length() - (int)sizeof(uint32_t);
                        crcb = crc32(0, (const uint8_t *)rxData.data(), dl);
                        sdev->write((const char *)&crcb/*pack_len*/, sizeof(uint32_t));
                        //data_len -= sizeof(uint32_t);
                        recv_len += data_len;
                        seq_num++;
                        if (rTmp) rTmp->append(rxData, dl);
                        //
                        QString st = "Recv pack:" + QString::number(seq_num, 10) +
                             ", pack_len:0x" + QString::number(pack_len, 16) +
                             ", data_len:" + QString::number(data_len, 10) +
                             ", recv_len:" + QString::number(recv_len, 10) +
                             ", crc32:0x" + QString::number(crcb, 16).toUpper();
                        ui->status->setText(st);
                        toLogFile(st);
                        //
                        if (rx_done) {
                            enKey(true);
                            emit sig_rxDone();
                        } else {
                            pack_len = 0;
                            wait_ack = get_tmr(300);//3 sec
                        }
                    }
                }
                rxData.clear();
            break;
        }
    }
}
//-----------------------------------------------------------------------
// Функция выполняет передачу очередного пакета данных (не более BUF_SIZE)
//        по последовательному порту
//
void MainWindow::slot_Ready()
{
    wait_ack = 0;

    if (send_len < hdr_file.size) {
        int8_t er = 1;

        if (hdr_file.size < BUF_SIZE) {
            blk_len = hdr_file.size;
            tx_done = 1;
        } else {
            blk_len = BUF_SIZE;
            if (blk_cnt == 1) {
                blk_len = last_len;
                tx_done = 1;
            }
        }
        //
        blk_idx++;
        data_len = blk_len;
        uint32_t len = blk_len + sizeof(uint32_t);
        uint32_t dl = len;
        if (tx_done) len |= PACK_MASK;
        //
        ukTmp = pTmp + (blk_idx * data_len);
        //
        if (sdev->write((const char *)&len, sizeof(uint32_t)) == sizeof(uint32_t)) {//send block_le
            memcpy(buff, ukTmp, data_len);
            crcb = crc32(0, (const uint8_t *)buff, data_len);
            //crcf = crc32(crcf, (const uint8_t *)buff, data_len);
            memcpy(buff + data_len, (uint8_t *)&crcb, sizeof(uint32_t));
            if (sdev->write((const char *)buff, dl) == dl) {
                er = 0;
                seq_num++;
                send_len += data_len;
                blk_cnt--;
            }
            wait_ack = get_tmr(300);//3 sec
        }

        QString st;
        if (er) {
            st = "Error writing blk_idx:" + QString::number(blk_idx, 10);
        } else {
            st = "Send pack:" + QString::number(seq_num, 10) +
                 ", len:0x" + QString::number(len, 16) +
                 ", data_len:" + QString::number(data_len, 10) +
                 ", send_len:" + QString::number(send_len, 10) +
                 ", crc32:0x" + QString::number(crcb, 16).toUpper();
        }
        ui->status->setText(st);
        toLogFile(st);
    }
}
//--------------------------------------------------------------------------------
//          Функция вызывается по завершению передачи файла по
//                  последовательному порту
//
void MainWindow::slot_txDone()
{
    itProcDone();

    QString nm(hdr_file.name);
    QString st = "Transfer file '" + nm +
                "', size:" + QString::number(hdr_file.size, 10) +
                ", packs:" + QString::number(seq_num, 10) +
                ", crc32:0x" + QString::number(hdr_file.crc, 16).toUpper() + " done.";
    ui->status->setText(st);
    LogSave(nullptr, st, true);
    st = "Transfer file '" + nm +
         "'\nsize:" + QString::number(hdr_file.size, 10) +
         "\npacks:" + QString::number(seq_num, 10) +
         "\ncrc32:0x" + QString::number(hdr_file.crc, 16).toUpper() + "\n";
    QMessageBox::information(this, "Send file done", st);
}
//--------------------------------------------------------------------------------
//     Функция выполняет инициализацию объекта "QProgressBar" для индикации
//         процесса передачи файла по последовательному порту
//
void MainWindow::itProcInit(int max)
{
    itProcDone();

    itProc = new QProgressBar(this);
    if (itProc) {
        QRect rr = this->geometry();
        int w = 256, h = 36;
        itProc->setRange(0, max);
        itProc->setValue(0);
        itProc->resize(w, h);
        itProc->move(rr.width()/2 - w/2, rr.height()/2 - h/2);
        if (cmd == txCmd)
            itProc->setStyleSheet("*::chunk {background-color: rgba(0, 0, 255, 255); border-radius: 4;}");
        else
            itProc->setStyleSheet("*::chunk {background-color: rgba(0, 255, 0, 255); border-radius: 4;}");
        itProc->setAlignment(Qt::AlignCenter);
        connect(this, &MainWindow::sig_txFile, this, &MainWindow::slot_txFile);
        itProc->show();
    }
}
//--------------------------------------------------------------------------------
//        Функция корректирует текущий процент передаваемого файла
//
void MainWindow::slot_txFile()
{
    if (itProc) itProc->setValue(max_cnt - blk_cnt);
}
//--------------------------------------------------------------------------------
//         Функция выполняет "уничтожение" объекта "QProgressBar"
//
void MainWindow::itProcDone()
{
    if (itProc) {
        delete itProc;
        itProc = nullptr;
    }
}
//--------------------------------------------------------------------------------
//   Функция обеспечивает начало приема файла по последовательному порту
//
void MainWindow::rxFile()
{
    if (!sdev) return;

    rxData.clear();
    ui->status->clear();
    enKey(false);
    mode = cmd = rxCmd;
    whoNow = hdrNow;
    ui->status->setText("Wait file....");
}
//--------------------------------------------------------------------------------
void MainWindow::slot_rxDone()
{
    itProcDone();

    if (rTmp) {
        QString nm(hdr_file.name);

        QString st = "Received file '" + nm +
                "', size:" + QString::number(hdr_file.size, 10) +
                ", packs:" + QString::number(seq_num, 10) +
                ", crc32:0x" + QString::number(hdr_file.crc, 16).toUpper() + " done.";
        ui->status->setText(st);
        LogSave(nullptr, st, true);

        QString tit;
        uint32_t crc = crc32(0, (const uint8_t *)rTmp->data(), rTmp->length());
        if (crc == hdr_file.crc) {
            int svf = putFile(&hdr_file);
            if (!svf) {
                tit = "Received and save file successfully.";
            } else {
                tit = "Received file successfully but save error.";
            }
        } else {
            tit = "Receive file error !!!";
        }
        st = "Received file '" + nm +
             "'\nsize:" + QString::number(hdr_file.size, 10) +
             "\npacks:" + QString::number(seq_num, 10) +
             "\ncrc32:0x" + QString::number(hdr_file.crc, 16).toUpper() + "\n";
        QMessageBox::information(this, tit, st);

        delete rTmp;
        rTmp = nullptr;
    }
}
//--------------------------------------------------------------------------------
int MainWindow::putFile(hdr_file_t *hdr)
{
int ret = -1;

    if (rTmp) {
        QString nm(hdr->name);
        QFile fil(nm);
        fileSize = hdr->size;
        if (fileSize) {
            if (fil.open(QIODevice::WriteOnly)) {
                if (fil.write((char *)rTmp->data(), fileSize) == fileSize) ret = 0;
                fil.close();
            }
        }
    }

    return ret;
}
//--------------------------------------------------------------------------------



