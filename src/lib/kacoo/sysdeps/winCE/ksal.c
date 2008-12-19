#include <ksal.h>
#include <stdio.h>
#include <windows.h>
#include "ivisa_plfm.h"
#if !defined(_WIN32_WCE)
#include <process.h>
#include <io.h>
#endif

#define WINCE501_EMULATOR

//kint kmemavail(kchar *str)
//{
//    kchar strBuffer[1024];
//	WCHAR outBuffer[1024];
//	MEMORYSTATUS memory;
//
//	GlobalMemoryStatus(&memory);
//	sprintf(strBuffer,"[MEMORY]%s:%d B\n",str,memory.dwAvailPhys);
//
//	MultiByteToWideChar(CP_OEMCP, 0, strBuffer, -1, outBuffer, 1024 );
//    OutputDebugString(outBuffer);
//	return 0;
//}

kint kmemavail(const kchar *format, ...)
{
    kchar strBuffer[512];
    kchar str[512];
    WCHAR outBuffer[512];
    MEMORYSTATUS memory;
    va_list args;

    va_start(args, format);
    _vsnprintf(strBuffer, sizeof(strBuffer), format, args);
    va_end(args);

    GlobalMemoryStatus(&memory);
    sprintf(str,"[MEMORY]%s:%d B\n",strBuffer,memory.dwAvailPhys);

	MultiByteToWideChar(CP_OEMCP, 0, str, -1, outBuffer, 1024 );
    OutputDebugString(outBuffer);
    return 0;
}

kint kprintf(const kchar *format, ...)
{
    kchar strBuffer[1024];
    WCHAR outBuffer[1024];
    va_list args;

    va_start(args, format);
    _vsnprintf(strBuffer, sizeof(strBuffer), format, args);
    va_end(args);

    MultiByteToWideChar(CP_OEMCP, 0, strBuffer, -1, outBuffer, 1024 );
    OutputDebugString(outBuffer);
    return 0;
}


kint stricmp(const char *str0,const char *str1)
{
	return _stricmp(str0, str1);
}


kint strnicmp(const char *str0, const char *str1, size_t len)
{
	return _strnicmp(str0, str1,len);
}

kvoid kdbgbrk(kvoid)
{
    //   __asm int 3
}

//
//
kulong ksal_get_tick(void)
{
    return GetTickCount();
}

//
//
kuint ksal_cur_time(void)
{
    time_t curtime;
    time(&curtime);
    return curtime;
}

kvoid ksal_tsk_exit(kvoid)
{
    ExitThread(0);
}

//
kbean ksal_tsk_new(kvoid (*entrypoint)(kvoid), kuchar priority, kuint stackSize, void *param)
{
    kuint ThreadId;
    CreateThread(NULL, stackSize, (LPTHREAD_START_ROUTINE)entrypoint, param, 0, &ThreadId);
    return (kbean)ThreadId;
}

kint ksal_tsk_del(kbean taskId)
{
    return CloseHandle(taskId);
}
kbean ksal_tsk_cur(kvoid)
{
    kuint tskid = GetCurrentThreadId();
    return (kbean)tskid;
}

//version  for memory check
static kint __g_mem_alloc_cnt = 0;          /**< count to call malloc */
static kint __g_mem_free_cnt = 0;           /**< count to call free */

static kint __g_mem_peak = 0;               /**< max ever has */
static kint __g_mem_cur = 0;                /**< current allocated */

void show_mem_stat()
{
    kprintf("KMEM_STAT :: allCnt:%d, freeCnt:%d, peak:%d, curr:%d\n",
            __g_mem_alloc_cnt, __g_mem_free_cnt, __g_mem_peak, __g_mem_cur);
}

static HANDLE __g_heap_handle = (HANDLE)0;

kint ksal_mem_init(kuint a_flg)
{
    if (!__g_heap_handle) {
        __g_heap_handle = HeapCreate(0, 2 * 1024 * 1024, 0);
    }
    return 0;
}
kint ksal_mem_final(kvoid)
{
    if (!__g_heap_handle) {
        HeapDestroy(__g_heap_handle);
        __g_heap_handle = (HANDLE)0;
    }
    return 0;
}

