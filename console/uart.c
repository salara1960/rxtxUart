#include "uart.h"


//--------------------------------------------------------------------------------------------------
// Таблица для подсчета контрольной суммы CRC32 (polynomial 0xedb88320)
//
//CRC-32-IEEE 802.3  (x^{32} + x^{26} + x^{23} + x^{22} + x^{16} + x^{12} + x^{11} + x^{10} + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1)
//
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
//--------------------------------------------------------------------------------------------------

int fd = -1;
char fileName[128];
FILE *fd_bin = NULL;
int8_t mode = noneCmd;
uint32_t SPEED = 0;
uint8_t from_dev[buf_size + 4];
uint8_t to_dev[buf_size + 4];
const uint32_t len4 = sizeof(uint32_t);
uint8_t dbg = logOn;

struct termios oldtio;
struct termios newtio;

const uint8_t total_speed = 13;
val_speed_t all_speed[] = {
        {9600,    B9600},
        {19200,   B19200},
        {38400,   B38400},
        {57600,   B57600},
        {115200,  B115200},
        {230400,  B230400},
        {460800,  B460800},
        {500000,  B500000},
        {576000,  B576000},
        {921600,  B921600},
        {1000000, B1000000},
        {1152000, B1152000},
        {1500000, B1500000}
};

const int code_err[] = {devOk, devMem, devTimeout, devUart};
const char *str_err[] = {"devOk","devMem","devTimeout","devUart"};
const char *all_param[] = {"--dev=", "--mode=", "--file=", "--speed=", "--dbg="};
const char *all_mode[] = {"crc", "rx", "tx", "???"};
const char *fmode_rx = "rb";
const char *fmode_tx = "wb";


uint8_t whoNow = noneNow;



extern void Report(const uint8_t addTime, const char *fmt, ...);


//--------------------------------------------------------------------------------
//   Функция возвращает контрольную сумму CRC32 массива байт
//
uint32_t crc32(const uint32_t crc_origin, const uint8_t *buf, const uint32_t size)
{
uint32_t crc = ~crc_origin;
const uint8_t *p = buf;

    if (!buf || !size) return ~crc;

    for (uint32_t i = 0; i < size; i++) crc = CRC32(crc, *p++);

    return ~crc;

}
//----------------------------------------------------------------------

uint32_t get_sec(uint32_t tm)
{
    return (uint32_t)(time(NULL) + tm);
}
//
int check_sec(uint32_t tm)
{
    return ((uint32_t)time(NULL) >= tm ? 1 : 0);
}
//----------------------------------------------------------------------
int speedShow(uint32_t spd)
{
    for(int8_t i = 0; i < total_speed; i++) {
        if (all_speed[i].out_spd == spd) return all_speed[i].in_spd;
    }

    return 0;
}
//----------------------------------------------------------------------
//   Функция анализирует параметры, заданные при старте утилиты
//    и определяющие режим работы и последовательность действий
//
void parse_param_start(char *p_line)
{
int i;

    char *uks = strchr(p_line, '\n');
    if (uks) *uks = '\0';
    for (i = 0; i < MAX_PARAM; i++) {
        uks = strstr(p_line, all_param[i]);
        if (uks) {
            uks += strlen(all_param[i]);
            switch (i) {
                case pDev:
                    strcpy(dev_name, uks);
                break;
                case pMode:
                    if (strstr(uks, "crc") != NULL) {
                        mode = crcCmd;
                    } else if (strstr(uks, "rx") != NULL) {
                        mode = rxCmd;//read data from uart
                    } else if (strstr(uks, "tx") != NULL) {
                        mode = txCmd;//send data via uart
                    }
                break;
                case pFile:
                    memset(fileName, 0, sizeof(fileName));
                    if ((!strstr(uks, "null")) && (strlen(uks))) strcpy(fileName, uks);
                break;
                case pSpeed:
                {
                    int spd = atoi(uks);
                    for (int j = 0; j < total_speed; j++) {
                        if (spd == all_speed[j].in_spd) {
                            SPEED = all_speed[j].out_spd;
                            break;
                        }
                    }
                }
                break;
                case pDebug:
                    if (!strcmp(uks, "off")) {
                        dbg = logOff;
                    } else if (!strcmp(uks, "on")) {
                        dbg = logOn;
                    } else if (!strcmp(uks, "debug")) {
                        dbg = logDebug;
                    } else if (!strcmp(uks, "dump")) {
                        dbg = logDump;
                    }
                break;
            }
        }
    }
    if (!SPEED) SPEED = DEF_SPEED;

}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

