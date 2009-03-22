/* vim:set et sw=4 sts=4: */
#ifndef __IV_PLTFM_H__
#define __IV_PLTFM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ivisa_types.h"
#include "ivisa_err_code.h"

//
// ָ���������е����ȼ�
//
enum {
    IV_TSK_PRIORITY_HIGHEST,
    IV_TSK_PRIORITY_HIGH,
    IV_TSK_PRIORITY_NORMAL,
    IV_TSK_PRIORITY_LOW,
    IV_TSK_PRIORITY_LOWEST
};

//
// ��������ԭ��
// �μ� Iv_OS_TaskCreate �Ĳ��� entrypoint
//
typedef void (*IV_TASK_PROC)(void *param);

//
// �õ�ϵͳ������ĺ�����
//
kulong Iv_OS_GetMilliSec(void);

//
// �õ���ǰ����ʱ��
//
kuint ksal_cur_time(void);

//
// �����������
//
kuint Iv_OS_TaskCreate(IV_TASK_PROC entrypoint, kuchar priority, kuint stackSize, void *param);
kint  Iv_OS_TaskDestroy(kuint taskId);
kuint Iv_OS_GetCurrentTask(kvoid);

//
// �ź���
//
kint Iv_OS_SemaphoreCreate(kint initValue);
kint Iv_OS_SemaphoreDestroy(kint semaphId);
kint Iv_OS_SemaphoreRetrieve(kint semaphId, kint timeout);
kint Iv_OS_SemaphoreRelease(kint semaphId);

//
// ��̬�ڴ����
//
void* Iv_OS_MemAlloc(kuint size, kuint align);
void  Iv_OS_MemFree(void * ptr);

//
// ��������
//
void Iv_OS_Sleep(kuint msec);

//
// �ļ���ز���
//

// Iv_FS_fseek::origin
#define IV_FSEEK_CUR 1 /*��ǰλ��*/
#define IV_FSEEK_END 2 /*�ļ���β*/
#define IV_FSEEK_SET 0 /*�ļ���ͷ*/

// Iv_FS_findfirst::fInfo
typedef struct _IV_FINDDATA_ {
    kuchar attrib; /* OR-ed of IV_A_XXX */
    kint size;
    kchar *name;
} IV_FINDDATA;

// IV_FINDDATA::attrib
#define IV_A_NORMAL 0x00 /* Normal file - No read/write restrictions */
#define IV_A_RDONLY 0x01 /* Read only file */
#define IV_A_HIDDEN 0x02 /* Hidden file */
#define IV_A_SYSTEM 0x04 /* System file */
#define IV_A_SUBDIR 0x10 /* Subdirectory */
#define IV_A_ARCH   0x20 /* Archive file */

kchar Iv_FS_getDirDelimeter(void);
kint  Iv_FS_exist(const kchar *path);

kint  Iv_FS_fopen(kchar *filename, kchar *mode);
kint  Iv_FS_fclose(kint hFile);
kint  Iv_FS_fread(void *buf, kint size, kint count, kint hFile);
kint  Iv_FS_fwrite(void *buf, kint size, kint count, kint hFile);
kint  Iv_FS_fprintf(kint hFile, kchar *format, ...);
kint  Iv_FS_fseek(kint hFile, kint offset, kint origin);
kint  Iv_FS_ftell(kint hFile);

kint  Iv_FS_getcwd(kchar *buffer, kint size);

kint  Iv_FS_chdir(const kchar *dirname);
kint  Iv_FS_mkdir(const kchar *dirname);
kint  Iv_FS_rmdir(const kchar *dirname);

kint  Iv_FS_remove(const kchar *path);

kint  Iv_FS_findfirst(const kchar *fSpec, IV_FINDDATA *fInfo);
kint  Iv_FS_findnext(kint hFind, IV_FINDDATA *fileinfo);
kint  Iv_FS_findclose(kint hFind);
kint  Iv_FS_copy(const kchar *src, const kchar *dest);
kint  Iv_FS_rename(const kchar *oName, const kchar *nName);
kchar *Iv_FS_tmpnam(const kchar *path);

void Iv_printf(kchar *format, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __IV_PLTFM_H__