void* ksal_mem_alloc(kuint size)
{
    void *ptr;
    if (!__g_heap_handle) {
        __g_heap_handle = HeapCreate(0, 2 * 1024 * 1024, 0);
    }

    ptr = HeapAlloc(__g_heap_handle, 0, size);
    size = HeapSize(__g_heap_handle, 0, ptr);

    __g_mem_alloc_cnt++;
    __g_mem_cur += size;
    if (__g_mem_cur > __g_mem_peak) {
        __g_mem_peak = __g_mem_cur;
    }
    if (!(__g_mem_alloc_cnt % 200)) {
        show_mem_stat();
    }
    return ptr;
}
void ksal_mem_free(void * ptr)
{
    kint size;
    size = HeapSize(__g_heap_handle, 0, ptr);

    HeapFree(__g_heap_handle, 0, ptr);

    __g_mem_free_cnt++;
    __g_mem_cur -= size;
    if (!(__g_mem_free_cnt % 200)) {
        show_mem_stat();
    }
}

//
//
void ksal_tsk_sleep(kuint msec)
{
    Sleep(msec);
}

kbean ksal_sema_new(kint initValue)
{
    return CreateSemaphore(NULL, initValue, 20, NULL);
}
kint ksal_sema_del(kbean semaphId)
{
    CloseHandle(semaphId);
    return 0;
}
kint ksal_sema_get(kbean semaphId, kint timeout)
{
    return WaitForSingleObject(semaphId, timeout);
}
kint ksal_sema_rel(kbean semaphId)
{
    kulong oldcnt;
    ReleaseSemaphore(semaphId, 1, &oldcnt);
    return 0;
}



/*
 * file operation
 */

/*
 * @brief seperator for directory, in *nix it should be '/' in win32s, should be '\\'
 */
kchar kvfs_path_sep(kvoid)
{
	return '\\';
}

/**
 * @brief is this path, include file or directory exist
 */
kbool kvfs_exist(const kchar *a_path)
{
	WCHAR wpath[MAX_PATH];
	MultiByteToWideChar(CP_OEMCP, 0, a_path, -1, wpath, MAX_PATH );

	if((GetFileAttributes(wpath)) != 0xFFFFFFFF) {//0xFFFFFFFF failure
		return 1;
	}
	return 0;
}

kbean kvfs_open(const kchar *a_path, const kchar *a_mode, kuint a_flg)
{
	if(a_path)
		return (kbean)fopen(a_path, a_mode);
	return 0;
}

kint kvfs_close(kbean a_file)
{
	if(a_file)
		return fclose((FILE*)a_file);
	return 0;
}

/**
 * @brief Read data from a opened file.
 *
 * @param[in out] a_buf if a_buf is null, allocate buffer for it, the buffer should be freed by ksal_mem_free.
 * if a_buf is not null, read data and directly fill the data into a_buf;
 * @param[in out] a_size in, is the size of a_buf when a_buf not null, and return bytes read
 *
 * @return bytes read, if failed return -1
 */
kint kvfs_read(kbean a_file, kvoid **a_buf, kint *a_size)
{
    kint readsize, leftsize, curpos;

    if (!(*a_buf)) {
        curpos = kvfs_tell(a_file);
        kvfs_seek(a_file, 0, SEEK_END);
        leftsize = kvfs_tell(a_file) - curpos;
        readsize = (leftsize > (*a_size)) ? (*a_size) : leftsize;
        kvfs_seek(a_file, curpos, SEEK_SET);

        *a_buf = ksal_mem_alloc(readsize);
        *a_size = fread(*a_buf, 1, readsize, a_file);
    } else {
        *a_size = fread(*a_buf, 1, *a_size, (FILE*)a_file);
    }

    return *a_size;
}

