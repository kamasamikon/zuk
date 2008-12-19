#include <string.h>
#include "ksupport.h"
#include <kflg.h>
#include <kdbg.h>

#include "sg_mgr2.h"
#include "sg_bs.h"
#include <kmem.h>
#include <ktsk.h>
#include <kstr.h>
#include <ksyn.h>
#include "sg_root.h"

#include "ipaddr.h"

extern kchar __dir_delimeter;

static kint sg_mgr_providers_load(sg_mgr *mgr);

/**
 * @brief Constructe the SG manager, only one instance in system.
 *
 * @param mgr If mgr set, will use the memory allocated by caller or else dynamic allocate memory.
 *
 * @return 0 for success, else for fail.
 */
kint sg_mgr_new(sg_mgr **mgr, sg_env *env)
{
    kuint flg = MGR_USED;

    if (!(*mgr)) {
        (*mgr) = kmem_alloc(sizeof(sg_mgr));
        flg |= MGR_DYN_MEM;
    }
    if ((*mgr)) {
        memset((*mgr), 0, sizeof(sg_mgr));
        (*mgr)->flg = flg;
        init_dlist_head(&(*mgr)->provhdr);

        /* first, set the env */
        (*mgr)->env = env;

        /*
         * Set default configuration
         */
        (*mgr)->conf.curFrequency = 0;
        (*mgr)->conf.autoUpdateTime = 180;
        (*mgr)->conf.lang = knil;

        /*
         * all the provider base information is saved in disk by xml format,
         * first try to load them, if load failed or not such file exist,
         * the user layer can not get anything via ivisa_SG_GetProviderList,
         * user will call channel scan for channel search.
         */
        /* XXX this is bs routine? this is mgr routine */
        sg_mgr_providers_load(*mgr);

        return 0;
    }
    kerror(("sg_mgr_new: failed\n"));
    return -1;
}

/**
 * @brief Undo ::sg_mgr_del
 *
 * @param mgr Manager should be deleted.
 *
 * @return 0 for success, other for error.
 */
kint sg_mgr_del(sg_mgr *mgr)
{
    K_dlist_entry *entry, *hdr;
    sg_root *root;
    K_pvdr *pvdr;
    K_pvdr_acc *acc;

    if (!kflg_chk(mgr->flg, MGR_USED)) {
        kerror(("sg_mgr_del: unused, quit\n"));
        return -1;
    }

    if (mgr->scan) {
        sg_scan_del(mgr->scan);
        mgr->scan = knil;
    }
    /* stop and free all the provider */
    hdr = &mgr->provhdr;
    while (!is_dlist_empty(hdr)) {
        entry = remove_dlist_tail_entry(hdr);

        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        if (pvdr->root) {
            /* stop and delete the sg provider */
            sg_root_del(pvdr->root, kfalse);
        }

        /* free xml doc */
        xmldoc_del(pvdr->doc);

        /* free K_pvdr_acc */
        while (!is_dlist_empty(&pvdr->acchdr)) {
            acc = FIELD_TO_STRUCTURE(pvdr->acchdr.next, K_pvdr_acc, le);

            remove_dlist_entry(&acc->le);

            if (acc->mot) {
                kmem_free(acc->mot);
            }
            if (acc->flute) {
                kmem_free(acc->flute);
            }
            if (acc->bdir) {
                kmem_free(acc->bdir->nid);
                kmem_free(acc->bdir->nname);
                kmem_free(acc->bdir->url);
                kmem_free(acc->bdir->tsid);
                kmem_free(acc->bdir->tsname);
                kmem_free(acc->bdir);
            }

            kmem_free(acc);
        }

        kmem_free(pvdr);
    }

    /* free this should after all the sg stopped */
    kmem_free(mgr->conf.lang);

    /* suicide */
    kmem_free(mgr);
    return 0;
}

/**
 * @brief load the config and access info for frequency in dir
 * such as /zedoary/install/modules/dab/data/00000003/43543435
 *
 * - /zedoary/install/modules/dab/data/00000003/43543435/bs.bin     : for flute and mot
 * - /zedoary/install/modules/dab/data/00000003/43543435/bd.xml     : for b-directory sg
 */
