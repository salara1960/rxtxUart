#ifndef FUNC_H_
#define FUNC_H_

#include "inc.h"


//-----------------------------------------------------------------

#define SDEF           256
#define BUF_SIZE_PRN  2048//4096

//-----------------------------------------------------------------------
/*
#define _1ms 1
#define _2ms 2
#define _3ms 3
#define _4ms 4
#define _5ms 5
#define _6ms 6
#define _7ms 7
#define _8ms 8
#define _9ms 9
#define _10ms 10
#define _20ms 2 * _10ms
#define _30ms 3 * _10ms
#define _40ms 4 * _10ms
#define _50ms 5 * _10ms
#define _60ms 6 * _10ms
#define _70ms 7 * _10ms
#define _80ms 8 * _10ms
#define _90ms 9 * _10ms
#define _100ms 10 * _10ms

#define _150ms _100ms + _50ms
#define _200ms 2 * _100ms
#define _250ms _200ms + _50ms
#define _300ms 3 * _100ms
#define _350ms _300ms + _50ms
#define _400ms 4 * _100ms
#define _450ms _400ms + _50ms
#define _500ms 5 * _100ms
#define _550ms _500ms + _50ms
#define _600ms 6 * _100ms
#define _650ms _600ms + _50ms
#define _700ms 7 * _100ms
#define _750ms _700ms + _50ms
#define _800ms 8 * _100ms
#define _850ms _800ms + _50ms
#define _900ms 9 * _100ms
#define _950ms _900ms + _50ms
#define _1000ms 10 * _100ms

#define _1s _1000ms
#define _1s1 _1s + _100ms
#define _1s2 _1s + _200ms
#define _1s3 _1s + _300ms
#define _1s4 _1s + _400ms
#define _1s5 _1s + _500ms
#define _1s6 _1s + _600ms
#define _1s7 _1s + _700ms
#define _1s8 _1s + _800ms
#define _1s9 _1s + _900ms
#define _2s _1s * 2
#define _2s5 (_1s * 2) + _500ms
#define _3s _1s * 3
#define _3s5 (_1s * 3) + _500ms
#define _4s _1s * 4
#define _4s5 (_1s * 4) + _500ms
#define _5s _1s * 5
#define _5s5 (_1s * 5) + _500ms
#define _6s _1s * 6
#define _6s5 (_1s * 6) + _500ms
#define _7s _1s * 7
#define _7s5 (_1s * 7) + _500ms
#define _8s _1s * 8
#define _8s5 (_1s * 8) + _500ms
#define _9s _1s * 9
#define _9s5 (_1s * 9) + _500ms
#define _10s _1s * 10
#define _15s _1s * 15
#define _20s _1s * 20
#define _25s _1s * 25
#define _30s _1s * 30
*/

//------------------------------------------------------------------------

const char *version;

uint8_t QuitAll;
uint8_t devError;
char dev_name[SDEF];

char *buf_prn;

//-------------------------------------------------------------------------

/*char *errNames(uint8_t e, char *st);
void printBuffer(const uint8_t *buf, uint32_t len, uint32_t lsize);
void hex_dump(const void *src, size_t length, size_t line_size, char *prefix);
uint32_t hex2bin(const char *buf, uint8_t len);
void hex_show(const uint8_t *src, size_t length, size_t line_size, char *prefix);
int mkTimer();
uint32_t get_tmr(uint32_t tm);
int check_tmr(uint32_t tm);*/
void GetSignal_(int sig);
void setSigSupport();
int TNP(char *ts);
void Report(const uint8_t addTime, const char *fmt, ...);
uint32_t min_from_sec(uint32_t *seconda);

//-------------------------------------------------------------------------


#endif

