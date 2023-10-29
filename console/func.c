#include "func.h"

//-----------------------------------------------------------------------

//const char *version = "0.1";//04.03.2023
//const char *version = "0.2";//06.03.2023
//const char *version = "0.3";//10.03.2023
//const char *version = "0.4";//11.03.2023
//const char *version = "0.5";//16.03.2023
//const char *version = "0.6";//02.07.2023
const char *version = "0.7";//15.09.2023




uint8_t QuitAll  = 0;
uint8_t devError = 0;
char dev_name[SDEF] = {0};

//static uint32_t varta = 0;

uint8_t SIGHUPs  = 1;
uint8_t SIGTERMs = 1;
uint8_t SIGINTs  = 1;
uint8_t SIGKILLs = 1;
uint8_t SIGSEGVs = 1;
uint8_t SIGABRTs = 1;
uint8_t SIGSYSs  = 1;
uint8_t SIGTRAPs = 1;

char prnBuff[BUF_SIZE_PRN];
char *buf_prn = NULL;

#ifdef SET_PRN_MUTEX
    pthread_mutex_t prn_buf_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif


//----------------------------------------------------------------------
/*
char *errNames(uint8_t e, char *st)
{
    if (!e) return strcpy(st, "devOk");

    *st = '\0';
    for (int8_t i = 1; i < MAX_ERROR; i++) {
        if (e & code_err[i]) sprintf(st+strlen(st), " %s", str_err[i]);
    }

    return st;
};
//-----------------------------------------------------------------------
void hex_dump(const void *src, size_t length, size_t line_size, char *prefix)
{
int i = 0;
const uint8_t *address = src;
const uint8_t *line = address;
uint8_t c;

    printf("%s | ", prefix);
    while (length-- > 0) {
        printf("%02X ", *address++);
        if (!(++i % line_size) || (length == 0 && i % line_size)) {
            if (length == 0) {
                while (i++ % line_size) printf("__ ");
            }
            printf(" |");
            while (line < address) {
                c = *line++;
                printf("%c", (c < 32 || c > 126) ? '.' : c);
            }
            printf("|\n");
            if (length > 0) printf("%s | ", prefix);
        }
        printf("\n");
    }
}
//------------------------------------------------------------------------------------------
void hex_show(const uint8_t *src, size_t length, size_t line_size, char *prefix)
{
uint32_t ind = 0;
char st[256] = {0};

    while (ind < length) {
        st[0] = '\0';
        for (int i = 0; i < line_size; i++) {
            sprintf(st+strlen(st), "%02X ", src[i + ind]);
            if ((i + ind) >= length - 1) break;
        }
        strcat(st, "\n");
        ind += line_size;
        Report(0, "%s", st);
    }
}
//----------------------------------------------------------------------

#ifdef SET_OLED_I2C

//----------------------------------------------------------------------
//   Функция формирует символьную строку с текущими значениями даты и времени
//
char *Tstamp(char *ts)
{
struct timeval tvl;

    gettimeofday(&tvl, NULL);
    struct tm *ctimka = localtime(&tvl.tv_sec);
    sprintf(ts, "%02d.%02d %02d:%02d:%02d",
                ctimka->tm_mday,
                ctimka->tm_mon + 1,
                ctimka->tm_hour,
                ctimka->tm_min,
                ctimka->tm_sec);

    return ts;
}
//----------------------------------------------------------------------

#endif
*/
//-----------------------------------------------------------------------
//  Функция устанавливает периодический таймер (период - 1 миллисекунда)
//  Функция возвращает ноль в успешном случае, или -1 в противном случае (чтение errno детализирует ошибку)
/*
int mkTimer()
{
    static struct itimerval itmr;

    //  SET TIMER to 1ms
    itmr.it_value.tv_sec     = 0;
    itmr.it_value.tv_usec    = 1000;
    itmr.it_interval.tv_sec  = 0;
    itmr.it_interval.tv_usec = 1000;

    return (setitimer(ITIMER_REAL, &itmr, NULL));
}
*/
//--------------------  function for recive SIGNAL from system -----------------------
// Функция обрабатывает сигналы, полученные от системы в ходе выполнения утилиты
//
void GetSignal_(int sig)
{
uint8_t out = 0;
char stx[64] = {0};

    switch (sig) {
        /*case SIGALRM://получет сигнал от Таймера - истек период в 10 миллисекунд
            varta++;
            return;*/
        case SIGHUP:
            strcpy(stx, "\tSignal SIGHUP\n");
        break;
        case SIGKILL:
            if (SIGKILLs) {
                SIGKILLs = 0;
                strcpy(stx, "\tSignal SIGKILL\n");
                out = 1;
            }
        break;
        case SIGPIPE:
            strcpy(stx, "\tSignal SIGPIPE\n");
        break;
        case SIGTERM:
            if (SIGTERMs) {
                SIGTERMs = 0;
                strcpy(stx, "\tSignal SIGTERM\n");
                out = 1;
            }
        break;
        case SIGINT:
            if (SIGINTs) {
                SIGINTs = 0;
                strcpy(stx, "\tSignal SIGINT\n");
                out = 1;
            }
        break;
        case SIGSEGV:
            if (SIGSEGVs) {
                SIGSEGVs = 0;
                strcpy(stx, "\tSignal SIGSEGV\n");
                out = 1;
            }
        break;
        case SIGABRT:
            if (SIGABRTs) {
                SIGABRTs = 0;
                strcpy(stx, "\tSignal SIGABRT\n");
                out = 1;
            }
        break;
        case SIGSYS:
            if (SIGSYSs) {
                SIGSYSs = 0;
                strcpy(stx, "\tSignal SIGSYS\n");
                out = 1;
            }
        break;
        case SIGTRAP:
            if (SIGTRAPs) {
                SIGTRAPs = 0;
                strcpy(stx, "\tSignal SIGTRAP\n");
                out = 1;
            }
        break;
            default : sprintf(stx, "\tUNKNOWN signal %d", sig);
    }

    if (strlen(stx)) printf("%s", stx);

    if (out) QuitAll = 1;
}
//-----------------------------------------------------------------------
//  Функция устанавливает обработчик (GetSignal_) некоторых сигналов ОС Linux
//
void setSigSupport()
{
static struct sigaction Act, OldAct;

    memset((uint8_t *)&Act,    0, sizeof(struct sigaction));
    memset((uint8_t *)&OldAct, 0, sizeof(struct sigaction));
    Act.sa_handler = &GetSignal_;
    Act.sa_flags   = 0;
    sigaction(SIGPIPE, &Act, &OldAct);
    sigaction(SIGHUP,  &Act, &OldAct);
    sigaction(SIGSEGV, &Act, &OldAct);
    sigaction(SIGTERM, &Act, &OldAct);
    sigaction(SIGABRT, &Act, &OldAct);
    sigaction(SIGINT,  &Act, &OldAct);
    sigaction(SIGSYS,  &Act, &OldAct);
    sigaction(SIGKILL, &Act, &OldAct);
    sigaction(SIGTRAP, &Act, &OldAct);

    //sigaction(SIGALRM, &Act, &OldAct);
}
//-----------------------------------------------------------------------
//  Функции для установки временных интервалов , а также их проверки
/*
uint32_t get_ms()
{
    return varta;
}
//
uint32_t get_tmr(uint32_t tm)
{
    return (get_ms() + tm);
}
//
int check_tmr(uint32_t tm)
{
    return (get_ms() >= tm ? 1 : 0);
}
*/
//----------------------------------------------------------------------
//   Функция формирует символьную строку с текущими значениями даты и времени
//
int TNP(char *ts)
{
struct timeval tvl;

    gettimeofday(&tvl, NULL);
    struct tm *ctimka = localtime(&tvl.tv_sec);
    return sprintf(ts, "%02d.%02d %02d:%02d:%02d.%03d | ",
                ctimka->tm_mday,
                ctimka->tm_mon + 1,
                ctimka->tm_hour,
                ctimka->tm_min,
                ctimka->tm_sec,
                (int)(tvl.tv_usec/1000));
}
//-----------------------------------------------------------------------
void Report(const uint8_t addTime, const char *fmt, ...)
{
size_t len = BUF_SIZE_PRN;

#ifdef SET_PRN_MUTEX
    int ria = 1, cnt = 20;

    while ((ria = pthread_mutex_trylock(&prn_buf_mutex))) {
        usleep(100);
        cnt--;
    }
    if (!cnt && ria) {
        devError |= devMutex;
        return;
    }
#endif
    buf_prn = (char *)calloc(1, len);//&prnBuff[0];
    if (buf_prn) {
        len--;
        va_list args;
        int dl = 0;
        if (addTime) dl = TNP(buf_prn);

        va_start(args, fmt);
        vsnprintf(buf_prn + dl, len - dl, fmt, args);
        va_end(args);

        dl = write(1, buf_prn, strlen(buf_prn) + 1);
        free(buf_prn);
    } else {
        devError |= devMem;
    }
#ifdef SET_PRN_MUTEX
    pthread_mutex_unlock(&prn_buf_mutex);
#endif

}
//------------------------------------------------------------------------------------------
uint32_t min_from_sec(uint32_t *seconda)
{
uint32_t sec = *seconda;

    sec %= (60 * 60 * 24);
    sec %= (60 * 60);
    uint32_t min = sec / 60;
    sec %= 60;
    *seconda = sec;

    return min;
}
//------------------------------------------------------------------------------------------
/*
void printBuffer(const uint8_t *buf, uint32_t len, uint32_t lsize)
{
    if (!len) return;

    char *stx = (char *)calloc(1, 1024);//lsize << 2);
    if (stx) {
        int i = -1;
        while (++i < len) {
            if ( (!(i % lsize)) && (i > 0) ) strcat(stx, "\n");
            sprintf(stx+strlen(stx), " %02X", *(uint8_t *)(buf + i));
        }
        if (stx[strlen(stx) - 1] != '\n') strcat(stx, "\n");
        Report(0, "%s", stx);
        free(stx);
    } else {
        devError |= devMem;
    }
}
//-------------------------------------------------------------------------------------------
//      Функция преобразует hex-строку в бинарное число типа uint32_t
//
uint32_t hex2bin(const char *buf, uint8_t len)
{
uint8_t i, j, jk, k;
uint8_t mas[8] = {0x30}, bt[2] = {0};
uint32_t dword, ret = 0;

    if (!len || !buf) return ret;
    if (len > 8) len = 8;
    k = 8 - len;
    memcpy(&mas[k], buf, len);

    k = j = 0;
    while (k < 4) {
        jk = j + 2;
        for (i = j; i < jk; i++) {
                 if ((mas[i] >= 0x30) && (mas[i] <= 0x39)) bt[i&1] = mas[i] - 0x30;
            else if ((mas[i] >= 0x61) && (mas[i] <= 0x66)) bt[i&1] = mas[i] - 0x57;//a,b,c,d,e,f
            else if ((mas[i] >= 0x41) && (mas[i] <= 0x46)) bt[i&1] = mas[i] - 0x37;//A,B,C,D,E,F
        }
        dword = (bt[0] << 4) | (bt[1] & 0xf);
        ret |= (dword << 8 * (4 - k - 1));
        k++;
        j += 2;
    }

    return ret;
}
//--------------------------------------------------------------------------------
//  Функция преобразует строковое значение hex-строки в бинарное занчение,
//   возвращая его в 32-х разрядной переменной
uint32_t strhex2bin(const char *buf, uint8_t len)
{
uint8_t i, j, jk, k;
uint8_t mas[8] = {0x30}, bt[2] = {0};
uint32_t dword, ret = 0;

    if (!len || !buf) return ret;

    if (len > 8) len = 8;
    k = 8 - len;
    memcpy(&mas[k], buf, len);

    k = j = 0;
    while (k < 4) {
        jk = j + 2;
        for (i = j; i < jk; i++) {
                 if ((mas[i] >= 0x30) && (mas[i] <= 0x39)) bt[i&1] = mas[i] - 0x30;
            else if ((mas[i] >= 0x61) && (mas[i] <= 0x66)) bt[i&1] = mas[i] - 0x57;//a,b,c,d,e,f
            else if ((mas[i] >= 0x41) && (mas[i] <= 0x46)) bt[i&1] = mas[i] - 0x37;//A,B,C,D,E,F
        }
        dword = (bt[0] << 4) | (bt[1] & 0xf);
        ret |= (dword << 8*(4 - k - 1));
        k++;
        j += 2;
    }

    return ret;
}
*/

