#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <termios.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include "func.h"

//-----------------------------------------------------------------------

#define buf_size      4096

#define MAX_PARAM     5
#define MAX_MODE      4

#define DEF_SPEED     B115200
#define PACK_MASK     0x80000000

#define MAX_LEN_FNAME 24

#define MAX_GET_MEM   (4096 * 1024) - 8

//-----------------------------------------------------------------------

enum {
    logOff = 0,
    logOn,
    logDebug,
    logDump
};

enum {
    crcCmd = 0,
    rxCmd,
    txCmd,
    noneCmd
};

enum {
    hdrNow = 0,
    blkNow,
    noneNow
};


typedef enum {
    pDev = 0,
    pMode,
    pFile,
    pSpeed,
    pDebug
} param_t;


#pragma pack(push,1)
typedef struct {
    int in_spd;
    uint32_t out_spd;
} val_speed_t;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct {
    char name[MAX_LEN_FNAME];
    uint32_t size;
    uint32_t crc;
} hdr_file_t;
#pragma pack(pop)

//---------------------------------------------------------------------

char fileName[128];
FILE *fd_bin;
int8_t mode;
uint32_t SPEED;
int spd;
const char *all_mode[MAX_MODE];
uint8_t dbg;


int fd;
struct timeval mytv;
struct termios oldtio, newtio;
uint8_t from_dev[buf_size + 4];
uint8_t to_dev[buf_size + 4];

const char *fmode_rx;
const char *fmode_tx;
uint32_t scrc;

uint8_t whoNow;

//----------------------------------------------------------------------

uint32_t crc32(const uint32_t crc_origin, const uint8_t *buf, const uint32_t size);

int speedShow(uint32_t spd);
uint32_t get_FileSize(const char *fn);

void parse_param_start(char *p_line);
//int writes(uint8_t *data, int len);
//int reads(uint8_t *data, int len);
int init_uart(char *dev_name);
int deinit_uart(int fd);
int txFileHdr(int fd, hdr_file_t *hdr);
uint32_t calcCRCFile(const char *fname, uint32_t *sz);
int rxFile (int fd, char *fname);
int txFile (int fd, const char *fname, uint32_t file_len);

FILE *open_File(const char *fn, const char *fmode);
int close_File(FILE *f);
uint32_t get_FileSize(const char *fn);
size_t readFileBin(FILE *f, uint8_t *buf, size_t len);
size_t writeFileBin(FILE *f, const uint8_t *buf, size_t len);
//bool hand_shake(bool tx);

//----------------------------------------------------------------------
