
#ifndef __SG_BS_H__
#define __SG_BS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include <hget.h>

#include "sg_mgr2.h"
#include "sg_ses.h"

#include "sg_com.h"
#include "xmldoc.h"

typedef enum _BS_FT {
    BS_BIN = 0,                             /**< Bin file */
    BS_XML = 1,                             /**< XML file */

    XBS_TOP = 2                             /**< MAX of file type */
} BS_FT;

#define BSF_USED                0x00000001  /**< this record is used */
#define BSF_DYN_MEM             0x00000002  /**< dynamic memory, free when delete */
#define BSF_ENABLE              0x00000004  /**< ESG should be enable */
#define BSF_DEFAULT             0x00000008  /**< Set to be current SG */
#define BSF_SCAN                0x00000010  /**< temporary for scanning, if it's a valid, this bit clear, it become a normal one */
#define BSF_LIVE                0x00000020  /**< called when auto update */
#define BSF_VALID               0x00000040  /**< This is a valid pvdr or acc */

#define SCAN_BOOTSTRAP_START    0x00000001  /**< boot strap stage start */
#define SCAN_BOOTSTRAP_DONE     0x00000002
#define SCAN_STOP               0x00000003
#define SCAN_CHANGE_CNT         0x00000004  /**< change now scanning provider count */

typedef struct _K_x_acc_hdr {
    struct _K_pvdr_acc *pvdr_acc;           /**< back to parent */
    kchar tmode;                            /**< same as _K_pvdr_acc::tmode */
} K_x_acc_hdr;

typedef struct _K_flute_acc {
    K_x_acc_hdr hdr;                        /**< tree info */

    IV_FLUTE_ALC_SESSION_ENTRYINFO alc;
} K_flute_acc;

typedef struct _K_mot_acc {
    K_x_acc_hdr hdr;                        /**< tree info */

    IV_DAB_SESSION_ENTRYINFO dab;
} K_mot_acc;

/**
 * \brief Access information for B-Direction SG
 * \{
 */
typedef struct _K_bdir_acc {
    K_x_acc_hdr hdr;                        /**< tree info */

    kchar *url;                             /**< URL to get the data */

    kchar *nid;                             /**< Network ID */
    kchar *nname;                           /**< Network name */
    kuint ntype;                            /**< Network type */

    kchar *tsid;                            /**< Transport Stream ID */
    kchar *tsname;                          /**< Transport Stream name */
    kuint frequency;                        /**< The frequency this TS been used for */
} K_bdir_acc;
/* \} */

/**
 * \brief Collect ALL Access information for a frequency
 * Only describes the access information for one frequency
 *
 * \warning for a frequency, only one access instance can be exist
 * multi ESGAccessDescriptor is NOT supported.
 */
typedef struct _K_pvdr_acc {
    K_dlist_entry le;                   /**< queue to K_pvdr::acchdr */
    struct _K_pvdr *pvdr;               /**< back */

    kuint frequency;                    /**< what frequency */

    kuint protocol;                     /**< \sa ::SG_Protocol */
    kchar tmode[8];
    kuint tindex;

    K_mot_acc *mot;                     /**< ESGAccessDescriptor for mot */
    K_bdir_acc *bdir;                   /**< ESGAccessDescriptor for b-dir */
    K_flute_acc *flute;                 /**< ESGAccessDescriptor for flute */

} K_pvdr_acc;

/**
 * \name K_pvdr
 * \brief maintain the information for a single SG provider
 */
typedef struct _K_pvdr {
    K_dlist_entry le;                   /**< queue to sg_mgr::provhdr or sg_scan::provhdr */

    kuint flg;                          /**< */

    kuint protocol;                     /**< dab, ip, dmbth */

    kuint id;                           /**< id is important, so add here */

    /**
     * Save base information, such as ID, name, flg, etc.
     * Directy load the file in data_dir/$(providerID)/__pvdr.xml
     */
    KXmlDoc *doc;

    K_dlist_entry acchdr;               /**< queue header for K_pvdr_acc */

    struct _sg_root *root;              /**< new it when enable this sg */
} K_pvdr;

/**
 * \name sg_scan
 * \brief manager the BootStrap information.
 */
typedef struct _sg_scan {
    kuint flg;
    struct _sg_mgr *mgr;                /**< save the SG manager */

    kchar scan_ok;                      /**< count of pvdr which successfully start scan */
    kchar scan_ng;                      /**< count of pvdr which not successfully start scan */
    kchar now_scan;                     /**< count of provider now scaning */
    kchar all_scan_issued;

    struct {
        kuint frequency;
        kuint protocol;					/**< ip, dab, dvb */
        kuint timeout;					/**< timeout in millisecond */
        kchar tmode;					/**< SG_TMODE_XXX, \sa Transition_mode */
        kchar tcode;                 	/**< scan success? default false, when any scan success, set 1 */

        kvoid *userdata;                /**< user data set when start scan */

        /**
         * default settings
         */
        kchar *bdir_url;                /**< well known uri to get provider list */
        SOCKET *bdir_socket;            /**< socket when get bdir data */

        struct {
            kuint all;                  /**< all fragment or service */
            kuint cur;                  /**< current gotten fragment */
        } stat;
    } scan;

    struct _sg_ses ses;                 /**< the session to receive data */

    K_dlist_entry fhdrs[XBS_TOP];       /**< queue to save the BS_BIN and BS_XML files */

    kbean tmr_bootstrap_timeout;        /**< timeout for getting boot strap */

    kbean worker_thread;                /**< main or worker task */
} sg_scan;

#ifdef __cplusplus
}
#endif

#endif /* __SG_BS_H__ */

