#include <ksal.h>
#include <stdio.h>
#include <stdarg.h>

#include "ivisa_plfm.h"


kint kprintf(const kchar *format, ...)
{
    kchar strBuffer[2048];
    va_list args;

    va_start(args, format);
    _vsnprintf(strBuffer, sizeof(strBuffer), format, args);
    va_end(args);

    Iv_printf(strBuffer);
	return 0;
}

kvoid kdbgbrk(kvoid)
{
}

kulong ksal_get_tick(void)
{
    return Iv_OS_GetMilliSec();
}

kvoid ksal_tsk_exit(kvoid)
{
	Iv_OS_TaskDestroy(Iv_OS_GetCurrentTask());
}

kbean ksal_tsk_new(kvoid (*entrypoint)(kvoid), kuchar priority, kuint stackSize, void *param)
{
	return (kbean)Iv_OS_TaskCreate((IV_TASK_PROC)entrypoint, priority, stackSize, param);
}

kint ksal_tsk_del(kbean taskId)
{
    return Iv_OS_TaskDestroy((kint)taskId);
}
kbean ksal_tsk_cur(kvoid)
{
    return (kbean)Iv_OS_GetCurrentTask();
}

kint ksal_mem_init(kuint a_flg)
{
    return 0;
}
kint ksal_mem_final(kvoid)
{
    return 0;
}
void* ksal_mem_alloc(kuint size)
{
    return Iv_OS_MemAlloc(size, 4);
}
void ksal_mem_free(void * ptr)
{
    Iv_OS_MemFree(ptr);
}

void ksal_tsk_sleep(kuint msec)
{
    Iv_OS_Sleep(msec);
}

kbean ksal_sema_new(kint initValue)
{
    return (kbean)Iv_OS_SemaphoreCreate(initValue);
}
kint ksal_sema_del(kbean semaphId)
{
    Iv_OS_SemaphoreDestroy((kint)semaphId);
    return 0;
}
kint ksal_sema_get(kbean semaphId, kint timeout)
{
    return Iv_OS_SemaphoreRetrieve((kint)semaphId, timeout);
}
kint ksal_sema_rel(kbean semaphId)
{
    Iv_OS_SemaphoreRelease((kint)semaphId);
    return 0;
}

kint kvfs_length(kbean file)
{
	kint len = -1, pos = Iv_FS_ftell((kint)file);
	if (file) {
		Iv_FS_fseek((kint)file, 0, SEEK_END);
		len = Iv_FS_ftell((kint)file);
		Iv_FS_fseek((kint)file, pos, SEEK_SET);
	}
	return len;
}

kint kvfs_read(kbean file, kchar **retbuf, kuint *retlen)
{
	kint pos = Iv_FS_ftell((kint)file);
	Iv_FS_fseek((kint)file, 0, SEEK_END);
	*retlen = Iv_FS_ftell((kint)file);
	*retbuf = ksal_mem_alloc(*retlen);
	Iv_FS_fseek((kint)file, 0, SEEK_SET);
	Iv_FS_fread(*retbuf, 1, *retlen, (kint)file);
	Iv_FS_fseek((kint)file, pos, SEEK_SET);
	return 0;
}

//
// 文件相关操作
//
