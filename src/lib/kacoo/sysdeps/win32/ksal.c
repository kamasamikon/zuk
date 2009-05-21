/* vim:set et sw=4 sts=4 ff=unix: */
#include <ksal.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <io.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define UNIX_PATH(str)

kint kprintf(const kchar *format, ...)
{
    kchar buffer[2048];
    va_list args;

    va_start(args, format);
    _vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    OutputDebugStringA(buffer);
    return 0;
}

kvoid kdbgbrk(kvoid)
{
    // __asm int 3
}

kint ksal_get_commandline(kchar *buffer, kint buflen)
{
    int i, slen;
    kchar *ptr = buffer;
    for (i = 0; i < __argc; i++) {
        slen = strlen(__argv[i]);

        if (ptr - buffer + slen > buflen) {
            break;
        }

        strcat(ptr, __argv[i]);
        ptr += slen;
        *ptr = '\0';

        ptr++;
    }

    return 0;
}

kulong ksal_get_tick(void)
{
    return GetTickCount();
}

kuint ksal_cur_time(void)
{
    time_t curtime;
    time(&curtime);
    return (kuint)curtime;
}

kvoid ksal_tsk_exit(kvoid)
{
    ExitThread(0);
}

kbean ksal_tsk_new(kvoid (*entrypoint)(kvoid), kuchar priority, kuint stackSize, void *param)
{
    kuint ThreadId;
#if 10
    CreateThread(NULL, stackSize, (LPTHREAD_START_ROUTINE)entrypoint, param, 0, &ThreadId);
#else  // better security thread func.
    _beginthreadex(NULL, 0, (void *)entrypoint, NULL, 0, &ThreadId);
#endif
    return (kbean)ThreadId;
}

kint ksal_tsk_del(kbean taskId)
{
#if 0
    CloseHandle(taskId)
#endif
    return 0;
}
kbean ksal_tsk_cur(kvoid)
{
    kuint tskid = GetCurrentThreadId();
    return (kbean)tskid;
}

/*
 * Memory
 */
static kint __g_mem_alloc_cnt = 0;          /**< count to call malloc */
static kint __g_mem_free_cnt = 0;           /**< count to call free */

static kint __g_mem_peak = 0;               /**< max ever has */
static kint __g_mem_cur = 0;                /**< current allocated */

void show_mem_stat()
{
    kprintf("SG_MEM:: allCnt:%d, freeCnt:%d, peak:%d, curr:%d\n",
            __g_mem_alloc_cnt, __g_mem_free_cnt, __g_mem_peak, __g_mem_cur);
}

static kbool __g_mem_track = kfalse;
static kbool __g_mem_check = kfalse;

kint ksal_mem_init(kuint a_flg)
{
    if (a_flg & KMF_TRACK) {
        __g_mem_track = ktrue;
    }
    if (a_flg & KMF_CHECK) {
        __g_mem_check = ktrue;
    }
    return 0;
}
kint ksal_mem_final(kvoid)
{
    kprintf("into ksal_mem_final\n");
    show_mem_stat();
    return 0;
}

#define MEM_MGC0                    0x34
#define MEM_MGC1                    0xED
#define MEM_MGC2                    0x85
#define MEM_MGC3                    0xC9

void* ksal_mem_alloc(kuint size)
{
    kuchar *ptr;

    if (__g_mem_check) {
        /* [4byte: magic][4byte:allSize][Nbyte:memory][4byte: magic][4byte:allSize] */
        size += 16;
        ptr = malloc(size);

        /* premagic */
        ptr[0] = MEM_MGC0;
        ptr[1] = MEM_MGC1;
        ptr[2] = MEM_MGC2;
        ptr[3] = MEM_MGC3;

        /* preallSize */
        ptr[4] = (size & 0xFF000000) >> 24;
        ptr[5] = (size & 0x00FF0000) >> 16;
        ptr[6] = (size & 0x0000FF00) >> 8;
        ptr[7] = (size & 0x000000FF) >> 0;

        /* postmagic */
        ptr[size - 8 + 0] = MEM_MGC0;
        ptr[size - 8 + 1] = MEM_MGC1;
        ptr[size - 8 + 2] = MEM_MGC2;
        ptr[size - 8 + 3] = MEM_MGC3;

        /* postallSize */
        ptr[size - 8 + 4] = (size & 0xFF000000) >> 24;
        ptr[size - 8 + 5] = (size & 0x00FF0000) >> 16;
        ptr[size - 8 + 6] = (size & 0x0000FF00) >> 8;
        ptr[size - 8 + 7] = (size & 0x000000FF) >> 0;

        ptr += 8;
    } else {
        ptr = malloc(size);
    }

    if (__g_mem_track) {
        __g_mem_alloc_cnt++;
        __g_mem_cur += size;
        if (__g_mem_cur > __g_mem_peak) {
            __g_mem_peak = __g_mem_cur;
        }
        if (!(__g_mem_alloc_cnt % 100)) {
            show_mem_stat();
        }
    }

    return ptr;
}

