#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <inttypes.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __WIN32
    #include <windows.h>
#else
    #include <arpa/inet.h>
    #include <endian.h>
    #include <termios.h>
    #include <unistd.h>
#endif

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QByteArray>
#include <QFile>
#include <QDateTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QCloseEvent>
#include <QFileDialog>
#include <QIODevice>
#include <QMutex>
#include <QProgressBar>


//********************************************************************************


#define SET_DEBUG
#define SET_MOUSE_KEY


#define BUF_SIZE         4096
#define MAX_PARAM        5
#define MAX_MODE         3
#define DEF_SPEED        B115200
#define PACK_MASK        0x80000000
#define keyCnt           3

#define MAX_ERR_STR      14
#define MAX_TRY          3

#define MAX_LEN_FNAME    24

// Маркосы-аналоги функций htons() htonl()
#define HTONS(x) \
    ((uint16_t)((x >> 8) | ((x << 8) & 0xff00)))
#define HTONL(x) \
    ((uint32_t)((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | ((x << 24) & 0xff000000)))

//********************************************************************************

enum {
    rxCmd = 0,
    txCmd,
    crcCmd,
    noneCmd
};

enum {
    hdrNow = 0,
    blkNow,
    noneNow
};

#pragma pack(push,1)
typedef struct {
    char name[MAX_LEN_FNAME];
    uint32_t size;
    uint32_t crc;
} hdr_file_t;
#pragma pack(pop)

//********************************************************************************



//********************************************************************************

namespace Ui {
class MainWindow;
}

class SettingsDialog;
//class pwdDialog;

//********************************************************************************
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    class TheError {
        public :
            int code;
            TheError(int);
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void timerEvent(QTimerEvent *event);
    int chkDone(QByteArray *buf);

public slots:

    void setLogs();
    void toLogFile(const QString &);
    int initSerial();
    uint32_t crc32(const uint32_t crc_origin, const uint8_t *buf, const uint32_t size);
    void deinitSerial();
    void LogSave(const char *, QByteArray &, bool);
    void LogSave(const char *, const QString &, bool);
    void About();
    unsigned char myhextobin(const char *);
    void hexTobin(const char *, QByteArray *);
    void clrLog();
    //
    uint32_t get10ms();
    uint32_t get_tmr(uint32_t);
    int check_tmr(uint32_t);
    void itProcInit(int);
    void itProcDone();
    int putFile(hdr_file_t *);

private slots:
    void ReadData();
    void slotError(QSerialPort::SerialPortError);
    void on_connect();
    void on_disconnect();
    void crcCalc();
    void enKey(bool);
    int getFile(uint8_t);
    void txFile();
    void rxFile();
    void slot_txDone();
    void slot_rxDone();
    void slot_Ready();
    void slot_timeOutAck();
    void slotWrite(QByteArray &);
    void slot_txFile();

signals:
    void sigWrite(QByteArray &);
    void sigAbout();
    void sigConn();
    void sigDisc();
    void sig_crcCalc();
    void sig_Ready();
    void sig_timeOutAck();
    void sig_answer_clicked();
    void sig_mkList(int8_t);
    void sig_txDone();
    void sig_rxDone();
    void sig_txFile();

#ifdef SET_MOUSE_KEY
    void sigRM(int, int);
#endif

private:

    //const QString vers = "0.4";//19.09.2023
    //const QString vers = "0.5";//20.09.2023
    //const QString vers = "0.6";//21.09.2023
    const QString vers = "0.7";//25.09.2023

    const QString title = "Serial rxtxUtil";

    const QString main_pic = "png/main.png";
    const QString nii_pic  = "png/niichavo.png";

    const QByteArray cr_lf = "\r\n";


    Ui::MainWindow *ui;
    int tmr_sec, MyError;
    QSerialPort *sdev;
    QByteArray rxData, txData;
    QString sdevName, sdevConf;
    bool first, con;

    //settings
    SettingsDialog *conf = nullptr;
    uint32_t Tik, ms10;

    qint64 fileSize;
    uint8_t *pTmp, *ukTmp;
    uint32_t crcFile, crcb = 0, crcf = 0;
    QByteArray *rTmp = nullptr;

    uint8_t mode = noneCmd;
    uint8_t devErr, cmd, whoNow = noneNow;
    uint32_t tmr_cmd, wait_ack;
    bool goCmd;
    char buff[BUF_SIZE + 4] = {0};
    QByteArray fileName, fmName;
    QString fName;
    const QString all_mode[MAX_MODE] = {"recv_file", "send_file", "crc_calc"};

    int8_t tx_done = 0, rx_done = 0, tx_err = 0;
    hdr_file_t hdr_file;
    uint32_t data_ack;
    uint32_t pack_len = 0, send_len = 0, recv_len = 0, seq_num = 0, last_len = 0;
    uint32_t blk_len = 0, data_len = 0, blk_cnt = 0, max_cnt = 0;
    int blk_idx = 0;

    bool logflag = false;
    QFile logFile;
    QString logFileName;
    QMutex logMutex;
    QProgressBar *itProc = nullptr;

};

#endif // MAINWINDOW_H
