/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __K_KSAL_H__
#define __K_KSAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#include <ktypes.h>

/**
 * cause a debug break
 */
kvoid kdbgbrk(kvoid);

/**
 * printf like function, to stdio or anywhere else
 */
kint kprintf(const kchar *format, ...);

/**
 * tick count, with millisecond
 */
kulong ksal_get_tick(kvoid);

/**
 * UTC, 1970, second
 */
kuint ksal_cur_time(kvoid);

/**
 * Get applications's command line, argv seperated by '\0'
 */
kint ksal_get_commandline(kchar *buffer, kint buflen);

/**
 * @name thread_api
 * @{
 */
kbean ksal_tsk_new(kvoid(*a_entrypoint) (kvoid), kuchar a_priority, kuint a_stackSize, kvoid *a_param);

/*
 * return when the task completely destroyed
 */
kint ksal_tsk_del(kbean a_taskId);

kbean ksal_tsk_cur(kvoid);
/** @} */

/*
 * quit the current thread and free internal resources
 *
 * XXX, this is the LAST statements, after this function called,
 * all the rest statements after this can not be called.
 */
kvoid ksal_tsk_exit(kvoid);

/**
 * @name memory_api
 * @{
 */

#define KMF_TRACK       0x00000001	    /**< Track the memory usage */
#define KMF_CHECK       0x00000002	    /**< add pading bytes to check mismatch free, and overwrite */

kint ksal_mem_init(kuint a_flg);
kint ksal_mem_final(kvoid);

kvoid *ksal_mem_alloc(kuint a_size);
kvoid ksal_mem_free(kvoid *a_ptr);
/** @} */

kvoid ksal_tsk_sleep(kuint a_msec);

/**
 * @name sync_api
 * @{
 */
kbean ksal_sema_new(kint a_initValue);
kint ksal_sema_del(kbean a_semaphId);
kint ksal_sema_get(kbean a_semaphId, kint a_timeout);
kint ksal_sema_rel(kbean a_semaphId);
/** @} */

/**
 * @name process_api
 * @{
 */
/**
 * @brief do like execvp but you can wait till it quit
 *
 * @param a_retbean return the identifier of the new created applications.
 * When process created, the identifier is set, if wait and wait success, is reset to be zero.
 *
 * @return 0 for success, -1 for error.
 */
kint ksal_exec(kint a_xiuflg, kbool a_block, kbean *a_retbean, const kchar *a_args, ...);
kint ksal_kill(kbean a_pid, kuint a_sig);
/** @} */

/**
 * @brief from argv and the directory call the application, get the real exe's path
 *
 * @param a_exedir Caller should provide enough memory to hold the return path.
 */
kvoid ksal_exedir(kchar *a_rundir, kchar **a_argv, kchar *a_exedir);

/**
 * @name file_dir_operation_api
 * @{
 */
/**
 * @brief get delimeter for path, in unix this is '/', win32 is '\'
 */
kchar kvfs_path_sep(kvoid);

/**
 * @brief if file or directory exist
 */
kbool kvfs_exist(const kchar *a_path);

/**
 * @brief open a file for operate
 * @return a bean used by ::kvfs_close ::kvfs_read ::kvfs_write ::kvfs_length ::kvfs_printf ::kvfs_seek ::kvfs_tell
 */
kbean kvfs_open(const kchar *a_path, const kchar *a_mode, kuint a_flg);
kint kvfs_close(kbean a_file);
kint kvfs_read(kbean a_file, kvoid **a_buf, kint *a_size);
kint kvfs_write(kbean a_file, kvoid *a_buf, kint a_size);
kint kvfs_length(kbean file);
kint kvfs_printf(kbean a_file, kchar *a_fmt, ...);
kint kvfs_seek(kbean a_file, kint a_offset, kint a_origin);
kint kvfs_tell(kbean a_file);

/**
 * @brief copy a_srcfile to a_dstfile, not directory
 */
kint kvfs_copy(const kchar *a_srcfile, const kchar *a_dstfile);

kint kvfs_remove(const kchar *a_path);
kint kvfs_chdir(const kchar *a_path);
kint kvfs_cpdir(const kchar *a_srcpath, const kchar *a_dstpath);
kint kvfs_mkdir(const kchar *a_path);
kint kvfs_rmdir(const kchar *a_path);
kint kvfs_cwd(kchar *a_buf, kint a_size);

/**
 * @brief get the disk free space for the disk where path resides
 * @return space in megabytes, -1 for error
 */
kint kvfs_dsk_free(kchar *a_path);

#define KVFS_A_NORMAL 0x00		    /**< Normal file - No read/write restrictions */
#define KVFS_A_RDONLY 0x01		    /**< Read only file */
#define KVFS_A_HIDDEN 0x02		    /**< Hidden file */
#define KVFS_A_SYSTEM 0x04		    /**< System file */
#define KVFS_A_SUBDIR 0x10		    /**< Subdirectory */
#define KVFS_A_ARCH   0x20		    /**< Archive file */

typedef struct _KVFS_FINDDATA_ {
	kuchar attrib;			    /**< OR-ed of KVFS_A_XXX */
	kint size;			    /**< file size in bytes */
	kchar name[260];		    /**< Null-terminated name of matched file/directory, without the path */
} KVFS_FINDDATA;

/**
 * find directory with path == a_fspec not /xxx/ *.* and return each found files
 */
kbean kvfs_findfirst(const kchar *a_fspec, KVFS_FINDDATA *a_finfo);
kint kvfs_findnext(kbean a_find, KVFS_FINDDATA *a_finfo);
kint kvfs_findclose(kbean a_find);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* __K_KSAL_H__ */
