/* vim:set et sw=4 sts=4: */

#ifndef __SG_ROOT_H__
#define __SG_ROOT_H__

#include <ktypes.h>
#include <kdbg.h>
#include <sdlist.h>

#include <ktmr.h>

#include "sg_bs.h"

#include "sg_ses.h"

#include "sg_zbd.h"
#include "sg_fdt.h"
#include "sg_sdd.h"
#include "sg_ser.h"
#include "sg_ctt.h"
#include "sg_sch.h"
#include "sg_acc.h"
#include "sg_pci.h"
#include "sg_pcd.h"
#include "sg_pcc.h"
#include "sg_pvd.h"
#include "sg_iad.h"
#include "sg_sdu.h"
#include "sg_sdp.h"

#include "ivisa_sg_user_api.h"

/**
 * @name SG_ROOT_FLAGS
 * @{
 */
#define ERF_DYN_MEM     0x00000001      /**< The memory should be freed when sg_root deleted */
#define ERF_ENABLE      0x00000002      /**< @deprecated should the flag should be set in sg_rec */
#define ERF_DATA        0x00000008
#define ERF_DEFAULT     0x00000020      /**< Indicate this is the default SG */
#define ERF_SCAN        0x00000040      /**< This sg is now channel searching */
#define ERF_CAC_LOAD    0x00000080      /**< cache already loaded */
#define ERF_LOADING     0x00000100      /**< loading cache */
#define ERF_USED        0x00000200
#define ERF_AUTO_SYNC   0x00000400      /**< auto sync mode */
#define ERF_NOW_SYNC    0x00000800      /**< auto sync now working */
/**
 * @}
 */

/**
 * @name SGDU MARCO
 * @warning should this move to sg_sdu.cpp?
 * @{
 */
#define MK_INT08(p) (*((kuchar*)p + 0))
#define MK_INT16(p) ((*((kuchar*)p + 0) << 8) + ((*((kuchar*)p + 1)) << 0))
#define MK_INT24(p) ((*((kuchar*)p + 0) << 16) + ((*((kuchar*)p + 1)) << 8) + ((*((kuchar*)p + 2)) << 0))
#define MK_INT32(p) ((*((kuchar*)p + 0) << 24) + ((*((kuchar*)p + 1)) << 16) + ((*((kuchar*)p + 2)) << 8) + ((*((kuchar*)p + 3)) << 0))

#define sgdu_hdr_get_extension_offset(p) \
    MK_INT32(p)
#define sgdu_hdr_get_n_o_service_guide_fragments(p) \
    MK_INT24((p) + 6)
#define sgdu_hdr_get_fragmentTransportID(p, index) \
    MK_INT32((p) + 9 + 12 * (index))
#define sgdu_hdr_get_fragmentVersion(p, index) \
    MK_INT32((p) + 9 + 4 + 12 * (index))
#define sgdu_hdr_get_fragmentOffset(p, index) \
    MK_INT32((p) + 9 + 4 + 4 + 12 * (index))

/** the ofs is payload offset, not the sgdu start address */
#define sgdu_payload_get_start_addr(p) \
    sgdu_hdr_get_n_o_service_guide_fragments(p) * 12 + 9 + p

#define sgdu_payload_get_fragmentEncoding(ofs) \
    MK_INT08((ofs))

#define sgdu_payload_XML_hdrlen 2
#define sgdu_payload_get_0_fragmentType(ofs) \
    MK_INT08((ofs) + 1)
#define sgdu_payload_get_0_XMLFragment(ofs) \
    ((ofs) + 2)

#define sgdu_payload_get_1_validFrom(ofs) \
    MK_INT32((ofs) + 1)
#define sgdu_payload_get_1_validTo(ofs) \
    MK_INT32((ofs) + 1 + 4)
#define sgdu_payload_get_1_fragmentID(ofs) \
    ((ofs) + 1 + 4 + 4)
#define sgdu_payload_get_1_SDPfragment(ofs) \
    (ofs + 10 + strlen(sgdu_payload_get_1_fragmentID(ofs)))


#define sgdu_payload_get_2_validFrom(ofs) \
    MK_INT32((ofs) + 1)
#define sgdu_payload_get_2_validTo(ofs) \
    MK_INT32((ofs) + 1 + 4)
#define sgdu_payload_get_2_fragmentID(ofs) \
    ((ofs) + 1 + 4 + 4)
#define sgdu_payload_get_2_USBDfragment(ofs) \
    (ofs + 10 + strlen(sgdu_payload_get_2_fragmentID(ofs)))


#define sgdu_payload_get_3_validFrom(ofs) \
    MK_INT32((ofs) + 1)