kint kvfs_write(kbean a_file, kvoid *a_buf, kint a_size)
{
	if(a_file){
		FILE *fp = (FILE*)a_file;
		return fwrite(a_buf, 1, a_size, (FILE*)a_file);
	}else{
		return 0;
	}
}

kint kvfs_length(kbean file)
{
	kint len = -1, pos;
	if (file) {
        pos = kvfs_tell(file);
		kvfs_seek(file, 0, SEEK_END);
		len = kvfs_tell(file);
		kvfs_seek(file, pos, SEEK_SET);
	}
	return len;
}

kint kvfs_printf(kbean a_file, kchar *a_fmt, ...)
{
	va_list arg;
	kint done;

	va_start(arg, a_fmt);
	done = vfprintf((FILE*)a_file, a_fmt, arg);
	va_end(arg);

	return done;
}

kint kvfs_seek(kbean a_file, kint a_offset, kint a_origin)
{
	if(a_file){
		FILE *fp = (FILE*)a_file;
		return fseek(fp, a_offset, a_origin);
	}
	return 0;
}

kint kvfs_tell(kbean a_file)
{
	if(a_file){
		FILE *fp = (FILE*)a_file;
		return ftell(fp);
	}
	return 0;
}

kint kvfs_remove(const kchar *a_path)
{
	WCHAR wpath[MAX_PATH];
	MultiByteToWideChar(CP_OEMCP, 0, a_path, -1, wpath, MAX_PATH );

	return DeleteFile(wpath);
}

kint kvfs_chdir(const kchar *a_path) // no current work directoy concept  in WINCE
{
	return 0;
}

kint kvfs_cpdir(const kchar *a_srcpath, const kchar *a_dstpath)
{
	kchar tmppath[1024], targetPath[1024];
    KVFS_FINDDATA fInfo;
    kbean fd = 0;

    kvfs_mkdir(a_dstpath);

    fd = kvfs_findfirst(a_srcpath, &fInfo);
    if (fd) {
        do {
            if (0 != strcmp(fInfo.name, ".") && 0 !=strcmp(fInfo.name, "..")) {
				
				
				sprintf(tmppath, "%s%c%s", a_srcpath, '/', fInfo.name);
				sprintf(targetPath, "%s%c%s", a_dstpath, '/', fInfo.name);
				
				if (KVFS_A_SUBDIR & fInfo.attrib) {
					kvfs_mkdir(tmppath);
					kvfs_cpdir(tmppath, targetPath);
					continue;
				}
				
				kvfs_copy(tmppath, targetPath);
			}
        } while (-1 != kvfs_findnext(fd, &fInfo));

        kvfs_findclose(fd);
    }
    return 0;

}

kint kvfs_mkdir(const kchar *a_path)
{
	WCHAR wpath[MAX_PATH];

	MultiByteToWideChar(CP_OEMCP, 0, a_path, -1, wpath, MAX_PATH );
	return CreateDirectory(wpath,NULL);
}

kint kvfs_rmdir(const kchar *a_path)
{
	WCHAR wpath[MAX_PATH];

	MultiByteToWideChar(CP_OEMCP, 0, a_path, -1, wpath, MAX_PATH );
	return RemoveDirectory(wpath);
}

kint kvfs_cwd(kchar *a_buf, kint a_size) // no current work dir concept in WINCE
{
	return 0;
}