static kint sg_mgr_single_provider_freq_load(sg_mgr *mgr, kchar *dir, K_pvdr *pvdr)
{
    kbean file = 0;
    kchar *buf = knil, *val;

    kuint i, freq, len;

    KXmlDoc *doc;
    KXmlNode *node;

    kchar path[IV_PATH_MAX_SIZE];

    K_pvdr_acc *acc;

    freq = strtoul(strrchr(dir, __dir_delimeter) + 1, 0, 16);

    /*
     * 1. The __access.xml in this directory
     */

    /* 1.1 open __access.xml */
    sprintf(path, "%s%c__access.xml", dir, __dir_delimeter);
    file = kvfs_open(path, "rb", 0);
    if (!file) {
        kerror(("this is not a frequency dir\n"));
        return -1;
    }

    /* 1.2 load data and parse it */
    buf = knil;
    len = 0x7FFFFFFF;
    kvfs_read(file, &buf, &len);
    kvfs_close(file);

    /* 1.3 conver to xml */
    doc = xmldoc_new(knil);
    xmldoc_parse(doc, buf, len);
    node = xmldoc_gotoNode(doc, "access", 0);

    kmem_free(buf);

    if (!node) {
        kerror(("bad file :%s\n", path));
        return -1;
    }

    /*
     * 2, convert the xml into K_pvdr_acc struct
     */

    /* 2.1 add or update K_pvdr_acc struct */

    acc = sg_mgr_pvdr_acc_find(mgr, pvdr, freq);
    if (!acc) {
        /* same acc not exist */
        klog(("pvdr [%08x] access [%08x] not in memory\n", pvdr->id, freq));

        acc = (K_pvdr_acc*)kmem_alloz(sizeof(K_pvdr_acc));
        acc->protocol = strtoul(xmlnode_get_attr_value(node, "protocol"), 0, 10);
        acc->frequency = freq;

        /* save the tmode */
        val = xmlnode_get_attr_value(node, "tmode");
        for (i = 0; i < strlen(val); i++) {
            if (val[i] == 'b') {
                acc->tmode[i] = SG_TMODE_BDIR;
            }
            if (val[i] == 'f') {
                acc->tmode[i] = SG_TMODE_FLUTE;
            }
            if (val[i] == 'm') {
                acc->tmode[i] = SG_TMODE_MOT;
            }
        }
        acc->tmode[i] = SG_TMODE_LAST;

        /* add the acc to K_pvdr::freq::arr */
        insert_dlist_tail_entry(&pvdr->acchdr, &acc->le);

    } else {
        /* same already in memory, append it */
        klog(("pvdr [%08x] access [%08x] already in memory\n", pvdr->id, freq));

        /*
         * XXX How to handle it?
         * A, delete the old
         * B, append the new
         *
         * what if reduplicate access exist?
         */

    }

    /* 2.2 process flute */
    /* only one flute acc allowed */
    xmldoc_gotoNode(doc, "/", 0);
    xmldoc_gotoNode(doc, "access", 0);
    node = xmldoc_gotoNode(doc, "alc", 0);
    if (node) {
        acc->flute = kmem_alloz(sizeof(K_flute_acc));
        acc->flute->hdr.tmode = SG_TMODE_FLUTE;
        acc->flute->hdr.pvdr_acc = acc;

        acc->flute->alc.frequency = freq;
        acc->flute->alc.uiTSI = strtoul(xmlnode_get_attr_value(node, "tsi"), 0, 10);
        acc->flute->alc.iSSM_flag = strtoul(xmlnode_get_attr_value(node, "issm_flg"), 0, 10);
        val = xmlnode_get_attr_value(node, "src_addr");
        if (val) {
            if (strstr(val, ":")) {
                acc->flute->alc.source_address.iIPv4_flag = 0;
                ipv6_scan(val, (kuchar*)acc->flute->alc.source_address.ubIP);
            } else {
                acc->flute->alc.source_address.iIPv4_flag = 1;
                ipv4_scan(val, (kuchar*)acc->flute->alc.source_address.ubIP);
            }
        }

        node = xmldoc_gotoNode(doc, "channel", 0);
        if (node) {
            acc->flute->alc.iAlcChannelNum = 1;

            val = xmlnode_get_attr_value(node, "port");
            acc->flute->alc.pAlcChannelHead->u16TargetPort = strtoul(val, 0, 10);

            val = xmlnode_get_attr_value(node, "dst_addr");
            if (strstr(val, ":")) {
                acc->flute->alc.pAlcChannelHead->target_address.iIPv4_flag = 0;
                ipv6_scan(val, (kuchar*)acc->flute->alc.pAlcChannelHead->target_address.ubIP);
            } else {
                acc->flute->alc.pAlcChannelHead->target_address.iIPv4_flag = 1;
                ipv4_scan(val, (kuchar*)acc->flute->alc.pAlcChannelHead->target_address.ubIP);
            }
        }
    }

    /* 2.3 process mot */
    /* only one mot acc allowed */
    xmldoc_gotoNode(doc, "/", 0);
    xmldoc_gotoNode(doc, "access", 0);
    node = xmldoc_gotoNode(doc, "dab", 0);
    if (node) {
        acc->mot = kmem_alloz(sizeof(K_mot_acc));
        acc->mot->hdr.pvdr_acc = acc;
        acc->mot->hdr.tmode = SG_TMODE_MOT;

        val = xmlnode_get_attr_value(node, "eid");
        acc->mot->dab.eID = strtoul(val, 0, 10);

        val = xmlnode_get_attr_value(node, "mode");
        acc->mot->dab.mode = strtoul(val, 0, 10);

        val = xmlnode_get_attr_value(node, "scid");
        acc->mot->dab.serviceComponentID = strtoul(val, 0, 10);
    }

    /* 2.4 process bdir */
    /* only one bdir acc allowed */
    xmldoc_gotoNode(doc, "/", 0);
    xmldoc_gotoNode(doc, "access", 0);
    node = xmldoc_gotoNode(doc, "network", 0);
    if (node) {
        acc->bdir = kmem_alloz(sizeof(K_bdir_acc));
        acc->bdir->hdr.pvdr_acc = acc;
        acc->bdir->hdr.tmode = SG_TMODE_BDIR;

        acc->bdir->frequency = freq;

        val = xmlnode_get_attr_value(node, "nid");
        acc->bdir->nid = kstr_dup(val);

        val = xmlnode_get_attr_value(node, "nname");
        acc->bdir->nname = kstr_dup(val);

        val = xmlnode_get_attr_value(node, "ntype");
        acc->bdir->ntype = strtoul(val, 0, 10);

        node = xmldoc_gotoNode(doc, "ts", 0);

        val = xmlnode_get_attr_value(node, "url");
        acc->bdir->url = kstr_dup(val);

        val = xmlnode_get_attr_value(node, "tsid");
        acc->bdir->tsid = kstr_dup(val);

        val = xmlnode_get_attr_value(node, "tsname");
        acc->bdir->tsname = kstr_dup(val);
    }

    xmldoc_del(doc);

    /* Load Networks, and change to inner format */
    return 0;
}