//-----------------------------------------------------------------------
/*
inline int writes(uint8_t *data, int len)
{
    return write(fd, data, len);
}
//-----------------------------------------------------------------------
inline int reads(uint8_t *data, int len)
{
    return (read(fd, data, len));
}
*/
//-----------------------------------------------------------------------
int init_uart(char *dev_name)
{
//uint32_t cnt = 0;

    int fd = open(dev_name, O_RDWR | O_NONBLOCK, 0664);
    if (fd > 0) {
        tcgetattr(fd, &oldtio);
        memset(&newtio, 0, sizeof(newtio));
        memcpy(&newtio, &oldtio, sizeof(oldtio));

        cfmakeraw(&newtio);//set RAW mode

        newtio.c_cflag = SPEED | CS8 | CLOCAL | CREAD;

        tcflush(fd, TCIFLUSH);

        tcsetattr(fd, TCSANOW, &newtio);
/*
        while (reads(from_dev, 1) > 0) {
            cnt++;
            usleep(1000);
        }
        if ((dbg != logOff) && cnt) Report(1, "[%s] total bytes received %u\n", __func__, cnt);
*/
        memset(from_dev, 0, sizeof(from_dev));
        memset(to_dev, 0, sizeof(to_dev));
    }

    return fd;
}
//-----------------------------------------------------------------------
int deinit_uart(int fd)
{
    if (fd < 0) return fd;

    tcsetattr(fd, TCSANOW, &oldtio);

    return close(fd);
}
//-----------------------------------------------------------------------
uint32_t calcCRCFile(const char *fname, uint32_t *sz)
{
uint32_t crc = 0;
uint32_t s = 0;


    FILE *f = open_File(fname, fmode_rx);
    if (f) {
        s = get_FileSize(fname);
        if (s > 0) {
            uint32_t rdx = 0, fs = 0;
            if (s > MAX_GET_MEM) {
                while (fs < s) {
                    if ((rdx = readFileBin(f, from_dev, buf_size)) > 0) {
                        crc = crc32(crc, from_dev, rdx);
                        fs += rdx;
                    } else {
                        break;
                    }
                }
            } else {
                uint8_t *buf = (uint8_t *)calloc(1, s);
                if (buf) {
                    if ((rdx = readFileBin(f, buf, s)) > 0) crc = crc32(0, buf, rdx);
                }
            }
        }
        close_File(f);
    }

    *sz = s;

    return crc;
}
//-----------------------------------------------------------------------
int rxFile (int fd, char *fname)
{
int lenr = 0, lenr_tmp = 0, ukb = 0;
struct timeval mytv = {0, 10000};
fd_set Fdr, Fdt, Fde;
int8_t rdy = 0, Vixod = 0, done = 0, err = 0, en_tx = 0;
uint32_t pack_len = 0, file_len = 0, seq_num = 0, rx_len = 4;
uint32_t crc = 0, crc_pack = 0, crcf = 0;
size_t rt = 0;
FILE *fil = NULL;
uint32_t tmr = 0, tmr_ack = 0;
hdr_file_t hdr_file = {0};



    if (fd < 0)  {
        Report(1, "[%s] ERROR: uart not init\n", __func__);
        return -1;
    }

    if (whoNow == hdrNow) rx_len = sizeof(hdr_file_t);

    while (!Vixod && !QuitAll) {

        FD_ZERO(&Fdr); FD_SET(fd, &Fdr);
        FD_ZERO(&Fdt); FD_SET(fd, &Fdt);
        FD_ZERO(&Fde); FD_SET(fd, &Fde);
        if (select(fd + 1, &Fdr, &Fdt, &Fde, &mytv) > 0) {
            if (FD_ISSET(fd, &Fdr)) {// event from my device

                lenr_tmp = read(fd, &from_dev[ukb], rx_len - lenr);//reads(&from_dev[ukb], rx_len - lenr);

                if (lenr_tmp == 0) {
                    Report(1, "[%s] Can't read from device '%s'\n", __func__, dev_name);
                    break;
                } else if (lenr_tmp > 0) {
                    lenr += lenr_tmp;
                    ukb += lenr_tmp;
                    //if (!pack_len) rx_len -= lenr;
                    if (lenr > sizeof(from_dev)) {
                        rdy = 1;
                        Vixod = 1;
                        err = -2;
                    } else {
                        if (whoNow == hdrNow) {
                            if (lenr == sizeof(hdr_file_t)) {
                                memcpy(&hdr_file, from_dev, lenr);
                                //whoNow = blkNow;
                                //rx_len = sizeof(uint32_t);
                                //lenr = ukb = 0;
                                rdy = 1;
                                err = 0;
                            }            
                        } else {
                            if (!pack_len) {
                                if (lenr == len4) {
                                    pack_len = *(uint32_t *)from_dev;
                                    if (pack_len & PACK_MASK) {//last packect
                                        pack_len &= ~PACK_MASK;
                                        done = 1;
                                    }
                                    tmr = 0;
                                    if (pack_len > sizeof(from_dev)) {
                                        err = -3;
                                        return err;
                                    }
                                    lenr = ukb = 0;
                                    rx_len = pack_len;//1;
                                    memset(from_dev, 0, sizeof(from_dev));
                                }
                            } else {
                                if (dbg == logDump)
                                    if (lenr < (pack_len - len4)) Report(0, "%c", from_dev[ukb - lenr_tmp]);
                            }
                            if (lenr == pack_len) {
                                pack_len -= len4;
                                file_len += pack_len;
                                rdy = 1;
                                err = 0;
                            }
                        }
                    }
                }
                if (rdy) {
                    rdy = 0;
                    if (whoNow != hdrNow) {
                        rx_len = 4;
                        crc_pack = *(uint32_t *)&from_dev[pack_len];
                        crc = crc32(0, from_dev, pack_len);
                        //crcf = crc32(crcf, from_dev, pack_len);
                        if (crc_pack != crc) err = -4;
                        tmr_ack = get_sec(1);
                        while (!en_tx) {
                            if (check_sec(tmr_ack)) break;
                        }
                        if (en_tx) {    
                            if (write(fd, (uint8_t *)&crc/*pack_len*/, len4) != len4) {
                                err = -5;
                            } else {
                                if (crc_pack == crc) {
                                    if (strlen(fname)) {
                                        if (!fil) fil = open_File(fname, fmode_tx);
                                        if (fil) {
                                            rt = writeFileBin(fil, from_dev, pack_len);
                                            if (rt != pack_len) err = -6;
                                        }
                                    }
                                }
                                tmr = get_sec(5);//get_tmr(_5s);
                            }
                            seq_num++;
                        } else err = -7;
                            if (crc_pack != crc) {
                                if (dbg != logOff)
                                    Report(1, "[%s] recv_pack=%u pack_len=%u '%s' err=%d CRC Error : IN/CALC=0x%08X/0x%08X\n",
                                              __func__, seq_num, pack_len, (done) ? "last" : "current", err, crc_pack, crc);
                             } else {
                                if (dbg >= logDebug)
                                    Report(1, "[%s] recv_pack=%u pack_len=%u '%s' err=%d CRC=0x%08X\n",
                                              __func__, seq_num, pack_len, (done) ? "last" : "current", err, crc);
                            }
                        
                        if (done) {
                            tmr = 0;
                            Vixod = 1;
                        } else {
                            ukb = 0;
                            lenr = lenr_tmp = 0;
                            pack_len = 0;
                            memset(from_dev, 0, sizeof(from_dev));
                        }
                        if (dbg == logOn) Report(0, "R");
                    } else {
                        pack_len = lenr;
                        if (dbg >= logOn) Report(1, "[%s] received hdr_len=%u file[name:'%.*s' size:%u crc:0x%08X]\n",
                                                    __func__, lenr, MAX_LEN_FNAME, hdr_file.name, hdr_file.size, hdr_file.crc);
                        if (strlen(hdr_file.name)) strcpy(fname, hdr_file.name);
                        tmr_ack = get_sec(1);
                        while (!en_tx) {
                            if (check_sec(tmr_ack)) break;
                        }
                        if (write(fd, (uint8_t *)&pack_len, len4) != len4) {
                            err = -5;
                            Vixod = 1;
                        } else {
                            if (dbg >= logDebug) Report(1, "[%s] to device ack[len:%u data:%u]\n", __func__, len4, pack_len);
                            tmr = get_sec(5);
                        }
                        ukb = 0;
                        lenr = lenr_tmp = 0;
                        pack_len = 0;
                        whoNow = blkNow;
                        rx_len = 4;
                        memset(from_dev, 0, sizeof(from_dev));
                    }
                }

            }//if (FD_ISSET(fd, &Fdr))
            if (FD_ISSET(fd, &Fde)) {
                err = -8;
            }
            if (FD_ISSET(fd, &Fdt)) {
                en_tx = 1;
            }

        }//if (select
/**/
        if (tmr) {
            if (check_sec(tmr)) {
                if ((dbg == logOn) || (dbg == logDump)) Report(0, "\n");
                Report(1, "[%s] Error timeout : no received end_of_packet flag\n", __func__);
                break;
            }
        }
/**/
        //---------------------------------------------------
    }//while (!Vixod)

    if (((dbg == logOn) || (dbg == logDump)) && !tmr) Report(0, "\n");

    if (fil) close_File(fil);
    if (!done && strlen(fname)) {
        if (unlink(fname) < 0) err = -7;
    }

    if (done) 
        crcf = calcCRCFile(fname, &file_len);
    else
        crcf = 0;

    if (crcf && dbg) Report(1, "[%s] File '%s' write done : packets=%u len=%u crc32=0x%X (err:%d)\n",
                               __func__, fname, seq_num, file_len, crcf, err);

    return file_len;
}
//-----------------------------------------------------------------------
int txFileHdr(int fd, hdr_file_t *hdr)
{
int ret = 0, lenr = 0, lenr_tmp, ukb = 0;
struct timeval mytv = {0, 10000};
fd_set Fdr;
int8_t rdy = 0, Vixod = 0;
uint32_t tmr = 0, pack_len = 0;
uint8_t buf[4] = {0};



    if ((fd < 0) || !hdr) return ret;


    if (dbg >= logOn) {
        Report(1, "[%s] Send file's header : name=%.*s, size=%u, crc32=0x%X\n",
                  __func__, strlen(hdr->name), hdr->name, hdr->size, hdr->crc);
    }

    if (write(fd, (uint8_t *)hdr, sizeof(hdr_file_t)) == sizeof(hdr_file_t)) {
    
        tmr = get_sec(5);

        while (!Vixod && !QuitAll) {

            FD_ZERO(&Fdr);
            FD_SET(fd, &Fdr);
            if (select(fd + 1, &Fdr, NULL, NULL, &mytv) > 0) {                
                if (FD_ISSET(fd, &Fdr)) {

                    lenr_tmp = read(fd, &buf[ukb], 1);

                    if (lenr > sizeof(buf)) {
                        Report(1, "[%s] Error overload input buffer (lenr=%d)\n", __func__, lenr);
                        return 0;
                    } else if (lenr_tmp > 0) {
                        lenr += lenr_tmp;
                        ukb += lenr_tmp;
                        if (lenr == len4) {
                            pack_len = *(uint32_t *)buf;
                            rdy = 1;
                        }
                    }
                    if (rdy) {
                        rdy = 0;
                        Vixod = 1;
                        if (pack_len != sizeof(hdr_file_t)) ret = 0; else ret = pack_len;
                        if (dbg >= logOn) {
                            Report(1, "[%s] recv ack=%u for sending hdr_file\n", __func__, pack_len);
                        }
                    }
                }//if (FD_ISSET(fd, &Fds))
            }//if (select
            //
            if (check_sec(tmr)) {
                if (!Vixod) {
                    Vixod = 1;
                    Report(1, "[%s] Error waiting ack from device\n", __func__);
                    sleep(1);
                    ret = 0;
                }
            }
            //
        }

    }

    return ret;
}
//-----------------------------------------------------------------------
int txFile(int fd, const char *fname, uint32_t file_len)
{
int lenr = 0, lenr_tmp, ukb = 0, rt = 0;
struct timeval mytv = {0, 10000};
fd_set Fdr, Fdt, Fde;
int8_t rdy = 0, Vixod = 0, done = 0, err = 0, try = 3;
uint32_t pack_len = 0, send_len = 0, seq_num = 0;
uint32_t blk_len = 0, len = 0, data_len = 0;
uint32_t crc = 0;
uint8_t from_buf[4];
uint32_t tmr = 0;
const uint8_t *uk = NULL;//buf;
uint32_t tail_len = file_len;
bool txd = true;


    if ((fd < 0) || !file_len)  {
        Report(1, "[%s] ERROR !\n", __func__);
        return -1;
    }

    fd_bin = open_File(fname, fmode_rx);
    if (!fd_bin) return -1;


    while (send_len < file_len) {

        done = 0;

        if ((blk_len = readFileBin(fd_bin, to_dev, buf_size)) < 0) break;
        if (blk_len < buf_size) {
            done = 1;
        } else {
            if ((send_len + blk_len) == file_len) done = 1;
        }
        if (done)
            if (fd_bin) { close_File(fd_bin); fd_bin = NULL; }

        Vixod = 0;
        lenr = ukb = 0;
        pack_len = 0;

        if (txd) {
            txd = false;

            blk_len += len4;
            len = blk_len;
            if (done) blk_len |= PACK_MASK;
            rt = write(fd, (uint8_t *)&blk_len, len4);//send block_len
            //usleep(1000);
            data_len = len - len4;
            crc = crc32(0, to_dev, (uint16_t)data_len);
            memcpy(to_dev + data_len, (uint8_t *)&crc, len4);
            rt = write(fd, to_dev, len);//send body + crc

            if (dbg == logDebug) Report(1, "[%s] tail_len=%u, blk_len=%u, %s, rt=%u, CRC=0x%X\n",
                                           __func__, tail_len, len, (done) ? "last" : "current", rt, crc);
            else
            if (dbg == logOn) {
                if (try == 3) Report(0, "T"); else Report(0, "<");
            }

            tmr = get_sec(5);
        }

        while (!Vixod && !QuitAll) {

            FD_ZERO(&Fdr); FD_SET(fd, &Fdr);
            FD_ZERO(&Fdt); FD_SET(fd, &Fdt);
            FD_ZERO(&Fde); FD_SET(fd, &Fde);
            if (select(fd + 1, &Fdr, &Fdt, &Fde, &mytv) > 0) {                
                if (FD_ISSET(fd, &Fdr)) {

                    lenr_tmp = read(fd, &from_buf[ukb], 1);

                    if (lenr > sizeof(from_buf)) {
                        Report(1, "[%s] Error overload input buffer (lenr=%d)\n", __func__, lenr);
                        err = -2;
                        return send_len;
                    } else if (lenr_tmp > 0) {
                        lenr += lenr_tmp;
                        ukb += lenr_tmp;
                        if (lenr == len4) {
                            pack_len = *(uint32_t *)from_buf;
                            rdy = 1;
                        }
                    }
                    if (rdy) {
                        rdy = 0;
                        seq_num++;
                        uk += data_len;
                        tail_len -= data_len;
                        if (rt > len4) send_len += (rt - len4);
                        Vixod = 1;
                        
                        if (dbg == logDebug)
                            Report(1, "[%s]   recv ack=0x%08X %s for packet with len=%u data_len=%u packet_num=%u\n",
                                      __func__, pack_len, (crc != pack_len) ? "ACK != CRC" : "OK", len, data_len, seq_num);

                        if (crc != pack_len) {
                            tmr = get_sec(0);
                        } else {
                            usleep(1000);
                            tmr = get_sec(5);//get_tmr(_3s);
                        }
                    }
                }//if (FD_ISSET(fd, &Fds))
                if (FD_ISSET(fd, &Fde)) {
                    err = -8;
                }
                if (FD_ISSET(fd, &Fdr)) {
                    txd = true;
                }

            }//if (select

/**/
            if (check_sec(tmr)) {
                if (!Vixod) {
                    if (try) try--;
                    if (!try) {
                        err = -3;
                        Report(1, "[%s] Error sending data after 3 attempt\n", __func__);
                        break;
                    } else {
                        Vixod = 1;
                        Report(1, "[%s] Error sending data after %d attempt\n", __func__, 3 - try);
                        sleep(1);
                    }
                } else try = 3;
            }
/**/
            //usleep(1000);//10000

        }//while (!Vixod && !QuitAll)...
        //---------------------------------------------------

        if (QuitAll) break;
        else
        if (done) break;
        else
        if (err) break;

    }//while (1)

    if (fd_bin) close_File(fd_bin);

    if (dbg == logOn) Report(0, "\n");

    if (QuitAll || err) {
        if (dbg == logDebug) Report(1, "[%s] tail_len=%u lenr=%d ukb=%d ack=%u(0x%X)\n:", __func__, tail_len, lenr, ukb, *(uint32_t *)from_buf, *(uint32_t *)from_buf);
    } else {
        if (dbg == logOn) Report(1, "[%s] transfer file with size %u in %u packets\n", __func__, send_len, seq_num);
    }

    return send_len;
}
//----------------------------------------------------------------------
//   Функция открывает файл по его имени, возвращая NULL в случае возникновения ошибки
//
FILE *open_File(const char *fn, const char *fmode)
{
    return (fopen(fileName, fmode));
}
//----------------------------------------------------------------------
//   Функция закрывает файл по его указателю, возвращая 0 в случае успешного выполнения
//    операции и EOF в противном случае (чтение errno детализирует ошибку)
//
int close_File(FILE *f)
{
    return fclose(f);
}
//----------------------------------------------------------------------
//   Функция возвращает размер файла по его имени
//
uint32_t get_FileSize(const char *fn)
{
struct stat sb;

    if (!stat(fn, &sb)) return sb.st_size; else return 0;
}
//----------------------------------------------------------------------
//   Функция читает заданное количество байт в буфер из файла по его указателю
//
size_t readFileBin(FILE *f, uint8_t *buf, size_t len)
{
    return fread(buf, 1, len, f);
}
//----------------------------------------------------------------------
size_t writeFileBin(FILE *f, const uint8_t *buf, size_t len)
{
    return fwrite(buf, 1, len, f);
}
//----------------------------------------------------------------------
/*
bool hand_shake(bool txrx)
{
const char sym = 's';
const char otv = 'o';
struct timeval mytv;
fd_set Fdr, Fdt;
int lenr = 0;
char tx_buf[4], rx_buf[4];
bool tx = false;
char cha, recv;
uint32_t tmr = get_sec(0);


    if (txrx) { cha = sym; recv = otv; }
         else { cha = otv; recv = sym; }

    while (!QuitAll) {
        //
        if (check_sec(tmr)) {
            if (tx) {
                tmr = get_sec(1);
                tx_buf[0] = cha;
                writes((uint8_t *)tx_buf, 1);
                Report(0,"%c", tx_buf[0]);
                tx = false;
            }
        }
        //
        FD_ZERO(&Fdr); FD_SET(fd, &Fdr);
        FD_ZERO(&Fdt); FD_SET(fd, &Fdt);
        mytv.tv_sec = 0; mytv.tv_usec = 5000;
        if (select(fd + 1, &Fdr, &Fdt, NULL, &mytv) > 0) {
            if (FD_ISSET(fd, &Fdr)) {
                lenr = reads((uint8_t *)rx_buf, 1);
                if (lenr > 0) {
                    Report(0, "%c", rx_buf[0]);
                    if (rx_buf[0] == recv) {
                        Report(0, "\n");
                        if (txrx) sleep(1);
                        return true;
                    }
                }
            }
            if (FD_ISSET(fd, &Fdt)) {
                tx = true;
            }
        }
        //
    }

    return false;
}
*/
//----------------------------------------------------------------------
