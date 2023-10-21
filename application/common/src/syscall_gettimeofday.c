#include <sys/time.h>
#include <time.h>
#include "main.h"

extern RTC_HandleTypeDef hrtc;

int _gettimeofday(struct timeval *tp, void *tzp __attribute__((__unused__)))
{
    RTC_TimeTypeDef stime;
    RTC_DateTypeDef sdate;
    HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sdate, RTC_FORMAT_BIN);

    struct tm ts;
    ts.tm_year = sdate.Year + 100;
    ts.tm_mon = sdate.Month;
    ts.tm_mday = sdate.Date;
    ts.tm_hour = stime.Hours;
    ts.tm_min = stime.Minutes;
    ts.tm_sec = stime.Seconds;

    tp->tv_sec = mktime(&ts);
    tp->tv_usec = 0;

    return 0;
}