/**
 * @brief load the provider laies in dir
 * such as /zedoary/install/modules/dab/data/00000003       # 00000003 is provider ID
 *
 * - /zedoary/install/modules/dab/data/00000003/cfg.xml     # save provider name etc
 * - /zedoary/install/modules/dab/data/00000003/43543435    # 43543435 is frequency
 * - /zedoary/install/modules/dab/data/00000003/87345934
 */
static kint sg_mgr_single_provider_load(sg_mgr *mgr, kchar *provpath)
{
    KVFS_FINDDATA finf;
    kbean file;
    kuint len, pvdrid;
    kchar *buf = knil, path[IV_PATH_MAX_SIZE];
    K_pvdr *pvdr;
    KXmlDoc *doc;
    KXmlNode *node;
    kchar *str;

    /*
     * 1. The cfg.xml in this directory
     */

    pvdrid = strtoul(strrchr(provpath, __dir_delimeter) + 1, 0, 16);
    pvdr = sg_mgr_find_provider(mgr, pvdrid);
    if (!pvdr) {

        /* no same provider exist */
        klog(("provider : %08x is not exist in current provhdr\n",pvdrid));

        /* 1.1 open __pvdr.xml */
        /*
         * <pvdr id="23423" flg="34234", name="name" uri="http://omg.cn" infuri="http://6.cn" auxdat="base64" logo="base64" />
         */
        sprintf(path, "%s%c__pvdr.xml", provpath, __dir_delimeter);
        file = kvfs_open(path, "rb", 0);
        if (!file) {
            kerror(("Must has __pvdr.xml"));
            return -1;
        }

        /* 1.2 load and parse __pvdr.xml */
        buf = knil;
        len = 0x7FFFFFFF;
        kvfs_read(file, &buf, &len);
        kvfs_close(file);

        doc = xmldoc_new(knil);
        xmldoc_parse(doc, buf, len);

        if (buf) {
            kmem_free(buf);
        }

        /*
         * 1.3 a provider has a cfg file, when means it should has a K_pvdr struct.
         * OK, new K_pvdr, and queue it
         */
        pvdr = (K_pvdr*)kmem_alloc(sizeof(K_pvdr));
        init_dlist_head(&pvdr->acchdr);
        pvdr->flg = 0;
        init_dlist_head(&pvdr->le);
        pvdr->root = knil;

        /* the name of the current directory is the ProviderID with %08x format */
        pvdr->id = pvdrid;
        pvdr->doc = doc;

        /* Load the attr */
        if (node = xmldoc_gotoNode(doc, "pvdr", 0)) {
            if (str = xmlnode_get_attr_value(node, "flg")) {
                pvdr->flg = strtoul(str, 0, 10);
            }
            if (str = xmlnode_get_attr_value(node, "protocol")) {
                pvdr->protocol = strtoul(str, 0, 10);
            }
        }

        /* Insert the current */
        insert_dlist_tail_entry(&mgr->provhdr, &pvdr->le);
    } else {
        /* a same one already in queue */
        klog(("same provider [%08x] already in queue, directly load the sub frequency\n"));
    }

    /*
     * 2. Load sub frequency
     */
    sprintf(path, "%s", provpath);

    file = kvfs_findfirst(path, &finf);
    if (file) {
        do {
            if (0 != strcmp(finf.name, ".") && 0 != strcmp(finf.name, "..")) {

                sprintf(path, "%s%c%s", provpath, __dir_delimeter, finf.name);

                if (KVFS_A_SUBDIR & finf.attrib) {
                    /* OK, found a provider */
                    klog(("Load provider for frequency : Found : %s\n", path));
                    sg_mgr_single_provider_freq_load(mgr, path, pvdr);
                }
            }
        } while (-1 != kvfs_findnext(file, &finf));

        kvfs_findclose(file);
    }
    return 0;
}

