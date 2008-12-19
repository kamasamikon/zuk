#include <string.h>
#include <stdio.h>
#include <time.h>
#if (defined(__WIN32__) || defined(__WINCE__))
#include <windows.h>
#endif

#include "ksupport.h"
#include <kmem.h>
#include <kdbg.h>

kbool streq(const kchar *s1, const kchar *s2)
{
    if (s1 && s2) {
        return !strcmp(s1, s2);
    }
    return kfalse;
}

kchar *kspt_strdup(const kchar *src)
{
    kint slen;
    kchar *ret;
    if (src) {
        slen = strlen(src);
        ret = kmem_alloc(slen + 1);
        memcpy(ret, src, slen + 1);
        return ret;
    }
    return knil;
}

kchar *kspt_get_next_file_name(void)
{
    static kuint next_file_name_index;
    static kchar next_file_name[8 + 1];
    static kbool sync = kfalse;
    if (!sync) {
        // XXX this should be read from config
        next_file_name_index = 0;// kini_getint("next_file_name_index");
        sync = ktrue;
    }
    sprintf(next_file_name, "%08x", next_file_name_index);
    next_file_name_index++;
    return next_file_name;
}


kulong Get1900ToCurSecond(int nYearTo, int nMonthTo, int nDayTo, int nHourTo, int nMinuteTo, int nSecondTo)
{
    kuint nLeapYear = 0, nYear, nMonth;
    kulong nYear_Second;
    kuint nMonth_Day;
    kulong nMonth_Second ;
    kulong nDay_Second;
    kulong nHour_Sencond;
    kulong nMinute_Second;
    kulong nSecond_Second;


    for (nYear=1900; nYear<nYearTo; nYear++)
    {
        if ((nYear % 4 == 0 && nYear % 100 != 0) || (nYear % 400 == 0))
            nLeapYear++;
    }
    nYear_Second = (kulong)((nYearTo - 1900 - nLeapYear) * 365 + nLeapYear * 366) * 24 * 3600;

    nMonth_Day = 0;
    for (nMonth = 1; nMonth < nMonthTo; nMonth++)
    {
        if (nMonth == 1 || nMonth == 3 || nMonth == 5 || nMonth == 7 || nMonth == 8 ||
                nMonth == 10 || nMonth == 12)
        {
            nMonth_Day += 31;
        }
        else if (nMonth == 2)
        {
            if ((nYearTo % 4 == 0 && nYearTo % 100 != 0) || (nYearTo % 400 == 0))
                nMonth_Day += 29;
            else
                nMonth_Day += 28;
        }
        else
        {
            nMonth_Day += 30;
        }
    }
    nMonth_Second = (kulong)nMonth_Day * 24 * 3600;
    nDay_Second   = (kulong)((nDayTo - 1) * 24 * 3600);
    nHour_Sencond = (kulong)(nHourTo * 3600);
    nMinute_Second = (kulong)(nMinuteTo * 60);
    nSecond_Second = (kulong)nSecondTo;

    return (nYear_Second + nMonth_Second + nDay_Second + nHour_Sencond + nMinute_Second + nSecond_Second);
}


kuint kspt_dateTime_to_ntp(kchar *dateTime)// get the time from 1900
{
#if !defined(_WIN32_WCE)
    struct tm tm;

    kint year, month, day, hour, minute, second = 0;
    memset(&tm, 0, sizeof(struct tm));

    sscanf(dateTime, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;

    return (kuint)(2208988800U + mktime(&tm));
#else

    //SYSTEMTIME time;
    //time_t dosTime;
    kint year=1900, month=1, day=1, hour=0, minute=0, second = 0,diffHour=0,diffMin=0,timeDiffinSec;
    sscanf(dateTime, "%d-%d-%dT%d:%d:%d+%d:%d", &year, &month, &day, &hour, &minute, &second,&diffHour,&diffMin);
    timeDiffinSec = diffHour*3600+diffMin*60;


    //time.wYear = year - 1900;
    //time.wMonth = month - 1;
    //time.wDayOfWeek = GetWeekDay(year,month,day);
    //time.wDay = day;
    //time.wHour = hour;
    //time.wMinute = minute;
    //time.wSecond = second;
    //time.wMilliseconds = 0;

    //SystemTimeToTime_t(time, dosTime);// since Jan.1, 1970
    //return 2208988800U +dosTime;

    return (kuint)(Get1900ToCurSecond(year,month,day,hour,minute, second)-timeDiffinSec);
#endif
}

kuint kspt_gzip_orglen(kuchar *buf, kuint buflen)
{
    kuchar byte0, byte1, byte2, byte3;
    byte0 = buf[buflen - 1];
    byte1 = buf[buflen - 2];
    byte2 = buf[buflen - 3];
    byte3 = buf[buflen - 4];

    return (byte0 << 24) + (byte1 << 16) + (byte2 << 8) + byte3;
}

kbool kspt_str_to_bool(kchar *str)
{
    if (str && (!strcmp(str, "true"))) {
        return ktrue;
    }
    return kfalse;
}

kuint ksys_ntp_time(kvoid)
{
    return 2208988800 + ksal_cur_time();
}

kint ksal_file_save(const kchar *fn, const kchar *dat, kint datlen)
{
    kbean fp;

    fp = kvfs_open((kchar*)fn, "wb", 0);
    if (fp) {
        kvfs_write(fp, (void*)dat, datlen);
        kvfs_close(fp);
        return 0;
    }
    return -1;
}