kvoid ksal_mem_free(kvoid *a_ptr)
{
    kuint size;
    kchar *ptr = (kchar*)a_ptr;

    if (!ptr) {
        return;
    }

    if (__g_mem_check) {
        /* [4byte: magic][4byte:allSize][Nbyte:memory][4byte: magic][4byte:allSize] */
        ptr -= 8;
        size = _msize(ptr);

        if (
                (ptr[0] != MEM_MGC0) || (ptr[1] != MEM_MGC1) || (ptr[2] != MEM_MGC2) || (ptr[3] != MEM_MGC3) ||

                (ptr[4] != (size & 0xFF000000) >> 24) || (ptr[5] != (size & 0x00FF0000) >> 16) ||
                (ptr[6] != (size & 0x0000FF00) >> 8) || (ptr[7] != (size & 0x000000FF) >> 0) ||

                (ptr[size - 8 + 0] != MEM_MGC0) || (ptr[size - 8 + 1] != MEM_MGC1) ||
                (ptr[size - 8 + 2] != MEM_MGC2) || (ptr[size - 8 + 3] != MEM_MGC3) ||

                (ptr[size - 8 + 4] != (size & 0xFF000000) >> 24) || (ptr[size - 8 + 5] != (size & 0x00FF0000) >> 16) ||
                (ptr[size - 8 + 6] != (size & 0x0000FF00) >> 8) || (ptr[size - 8 + 7] != (size & 0x000000FF) >> 0)
           ) {
            kprintf("ksal_mem_free: memory check failed\n");
            kdbgbrk();
        }

        free(ptr);

    } else {
        if (__g_mem_track) {
            size = _msize(ptr);
        }
        free(ptr);
    }

    if (__g_mem_track) {
        __g_mem_free_cnt++;
        __g_mem_cur -= size;
        if (!(__g_mem_free_cnt % 100)) {
            show_mem_stat();
        }
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

kint ksal_exec(kint a_xiuflg, kbool a_block, kbean *a_retbean, const kchar *a_args, ...)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    CHAR szCmdLine[4096] = "";
    kint i, ret;

    kchar *arg;
    va_list args;

    // 2, retrieve the data
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = a_xiuflg ? SW_SHOW : SW_HIDE;

    va_start(args, a_args);
    while (1) {
        arg = va_arg(args, kchar*);
        if (!arg) {
            break;
        }
        strcat(szCmdLine, "\"");
        strcat(szCmdLine, arg);
        strcat(szCmdLine, "\"");
        strcat(szCmdLine, " ");
    }
    va_end(args);

    if (a_retbean) {
        *a_retbean = knil;
    }
    if (!CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi)) {
        return -1;
    }
    if (a_retbean) {
        *a_retbean = (kbean)pi.hProcess;
    }
    if (NULL != pi.hProcess && a_block) {
        if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess, INFINITE)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            ret = 0;
        } else {
            ret = -1;
        }

        if (a_retbean) {
            *a_retbean = knil;
        }
    }
    return ret;
}

kint ksal_kill(kbean a_pid, kuint a_sig)
{
    TerminateProcess((HANDLE)a_pid, -1);
    return 0;
}

