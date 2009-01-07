/* #include "vld.h" */
#include <ksal.h>
#include <stdio.h>
#include <windows.h>
#include "ivisa_plfm.h"
#include <process.h>
#include <io.h>

kint kprintf(const kchar *format, ...)
{
    kchar buffer[2048];
    va_list args;

    va_start(args, format);
    _vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    OutputDebugString(buffer);
    printf(buffer);
    return 0;
}

kvoid kdbgbrk(kvoid)
{
    __asm int 3
}

kulong ksal_get_tick(void)
{
    return GetTickCount();
}

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

static kint __g_mem_alloc_cnt = 0;          /**< count to call malloc */
static kint __g_mem_free_cnt = 0;           /**< count to call free */

static kint __g_mem_peak = 0;               /**< max ever has */
static kint __g_mem_cur = 0;                /**< current allocated */

void show_mem_stat()
{
    kprintf("SG_MEM:: allCnt:%d, freeCnt:%d, peak:%d, curr:%d\n",
            __g_mem_alloc_cnt, __g_mem_free_cnt, __g_mem_peak, __g_mem_cur);
}

kint ksal_mem_init(kuint a_flg)
{
    return 0;
}
kint ksal_mem_final(kvoid)
{
    kprintf("into ksal_mem_final\n");
    show_mem_stat();
    return 0;
}
void* ksal_mem_alloc(kuint size)
{
    kuchar *ptr;
    size += 4;
    ptr = malloc(size);
    size = _msize(ptr);

    __g_mem_alloc_cnt++;
    __g_mem_cur += size;
    if (__g_mem_cur > __g_mem_peak) {
        __g_mem_peak = __g_mem_cur;
    }
    if (!(__g_mem_alloc_cnt % 100)) {
        show_mem_stat();
    }

    ptr[0] = 0x34;
    ptr[1] = 0x89;
    ptr[2] = 0xa4;
    ptr[3] = 0xd9;

    return ptr + 4;
}
void ksal_mem_free(kuchar * ptr)
{
    kint size;
    if (!ptr) {
        return;
    }
    ptr -= 4;
    size = _msize(ptr);

    if ((ptr[0] != 0x34) || (ptr[1] != 0x89) || (ptr[2] != 0xa4) || (ptr[3] != 0xd9)) {
        kdbgbrk();
    }

    free(ptr);

    __g_mem_free_cnt++;
    __g_mem_cur -= size;
    if (!(__g_mem_free_cnt % 100)) {
        show_mem_stat();
    }
}

void ksal_tsk_sleep(kuint msec)
{
    Sleep(msec);
}

kbean ksal_sema_new(kint initValue)
{
    return CreateSemaphore(NULL, initValue, 20000, NULL);
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

/**
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
    if (-1 != _access(a_path, 0)) {
        return ktrue;
    }
    return kfalse;
}
kbean kvfs_open(const kchar *a_path, const kchar *a_mode, kuint a_flg)
{
    return (kbean)fopen(a_path, a_mode);
}

kint kvfs_close(kbean a_file)
{
    return fclose((FILE*)a_file);
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
    kvoid *buf;
    kint readsize, leftsize, curpos;

    if (!(*a_buf)) {
        curpos = kvfs_tell(a_file);
        kvfs_seek(a_file, 0, SEEK_END);
        leftsize = kvfs_tell(a_file) - curpos;
        readsize = (leftsize > (*a_size)) ? (*a_size) : leftsize;
        kvfs_seek(a_file, curpos, SEEK_SET);

        *a_buf = ksal_mem_alloc(readsize);
        *a_size = fread(*a_buf, 1, readsize, (FILE*)a_file);
    } else {
        *a_size = fread(*a_buf, 1, *a_size, (FILE*)a_file);
    }

    return *a_size;
}

kint kvfs_write(kbean a_file, kvoid *a_buf, kint a_size)
{
    return fwrite(a_buf, 1, a_size, (FILE*)a_file);
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
    return fseek((FILE*)a_file, a_offset, a_origin);
}

kint kvfs_tell(kbean a_file)
{
    return ftell((FILE*)a_file);
}

kint kvfs_remove(const kchar *a_path)
{
    return remove(a_path);
}

kint kvfs_chdir(const kchar *a_path)
{
    return _chdir(a_path);
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
    return _mkdir(a_path);
}

kint kvfs_rmdir(const kchar *a_path)
{
    return _rmdir(a_path);
}

kint kvfs_cwd(kchar *a_buf, kint a_size)
{
    return getcwd(a_buf, a_size);
}

kbean kvfs_findfirst(const kchar *a_fspec, KVFS_FINDDATA *a_finfo)
{
    kchar fspec[300];
    struct _finddata_t fileinfo;
    kint hfile;

    sprintf(fspec, "%s\\*.*", a_fspec);

    hfile = _findfirst(a_fspec, &fileinfo);
    if (hfile != -1) {
        a_finfo->attrib = fileinfo.attrib;
        strncpy(a_finfo->name, fileinfo.name, sizeof(a_finfo->name) - 1);
        a_finfo->name[sizeof(a_finfo->name) - 1] = '\0';
        a_finfo->size = fileinfo.size;
        return hfile;
    } else {
        return 0;
    }
}

kint kvfs_findnext(kbean a_find, KVFS_FINDDATA *a_finfo)
{
    struct _finddata_t fileinfo;
    if (!_findnext(a_find, &fileinfo)) {
        a_finfo->attrib = fileinfo.attrib;
        strncpy(a_finfo->name, fileinfo.name, sizeof(a_finfo->name) - 1);
        a_finfo->name[sizeof(a_finfo->name) - 1] = '\0';
        a_finfo->size = fileinfo.size;
        return 0;
    } else {
        return -1;
    }
}

kint kvfs_findclose(kbean a_find)
{
    return _findclose(a_find);
}