kbean kvfs_findfirst(const kchar *a_fspec, KVFS_FINDDATA *a_finfo)
{
    kchar fspec[MAX_PATH];
	WIN32_FIND_DATA fileinfo;
	WCHAR wfSpec[MAX_PATH];
	char tmppath[MAX_PATH];
	HANDLE hfile;
	int result;
	DWORD temp;

    sprintf(fspec, "%s\\*.*", a_fspec);

	MultiByteToWideChar(CP_OEMCP, 0, fspec, -1, wfSpec, MAX_PATH );//CP_OEMCP:ok

	hfile = FindFirstFile(wfSpec, &fileinfo);
	if (hfile != INVALID_HANDLE_VALUE) {
		a_finfo->attrib = fileinfo.dwFileAttributes;

		result = WideCharToMultiByte(CP_OEMCP, NULL, fileinfo.cFileName, -1, tmppath, MAX_PATH,NULL,NULL );//wfSpec, should be char *, to save mem
											//MUST be CP_OEMCP, if CP_OEMCP will get error
		if(!result)
			temp =  GetLastError();
		strncpy(a_finfo->name, tmppath, strlen(tmppath)+1); //+1: NULL_teminated
		a_finfo->size = (kint)fileinfo.nFileSizeLow; //nFileSizeHigh is discard for use small size file only now
		return (kint)hfile;
	} else {
		//no more file
		DWORD error;
		error=GetLastError();
		return 0;
	}
}

kint kvfs_findnext(kbean a_find, KVFS_FINDDATA *a_finfo)
{
	WIN32_FIND_DATA fileinfo;
	char fSpec[MAX_PATH];
	int result;
	DWORD temp;
	kbean fHandle = a_find;


	if (FindNextFile((HANDLE)fHandle, &fileinfo)) {
		a_finfo->attrib = fileinfo.dwFileAttributes;

		result = WideCharToMultiByte(CP_OEMCP, NULL, fileinfo.cFileName, -1, fSpec, MAX_PATH,NULL,NULL );
		if(!result)
			temp =  GetLastError();
		strncpy(a_finfo->name, fSpec, strlen(fSpec)+1);//+1: NULL_teminated
		a_finfo->size = (kint)fileinfo.nFileSizeLow;//nFileSizeHigh is discard for use small size file only now
		return 0;
	} else {
		//no more file
		DWORD error;
		error=GetLastError();
		return -1;
	}
}

kint kvfs_findclose(kbean a_find)
{
	if(a_find)
		return FindClose((HANDLE)a_find);
	return 0;
}


//kint kvfs_length(kbean file)
//{
//    kint len = -1, pos = Iv_FS_ftell(file);
//    if (file) {
//        Iv_FS_fseek(file, 0, SEEK_END);
//        len = Iv_FS_ftell(file);
//        Iv_FS_fseek(file, pos, SEEK_SET);
//    }
//    return len;
//}

//kint kvfs_read(kbean file, kchar **retbuf, kuint *retlen)
//{
//    kint pos = Iv_FS_ftell(file);
//    Iv_FS_fseek(file, 0, SEEK_END);
//    *retlen = Iv_FS_ftell(file);
//    *retbuf = ksal_mem_alloc(*retlen);
//    Iv_FS_fseek(file, 0, SEEK_SET);
//    Iv_FS_fread(*retbuf, 1, *retlen, file);
//    Iv_FS_fseek(file, pos, SEEK_SET);
//    return 0;
//}
//
// 文件相关操作
//

kchar Iv_FS_getDirDelimeter(void)
{
    return '\\';
}
kint  Iv_FS_exist(const kchar *path)
{
    WCHAR wpath[MAX_PATH];
    MultiByteToWideChar(CP_OEMCP, 0, path, -1, wpath, MAX_PATH );

    if((GetFileAttributes(wpath)) != 0xFFFFFFFF) {//0xFFFFFFFF failure
        return 1;
    }
    return 0;
}

kint  Iv_FS_fopen(kchar *filename, kchar *mode)
{
    if(filename)
        return (kint)fopen(filename, mode);
	return 0;
}
kint  Iv_FS_fclose(kint hFile)
{
    if(hFile)
        return fclose((FILE*)hFile);
	return 0;
}
kint  Iv_FS_fread(void *buf, kint size, kint count, kint hFile)
{
    if(hFile){
        FILE *fp = (FILE*)hFile;
        return fread(buf, size, count, fp);
    }else{
        return 0;
    }

}
kint  Iv_FS_fwrite(void *buf, kint size, kint count, kint hFile)
{
    if(hFile){
        FILE *fp = (FILE*)hFile;
        return fwrite(buf, size, count, fp);
    }else{
        return 0;
    }

}
kint  Iv_FS_fprintf(kint hFile, kchar *format, ...)
{
    va_list arg;
    int done;

    va_start(arg, format);
    done = vfprintf(hFile, format, arg);
    va_end(arg);

    return done;
}
kint  Iv_FS_fseek(kint hFile, kint offset, kint origin)
{
    if(hFile){
        FILE *fp = (FILE*)hFile;
        return fseek(fp, offset, origin);
    }
	return 0;
}
kint  Iv_FS_ftell(kint hFile)
{
    if(hFile){
        FILE *fp = (FILE*)hFile;
        return ftell(fp);
    }
	return 0;
}