kvoid ksal_exedir(kchar *a_rundir, kchar **a_argv, kchar *a_exedir)
{
    kbool fullpath;
    char *pspos;

    kchar *ps = "\\";

    fullpath = (kbool)strchr(a_argv[0], ':');

    if (fullpath) {
        strcpy(a_exedir, a_argv[0]);
    } else {
        strcpy(a_exedir, a_rundir);
        strcat(a_exedir, ps);
        strcat(a_exedir, a_argv[0]);
    }
    pspos = strrchr(a_exedir, ps[0]);
    if (pspos) {
        *pspos = '\0';
    }
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
    kint readsize, leftsize, curpos;

    if (!(*a_buf)) {
        curpos = kvfs_tell(a_file);
        kvfs_seek(a_file, 0, SEEK_END);
        leftsize = kvfs_tell(a_file) - curpos;
        readsize = (leftsize > (*a_size)) ? (*a_size) : leftsize;
        kvfs_seek(a_file, curpos, SEEK_SET);

        *a_buf = ksal_mem_alloc(readsize + 1);
        ((kchar*)(*a_buf))[readsize] = '\0';
        *a_size = fread(*a_buf, 1, readsize, a_file);
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

kint kvfs_copy(const kchar *a_srcfile, const kchar *a_dstfile)
{
    int    src_fd;
    int    dst_fd;
    char    *buffer;
    long    max;
    long    nbytes;
    long    nbytes2;
    int    err;
    int    result;

    kchar srcfile[1024], dstfile[1024];
    strcpy(srcfile, a_srcfile);
    strcpy(dstfile, a_dstfile);
    UNIX_PATH(srcfile);
    UNIX_PATH(dstfile);


    result = -1;
    src_fd = open(srcfile, O_RDONLY, 0666);
    if (src_fd < 0)
        goto done;
    dst_fd = open(dstfile, O_WRONLY | _O_CREAT | _O_TRUNC, 0666);
    if (dst_fd < 0)
    {
        err = errno;
        close(src_fd);
        errno = err;
        goto done;
    }

    max = 1L << 13;
    errno = 0;
    buffer = (char *)malloc(max);
    if (!buffer)
    {
        err = errno ? errno : ENOMEM;
        close(dst_fd);
        close(src_fd);
        errno = err;
        goto done;
    }

    for (;;)
    {
        nbytes = read(src_fd, buffer, max);
        if (nbytes < 0)
        {
            err = errno;
            close(src_fd);
            close(dst_fd);
            free(buffer);
            errno = err;
            goto done;
        }
        if (nbytes == 0)
            break;

        nbytes2 = write(dst_fd, buffer, nbytes);
        if (nbytes2 < 0)
        {
            err = errno;
            close(src_fd);
            close(dst_fd);
            free(buffer);
            errno = err;
            goto done;
        }
        if (nbytes2 != nbytes)
        {
            close(src_fd);
            close(dst_fd);
            free(buffer);
            errno = EIO; /* weird device, probably */
            goto done;
        }
    }
    free(buffer);
    if (close(src_fd))
    {
        err = errno;
        close(dst_fd);
        errno = err;
        goto done;
    }
    result = close(dst_fd);

    /*
     * here for all exits
     */
done:
    return result;
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

                sprintf(tmppath, "%s%c%s", a_srcpath, '\\', fInfo.name);
                sprintf(targetPath, "%s%c%s", a_dstpath, '\\', fInfo.name);

                if (KVFS_A_SUBDIR & fInfo.attrib) {
                    kvfs_mkdir(tmppath);
                    kvfs_cpdir(tmppath, targetPath);
                    continue;
                }

                CopyFile(tmppath, targetPath, FALSE);
            }
        } while (-1 != kvfs_findnext(fd, &fInfo));

        kvfs_findclose(fd);
    }
    return 0;

}

kint kvfs_mkdir(const kchar *a_path)
{
    kint i, ret = -1;
    kchar path[1024];
    strcpy(path, a_path);

    ret = _mkdir(a_path);
    if (ret) {
        for (i = 0; path[i]; i++) {
            if ('\\' == path[i]) {
                path[i] = '\0';
                /* S_IRWXU: read,write,execute/search by owner */
                _mkdir(path);
                path[i] = '\\';
            }
        }
        ret = _mkdir(a_path);
    }
    return ret;
}

kint kvfs_rmdir(const kchar *a_path)
{
    kchar tmppath[1024];
    KVFS_FINDDATA fInfo;
    kbean fd = 0;

    fd = kvfs_findfirst(a_path, &fInfo);
    if (fd) {
        do {
            if (0 != strcmp(fInfo.name, ".") && 0 !=strcmp(fInfo.name, "..")) {

                sprintf(tmppath, "%s%c%s", a_path, '\\', fInfo.name);

                if (KVFS_A_SUBDIR & fInfo.attrib) {
                    kvfs_rmdir(tmppath);
                    continue;
                }

                kvfs_remove(tmppath);
            }
        } while (-1 != kvfs_findnext(fd, &fInfo));

        _rmdir(a_path);

        kvfs_findclose(fd);
    }
    return 1;
}

kint kvfs_cwd(kchar *a_buf, kint a_size)
{
    return getcwd(a_buf, a_size);
}

kint kvfs_dsk_free(kchar *a_path)
{
    __int64 space;
    kint sec_per_cluster, byte_per_sec, free_cluster, clusters;
    if (GetDiskFreeSpace(a_path, &sec_per_cluster, &byte_per_sec, &free_cluster, &clusters)) {
        space = free_cluster;
        space *= sec_per_cluster;
        space *= byte_per_sec;
        space /= (1024 * 1024);
        return (kint)space;
    }
    return -1;
}

kbean kvfs_findfirst(const kchar *a_fspec, KVFS_FINDDATA *a_finfo)
{
    kchar fspec[300];
    struct _finddata_t fileinfo;
    kint hfile;

    sprintf(fspec, "%s\\*.*", a_fspec);

    hfile = _findfirst(fspec, &fileinfo);
    if (hfile != -1) {
        a_finfo->attrib = fileinfo.attrib;
        strncpy(a_finfo->name, fileinfo.name, sizeof(a_finfo->name) - 1);
        a_finfo->name[sizeof(a_finfo->name) - 1] = '\0';
        a_finfo->size = fileinfo.size;
        return (kbean)hfile;
    } else {
        return (kbean)0;
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