/**
 * @brief load Providers information from disk to memory
 */
static kint sg_mgr_providers_load(sg_mgr *mgr)
{
    KVFS_FINDDATA finf;
    kint file = 0;

    kchar fullpath[IV_PATH_MAX_SIZE];   /* for findfirst *.* */
    kchar temppath[IV_PATH_MAX_SIZE];   /* for sub directory */

    /* walk data_dir, each dir be a provider */
    sprintf(fullpath, "%s", mgr->env->data_dir);

    file = kvfs_findfirst(fullpath, &finf);
    if (file) {
        do {
            if (0 != strcmp(finf.name, ".") && 0 != strcmp(finf.name, "..")) {

                sprintf(temppath, "%s%c%s", mgr->env->data_dir, __dir_delimeter, finf.name);

                if (KVFS_A_SUBDIR & finf.attrib) {
                    /* OK, found a provider */
                    klog(("Load provider for BS: Found : %s\n", temppath));
                    sg_mgr_single_provider_load(mgr, temppath);
                }
            }
        } while (-1 != kvfs_findnext(file, &finf));

        kvfs_findclose(file);
    }
    return 0;
}

kint sg_mgr_single_provider_freq_save(sg_mgr *mgr, K_pvdr *pvdr, K_pvdr_acc *acc)
{
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    kuint i;

    kuchar *ptr;
    kchar val[IV_PATH_MAX_SIZE];
    kchar path[IV_PATH_MAX_SIZE];   /* for findfirst *.* */

    /* save the acc to disk */

    /* create __access.xml doc */
    doc = xmldoc_new(knil);
    node = xmlnode_new(knil, "access", knil);

    xmldoc_addNode(doc, node, knil);
    xmldoc_gotoNode(doc, "access", 0);

    /* add base attr */
    sprintf(val, "%d", acc->protocol);
    xmlnode_set_attr_value(node, "protocol", val);

    for (i = 0; acc->tmode[i] != -1; i++) {
        if ((acc->tmode[i] == SG_TMODE_BDIR) && (acc->bdir)) {
            val[i] = 'b';
        }
        if ((acc->tmode[i] == SG_TMODE_FLUTE) && (acc->flute)) {
            val[i] = 'f';
        }
        if ((acc->tmode[i] == SG_TMODE_MOT) && (acc->mot)) {
            val[i] = 'm';
        }
    }
    val[i] = '\0';
    xmlnode_set_attr_value(node, "tmode", val);
    // xmlnode_set_attr_value(node, "frequency", frequency);

    if (acc->flute) {
        xmldoc_gotoNode(doc, "/", 0);
        xmldoc_gotoNode(doc, "access", 0);

        node = xmlnode_new(knil, "alc", knil);
        xmldoc_addNode(doc, node, knil);
        xmldoc_gotoNode(doc, "alc", 0);

        sprintf(val, "%d", acc->flute->alc.iSSM_flag);
        xmlnode_set_attr_value(node, "issm_flg", val);

        ptr = acc->flute->alc.source_address.ubIP;
        if (acc->flute->alc.source_address.iIPv4_flag) {
            sprintf(val, "%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
        } else {
            sprintf(val, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
                    ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7],
                    ptr[8], ptr[9], ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[16]);
        }
        xmlnode_set_attr_value(node, "src_addr", val);

        sprintf(val, "%d", acc->flute->alc.uiTSI);
        xmlnode_set_attr_value(node, "tsi", val);

        /* add channel sub element */
        node = xmlnode_new(knil, "channel", knil);
        xmldoc_addNode(doc, node, knil);

        ptr = acc->flute->alc.pAlcChannelHead->target_address.ubIP;
        if (acc->flute->alc.pAlcChannelHead->target_address.iIPv4_flag) {
            sprintf(val, "%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);
        } else {
            sprintf(val, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
                    ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7],
                    ptr[8], ptr[9], ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[16]);
        }
        xmlnode_set_attr_value(node, "dst_addr", val);

        sprintf(val, "%d", acc->flute->alc.pAlcChannelHead->u16TargetPort);
        xmlnode_set_attr_value(node, "port", val);
    }

    if (acc->mot) {
        xmldoc_gotoNode(doc, "/", 0);
        xmldoc_gotoNode(doc, "access", 0);

        node = xmlnode_new(knil, "dab", knil);
        xmldoc_addNode(doc, node, knil);

        sprintf(val, "%d", acc->mot->dab.eID);
        xmlnode_set_attr_value(node, "eid", val);

        sprintf(val, "%d", acc->mot->dab.mode);
        xmlnode_set_attr_value(node, "mode", val);

        sprintf(val, "%d", acc->mot->dab.serviceComponentID);
        xmlnode_set_attr_value(node, "scid", val);
    }

    if (acc->bdir) {
        xmldoc_gotoNode(doc, "/", 0);
        xmldoc_gotoNode(doc, "access", 0);

        /* fill network element */
        node = xmlnode_new(knil, "network", knil);
        xmldoc_addNode(doc, node, knil);
        xmldoc_gotoNode(doc, "network", 0);

        if (acc->bdir->nid) {
            xmlnode_set_attr_value(node, "nid", acc->bdir->nid);
        }

        xmlnode_set_attr_value(node, "nname", acc->bdir->nname ? acc->bdir->nname : "");

        sprintf(val, "%d", acc->bdir->ntype);
        xmlnode_set_attr_value(node, "ntype", val);

        /* fill ts sub element */
        node = xmlnode_new(knil, "ts", knil);
        node = xmldoc_addNode(doc, node, knil);

        if (acc->bdir->tsid) {
            xmlnode_set_attr_value(node, "tsid", acc->bdir->tsid);
        }
        if (acc->bdir->url) {
            xmlnode_set_attr_value(node, "url", acc->bdir->url);
        }

        xmlnode_set_attr_value(node, "tsname", acc->bdir->tsname ? acc->bdir->tsname : "");
    }

    /* save the xml to disk, $(datadir)/$(pvdrid)/$(freq) */
    sprintf(path, "%s%c%08x%c%08x", mgr->env->data_dir, __dir_delimeter, pvdr->id, __dir_delimeter, acc->frequency);
    kvfs_mkdir(path);
    sprintf(path, "%s%c%08x%c%08x%c__access.xml", mgr->env->data_dir, __dir_delimeter, pvdr->id, __dir_delimeter, acc->frequency, __dir_delimeter);
    xmldoc_save(doc, path);

    xmldoc_del(doc);

    return 0;
}