kint  Iv_FS_remove(const kchar *path)
{
    WCHAR wpath[MAX_PATH];
    MultiByteToWideChar(CP_OEMCP, 0, path, -1, wpath, MAX_PATH );

    return DeleteFile(wpath);
}

kint  Iv_FS_chdir(const kchar *dirname) // no current work directoy concept  in WINCE
{
    return 0;
}

//create a new dir
kint  Iv_FS_mkdir(const kchar *dirname)
{
    WCHAR wpath[MAX_PATH];

    MultiByteToWideChar(CP_OEMCP, 0, dirname, -1, wpath, MAX_PATH );
    return CreateDirectory(wpath,NULL);
}

//delete a empty dir
kint  Iv_FS_rmdir(const kchar *dirname)
{
    WCHAR wpath[MAX_PATH];

    MultiByteToWideChar(CP_OEMCP, 0, dirname, -1, wpath, MAX_PATH );
    return RemoveDirectory(wpath);
}

kint  Iv_FS_findfirst(const kchar *fSpec, IV_FINDDATA *fInfo){
    WIN32_FIND_DATA fileinfo;
    WCHAR wfSpec[MAX_PATH];
    char tmppath[MAX_PATH];
    HANDLE hfile;
    int result;
    DWORD temp;

    MultiByteToWideChar(CP_OEMCP, 0, fSpec, -1, wfSpec, MAX_PATH );//CP_OEMCP:ok

    hfile = FindFirstFile(wfSpec, &fileinfo);
    if (hfile != INVALID_HANDLE_VALUE) {
        fInfo->attrib = fileinfo.dwFileAttributes;

        result = WideCharToMultiByte(CP_OEMCP, NULL, fileinfo.cFileName, -1, tmppath, MAX_PATH,NULL,NULL );//wfSpec, should be char *, to save mem
        //MUST be CP_OEMCP, if CP_OEMCP will get error
        if(!result)
            temp =  GetLastError();
        fInfo->name = kstr_dup(tmppath);
        fInfo->size = fileinfo.nFileSizeLow; //nFileSizeHigh is discard for use small size file only now
        return (kint)hfile;
    } else {
        //no more file
        DWORD error;
        error=GetLastError();
        return 0;
    }
}

kint  Iv_FS_findnext(kint hFind, IV_FINDDATA *fInfo){
    WIN32_FIND_DATA fileinfo;
    char fSpec[MAX_PATH];
    int result;
    DWORD temp;

    if (FindNextFile((HANDLE)hFind, &fileinfo)) {
        fInfo->attrib = fileinfo.dwFileAttributes;

        result = WideCharToMultiByte(CP_OEMCP, NULL, fileinfo.cFileName, -1, fSpec, MAX_PATH,NULL,NULL );
        if(!result)
            temp =  GetLastError();
        fInfo->name = kstr_dup(fSpec);
        fInfo->size = fileinfo.nFileSizeLow;//nFileSizeHigh is discard for use small size file only now
        return 0;
    } else {
        //no more file
        DWORD error;
        error=GetLastError();
        return -1;
    }
}

kint  Iv_FS_findclose(kint hFind){
        if(hFind)
            return FindClose((HANDLE)hFind);
		return 0;
    }

kint  Iv_FS_getcwd(kchar *buffer, kint size){ // no current work dir concept in WINCE
    return 0;
}

