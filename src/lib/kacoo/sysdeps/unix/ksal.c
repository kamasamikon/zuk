#include <ksal.h>

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

#include <stdarg.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/vfs.h>

kvoid kdbgbrk(kvoid)
{
    asm("int $0x03");
}
kint kprintf(const kchar *format, ...)
{
    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf (stderr, format, arg);
    va_end (arg);

    return done;
}

kint ksal_get_commandline(kchar *buffer, kint buflen)
{
    char path[80];
    kbean file;

    sprintf(path, "/proc/%d/cmdline", getpid());
    file = kvfs_open(path, "rt", 0);
    if (file) {
        kvfs_read(file, (kvoid**)&buffer, &buflen);
        kvfs_close(file);
    }

    return 0;
}

/*
 * return millisecond
 */
kulong ksal_get_tick(kvoid)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

/*
 * UTC time 1970/01/01/00/00 with second
 */
kuint ksal_cur_time(kvoid)
{
    time_t curtime;
    time(&curtime);
    return curtime;
}

kvoid ksal_tsk_exit(kvoid)
{
    pthread_exit(0);
}

kbean ksal_tsk_new(kvoid (*entrypoint)(kvoid), kuchar priority, kuint stackSize, kvoid *param)
{
    pthread_t threadId;
    if (pthread_create(&threadId, NULL, entrypoint, (kvoid*)param)) {
        return 0;
    }
    return (kbean)threadId;
}

kint ksal_tsk_del(kbean taskId)
{
    return pthread_cancel(taskId);
}
kbean ksal_tsk_cur(kvoid)
{
    kuint tskid = pthread_self();
    return (kbean)tskid;
}

kint ksal_mem_init(kuint a_flg)
{
    return 0;
}
kint ksal_mem_final(kvoid)
{
    return 0;
}
kvoid* ksal_mem_alloc(kuint size)
{
    return malloc(size);
}
kvoid ksal_mem_free(kvoid * ptr)
{
    free(ptr);
}

kvoid ksal_tsk_sleep(kuint msec)
{
    usleep(msec * 1000);
}

kbean ksal_sema_new(kint initValue)
{
    sem_t *sema = ksal_mem_alloc(sizeof(sem_t));
    memset(sema, 0, sizeof(sem_t));
    sem_init(sema, 1, initValue);
    return sema;
}
kint ksal_sema_del(kbean semaphId)
{
    sem_destroy(semaphId);
    ksal_mem_free(semaphId);
    return 0;
}
kint ksal_sema_get(kbean semaphId, kint timeout)
{
    struct timespec ts;
    int value = -1, ret;

    // sem_getvalue(semaphId, &value);
    if (timeout < 0) {
        // printf("QQQ ksal_sema_get:%x:%d:%d\n", semaphId, value, timeout);
        while ((ret = sem_wait(semaphId)) == -1 && errno == EINTR) {
            continue;       /* Restart when interrupted by handler */
        }
        // sem_getvalue(semaphId, &value);
        // printf("HHH \tksal_sema_get:%x:%d:ret(%d), err(%d)\n", semaphId, value, ret, errno);
        return (0 == ret) ? 0 : -1;
    } else {
        clock_gettime(CLOCK_REALTIME, &ts);

        // printf("QQQ ksal_sema_get:%x:%d:%d, ts:(%d, %d)\n", semaphId, value, timeout, ts.tv_sec, ts.tv_nsec);

        ts.tv_nsec += (timeout % 1000) * 1000 * 1000;
        ts.tv_sec += (timeout / 1000) + (ts.tv_nsec / 1000000000);

        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;
        }

        // printf("XXX ts:(%d, %d)\n", ts.tv_sec, ts.tv_nsec);

        while ((ret = sem_timedwait(semaphId, &ts)) == -1 && errno == EINTR) {
            continue;       /* Restart when interrupted by handler */
        }

        // sem_getvalue(semaphId, &value);
        // printf("HHH \tksal_sema_get:%x:%d:ret(%d), err(%d)\n", semaphId, value, ret, errno);
        return (0 == ret) ? 0 : (((-1 == ret) && (ETIMEDOUT == errno)) ? 1 : -1);
    }
}
kint ksal_sema_rel(kbean semaphId)
{
    sem_post(semaphId);
    return 0;
}

kint ksal_execvp(const kchar *a_path, kchar *const a_argv[], const kchar *a_workdir, kint a_xiuflg, kbool a_block, kbean *a_retbean)
{
    kchar curdir[1024];
    kint status = 0;
    pid_t pid;

    if (a_retbean) {
        *a_retbean = knil;
    }

    pid = fork();
    if (0 == pid) {
        /* printf("\nchild process, execute the command:path:%s\n", path); */
        /* child process, execute the command */
        if (a_workdir) {
            getcwd(curdir, sizeof(curdir));
            chdir(a_workdir);
        }
        execvp(a_path, a_argv);
        _exit(EXIT_FAILURE);
    } else if (0 > pid) {
        /* fork failed,report failure */
        status = -1;
    }

    if (a_retbean) {
        *a_retbean = (kbean)pid;
    }

    if (0 < pid && a_block) {
        /* parent process, wait the end of child */
        /* printf("\nparent process, wait the end of child\n"); */
        if(waitpid(pid, &status, 0) != pid) {
            status = -1;
        }
        if (a_retbean) {
            *a_retbean = knil;
        }
    }
    if (status) {
        return -1;
    } else {
        return 0;
    }
}
kint ksal_kill(kbean a_pid, kuint a_sig)
{
    return kill(a_pid, a_sig);
}