/**
 * @brief convert K_pvdr into several XML and save into disk
 *
 * - Save pvdr.xml for root
 * - Save access.xml for each sub directory
 */
kint sg_mgr_single_provider_save(sg_mgr *mgr, K_pvdr *pvdr, kbool skip_acc)
{
    K_dlist_entry *hdr, *entry;
    KXmlDoc *doc;
    KXmlNode *node;
    KXmlAttr *attr;

    K_pvdr_acc *acc;

    kchar *val;
    kchar path[IV_PATH_MAX_SIZE];   /* for findfirst *.* */

    kuint pvdrid;

    if (kflg_chk(pvdr->flg, BSF_VALID)) {
        klog(("Still invalid, skip save it\n"));
        return 0;
    }
    kflg_set(pvdr->flg, BSF_VALID);

    /* 1, save __pvdr.xml */
    xmldoc_gotoNode(pvdr->doc, "/", 0);
    node = xmldoc_gotoNode(pvdr->doc, "pvdr", 0);
    val = xmlnode_get_attr_value(node, "id");

    pvdrid = strtoul(val, 0, 10);

    /* create main dir */
    sprintf(path, "%s%c%08x", mgr->env->data_dir, __dir_delimeter, pvdrid);
    kvfs_mkdir(path);

    /* create fragments dir */
    sprintf(path, "%s%c%08x%cfragments", mgr->env->data_dir, __dir_delimeter, pvdrid, __dir_delimeter);
    kvfs_mkdir(path);

    /* create cfg file */
    sprintf(path, "%s%c%08x%c__pvdr.xml", mgr->env->data_dir, __dir_delimeter, pvdrid, __dir_delimeter);
    xmldoc_save(pvdr->doc, path);

    /* 2, save sub frequency */
    if (!skip_acc) {
        klog(("K_pvdr_acc not save\n"));
        return 0;
    }

    hdr = &pvdr->acchdr;
    entry = hdr->next;
    while (entry != hdr) {
        acc = FIELD_TO_STRUCTURE(entry, K_pvdr_acc, le);
        entry = entry->next;

        sg_mgr_single_provider_freq_save(mgr, pvdr, acc);
    }

    return 0;
}