#define sgdu_payload_get_3_validTo(ofs) \
    MK_INT32((ofs) + 1 + 4)
#define sgdu_payload_get_3_fragmentID(ofs) \
    ((ofs) + 1 + 4 + 4)
#define sgdu_payload_get_3_ADPfragment(ofs) \
    (ofs + 10 + strlen(sgdu_payload_get_3_fragmentID(ofs)))
/** @} */

/**
 * @name KMsg definations
 * @{
 */
/** normal message */
#define KRTM_SGDU_PARSE                     0x00000002      /**< to parse the sgdu data*/
#define KRTM_OOD                            0x00000004      /**< check and delete the OOD fragements */
#define KRTM_LOAD_FROM_CACHE                0x00000005      /**< load the fragment from cache */
#define KRTM_SAVE_TO_CACHE                  0x00000006
#define KRTM_STOP                           0x00000007
#define KRTM_START_SYNC                     0x00000008
#define KRTM_ABORT_SYNC                     0x00000009
#define KRTM_START_SCAN                     0x0000000A
#define KRTM_ABORT_SCAN                     0x0000000B
#define KRTM_CALL_EVT_CBK                   0x0000000C

/** session message */
#define KRTM_SGDD_DIR                       0x00010001      /**< SGDD directory callback */
#define KRTM_SGDD_OBJ                       0x00010002      /**< SGDD object callback */
#define KRTM_SGDU_OBJ                       0x00010003      /**< SGDU object callback */

/** interactivity related message */
#define KRTM_MOS_FILE                       0x00020001      /**< Got Media Object Set data */
#define KRTM_IAD_SEARCH_FOR_FREQ            0x00020002      /**< start iad search for a frequency */
#define KRTM_IAD_SEARCH                     0x00020003      /**< start iad search for a service */
#define KRTM_IAD_DIR                        0x00020004
#define KRTM_IAD_OBJ                        0x00020005
#define KRTM_IAD_ABORT                      0x00020006

/** user api's message */
#define KRTM_GET_SERVICE_ACCESS_INFO        0x00030008
#define KRTM_GET_SERVICE_DETAIL_INFO        0x00030009
#define KRTM_GET_SERVICE_PREVIEW_INFO       0x0003000A
#define KRTM_GET_PROGRAM_LIST               0x0003000B
#define KRTM_GET_PROG_DESC                  0x0003000C
#define KRTM_GET_CURRENT_INTERACTIVITY      0x0003000D

/** frequency switch */
#define KRTM_FREQ_CHG                       0x00040000
/** @} */

/**
 * @name FragmentType
 * @brief Define all the Data fragments, include SGDU fragment with types/class \n
 * all these fragment should be queued into ::sg_root::allhdr.
 * @{
 */
typedef enum _FT {
    FDT,                        /**< FDT or List or Dir describe SGDD list */
    SDD,                        /**< SGDD */
    SDU,                        /**< SGDU */
    SER,                        /**< Service */
    SCH,                        /**< Schedule */
    CTT,                        /**< Content */
    PCI,                        /**< Purchase Item */
    PCD,                        /**< Purchase Data */
    PCC,                        /**< purchase Channel */
    PVD,                        /**< Preview Data */
    IAD,                        /**< Interactivity Data */
    ACC,                        /**< Access */
    SDP,                        /**< SDP file */
    TOP                         /**< MAX Count of fragment */
} FT;
/** @} */

/**
 * @name K_freq
 * @brief All the data in sg root MUST belong to a certain frequency, and
 * all the data should saved in disk in $(providerID as fmt=%08x)/$(freq as fmt=%08x);
 * All the data in a freqency has same sg_env::tmode and sg_env::protocol.
 */
typedef struct _K_freq {
    K_dlist_entry le;           /**< queue to sg_root::freqhdr */
    struct _sg_root *rt;        /**< fast access to parent SG root */
    struct _K_pvdr_acc *acc;    /**< fast access the real information */

    /**
     * All the data, besides saved int sg_root::allhdr, it also been saved
     * classified in fhdrs.\nThe first letter `f` stand for 'file' or 'fragment'.
     */
    K_dlist_entry fhdrs[TOP];

    /** session, for current frequency */
    struct {
        sg_ses sdd;             /**< session for SGDD */
        K_dlist_entry subhdr;   /**< for sgdu */
    } data_ses;                 /**< session for normal data */

    sg_ses ii_ses;              /**< session for interactivity */

    /** statistic, for just ONE time Sync, all the field will be reset when next sync */
    struct {
        kint dir_set;           /**< all dir request count */
        kint dir_ret;           /**< all dir success returned and processed */

        kint sdd_set;           /**< all obj request count */
        kint sdd_ret;           /**< all obj success returned and processed */

        kint sdu_set;           /**< all sdu request count */
        kint sdu_ret;           /**< all sdu success returned and processed */
        kint sdu_parsed;        /**< all sdu success parsed */

        kint new_ser_cnt;       /**< new or updated service count */
        kint new_ctt_cnt;       /**< new or updated content count */
        kint new_sch_cnt;       /**< new or updated schedule count */
        kint new_acc_cnt;       /**< new or updated access count */
        kint new_pci_cnt;       /**< new or updated purchase item count */
        kint new_pcd_cnt;       /**< new or updated purchase data count */
        kint new_pcc_cnt;       /**< new or updated purchase channel count */
        kint new_pvd_cnt;       /**< new or updated preview data count */
        kint new_iad_cnt;       /**< new or updated interactivity data count */
    } stat;
} K_freq;

