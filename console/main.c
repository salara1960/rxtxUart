/*
    Автор : Ильминский А.Н.
    Дата : 04.03.2023 - 02.07.2023
    Утилита обеспечивает прием/передачу файла через интерфейс uart
*/


#include "func.h"
#include "uart.h"


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

int main (int argc, char *argv[])
{
const char *uName = "uartRXTX";
time_t tbegin = time(NULL);
char chap[BUF_SIZE_PRN];
int8_t i;
uint32_t crcFile = 0;
uint32_t fileSize = 0;
uint8_t *pTmp = NULL;
int res = 0;
hdr_file_t hdr_file = {0};


    setlocale(LC_ALL, "utf8");


    if (argc < 2) {
        Report(1, "[%s] Ver. %s %s | ./uartRXTX --help\n", __func__, uName, version);
        Report(0, "Параметры:\n\t--dev=<последовательный порт>"
                  " - %s, где X=0,1,...\n"
                  "\t--mode=<режим>, поддерживаются такие режимы:\n"
                  "\t\tcrc - подсчёт контрольной суммы файла - CRC32\n"
                  "\t\trx - приём данных и запись их в файл\n"
                  "\t\ttx - передача данных из файла\n"
                  "\t--file=<имя файла>\n"
                  "\t--speed=<скорость в порту> (9600..1500000, defaul - 115200)\n"
                  "\t--dbg=<уровень логирования> ('off', 'on', 'debug', default - on)\n", UART_DEV_NAME);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        strcpy(chap, argv[i]);
        parse_param_start(chap);
    }

    setSigSupport();
    /*  SET TIMER to 10ms
    if (mkTimer() != 0) {
        Report(1, "[%s] Error: Can't start timer. End of job.\n", __func__);
        return 1;
    }*/


    if (mode == noneCmd) {
        Report(1, "[%s] Error: invalid mode. End of job.\n", __func__);
        return 1;
    }

    if (mode != rxCmd)  {
        chap[0] = '\0';

        crcFile = calcCRCFile(fileName, &fileSize);
        
        hdr_file.crc = crcFile;
        hdr_file.size = fileSize;
        memcpy(hdr_file.name, fileName, MAX_LEN_FNAME);

        sprintf(chap, "CRC=0x%X", crcFile);
        if ((dbg > logOff) && strlen(chap)) Report(1, "[%s] Start %s ver.%s with speed %d in mode '%s' for file '%s'(%u bytes) -> %s\n",
                                                      __func__, uName, version, speedShow(SPEED), all_mode[mode], fileName, fileSize, chap);
        if (mode == crcCmd) return 0;
    } else {
        if (dbg > logOff) Report(1, "[%s] Start %s ver.%s with speed %d in mode '%s' for file '%s'\n",
                                    __func__, uName, version, speedShow(SPEED), all_mode[mode], fileName);
    }

    //-------------------------------------------------------------

    fd = init_uart(dev_name);
    if (fd > 0) {
        switch (mode) {
            case txCmd:
                res = txFileHdr(fd, &hdr_file);
                if (res == sizeof(hdr_file_t)) {
                    res = txFile(fd, fileName, fileSize);
                    if (res == -1) {
                        Report(1, "[%s] Error open file '%s' (%s)\n", __func__, fileName, strerror(errno));
                    } else if (res > 0) {
                        if (dbg >= logDebug) Report(1, "[%s] write to '%s' bytes %d/%d\n", __func__, dev_name, fileSize, res);
                    }
                } else {
                    if (dbg >= logDebug) Report(1, "[%s] Error send hdr to '%s' (%d)\n", __func__, dev_name, res);
                }
            break;
            case rxCmd:
                if (!strlen(fileName)) whoNow = hdrNow; else whoNow = blkNow;
                res = rxFile(fd, fileName);
                if (dbg == logDebug) Report(1, "[%s] read from '%s' bytes %d\n", __func__, dev_name, res);
            break;
        }
    } else {
        Report(1, "[%s] Error: Can't open serial device '%s' (%s - %u).\n", __func__, dev_name, strerror(errno), errno);
    }
    if (pTmp) free(pTmp);

    //-------------------------------------------------------------

    deinit_uart(fd);

    uint32_t sec = (uint32_t)(time(NULL) - tbegin);
    uint32_t min = min_from_sec(&sec);

    Report(1, "[%s] Stop %s for %s (min:%u sec:%u)\n", __func__, uName, dev_name, min, sec);

    return 0;
}