/**
 * @brief save providers in memory to disk
 */
kint sg_mgr_providers_save(sg_mgr *mgr, kchar *path)
{
    K_dlist_entry *entry, *hdr;
    KXmlDoc *doc;
    KXmlNode *node;
    K_pvdr *pvdr;

    doc = xmldoc_new(knil);

    /* merge all the K_pvdr into a xmldoc */
    hdr = &mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        /* get each K_pvdr */
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        sg_mgr_single_provider_save(mgr, pvdr, ktrue);
    }

    xmldoc_del(doc);
    return 0;
}

/**
 * @brief start channel scan
 * @param[in] mgr The global SG manager instance.
 * @param[in] freq The frequency currently working.
 * @param[in] protocol SG_PROT_DAB or SG_PROT_DMB_TH or SG_PROT_IP.
 * @param[in] tmode Transfer mode.
 * @param[in] timeout Timeout for each transfer mode in tmode.
 * @param[in] userdata User's data.
 */
kint sg_mgr_channel_scan_start(sg_mgr *mgr, kint freq, kuint protocol, kchar tmode, kuint timeout, kvoid *userdata)
{
    sg_scan *scan;

    if (mgr->scan) {
        kerror(("sg_mgr_channel_scan_start, another still running, abort.\n"));
        return IVEC_FAILURE;
    }
    /*
     * Tell all the SG, the current frequency is set.
     * When gotten data from SG, it will match the frequency and save
     * frequency information into service fragment
     */
    /* XXX What can be saved in this structure */
    mgr->scan = knil;
    sg_scan_new(&mgr->scan, mgr);

    /* This scan is used for channel scan, not for live update */
    kflg_set(mgr->scan->flg, BSF_SCAN);
    mgr->scan->scan.frequency = freq;

    /* XXX what if user scan the same frequency with different protocol */
    mgr->scan->scan.protocol = protocol;       /* protocol is global for a whole SG */
    mgr->scan->scan.tmode = tmode;
    mgr->scan->scan.tcode = 0;
    mgr->scan->scan.timeout = timeout;
    mgr->scan->scan.userdata = userdata;

    /* OK, start to get boot strap, then get data for each found provider */
    return sg_scan_start(mgr->scan);
}


