/* vim:set et sw=4 sts=4: */


#ifndef __SG_IMD_H__
#define __SG_IMD_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "esgfile.h"
#include "sg_com.h"

#include "sg_root.h"
#include "ivisa_sg_user_api.h"

typedef InteractivityMediaDocument_t sg_imd;

kint sg_imd_new(sg_imd **imd);
kint sg_imd_del(sg_imd *imd, kbool rmcache);
kint sg_imd_add_cache(sg_imd *imd);
kint sg_imd_del_cache(sg_imd *imd);
kint sg_imd_ld_cache(sg_imd *imd);
kint sg_imd_x2s(sg_imd *imd);
kint sg_img_free_idir(IV_INTERACTIVITY_DIRECTORY *idir);
kint sg_imd_copy_idir(IV_INTERACTIVITY_DIRECTORY **dst_idir, IV_INTERACTIVITY_DIRECTORY *src_idir,kchar *dst_directory, kchar *src_directory);

#ifdef __cplusplus
}
#endif

#endif // __SG_IMD_H__