kvoid ksal_exedir(kchar *a_rundir, kchar **a_argv, kchar *a_exedir)
{
    kbool fullpath;
    char *pspos;

    kchar *ps = "/";

    fullpath = (kbool)(a_argv[0][0] == '/');

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
    return '/';
}

/**
 * @brief is this path, include file or directory exist
 */
kbool kvfs_exist(const kchar *a_path)
{
    if (-1 != access(a_path, 0)) {
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

    result = -1;
    src_fd = open(a_srcfile, O_RDONLY, 0666);
    if (src_fd < 0)
        goto done;
    dst_fd = open(a_dstfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
    return chdir(a_path);
}

kint kvfs_cpdir(const kchar *a_srcpath, const kchar *a_dstpath)
{
    kchar tmppath[1024] = "", targetPath[1024] = "";
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
    kint i, ret = -1;
    kchar path[1024];
    strcpy(path, a_path);

    ret = mkdir(a_path, S_IRWXU);
    if (ret) {
        for (i = 0; path[i]; i++) {
            if ('/' == path[i]) {
                path[i] = '\0';
                /* S_IRWXU: read,write,execute/search by owner */
                mkdir(path, S_IRWXU);
                path[i] = '/';
            }
        }
        ret = mkdir(a_path, S_IRWXU);
    }
    return ret;
}

kint kvfs_rmdir(const kchar *a_path)
{
    kint ret, len;
    kchar curpath[1024], tmppath[1024];
    DIR *dir;
    struct dirent *dirp;

    ret = rmdir(a_path);
    if (ret) {
        strcpy(curpath, a_path);
        len = strlen(curpath);
        if (curpath[len] == '/') {
            curpath[len] = '\0';
        }

        dir = opendir(a_path);
        if (!dir) {
            return -1;
        }

        dirp = readdir(dir);
        while (dirp) {
            if (DT_DIR & dirp->d_type) {
                if ((!strcmp(dirp->d_name, ".")) || (!strcmp(dirp->d_name, ".."))) {
                    dirp = readdir(dir);
                    continue;
                }
                sprintf(tmppath, "%s/%s", curpath, dirp->d_name);
                kvfs_rmdir(tmppath);
            } else {
                sprintf(tmppath, "%s/%s", curpath, dirp->d_name);
                remove(tmppath);
            }
            dirp = readdir(dir);
        }
        closedir(dir);

        ret = rmdir(a_path);
    }
    return ret;
}

kint kvfs_cwd(kchar *a_buf, kint a_size)
{
    return getcwd(a_buf, a_size) ? 0 : -1;
}

kint kvfs_dsk_free(kchar *a_path)
{
    long long space;
    struct statfs stat;
    memset(&stat, 0, sizeof(struct statfs));
    if (0 == statfs(a_path, &stat)) {
        space = (stat.f_bavail * (stat.f_bsize / 1024) / 1024);
        return (kint)space;
    }
    return -1;
}

kbean kvfs_findfirst(const kchar *a_fspec, KVFS_FINDDATA *a_finfo)
{
    DIR *dir;
    struct dirent *dirp;

    dir = opendir(a_fspec);
    if (!dir) {
        return knil;
    }

    dirp = readdir(dir);
    if (!dirp) {
        closedir(dir);
        return knil;
    }

    a_finfo->attrib = (DT_DIR & dirp->d_type) ? KVFS_A_SUBDIR : 0;
    strncpy(a_finfo->name, dirp->d_name, sizeof(a_finfo->name) - 1);
    a_finfo->name[sizeof(a_finfo->name) - 1] = '\0';
    a_finfo->size = 0;

    return (kbean)dir;
}

kint kvfs_findnext(kbean a_find, KVFS_FINDDATA *a_finfo)
{
    DIR *dir = (DIR*)a_find;
    struct dirent *dirp;

    dirp = readdir(dir);
    if (!dirp) {
        return -1;
    }

    a_finfo->attrib = (DT_DIR & dirp->d_type) ? KVFS_A_SUBDIR : 0;
    strncpy(a_finfo->name, dirp->d_name, sizeof(a_finfo->name) - 1);
    a_finfo->name[sizeof(a_finfo->name) - 1] = '\0';
    a_finfo->size = 0;

    return 0;
}

kint kvfs_findclose(kbean a_find)
{
    DIR *dir = (DIR*)a_find;
    if (dir) {
        closedir(dir);
        return 0;
    }
    return -1;
}