/**
 * @brief stop current scan
 */
kint sg_mgr_channel_scan_stop(sg_mgr *mgr)
{
    if (mgr->scan) {
        /* FIXME: All the scan, include scan struture and scan pvdr should be stopped till return */
        kmsg_send(mgr->scan->worker_thread, SCAN_STOP, mgr->scan, 0, 0, 0);
        kmsg_send(mgr->scan->worker_thread, KMSG_QUIT, 0, 0, 0, 0);
    }
    return 0;
}

/**
 * @brief Start all the SG in current manager.
 * If SG root not created, create and start it.
 * Caller will call ::ivisa_SG_EnableAutoUpdate to make SG root sync data with server.
 *
 * @param mgr All the SG root in this mgr should be started.
 *
 * @return SG root count successfully created.
 *
 * @see ::sg_mgr_stop_sg
 */
kint sg_mgr_start_sg(sg_mgr *mgr)
{
    kint bootCnt = 0;
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;

    hdr = &mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        /* for those who already started, do not restart */
        if (!pvdr->root) {
            sg_root_new(&pvdr->root, mgr, pvdr);

            /* only load the cache, do not start sync for new version data */
            sg_root_load_from_cache(pvdr->root, ktrue);

            if (kflg_chk(mgr->flg, MGR_AUTO_SYNC)) {
                sg_root_set_auto_update_flg(pvdr->root);
                sg_root_get_sdd_dir(pvdr->root);
            }

            bootCnt++;
        }
    }
    klog(("sg_mgr_start: %d provider started\n", bootCnt));
    return bootCnt;
}

/**
 * @brief Undo ::sg_mgr_start_sg
 *
 * @param mgr
 *
 * @return count of SG root successfully stopped.
 *
 * @see ::sg_mgr_start_sg
 */
kint sg_mgr_stop_sg(sg_mgr *mgr)
{
    kint bootCnt = 0;
    K_dlist_entry *entry, *hdr;
    sg_root *root;
    K_pvdr *pvdr;

    hdr = &mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        if (pvdr->root) {
            sg_root_stop(pvdr->root, kfalse);

            bootCnt++;
        }
    }
    return bootCnt;
}

/* is the provider with id equal to pid already in mgr->provhdr */
/**
 * @brief Find K_pvdr according provider ID.
 *
 * @param mgr
 * @param pid Provider ID.
 *
 * @retval knil Can not find K_pvdr according to Provider ID.
 * @retval provider ID.
 */
K_pvdr* sg_mgr_find_provider(sg_mgr *mgr, kuint pid)
{
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;

    hdr = &mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        if (pvdr->id == pid) {
            klog(("sg provider with id == %d is already in queue\n", pid));
            return pvdr;
        }
    }
    klog(("sg provider with id == %d is not in queue\n", pid));
    return knil;
}

struct _K_pvdr_acc *sg_mgr_pvdr_acc_find(sg_mgr *mgr, struct _K_pvdr *pvdr, kuint frequency)
{
    K_dlist_entry *entry, *hdr;
    K_pvdr_acc *acc;

    hdr = &pvdr->acchdr;
    entry = hdr->next;
    while (entry != hdr) {
        acc = FIELD_TO_STRUCTURE(entry, K_pvdr_acc, le);
        entry = entry->next;

        if (acc->frequency == frequency) {
            return acc;
        }
    }
    return knil;
}

