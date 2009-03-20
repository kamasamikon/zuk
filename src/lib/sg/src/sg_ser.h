/* vim:set et sw=4 sts=4: */
#ifndef __SG_SER_H__
#define __SG_SER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sg_root.h"
#include "esgfile.h"
#include "sg_com.h"

#define SER_DYNMEM              0x00000001          /**< sg_ser structure allocated by kmm_alloc */
#define SER_IAD_TIMER           0x00000002          /**< probe timer set for this service */
#define SER_IAD                 0x00000004          /**< sg_iad_retrieve_data called for this service */
#define SER_CHG_SER             0x00000008          /**< auto update, changed, service, not evtCallback */
#define SER_CHG_PRG             0x00000010          /**< auto update, changed, program, not evtCallback */
#define SER_CHG_ACC             0x00000020          /**< auto update, changed, access, not evtCallback */
#define SER_CHG_PCS             0x00000040          /**< auto update, changed, purchase, not evtCallback */

typedef Service_t sg_ser;

//kint sg_ser_new_from_cache(sg_root *root, K_freq *fblk, ef_cache_rec *rec);
kint sg_ser_new(sg_ser **ser);
kint sg_ser_del(sg_ser *ser, kbool rmcache);
kint sg_ser_add_cache(sg_ser *ser);
kint sg_ser_del_cache(sg_ser *ser);
kint sg_ser_ld_cache(sg_ser *ser);


#ifdef __cplusplus
}
#endif

#endif // __SG_SER_H__

