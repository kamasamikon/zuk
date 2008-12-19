#include "ivisa_plfm.h"
#include <stdio.h>
#include <time.h>
#include <kdbg.h>

#include <Windows.h>
#include <io.h>

//
// 得到系统启动后的毫秒数
//
kulong Iv_OS_GetMilliSec(void)
{
    return GetTickCount();
}

//
// 得到当前本地时间
//
kuint ksal_cur_time(void)
{
    time_t curtime;
    time(&curtime);
    return curtime;
}

//
// 任务操作函数
//
kuint Iv_OS_TaskCreate(IV_TASK_PROC entrypoint, kuchar priority, kuint stackSize, void *param)
{
    kuint ThreadId;
    CreateThread(NULL, stackSize, (LPTHREAD_START_ROUTINE)entrypoint, param, 0, &ThreadId);
    return ThreadId;
}

kint  Iv_OS_TaskDestroy(kuint taskId)
{
    return CloseHandle(taskId);
}
kuint Iv_OS_GetCurrentTask()
{
    kuint tskid = GetCurrentThreadId();
    return tskid;
}


//
// 信号量
//
kint Iv_OS_SemaphoreCreate(kint initValue)
{
    return CreateSemaphore(NULL, initValue, 2000, NULL);
}
kint Iv_OS_SemaphoreDestroy(kint semaphId)
{
    CloseHandle(semaphId);
    return 0;
}
kint Iv_OS_SemaphoreRetrieve(kint semaphId, kint timeout)
{
    return WaitForSingleObject(semaphId, timeout);
}
kint Iv_OS_SemaphoreRelease(kint semaphId)
{
    kulong oldcnt;
    kint ret;
    ret = ReleaseSemaphore(semaphId, 1, &oldcnt);
    if (0 == ret) {
        kerror(("ReleaseSemaphore:gle:%d\n", GetLastError()));
    }
    return 0;
}

//
// 动态内存管理
//
void* Iv_OS_MemAlloc(kuint size, kuint align)
{
    return malloc(size);
}
void Iv_OS_MemFree(void * ptr)
{
    free(ptr);
}

//
// 任务休眠
//
void Iv_OS_Sleep(kuint msec)
{
    Sleep(msec);
}

//
// 文件相关操作
//

kchar Iv_FS_getDirDelimeter(void)
{
    return '\\';
}
kint  Iv_FS_exist(const kchar *path)
{
    if (_access(path, 0) != -1) {
        return 1;
    }
    return 0;
}

kint  Iv_FS_fopen(kchar *filename, kchar *mode)
{
    return fopen(filename, mode);
}
kint  Iv_FS_fclose(kint hFile)
{
    return fclose(hFile);
}
kint  Iv_FS_fread(void *buf, kint size, kint count, kint hFile)
{
    FILE *fp = (FILE*)hFile;
    return fread(buf, size, count, fp);
}
kint  Iv_FS_fwrite(void *buf, kint size, kint count, kint hFile)
{
    FILE *fp = (FILE*)hFile;
    return fwrite(buf, size, count, fp);
}
kint  Iv_FS_fprintf(kint hFile, kchar *format, ...)
{
    return 0;
}
kint  Iv_FS_fseek(kint hFile, kint offset, kint origin)
{
    FILE *fp = (FILE*)hFile;
    return fseek(fp, offset, origin);
}
kint  Iv_FS_ftell(kint hFile)
{
    FILE *fp = (FILE*)hFile;
    return ftell(fp);
}

kint  Iv_FS_remove(const kchar *path)
{
    return remove(path);
}

kint  Iv_FS_chdir(const kchar *dirname)
{
    return _chdir(dirname);
}
kint  Iv_FS_mkdir(const kchar *dirname)
{
    return _mkdir(dirname);
}
kint  Iv_FS_rmdir(const kchar *dirname)
{
    return _rmdir(dirname);
}

kint  Iv_FS_findfirst(const kchar *fSpec, IV_FINDDATA *fInfo){
    struct _finddata_t fileinfo;
    kint hfile = _findfirst(fSpec, &fileinfo);
    if (hfile != -1) {
        fInfo->attrib = fileinfo.attrib;
        fInfo->name = kspt_strdup(fileinfo.name);
        fInfo->size = fileinfo.size;
        return hfile;
    } else {
        return 0;
    }
}

kint  Iv_FS_findnext(kint hFind, IV_FINDDATA *fInfo){
    struct _finddata_t fileinfo;
    if (!_findnext(hFind, &fileinfo)) {
        fInfo->attrib = fileinfo.attrib;
        fInfo->name = kspt_strdup(fileinfo.name);
        fInfo->size = fileinfo.size;
        return 0;
    } else {
        return -1;
    }
}

kint  Iv_FS_findclose(kint hFind){
    return _findclose(hFind);
}

kint  Iv_FS_getcwd(kchar *buffer, kint size){
    return getcwd(buffer, size);
}