/**
 * @struct sg_root
 * @{
 */
typedef struct _sg_root {
    kuint flg;                  /**< @see ::SG_ROOT_FLAGS */
    struct _sg_mgr *mgr;        /**< pointer back to the SG manager. */
    struct _K_pvdr *pvdr;       /**< the provider this root belong to */

    kbean tmr_scan_data_timeout;/**< only used for scan */

    /**
     * maintain the service now playing in current provider,
     *
     * XXX when the SG is not current used, then no service
     * in this provider should be CURRENT, in this occassion,
     * how to set the curser?
     *
     * @todo XXX Can multi services be set as current?
     */
    kchar *curser;              /**< current service, can be nil, when set, also save ::curFrequency */

    kuint curFrequency;            /**< has curFrequency, but no curser is possible */

    /**
     * All the data, include dir and fragment should be save in this queue.
     * And all the same fragements also queue into K_freq::fhdrs with it's cls_le
     */
    K_dlist_entry allhdr;

    /**
     * Garbage collecting timer.\n
     * this timer search all the overdue file and delete them
     */
    kbean gc_tmr;

    /**
     * auto update timer, this timer try to receive new version data periodically.\n
     * The period set by user and saved in sg_mgr::conf::autoUpdateTime variable.
     */
    kbean up_tmr;

    /** all the data should be process in this task, which will alway live till sg_root deleted */
    kbean worker_thread;

    K_dlist_entry frqhdr;       /**< list header for K_freq */

    /** OK, save all the frequency block here */
    struct {
        K_freq *current;        /**< Current frequency block, can be knil */

        kuint cnt;              /**< count of frequency this root covered */
        K_freq *arr;            /**< array for all the frequency */
    } freq;
} sg_root;
/** @} */

/////////////////////////////////////////////////////////////////////////
kint sg_root_new(sg_root **rt, struct _sg_mgr *mgr, struct _K_pvdr *pvdr);
kint sg_root_del(sg_root *rt, kbool rmcache);

kint sg_root_new_rti(sg_root *rt);

kint sg_root_cleanup(sg_root *rt, kbool rmcache);

kint sg_root_load_form_cache(sg_root *rt, kbool force);
kint sg_root_save_to_cache(sg_root *rt);

kint sg_root_stop(sg_root *rt, kbool rmcache);

kint sg_delete_file(struct _sg_zbd *zbd, kbool rmcache);

sg_ctt **sg_root_get_ctt_by_ser(sg_root *rt, kchar *serid);
sg_acc **sg_root_get_acc_by_sch(sg_root *rt, kchar *schid);
sg_acc **sg_root_get_acc_by_ser(sg_root *rt, kchar *serid);
sg_sdu *sg_root_get_sdu_by_id(sg_root *rt, kuint tid);
struct _sg_zbd *sg_root_find_file(sg_root *rt, kint cls, kuint *ftid, kchar *id);
kint sg_root_del_cache_rec(sg_root *rt, ef_cache_rec *rec);
kbool sg_root_chk_cache_rec(sg_root *rt, ef_cache_rec *rec);
kint sg_root_get_cache_rec_dat(sg_root *rt, ef_cache_rec *rec, kchar **data, kint *datalen);
kvoid sg_root_rec_to_zbd(sg_root *rt, ef_cache_rec *rec, struct _sg_zbd *zbd);
sg_fdt* sg_root_get_dir_by_freq(sg_root *rt, kuint freq);

K_freq *sg_root_freq_blk_find(sg_root *rt, kuint frequency);
K_freq* sg_root_frequency_switch(sg_root *rt, kuint frequency);

kint sg_root_frequency_add(sg_root *rt, K_pvdr_acc *acc);

#endif /* __SG_ROOT_H__ */




