/* vim:set et sw=4 sts=4: */
#ifndef __K_MM_H__
#define __K_MM_H__

#include <kini.h>
#include <kim.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef kvoid (*WASP_JC)(KIM *a_im, kchar *a_ar0, kchar *a_ar1, kchar *a_ar2, kchar *a_ar3, kchar **a_result);

typedef struct _kmm_soinfo
{
    kchar *name;                /**< mod:modDir:modDir.dll */
    kbean bean;                 /**< m_LibraryHandle = LoadLibrary(m_LibraryName); */
    kchar *soid;                /**< DLL or SO's GUID */
} kmm_soinfo;

typedef struct _kmm_moditem
{
    kchar *basepath;            /**< mod:modDir */

    kbool used;
    kbool loaded;
    kint loadorder;             /**< Load order, used for unload sequency */

    /* From cfg.xml */
    kchar *id;
    kchar *homeurl;
    kchar *logourl;
    kchar *updturl;
    kchar *ver;

    kchar *name;                /**< only load for current language */
    kchar *desc;                /**< only load for current language */

    kuchar type;                /**< mod:modDir:type = text file */
    kuchar state;               /**< mod:modDir:state = text file */

    /* Only for current platform */
    kmm_soinfo entry;           /**< main DLL for module */
} kmm_moditem;

typedef struct _kmm_jc_his_entry
{
    kchar *soid;                /**< guid for SO, nil if this entry not used */
    kchar *name;                /**< proc name */
    WASP_JC proc;               /**< WASP_JC */
} kmm_jc_his_entry;

typedef struct _KMM
{
    KIM* im;

    struct {
        kint cnt;
        kmm_moditem *arr;
    } mod;

    struct {
        kint cnt;
        kmm_jc_his_entry *arr;
    } his;

    kint maxtype;
    kint loadorder;
} KMM;

kbean kmm_init(KIM *a_im);
kint kmm_final(kbean a_mm);

kint kmm_load_modules(kbean a_mm);
kint kmm_unload_modules(kbean a_mm);

kint kmm_jc_cmd(kbean a_mm, kchar *a_cname, kchar *a_soid, kchar *a_ar0, kchar *a_ar1, kchar *a_ar2, kchar *a_ar3, kchar **a_result);

#ifdef __cplusplus
}
#endif

#endif /* __K_MM_H__ */