// for
//#include <windows.h>

#ifdef _WIN32_WCE


kulong Get1970ToCurSecond(int nYearTo, int nMonthTo, int nDayTo, int nHourTo, int nMinuteTo, int nSecondTo)
{
    kuint nLeapYear = 0, nYear, nMonth;
    kulong nYear_Second;
    kuint nMonth_Day;
    kulong nMonth_Second ;
    kulong nDay_Second;
    kulong nHour_Sencond;
    kulong nMinute_Second;
    kulong nSecond_Second;


    for (nYear=1970; nYear<nYearTo; nYear++)
    {
        if ((nYear % 4 == 0 && nYear % 100 != 0) || (nYear % 400 == 0))
            nLeapYear++;
    }
    nYear_Second = (kulong)((nYearTo - 1970 - nLeapYear) * 365 + nLeapYear * 366) * 24 * 3600;

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

time_t time( time_t *inTT ) {//UTC, since 1970.1.1.0


    //Method 1:
    //Note: FILETIME: since jan.1,1601, in 100nanosecond
    //can use Get1900ToCurSecond() to implement

    //SYSTEMTIME sysTimeStruct;
    //FILETIME fTime;
    //ULARGE_INTEGER int64time;
    //time_t locTT = 0;
    //
    //if ( inTT == NULL ) {
    //  inTT = &locTT;
    //}
    //
    //GetSystemTime( &sysTimeStruct );
    //if ( SystemTimeToFileTime( &sysTimeStruct, &fTime ) ) {
    //  memcpy( &int64time, &fTime, sizeof( FILETIME ) );
    //  /* Subtract the value for 1970-01-01 00:00 (UTC) */
    //  int64time.QuadPart -= 0x19db1ded53e8000;
    //  /* Convert to seconds. */
    //  int64time.QuadPart /= 10000000;
    //  *inTT = int64time.QuadPart;
    //}
    //
    //return *inTT;

    //Method 2:
    SYSTEMTIME sysTime;
    GetSystemTime( &sysTime);
#ifdef WINCE501_EMULATOR  //WINCE5.0 emu's systemtime is more than 16 hours that real one.
    *inTT = (time_t)Get1970ToCurSecond(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond) - 57600;
#else
    *inTT = (time_t)Get1970ToCurSecond(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
#endif
    return *inTT;
}


char *strerror(int errnum) {
    return 0;
}

#if 0 //no use now
int GetWeekDay(int _Year, int _Month, int _Day)
{
    int Days[12] = {31,59,90,120,151,181,212,243,273,304,334,365};
    long n = (_Year-1L)*365 + _Day +(_Year-1L)/4 -(_Year-1L)/100 +(_Year-1L)/400;
    if(_Month>1)
        n+=Days[_Month-2];
    if((_Month>2)&&(_Year%4==0&&_Year%100!=0||_Year%400==0))
        n++;
    return   n   %   7;
}


void SystemTimeToTime_t(SYSTEMTIME *systemTime, time_t *dosTime)  {
    LARGE_INTEGER jan1970FT = {0};
    LARGE_INTEGER utcFT = {0};
    unsigned __int64 utcDosTime ;

    jan1970FT.QuadPart = 116444736000000000I64; // january 1st 1970
    SystemTimeToFileTime(systemTime, (FILETIME*)&utcFT);
    utcDosTime = (utcFT.QuadPart - jan1970FT.QuadPart)/10000000;
    *dosTime = (time_t)utcDosTime;
}



void Time_tToSystemTime(time_t dosTime, SYSTEMTIME *systemTime)  {
    LARGE_INTEGER jan1970FT = {0};
    LARGE_INTEGER utcFT = {0};
    jan1970FT.QuadPart = 116444736000000000I64; // january 1st 1970

    utcFT.QuadPart = ((unsigned __int64)dosTime)*10000000 + jan1970FT.QuadPart;
    FileTimeToSystemTime((FILETIME*)&utcFT, systemTime);
}
#endif

#endif
