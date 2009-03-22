/* vim:set et sw=4 sts=4: */
#ifndef __SG_MGR_H__
#define __SG_MGR_H__

#include <ktypes.h>

#include "sdlist.h"
#include <kdbg.h>

#include "sg_bs.h"

#include "ivisa_sg_config.h"
#include "ivisa_sg_filerev.h"
#include "ivisa_sg_user_api.h"

/**
 * @name SgManagerFlags
 */
#define MGR_USED            0x00000001
#define MGR_DYN_MEM         0x00000002
#define MGR_AUTO_SYNC       0x00000004

/**
 * @name SG_Protocol
 */
#define SG_PROT_INVALID     0x00000000      /**< invalid protocol */
#define SG_PROT_IP          0x00000001      /**< Pure ID sulotion */
#define SG_PROT_DMB_TH      0x00000002      /**< DMB TH */
#define SG_PROT_DAB         0x00000003      /**< DAB, use SG_TMODE_MOT */

/**
 * @name Transition_mode
 */
#define SG_TMODE_FLUTE      IV_FILEREV_PROTOCOL_FLUTEALC
#define SG_TMODE_MOT        IV_FILEREV_PROTOCOL_MOT
#define SG_TMODE_BDIR       IV_FILEREV_PROTOCOL_BDIR

#define SG_TMODE_LAST       -1               /**< stop scan when meet this tmode */

/**
 * @name SG_ENVIRONMENT
 * @{
 */

/**
 * @name sg_env
 * @brief Set the evironment for this sg library, set this via interface function
 * ::ivisa_SG_SetEnvironment, and some of environment must be set before
 * ::ivisa_SG_Init.
 */
typedef struct _sg_env {
    kchar data_dir[IV_PATH_MAX_SIZE];       /**< where to cache cache data, config data, etc */
    kchar temp_dir[IV_PATH_MAX_SIZE];       /**< like IV_SG_SWAP_DIRECTORY */
    kchar bdsg_url[2048];                   /**< Well known URL to get BDSG Bootstrap, IE is 2K, FF more than 10K */

    kchar bs_ipa[40];                       /**< Boot strap IP Address, The address will tell ipv4 or ipv6, ipv4 should be 39bytes */
    kuint bs_port;                          /**< Boot strap IP Address Port */

    kuint protocol;                         /**< see ::SG_Protocol */

    kint time_diff;                         /**< serverTime - localTime, used to sync current time and server time */
} sg_env;
/** @} */

/**
 * @name SG_MANAGER
 * @brief SG manager is the ONLY to run all the information in this library.
 * @{
 */
typedef struct _sg_mgr {
   kuint flg;                               /**< @see ::SgManagerFlags */

   sg_env *env;                             /**< point back to globle evironment */

    /**
     * all the provider, loaded, scaning
     * are all queue to here
     *
     * when sg mgr startup, it will scan all the saved provider,
     * and queue it to this queue.
     */
   K_dlist_entry provhdr;                   /**< queue for K_pvdr */

    /**
     * current used sg provider.
     */
   struct _sg_root *cursg;

    struct _sg_scan *scan;                  /**< used for scan channel */

   struct {
      kuint curFrequency;                   /**< current frequency, set when one service set */

      kuint autoUpdateTime;                 /**< in second, the interval to probe for new version of sg data */

      IV_FILE_REV_PROTOCOL prot;            /**< MOT or FLUTE or B-DIR */

      kchar *lang;                          /**< current used language */

        /**
         * if the sg module and the caller is in separate run time environment,
         * the SG module maybe can not call the named callback routine inside of
         * caller module. So the caller should specified a callback routine when
         * call ivisa_SG_Init
         */
      IVISA_SG_CALLBACK evtCallback;
   } conf;
} sg_mgr;
/** @} */

kint sg_mgr_setup(sg_mgr *mgr, kchar *key, kvoid *val);

kint sg_mgr_new(struct _sg_mgr **mgr, sg_env *env);
kint sg_mgr_del(struct _sg_mgr *mgr);

kint sg_mgr_probe_cap(struct _sg_mgr *mgr);

kint sg_mgr_start_sg(struct _sg_mgr *mgr);
kint sg_mgr_stop_sg(struct _sg_mgr *mgr);

struct _K_pvdr* sg_mgr_find_provider(sg_mgr *mgr, kuint pid);
struct _K_pvdr_acc *sg_mgr_pvdr_acc_find(sg_mgr *mgr, struct _K_pvdr *pvdr, kuint frequency);

#endif /* __SG_MGR_H__ */

