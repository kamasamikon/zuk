/* vim:set et sw=4 sts=4: */
#if VC6
#include <vld.h>
#endif
#include <string.h>

#include "sg_root.h"
#include "ivisa_sg_user_api.h"
#include "ivisa_err_code.h"

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__)
#include <Winsock2.h>
#endif

#include "sg_mgr2.h"

#include "sg_acc.h"
#include "sg_com.h"
#include "sg_ctt.h"
#include "sg_fdt.h"
#include "sg_iad.h"
#include "sg_pcc.h"
#include "sg_pcd.h"
#include "sg_pci.h"
#include "sg_pvd.h"
#include "sg_s2n_fun.h"
#include "sg_sch.h"
#include "sg_sdd.h"
#include "sg_sdp.h"
#include "sg_ser.h"
#include "sg_x2s_fun.h"
#include "sg_zbd.h"
#include "sg_imd.h"
#include "ksupport.h"

#include <kmem.h>
#include <ktsk.h>
#include <kflg.h>
#include <kdbg.h>

#include "base64.h"

#include <ktmr.h>
#include <kstr.h>
#include "ivisa_sg_config.h"

#ifdef __WIN32_TEST__
#define USE_UL_API
#endif

#define USE_UL_API

#ifdef  USE_UL_API
static sg_mgr *__g_sg_mgr = knil;
static sg_env __g_sg_env = { 0 };
kchar __bdsg_base_url[256];
char __dir_delimeter;
#endif

volatile static kbool __g_scan_done;
kint ivisa_SG_Callback_inner(IV_SG_CALLBACK_TYPE infoType, void *para1, void *para2,void *para3, void *para4);

#ifdef __WIN32_TEST__
/*---------------------------------------------------------------------------------
 * It's show time
 */
#if defined(_WIN32_WCE)
int WINAPI WinMain( HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPTSTR    lpCmdLine,
        int       nCmdShow)
#else
kint main(void)
#endif
{
    IV_SG_PROVIDER *prov;
    kuint prov_cnt = 0;

    IV_SERVICE_BASE_INFO *serbi = knil;
    kint serbi_cnt = 0;

    kint ch, idx, rv;
    kint i=0;

    kchar tmode[8] = "b";
    kuint timeout[8] = { 180000 };

    // auto restart per 120 second
    // Init only collect information
#if defined(_WIN32_WCE)
    ivisa_SG_SetEnvironment("data_dir", "\\program files\\MobileTV\\data_dir");
    ivisa_SG_SetEnvironment("temp_dir", "\\program files\\MobileTV\\temp_dir");
#else
    ivisa_SG_SetEnvironment("data_dir", "c:\\data_dir");
    ivisa_SG_SetEnvironment("temp_dir", "c:\\temp_dir");
#endif

    ivisa_SG_SetEnvironment("bdsg_url", "http://124.42.124.29/ESG/Portal.ashx");
    //ivisa_SG_SetEnvironment("bdsg_url", "http://192.168.1.102/ESG/Portal.ashx");
    ivisa_SG_SetEnvironment("protocol", "IP");
    //ivisa_SG_SetEnvironment("bs_ipa", "224.0.23.165");
    //ivisa_SG_SetEnvironment("bs_ipa", "192.168.1.215");
    //ivisa_SG_SetEnvironment("bs_port", "4090");

    ivisa_SG_Init(180, ivisa_SG_Callback_inner);
    ivisa_SG_SetLanguage("zh_CN");

    if ("test bdsg") {
        //        BdsgGetBootStrap(__g_sg_mgr, knil);
    }

    ivisa_SG_GetProviderList(&prov, &prov_cnt);
    //prov_cnt = 0;
    if (prov_cnt) {
        // found cache can now start SG
        ivisa_SG_SetProvider(prov->id);
        ivisa_SG_Start();
        sg_free_provider(prov, prov_cnt);
    } else {
#if 1   /* Do not auto scan */
        // 全部清除所有SG的数据
        ivisa_SG_Cleanup(0, 0);

        klog(("Scan channel...\n"));
        printf("Scan channel...\n");
        ivisa_SG_StartScan(666000, tmode, timeout, 0x12345678);

        printf("Scan done, any key to continue...\n");
#if defined(_WIN32_WCE)
        Sleep(100);
#else
        getch();
#endif

        ivisa_SG_GetProviderList(&prov, &prov_cnt);
        printf("call ivisa_SG_GetProviderList: cnt: %d\n", prov_cnt);
        if (prov_cnt) {
            // found cache can now start SG
            ivisa_SG_SetProvider(prov->id);
            sg_free_provider(prov, prov_cnt);
            ivisa_SG_EnableAutoUpdate(ktrue);
        }
#endif
    }
    ivisa_SG_EnableAutoUpdate(ktrue);

    klog(("main, loop...\n"));
    printf("\n\n\n\nmain, loop...\n");

#if defined(_WIN32_WCE)
    prov_cnt = 0;
    while(!prov_cnt){
        Sleep(5000);
        ivisa_SG_GetProviderList(&prov, &prov_cnt);
    }

#endif





    while (1) {
#if defined(_WIN32_WCE)
        Sleep(5000);
        i++;
        switch(i){
            case 8000:
                ch = 'Q';
                break;
            case 30:
                ch = '1';
                break;
            case 31:
                ch = 's';
                break;
            default:
                ch = '0';
                break;
        }
#else
        ch = getch();
#endif
        printf("\n\n\nyou have hit : %d\n\n", ch);
        if (ch == 'Q') {
            klog(("Quiting\n"));
            ivisa_SG_Finalize();
            klog(("Exit...\n"));
            //show_mem_stat();
            Iv_FileRev_MemoryStat();
            return IVEC_SUCCESS;
        }
        if (ch == '0') {
            //            kmem_report();
        }
        if (ch == '1') {
            kint ivisa_SG_GetProviderList(IV_SG_PROVIDER **providerInfo, kuint *providerNum);
            kint ivisa_SG_SetProvider(kuint providerID);

            ivisa_SG_GetProviderList(&prov, &prov_cnt);
            printf("ivisa_SG_GetProviderList:cnt:%d\n", prov_cnt);
            if (prov_cnt) {
                for (idx = 0; idx < prov_cnt; idx++) {
                    klog(("\nivisa_SG_GetProviderList:%d========================\n", idx));
                    klog(("id:%d\n", prov[idx].id));
                    klog(("auxData:%s\n", prov[idx].auxData));
                    klog(("infoUrl:%s\n", prov[idx].infoUrl));
                    klog(("name:%s\n", prov[idx].name));
                    klog(("uri:%s\n", prov[idx].uri));
                }
                ivisa_SG_SetProvider(prov->id);
                sg_free_provider(prov, prov_cnt);
            }
        }
        if (ch == 's') {
            static int curServiceIndex = 0;
            if (0 == serbi_cnt) {
                ivisa_SG_GetServiceList(&serbi, &serbi_cnt);
            }
            if (curServiceIndex < serbi_cnt) {
                ivisa_SG_SetCurrentService(serbi[curServiceIndex].serviceID);
                curServiceIndex++;

                if (curServiceIndex == serbi_cnt) {
                    curServiceIndex = 0;
                }
            }
        }
        if (ch == 'D') {
            kint frequency = 666000;

            kchar tmode = SG_TMODE_BDIR;
            kuint timeout = 180000;

            serbi_cnt = 0;
            ivisa_SG_Cleanup(0, 0);
            __g_scan_done = kfalse;

            sg_mgr_channel_scan_start(__g_sg_mgr, frequency, __g_sg_env.protocol, tmode, timeout, 0);

            while (!__g_scan_done) {
                ktsk_sleep(1000);
                printf("wo deng\n");
            }

            ivisa_SG_GetProviderList(&prov, &prov_cnt);
            printf("call ivisa_SG_GetProviderList: cnt: %d\n", prov_cnt);
            if (prov_cnt) {

                // found cache can now start SG
                ivisa_SG_SetProvider(prov->id);

                printf("call ivisa_SG_Start\n");
                ivisa_SG_Start();
                ivisa_SG_EnableAutoUpdate(ktrue);
            }

        }

        if (ch == 'x') {
            serbi_cnt = 0;
            ivisa_SG_Cleanup(0, 0);
            __g_scan_done = kfalse;
            ivisa_SG_StartScan(666000, tmode, timeout, 0x12345678);
            while (!__g_scan_done) {
                ktsk_sleep(1000);
                printf("wo deng\n");
            }

            ivisa_SG_GetProviderList(&prov, &prov_cnt);
            printf("call ivisa_SG_GetProviderList: cnt: %d\n", prov_cnt);
            if (prov_cnt) {

                // found cache can now start SG
                ivisa_SG_SetProvider(prov->id);

                printf("call ivisa_SG_Start\n");
                ivisa_SG_Start();
                ivisa_SG_EnableAutoUpdate(ktrue);
            }

        }
        if (ch == 'r') {
            sg_root *rt = __g_sg_mgr->cursg;
            if (rt) {
                sg_root_start_sync(rt);
            } else {
                printf("!!! Current SG not set\n");
            }
        }
        if (ch == 'p') {
            kuint i;
            IV_PURCHASE_ITEM_BASE_INFO *purchaseItemList;
            kint purchaseItemNum;
            IV_PURCHASE_ITEM_DETAIL_INFO purchaseItemInfo;


            ivisa_SG_GetServiceList(&serbi, &serbi_cnt);
            for (idx = 0; idx < serbi_cnt; idx++) {
                klog(("\nivisa_SG_GetPurchaseItemList:%d========================\n", idx));
                klog(("serviceID:%s\n", serbi[idx].serviceID));
                ivisa_SG_GetPurchaseItemList(serbi[idx].serviceID, 0, 0xFFFFFFFF, &purchaseItemList, &purchaseItemNum);

                for (i = 0; i < purchaseItemNum; i++) {
                    ivisa_SG_GetPurchaseItemDetails(purchaseItemList[i].purchaseItemID, &purchaseItemInfo);
                }
            }
        }
        if (ch == '2') {
            kint i;
            for (i = 0; i < prov_cnt; i++) {
                ivisa_SG_Cleanup(prov[i].id, 0);
            }
        }
        if (ch == '3') {
            IV_PROGRAM_BASE_INFO *progbi;
            kint progbi_cnt, i, j;
            IV_PREVIEW_INFO pvi;

            IV_ACCESS_INFO *ac;
            kint ac_cnt;

            IV_SERVICE_DETAIL_INFO sdi;

            ivisa_SG_GetServiceList(&serbi, &serbi_cnt);
            for (idx = 0; idx < serbi_cnt; idx++) {
                klog(("\nivisa_SG_GetServiceList:%d========================\n", idx));
                klog(("serviceID:%s\n", serbi[idx].serviceID));
                klog(("name:%s\n", serbi[idx].name));
                klog(("weight:%u\n", serbi[idx].weight));
                klog(("serviceProtection:%u\n", serbi[idx].serviceProtection));
                klog(("serviceType:%u\n", serbi[idx].serviceType));
                klog(("MIMEType:%s\n", serbi[idx].MIMEType));
            }
            // kint ivisa_SG_GetServiceDetailInfo(kchar *serviceID, IV_SERVICE_DETAIL_INFO *serviceInfo)
            for (i = 0; i < serbi_cnt; i++) {
                if (IVEC_SUCCESS == ivisa_SG_GetServiceDetailInfo(serbi[i].serviceID, &sdi)) {
                    klog(("\nivisa_SG_GetServiceDetailInfo=%s=======================\n", serbi[i].serviceID));

                    klog(("description:%s\n", sdi.description));
                    klog(("parentRating:%s\n", sdi.parentRating));
                    klog(("genre:%s\n", sdi.genre));
                    klog(("globalServiceID:%s\n", sdi.globalServiceID));
                }
            }

            for (i = 0; i < serbi_cnt; i++) {
                int w;
                kchar *desc;
                ivisa_SG_GetProgramList(serbi[i].serviceID, 1, 0x7FFFFF00, &progbi, &progbi_cnt);
                for (idx = 0; idx < progbi_cnt; idx++) {
                    klog(("\nivisa_SG_GetProgramList:%d=%s=======================\n", idx, serbi[i].serviceID));
                    klog(("name:%s\n", progbi[idx].name));
                    klog(("programID:%s\n", progbi[idx].programID));

                    klog(("startTime:%u\n", progbi[idx].startTime));
                    klog(("endTime:%u\n", progbi[idx].endTime));
                    klog(("duration:%u\n", progbi[idx].duration));
                    klog(("parentRating:%s\n", progbi[idx].parentRating));
                    klog(("genre:%s\n", progbi[idx].genre));
                    klog(("contentProtection:%s\n", progbi[idx].contentProtection));

                    desc = knil;
                    if (IVEC_SUCCESS == ivisa_SG_GetProgramDescription(progbi[idx].programID, &desc)) {
                        klog(("progDesc:%s\n", desc));
                    } else {
                        klog(("ivisa_SG_GetProgramDescription for %s failed\n", progbi[idx].programID));
                    }
                }
            }
            for (i = 0; i < serbi_cnt; i++) {
                ac_cnt = 0;
                rv = ivisa_SG_GetServiceAccessInfo(serbi[i].serviceID, &ac, &ac_cnt);

                for (idx = 0; (!rv) && idx < ac_cnt; idx++) {
                    klog(("\nivisa_SG_GetServiceAccessInfo:%u=%s=======================\n", idx, serbi[i].serviceID));
                    klog(("usage:%s\n", ac[idx].usage));
                    klog(("defaultAccessFlag:%u\n", ac[idx].defaultAccessFlag));
                    klog(("scNum:%u\n", ac[idx].scNum));
                    klog(("validFrom:%u\n", ac[idx].validFrom));
                    klog(("validTo:%u\n", ac[idx].validTo));

                    klog(("\nivisa_SG_GetServiceAccessInfo:%u=%s=======================\n", idx, serbi[i].serviceID));
                    klog(("usage:%s\n", ac[idx].usage));
                    klog(("defaultAccessFlag:%u\n", ac[idx].defaultAccessFlag));
                    klog(("scNum:%u\n", ac[idx].scNum));
                    klog(("validFrom:%u\n", ac[idx].validFrom));
                    klog(("validTo:%u\n", ac[idx].validTo));

                    klog(("\nivisa_SG_GetServiceAccessInfo:%u=%s=======================\n", idx, serbi[i].serviceID));
                    klog(("usage:%s\n", ac[idx].usage));
                    klog(("defaultAccessFlag:%u\n", ac[idx].defaultAccessFlag));
                    klog(("scNum:%u\n", ac[idx].scNum));
                    klog(("validFrom:%u\n", ac[idx].validFrom));
                    klog(("validTo:%u\n", ac[idx].validTo));
                    klog(("sessionParams.sdp.data:%s\n", ac[idx].sessionParams[0].sdp.data));
                    klog(("sessionParams.sdp.length:%u\n", ac[idx].sessionParams[0].sdp.length));
                    klog(("sessionParams.sdp.uri:%s\n", ac[idx].sessionParams[0].sdp.uri));

                    for(j = 0; j < ac[idx].scNum; j++){
                        klog(("sessionParams[%d].dvbAccess.meidaType:%d\n", j, ac[idx].sessionParams[j].dvbAccess.meidaType));
                        klog(("sessionParams[%d].dvbAccess.onID:%x\n", j, ac[idx].sessionParams[j].dvbAccess.onID));
                        klog(("sessionParams[%d].dvbAccess.protocolType:%u\n", j, ac[idx].sessionParams[j].dvbAccess.protocolType));
                        klog(("sessionParams[%d].dvbAccess.tsID:%x\n", j, ac[idx].sessionParams[j].dvbAccess.tsID));
                        klog(("sessionParams[%d].dvbAccess.serviceComponentID:%u\n", j, ac[idx].sessionParams[j].dvbAccess.serviceComponentID));

                        klog(("sessionParams[%d].dvbAccess.meidaType:%d\n", j, ac[idx].sessionParams[j].dabAccess.meidaType));
                        klog(("sessionParams[%d].dvbAccess.eID:%x\n", j, ac[idx].sessionParams[j].dabAccess.eID));
                        klog(("sessionParams[%d].dvbAccess.protocolType:%u\n", j, ac[idx].sessionParams[j].dabAccess.protocolType));
                        klog(("sessionParams[%d].dvbAccess.channelID:%u\n", j, ac[idx].sessionParams[j].dabAccess.channelID));
                        klog(("sessionParams[%d].dvbAccess.serviceComponentID:%u\n", j, ac[idx].sessionParams[j].dabAccess.serviceComponentID));
                        klog(("sessionParams[%d].dvbAccess.mode:%u\n", j, ac[idx].sessionParams[j].dabAccess.mode));

                    }
                }
            }
            for (i = 0; i < serbi_cnt; i++) {
                kuint j;
                for (j = 0; j < 6; j++) {
                    if (IVEC_SUCCESS == ivisa_SG_GetServicePreviewInfo(serbi[i].serviceID, j, &pvi)) {
                        klog(("\nivisa_SG_GetServicePreviewInfo:=%s=%d======================\n", serbi[i].serviceID, j));
                        klog(("MIMEType:%s\n", pvi.MIMEType));
                        klog(("file.data:%s\n", pvi.file.data));
                        klog(("file.length:%u\n", pvi.file.length));
                        klog(("file.uri:%s\n", pvi.file.uri));
                    }
                }
            }
        }
    }
    //show_mem_stat();
    Iv_FileRev_MemoryStat();
}
#endif /* __WIN32_TEST__ */

kint sg_free_provider(IV_SG_PROVIDER *prov, kuint cnt)
{
    int i;
    for (i = 0; i < cnt; i++) {
        kmem_free(prov[i].name);
        kmem_free(prov[i].uri);
        kmem_free(prov[i].infoUrl);
        kmem_free(prov[i].auxData);
        kmem_free(prov[i].logo.MIMEType);
        kmem_free(prov[i].logo.file.data);
        kmem_free(prov[i].logo.file.uri);
    }
    kmem_free(prov);
    return 0;
}

/**
 * @brief Set environment variables for sg module.
 *
 * @param key name of environment, string.
 * @param val value for the key, will be convert to right data type for key.
 *
 * @return IVEC_SUCCESS if successfully set, otherwise return IVEC_FAILURE.
 *
 * @pre The environment must be set <b>before</b> ::ivisa_SG_Init
 */
kint ivisa_SG_SetEnvironment(kchar *key, kchar *val)
{
    kint ret = IVEC_FAILURE;

    if (streq(key, "data_dir")) {
        strncpy(__g_sg_env.data_dir, val, sizeof(__g_sg_env.data_dir) - 1);
        __g_sg_env.data_dir[sizeof(__g_sg_env.data_dir) - 1] = '\0';
        ret = IVEC_SUCCESS;
    } else if (streq(key, "temp_dir")) {
        strncpy(__g_sg_env.temp_dir, val, sizeof(__g_sg_env.temp_dir) - 1);
        __g_sg_env.temp_dir[sizeof(__g_sg_env.temp_dir) - 1] = '\0';
        ret = IVEC_SUCCESS;
    } else if (streq(key, "bdsg_url")) {
        strncpy(__g_sg_env.bdsg_url, val, sizeof(__g_sg_env.bdsg_url) - 1);
        __g_sg_env.bdsg_url[sizeof(__g_sg_env.bdsg_url) - 1] = '\0';
        kstr_subs(__g_sg_env.bdsg_url, '\r', '\0');
        kstr_subs(__g_sg_env.bdsg_url, '\n', '\0');
        ret = IVEC_SUCCESS;
    } else if (streq(key, "protocol")) {
        kuint prot = SG_PROT_INVALID;
        ret = IVEC_SUCCESS;

        if (streq(val, "dmb_th")) {
            prot = SG_PROT_DMB_TH;
        } else if (streq(val, "dab")) {
            prot = SG_PROT_DAB;
        } else if (streq(val, "ip")) {
            prot = SG_PROT_IP;
        } else {
            kerror(("Bad protocol (%s) set, should be one of 'dmb_th' 'dab' or 'ip'\n", val));
            ret = IVEC_FAILURE;
        }

        __g_sg_env.protocol = prot;
    } else if (streq(key, "bs_ipa")) {
        strncpy(__g_sg_env.bs_ipa, val, sizeof(__g_sg_env.bs_ipa) - 1);
        __g_sg_env.bs_ipa[sizeof(__g_sg_env.bs_ipa) - 1] = '\0';
        ret = IVEC_SUCCESS;
    } else if (streq(key, "bs_port")) {
        __g_sg_env.bs_port = strtoul(val, 0, 10);
        ret = IVEC_SUCCESS;
    } else if (streq(key, "time_diff")) {
        __g_sg_env.time_diff = strtol(val, 0, 10);
        ret = IVEC_SUCCESS;
    }

    return ret;
}

static kvoid sg_FreeEnvironment(kvoid)
{
}

kint ivisa_SG_Start()
{
    IV_SG_PROVIDER *prov;
    kuint prov_cnt = 0;

    kchar path[IV_PATH_MAX_SIZE];

    ivisa_SG_GetProviderList(&prov, &prov_cnt);
    if (prov_cnt) {
        kint idx;
        for (idx = 0; idx < prov_cnt; idx++) {
            sprintf(path,"%s%c%08x%c%s",__g_sg_mgr->env->data_dir,__dir_delimeter,prov[idx].id,__dir_delimeter,ITV_DIR_NAME);
            kvfs_mkdir(path);

            sprintf(path,"%s%c%08x%c%s",__g_sg_mgr->env->temp_dir,__dir_delimeter,prov[idx].id,__dir_delimeter,ITV_DIR_NAME);
            kvfs_mkdir(path);
        }
        sg_free_provider(prov, prov_cnt);
    }

    sg_mgr_start_sg(__g_sg_mgr);
    return IVEC_SUCCESS;
}


#define USE_UL_API
#ifdef  USE_UL_API
// 1, check the cache bootstrap information
// 2, if can boot, boot, return ok
// 3, if no cache return ng
kint ivisa_SG_Init(kuint autoUpdateTime, IVISA_SG_CALLBACK evtCallback)
{
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__)
    WSADATA wsd;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsd)) {
        klog(("WSAStartup() failed\n"));
        return IVEC_FAILURE;
    }
#endif

    /* kmem_init(KMF_TRACK | KMF_CHECK); */
    kmem_init(0);

    if (!__g_sg_env.bdsg_url) {
        ivisa_SG_SetEnvironment("bdsg_url", "http://124.42.124.29/ESG/Portal.ashx");
    }

    /* set __dir_delimeter before sg_mgr_new */
    __dir_delimeter = kvfs_path_sep();

    /* should callbed before others */
    ktmr_init();

#ifdef USE_TWO_WAY_SG
    sg_bdsg_new();
#endif

    klog(("__g_sg_mgr:%x\n", __g_sg_mgr));
    kassert(!__g_sg_mgr);

    if (sg_mgr_new(&__g_sg_mgr, &__g_sg_env)) {
        kerror(("sg_mgr_new failed\n"));
        return IVEC_FAILURE;
    }

    klog(("All Inited: __g_sg_mgr:%x, __g_sg_env:%x\n", __g_sg_mgr, __g_sg_env));
    klog(("All Inited: data_dir:%s, temp_dir:%s\n", __g_sg_env.data_dir, __g_sg_env.temp_dir));

    /* min update period is 120 seconds */
    if (autoUpdateTime < 120) {
        autoUpdateTime = 120;
    }

    __g_sg_mgr->conf.autoUpdateTime = autoUpdateTime;
    __g_sg_mgr->conf.evtCallback = evtCallback;
    //__g_sg_mgr->conf.prot = IV_FILEREV_PROTOCOL_FLUTEALC;

    return IVEC_SUCCESS;
}

kint ivisa_SG_Finalize(kvoid)
{
    IV_SG_PROVIDER *prov;
    kuint prov_cnt = 0;
    kchar path[IV_PATH_MAX_SIZE];

    ivisa_SG_GetProviderList(&prov, &prov_cnt);
    if (prov_cnt) {
        kint idx;

        /* delete sub directories */
        for (idx = 0; idx < prov_cnt; idx++) {
            sprintf(path,"%s%c%08x%c%s",__g_sg_mgr->env->data_dir,__dir_delimeter,prov[idx].id,__dir_delimeter,ITV_DIR_NAME);
            klog(("ivisa_SG_Finalize::delete dir : %s\n", path));
            kvfs_rmdir(path);
        }

        /* delete temp_dir self */
        kvfs_rmdir(__g_sg_mgr->env->temp_dir);

        sg_free_provider(prov, prov_cnt);
    }

    /* delete the SG manager */
    klog(("ivisa_SG_Finalize::sg_mgr_del\n"));
    sg_mgr_del(__g_sg_mgr);

#ifdef USE_TWO_WAY_SG
    sg_bdsg_del();
#endif

    klog(("ivisa_SG_Finalize::ktmr_final\n"));
    ktmr_final();

    klog(("ivisa_SG_Finalize::sg_FreeEnvironment\n"));
    sg_FreeEnvironment();

    klog(("ivisa_SG_Finalize::bye\n"));

    kmem_final();
    return IVEC_SUCCESS;
}
kint ivisa_SG_GetVersion(kuint *majorVer, kuint *minorVer, kuint *revision)
{
    *majorVer = 1;
    *minorVer = 0;
    *revision = 0;
    return IVEC_SUCCESS;
}

/*
 * Get the provider list.
 * XXX what if the Provider is temporary.
 */
kint ivisa_SG_GetProviderList(IV_SG_PROVIDER **providerInfo, kuint *providerNum)
{
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;

    IV_SG_PROVIDER *prov;
    sg_root *rt;
    kint i, pcnt = 0;

    /*
     * All the provider in provhdr, and, the temporary for scanning also
     * in the queue, these temporary providers should be skipped.
     */
    hdr = &__g_sg_mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        if (kflg_chk(pvdr->flg, BSF_SCAN)) {
            klog(("this provider in scanning, skip it\n"));
            continue;
        }
        pcnt++;
    }

    if (pcnt) {
        prov = (IV_SG_PROVIDER*)kmem_alloz(sizeof(IV_SG_PROVIDER) * pcnt);
        if (!prov) {
            kerror(("memory failed for IV_SG_PROVIDER\n"));
            return IVEC_FAILURE;
        }
        *providerNum = pcnt;
        *providerInfo = prov;

        i = 0;
        entry = hdr->next;
        while (entry != hdr) {
            pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
            entry = entry->next;

            if (kflg_chk(pvdr->flg, BSF_SCAN)) {
                klog(("this provider in scanning, skip it\n"));
                continue;
            }

            prov[i].id = pvdr->id;
            //            STR_SET(prov[i].name,pvdr->name);
            //            STR_SET(prov[i].uri, pvdr->uri);
            //            STR_SET(prov[i].infoUrl, pvdr->infoUrl);
            //            STR_SET(prov[i].auxData, pvdr->auxData);
            i++;
        }

        kassert(i == pcnt);
    } else {
        *providerInfo = knil;
        *providerNum = 0;
    }
    return IVEC_SUCCESS;
}

/**
 * @brief Enable or disable auto update.
 *
 * @param enable ktrue to enable, kfalse to disable
 *
 * @return IVEC_SUCCESS
 *
 * @warning Current version only apply for created SG provider,
 * what if one provider newed after this called?
 */
kint ivisa_SG_EnableAutoUpdate(kbool enable)
{
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;

    kuint i;
    sg_root *rt, *tmp_root;

    /* first, set global flag */
    if (enable) {
        kflg_set(__g_sg_mgr->flg, MGR_AUTO_SYNC);
    } else {
        kflg_clr(__g_sg_mgr->flg, MGR_AUTO_SYNC);
    }

    /* apply it to current sg provider */
    hdr = &__g_sg_mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        if (pvdr->root) {
            if (enable) {
                /* mark, then try to start it */
                sg_root_set_auto_update_flg(pvdr->root);
                sg_root_start_sync(pvdr->root, 0);
            } else {
                sg_root_clr_auto_update_flg(pvdr->root);
                sg_root_abort_sync(pvdr->root);
            }
        }
    }
    return IVEC_SUCCESS;
}

/**
 * @brief Set the current SG provider
 *
 * @param providerID Identifier for SG provider, can not be zero
 *
 * @return IVEC_SUCCESS for success, IVEC_FAILURE for error
 */
kint ivisa_SG_SetProvider(kuint providerID)
{
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;

    kuint i;
    sg_root *rt, *tmp_root;

    klog(("ivisa_SG_SetProvider:%d\n", providerID));

    /* already set, skip */
    if (__g_sg_mgr->cursg && (__g_sg_mgr->cursg->pvdr->id == providerID)) {
        klog(("ivisa_SG_SetProvider: (%d) already set!\n", providerID));
        return IVEC_SUCCESS;
    }

    __g_sg_mgr->cursg = knil;

    hdr = &__g_sg_mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        /*
         * XXX for scanning provider, this operation should be going on,
         * because some provider can cover more than one frequency, so,
         * wen can not confirm if this provider is valid.
         */
        if (kflg_chk(pvdr->flg, BSF_SCAN)) {
            klog(("this provider in scanning, skip it\n"));
            continue;
        }

        if (providerID == pvdr->id) {
            if (!pvdr->root) {
                sg_root_new(&pvdr->root, __g_sg_mgr, pvdr);
                sg_root_load_from_cache(pvdr->root, ktrue);

                if (kflg_chk(__g_sg_mgr->flg, MGR_AUTO_SYNC)) {
                    sg_root_set_auto_update_flg(pvdr->root);
                    sg_root_get_sdd_dir(pvdr->root);
                }
            } else {
                /*
                 * If the transfer layer is auto update, do not need to stop the root
                 * already started, do not load the cache
                 */
            }

            __g_sg_mgr->cursg = pvdr->root;
            return IVEC_SUCCESS;
        }
    }
    /* if failed, do not switch the cursg */
    kerror(("ivisa_SG_SetProvider: %d not exist or temporary\n"));
    return IVEC_FAILURE;
}

// Only Cleanup all the data, if the thread runs, let it be.
// erase BS info
// erase cache data
// stop the thread
// free RTI
// XXX Do not free __g_sg_mgr->curser, it will be freed when the service deleted.
/*
 * XXX what if A provider's ID is zero
 */
/**
 * @brief Totally remove a SG provider from system.
 *
 * @param[in] providerID which provider to be removed, if set zero, ALL provider will be removed
 * @param[in] frequency which frequency for providerID to be removed, if set zero, ALL frequency will be removed
 *
 * @return IVEC_SUCCESS.
 */
kint ivisa_SG_Cleanup(kuint providerID, kuint frequency)
{
    K_dlist_entry *entry, *hdr;
    sg_root *rt;
    K_pvdr *pvdr;
    K_pvdr_acc *acc;

    kchar path[IV_PATH_MAX_SIZE];
    kuint provID;

    kerror(("\nTODO: cleanup for frequency not yet\n\n"));

    if (providerID) {
        pvdr = sg_mgr_find_provider(__g_sg_mgr, providerID);
        if (pvdr) {
            provID = pvdr->id;

            if (__g_sg_mgr->cursg == pvdr->root) {
                __g_sg_mgr->cursg = knil;
            }
            if (pvdr->root) {
                sg_root_del(pvdr->root, ktrue);
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
                    kmem_free(acc->bdir->tsid);
                    kmem_free(acc->bdir->tsname);
                    kmem_free(acc->bdir);
                }

                kmem_free(acc);
            }

            /* delete cache directory */
            sprintf(path, "%s%c%08x", __g_sg_mgr->env->data_dir, __dir_delimeter, provID);
            kvfs_rmdir(path);

            remove_dlist_entry(&pvdr->le);
            kmem_free(pvdr);
        }
    } else {
        /* Cleanup all the SG */
        ktmr_killall(0); /* stop all timer to avoid producing new action */
        klog(("Cleanup sg, stop all timer\n"));

        hdr = &__g_sg_mgr->provhdr;
        while (!is_dlist_empty(hdr)) {
            entry = remove_dlist_head_entry(hdr);
            pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);

            provID = pvdr->id;

            if (__g_sg_mgr->cursg == pvdr->root) {
                __g_sg_mgr->cursg = knil;
            }

            /* delete SG provider, memory also be removed */
            if (pvdr->root) {
                sg_root_del(pvdr->root, ktrue);
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
                    kmem_free(acc->bdir->tsid);
                    kmem_free(acc->bdir->tsname);
                    kmem_free(acc->bdir);
                }

                kmem_free(acc);
            }

            /* delete cache directory */
            sprintf(path, "%s%c%08x", __g_sg_mgr->env->data_dir, __dir_delimeter, provID);
            kvfs_rmdir(path);

            remove_dlist_entry(&pvdr->le);
            kmem_free(pvdr);
        }
    }
    return IVEC_SUCCESS;
}

kint ivisa_SG_StartScan(kint frequency, kchar tmode, kuint timeout, kvoid *userdata)
{
    kchar trans_mode;

    if (tmode == 'b' || tmode == 'B') {
        trans_mode = SG_TMODE_BDIR;
    } else if (tmode == 'f' || tmode == 'F') {
        trans_mode = SG_TMODE_FLUTE;
    } else if (tmode == 'm' || tmode == 'M') {
        trans_mode = SG_TMODE_MOT;
    } else {
        return IVEC_FAILURE;
    }

    if (sg_mgr_channel_scan_start(__g_sg_mgr, frequency, __g_sg_env.protocol, trans_mode, timeout, userdata)) {
        return IVEC_FAILURE;
    }
    return IVEC_SUCCESS;
}

kint ivisa_SG_StopScan(kvoid)
{
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;

    if (__g_sg_mgr->scan) {

        sg_mgr_channel_scan_stop(__g_sg_mgr);

        /* apply it to current sg provider */
        hdr = &__g_sg_mgr->provhdr;
        entry = hdr->next;
        while (entry != hdr) {
            pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
            entry = entry->next;

            if (!pvdr->root) {
                continue;
            }

            if (kflg_chk(pvdr->root->flg, ERF_SCAN)) {
                sg_root_clr_auto_update_flg(pvdr->root);
                sg_root_abort_sync(pvdr->root);
            }
        }
        return IVEC_SUCCESS;
    }
    return IVEC_FAILURE;
}


// this routine is impel by ui layer, SG simply call the callback to
// indicate the sg information has been changed.
kint ivisa_SG_Callback_inner(IV_SG_CALLBACK_TYPE infoType, void *para1, void *para2,void *para3, void *para4)
{
#ifdef __WIN32_TEST__
    kint ret;
    IV_INTERACTIVITY_DIRECTORY *idir = knil;
    switch (infoType) {
        case IV_SG_CALLBACK_SCAN:
            klog(("IV_SG_CALLBACK_SCAN:status(%d), receivedFileNum(%d), foundFileNum(%d), userData1(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_SCAN:status(%d), receivedFileNum(%d), foundFileNum(%d), userData1(%x)\n",
                    para1, para2, para3, para4);

            if (((kint)para1 == 0) && ((kint)para2 == (kint)para3)) {
                __g_scan_done = ktrue;
            }
            break;
        case IV_SG_CALLBACK_INTERACTIVITY:
            klog(("IV_SG_CALLBACK_INTERACTIVITY:serid(%s), par1(%d), par2(%d), par3(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_INTERACTIVITY:serid(%s), par1(%d), par2(%d), par3(%x)\n",
                    para1, para2, para3, para4);

            ret = ivisa_SG_GetCurrentInteractivity((kchar*)para1, &idir);
            if (!ret) {
                printf("GetCurrentInteractivity successfully\n");
            } else {
                printf("GetCurrentInteractivity failed\n");
            }
            break;
        case IV_SG_CALLBACK_NOTIFICATION:
            klog(("IV_SG_CALLBACK_NOTIFICATION:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_NOTIFICATION:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                    para1, para2, para3, para4);
            break;
        case IV_SG_CALLBACK_SERVICE_UPDATE:
            klog(("IV_SG_CALLBACK_SERVICE_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_SERVICE_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                    para1, para2, para3, para4);
            break;
        case IV_SG_CALLBACK_PROGRAM_UPDATE:
            klog(("IV_SG_CALLBACK_PROGRAM_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_PROGRAM_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                    para1, para2, para3, para4);
            break;
        case IV_SG_CALLBACK_ACCESS_UPDATE:
            klog(("IV_SG_CALLBACK_ACCESS_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_ACCESS_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                    para1, para2, para3, para4);
            break;
        case IV_SG_CALLBACK_PURCHASE_UPDATE:
            klog(("IV_SG_CALLBACK_PURCHASE_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                        para1, para2, para3, para4));
            printf("IV_SG_CALLBACK_PURCHASE_UPDATE:par0(%d), par1(%d), par2(%d), par3(%x)\n",
                    para1, para2, para3, para4);
            break;
        default:
            kerror(("ivisa_SG_Callback: unknown infoType:%d - %x, %x, %x, %x\n", infoType,
                        para1, para2, para3, para4));
    }
#endif /* __WIN32_TEST__ */
    return IVEC_SUCCESS;
}
kint ivisa_SG_SetLanguage(kchar *language)
{
    STR_SET(__g_sg_mgr->conf.lang, language);
    return IVEC_SUCCESS;
}
kint ivisa_SG_GetServiceList(IV_SERVICE_BASE_INFO **serviceList, kint *serviceNum)
{
    kchar *deflang;
    IV_SERVICE_BASE_INFO *sbi;
    kuint cnt = 0, curtime = ksys_ntp_time() + __g_sg_mgr->env->time_diff;
    K_dlist_entry *pvdr_entry, *pvdr_hdr, *fb_entry, *fb_hdr, *ser_entry, *ser_hdr;
    K_pvdr *pvdr;
    K_freq *fblk;
    sg_ser *ser;

    deflang = __g_sg_mgr->conf.lang;

    /*
     * first, get the count
     */

    /* walk all provider */
    pvdr_hdr = &__g_sg_mgr->provhdr;
    pvdr_entry = pvdr_hdr->next;
    while (pvdr_entry != pvdr_hdr) {
        pvdr = FIELD_TO_STRUCTURE(pvdr_entry, K_pvdr, le);
        pvdr_entry = pvdr_entry->next;

        if (!pvdr->root) {
            klog(("provider not loaded\n"));
            continue;
        }

        /* walk all frequency */
        fb_hdr = &pvdr->root->frqhdr;
        fb_entry = fb_hdr->next;
        while (fb_entry != fb_hdr) {
            fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
            fb_entry = fb_entry->next;

            /* walk all services to get count */
            ser_hdr = &fblk->fhdrs[SER];
            ser_entry = ser_hdr->next;
            while (ser_entry != ser_hdr) {
                ser = FIELD_TO_STRUCTURE(ser_entry, sg_ser, cls_le);
                ser_entry = ser_entry->next;
                cnt++;
            }
        }
    }
    if (!cnt) {
        *serviceList = knil;
        *serviceNum = 0;
        return IVEC_SUCCESS;
    }

    sbi = (IV_SERVICE_BASE_INFO*)kmem_alloz(cnt * sizeof(IV_SERVICE_BASE_INFO));

    /* second, fill the list */
    cnt = 0;

    /* walk all provider */
    pvdr_hdr = &__g_sg_mgr->provhdr;
    pvdr_entry = pvdr_hdr->next;
    while (pvdr_entry != pvdr_hdr) {
        pvdr = FIELD_TO_STRUCTURE(pvdr_entry, K_pvdr, le);
        pvdr_entry = pvdr_entry->next;

        if (!pvdr->root) {
            klog(("provider not loaded\n"));
            continue;
        }

        /* walk all frequency */
        fb_hdr = &pvdr->root->frqhdr;
        fb_entry = fb_hdr->next;
        while (fb_entry != fb_hdr) {
            fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
            fb_entry = fb_entry->next;

            /* walk all services to get count */
            ser_hdr = &fblk->fhdrs[SER];
            ser_entry = ser_hdr->next;
            while (ser_entry != ser_hdr) {
                ser = FIELD_TO_STRUCTURE(ser_entry, sg_ser, cls_le);
                ser_entry = ser_entry->next;

                sbi[cnt].MIMEType;

                if (ser->arr->Name) {
                    kuint j;
                    // language
                    for (j = 0; j < ser->arr->Name->ecnt; j++) {
                        if (streq(deflang, ser->arr->Name->arr[j].attr.xml_lang)) {
                            STR_SET(sbi[cnt].name, ser->arr->Name->arr[j].val.Name);
                            break;
                        }
                    }
                    if (j == ser->arr->Name->ecnt) {
                        sbi[cnt].name = kstr_dup(ser->arr->Name->arr[0].val.Name);
                    }
                }

                sbi[cnt].serviceID = kstr_dup(ser->attr.id);
                sbi[cnt].serviceProtection = ser->arr->attr.serviceContentProtection;
                if (ser->arr->ServiceType) {
                    sbi[cnt].serviceType = ser->arr->ServiceType->arr[0].val.ServiceType;
                }
                sbi[cnt].weight = ser->arr->attr.weight;

                cnt++;
            }
        }
    }

    *serviceList = sbi;
    *serviceNum = cnt;

    return IVEC_SUCCESS;
}

kint om_GetServiceDetailInfo(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kchar *serviceID = (kchar*)a_ar1;
    IV_SERVICE_DETAIL_INFO *serviceInfo = (IV_SERVICE_DETAIL_INFO*)a_ar2;
    kint *ret = (kint *)a_ar3;

    kchar *deflang;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_ser *ser;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        *ret = IVEC_FAILURE;
        return 0;
    }

    deflang = rt->mgr->conf.lang;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[SER];

        // first, get the count
        entry = hdr->next;
        while (entry != hdr) {
            ser = FIELD_TO_STRUCTURE(entry, sg_ser, cls_le);
            entry = entry->next;

            if (streq(serviceID, ser->attr.id)) {

                memset(serviceInfo, 0, sizeof(IV_SERVICE_DETAIL_INFO));

                // TODO fill the serviceInfo
                if (ser->arr->Description) {
                    kuint j;
                    // language
                    for (j = 0; j < ser->arr->Description->ecnt; j++) {
                        if (streq(deflang, ser->arr->Description->arr[j].attr.xml_lang)) {
                            serviceInfo->description = kstr_dup(ser->arr->Description->arr[j].val.Description);
                            break;
                        }
                    }
                    if (j == ser->arr->Description->ecnt) {
                        serviceInfo->description = kstr_dup(ser->arr->Description->arr[0].val.Description);
                    }
                }

                if (ser->arr->Genre) {
                    serviceInfo->genre = kstr_dup(ser->arr->Genre->arr[0].val.Genre);
                }
                if (ser->arr->GlobalServiceID) {
                    serviceInfo->globalServiceID = kstr_dup(ser->arr->GlobalServiceID->arr->val.GlobalServiceID);
                }
                if (ser->arr->ParentalRating) {
                    serviceInfo->parentRating = kstr_dup(ser->arr->ParentalRating->arr->val.ParentalRating);
                }

                // fill base info
                serviceInfo->serviceBaseInfo.MIMEType;
                if (ser->arr->Name) {
                    serviceInfo->serviceBaseInfo.name = kstr_dup(ser->arr->Name->arr->val.Name);
                }
                serviceInfo->serviceBaseInfo.serviceID = kstr_dup(ser->attr.id);
                serviceInfo->serviceBaseInfo.serviceProtection = ser->arr->attr.serviceContentProtection;
                if (ser->arr->ServiceType) {
                    serviceInfo->serviceBaseInfo.serviceType = ser->arr->ServiceType->arr->val.ServiceType;
                }
                serviceInfo->serviceBaseInfo.weight = ser->arr->attr.weight;

                *ret = IVEC_SUCCESS;
                return 0;
            }
        }
    }

    *ret = IVEC_FAILURE;
    return 0;
}
kint ivisa_SG_GetServiceDetailInfo(kchar *serviceID, IV_SERVICE_DETAIL_INFO *serviceInfo)
{
    kchar *deflang;
    sg_root *rt = __g_sg_mgr->cursg;
    K_dlist_entry *entry, *hdr;
    sg_ser *ser;
    kint ret;
    if (!rt) {
        return IVEC_FAILURE;
    }
    kmsg_send(rt->worker_thread, KRTM_GET_SERVICE_DETAIL_INFO, rt, serviceID, serviceInfo, &ret);
    return ret;
}

typedef struct _K_GetServicePreviewInfo {
    kchar *serviceID;
    IV_PREVIEW_TYPE infoType;
    IV_PREVIEW_INFO *previewInfo;
} K_GetServicePreviewInfo;

static kuchar pvdt_to_usage(kchar infoType)
{
    if (IV_PREVIEW_TYPE_LOGO_SMALL == infoType) {
        return 2;
    }
    if (IV_PREVIEW_TYPE_LOGO_LARGE == infoType) {
        return 2;
    }
    if (IV_PREVIEW_TYPE_SERVICE_PREVIEW == infoType) {
        return 3;
    }
    if (IV_PREVIEW_TYPE_SERVICE_SWITCH == infoType) {
        return 1;
    }
    if (IV_PREVIEW_TYPE_ALT_BLACKOUT == infoType) {
        return 5;
    }
    if (IV_PREVIEW_TYPE_SERVICE_PROMOTION == infoType) {
        return 4;
    }
    return 0;
}

kint om_GetServicePreviewInfo(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    K_GetServicePreviewInfo *gspi = (K_GetServicePreviewInfo*)a_ar1;
    kint *ret = (kint*)a_ar3;

    kchar *serviceID = gspi->serviceID;
    kint infoType = gspi->infoType;
    IV_PREVIEW_INFO *previewInfo = gspi->previewInfo;

    kuchar usage = pvdt_to_usage(infoType);

    K_dlist_entry *entry, *hdr;
    sg_pvd *pvd;
    sg_ser *ser;
    PreviewDataReference_rec *pdr_arr;
    IV_PREVIEW_INFO *pi;
    kuint i;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        *ret = IVEC_FAILURE;
        return 0;
    }

    ser = (sg_ser*)sg_root_find_file(rt, SER, knil, serviceID);
    if (!ser) {
        *ret = IVEC_FAILURE;
        return 0;
    }

    if (!ser->arr->PreviewDataReference) {
        *ret = IVEC_FAILURE;
        return 0;
    }

    pi = previewInfo;
    pdr_arr = ser->arr->PreviewDataReference->arr;
    for (i = 0; i < ser->arr->PreviewDataReference->ecnt; i++) {
        if (pdr_arr->attr.usage == usage) {
            pvd = (sg_pvd*)sg_root_find_file(rt, PVD, knil, pdr_arr->attr.idRef);
            if (!pvd) {
                pdr_arr++;
                continue;
            }
            if (usage == pvdt_to_usage(IV_PREVIEW_TYPE_LOGO_SMALL) ||
                    usage == pvdt_to_usage(IV_PREVIEW_TYPE_LOGO_LARGE)) {

                    pi->MIMEType = knil;
                    pi->file.data = knil;
                    pi->file.length = 0;
                    pi->file.uri = knil;

                    if (pvd->arr->Picture) {
                        if (pvd->arr->Picture->arr->MIMEType) {
                            pi->MIMEType = kstr_dup(pvd->arr->Picture->arr->MIMEType->arr->val.MIMEType);
                        }
                        if (pvd->arr->Picture->arr->PictureData) {
                            kint slen = strlen(pvd->arr->Picture->arr->PictureData->arr->val.PictureData);
                            kuchar *buffer = kmem_alloc(sizeof(kuchar) * (slen * 3 / 4));
                            base64_decode(pvd->arr->Picture->arr->PictureData->arr->val.PictureData, buffer);
                            pi->file.data = buffer;
                            pi->file.length = sizeof(kuchar) * (slen * 3 / 4);
                        } else {
                            if (pvd->arr->Picture->arr->PictureURI) {
                                pi->file.uri = kstr_dup(pvd->arr->Picture->arr->PictureURI->arr->val.PictureURI);
                            }
                        }
                        *ret = IVEC_SUCCESS;
                        return 0;
                    }
                    *ret = IVEC_FAILURE;
                    return 0;
            }

            if (usage == pvdt_to_usage(IV_PREVIEW_TYPE_SERVICE_PREVIEW) ||
                    usage == pvdt_to_usage(IV_PREVIEW_TYPE_SERVICE_SWITCH) ||
                    usage == pvdt_to_usage(IV_PREVIEW_TYPE_ALT_BLACKOUT) ||
                    usage == pvdt_to_usage(IV_PREVIEW_TYPE_SERVICE_PROMOTION)) {

                    pi->MIMEType = knil;
                    pi->file.data = knil;
                    pi->file.length = 0;
                    pi->file.uri = knil;
                    if (pvd->arr->Picture) {
                        if (pvd->arr->Picture->arr->MIMEType) {
                            pi->MIMEType = kstr_dup(pvd->arr->Picture->arr->MIMEType->arr->val.MIMEType);
                        }
                        if (pvd->arr->Picture->arr->PictureData) {
                            kchar *data = pvd->arr->Picture->arr->PictureData->arr->val.PictureData;
                            kint slen = strlen(data);
                            kuchar *buffer = kmem_alloc(sizeof(kuchar) * (slen * 3 / 4));
                            base64_decode(data, buffer);
                            pi->file.data = buffer;
                            pi->file.length = sizeof(kuchar) * (slen * 3 / 4);
                        } else {
                            if (pvd->arr->Picture->arr->PictureURI) {
                                pi->file.uri = kstr_dup(pvd->arr->Picture->arr->PictureURI->arr->val.PictureURI);
                            }
                        }
                        *ret = IVEC_SUCCESS;
                        return 0;
                    }

                    if (pvd->arr->Text) {
                        STR_SET(pi->MIMEType, "text/plain");
                        pi->file.data = kstr_dup(pvd->arr->Text->arr->val.Text);
                        pi->file.length = strlen(pi->file.data);
                        *ret = IVEC_SUCCESS;
                        return 0;
                    }

                    *ret = IVEC_FAILURE;
                    return 0;
            }
        }
        pdr_arr++;
    }

    *ret = IVEC_FAILURE;
    return 0;
}
kint ivisa_SG_GetServicePreviewInfo(kchar *serviceID, IV_PREVIEW_TYPE infoType, IV_PREVIEW_INFO *previewInfo)
{
    sg_root *rt = __g_sg_mgr->cursg;
    K_GetServicePreviewInfo gspi;
    kint ret;
    if (!rt) {
        return IVEC_FAILURE;
    }
    gspi.serviceID = serviceID;
    gspi.infoType = infoType;
    gspi.previewInfo = previewInfo;
    kmsg_send(rt->worker_thread, KRTM_GET_SERVICE_PREVIEW_INFO, rt, &gspi, knil, &ret);
    return ret;
}

typedef struct _K_GetServiceAccessInfo {
    kchar *serviceID;
    IV_ACCESS_INFO **accessInfoList;
    kint *AccessNum;
} K_GetServiceAccessInfo;

kint om_GetServiceAccessInfo(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    K_GetServiceAccessInfo *gsai = (K_GetServiceAccessInfo*)a_ar1;
    kint *ret = (kint*)a_ar3;

    kchar *serviceID = gsai->serviceID;
    IV_ACCESS_INFO **accessInfoList = gsai->accessInfoList;
    kint *AccessNum = gsai->AccessNum;

    // accessInfoList:: fill the saved frequency value which save when
    //ivisa_SG_StartScan  XXX
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    K_dlist_entry *entry2, *hdr2;
    sg_ser *ser;
    sg_sch *sch;
    sg_acc *acc, **acclst;


    kint i = 0;
    kint scNum = 0;

    PrivateExt_rec* prtExt_rec = knil;
    BroadcastPara_rec* bctPara_rec = knil;
    ServiceComponentID_t* serCmpID = knil;

    channelId_t *chId = knil;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        *ret = IVEC_FAILURE;
        return 0;
    }

    ser = (sg_ser*)sg_root_find_file(rt, SER, knil, serviceID);
    if (!ser) {
        *ret = IVEC_FAILURE;
        return 0;
    }

    acclst = sg_root_get_acc_by_ser(rt, serviceID);
    if (acclst && acclst[0]) {
        acc = acclst[0];

        *AccessNum = 1;

        if (rt->pvdr->protocol == SG_PROT_DMB_TH) {
            if(knil != acc->arr->PrivateExt){
                prtExt_rec = &(acc->arr->PrivateExt->arr[0]); // PrivateExt->ecnt always equals to 1

                scNum = prtExt_rec->BroadcastPara->ecnt;
                *accessInfoList = kmem_alloz(sizeof(IV_ACCESS_INFO) + (scNum - 1) * sizeof(IV_SESSION_PARAMS));
                (*accessInfoList)->scNum = scNum;
                (*accessInfoList)->accessType = IV_ACCESS_TYPE_DVB;
            }
            else{
                *ret = IVEC_FAILURE;
                return 0;
            }
        } else if (rt->pvdr->protocol == SG_PROT_DAB) {
            if(knil != acc->arr->PrivateExt){
                prtExt_rec = &(acc->arr->PrivateExt->arr[0]); // PrivateExt->ecnt always equals to 1

                scNum = prtExt_rec->BroadcastPara->ecnt;
                *accessInfoList = kmem_alloz(sizeof(IV_ACCESS_INFO) + (scNum - 1) * sizeof(IV_SESSION_PARAMS));
                (*accessInfoList)->scNum = scNum;
                (*accessInfoList)->accessType = IV_ACCESS_TYPE_DAB;
            }
            else{
                *ret = IVEC_FAILURE;
                return 0;
            }
        } else {
            *accessInfoList = kmem_alloz(sizeof(IV_ACCESS_INFO));
            (*accessInfoList)->accessType = IV_ACCESS_TYPE_IP;
            (*accessInfoList)->scNum = 1;
        }
        (*accessInfoList)->frequency = ser->frequency;
        (*accessInfoList)->defaultAccessFlag = 1;
        (*accessInfoList)->validFrom = acc->attr.validFrom;
        (*accessInfoList)->validTo = acc->attr.validTo;
        (*accessInfoList)->usage = knil;

        if (rt->pvdr->protocol == SG_PROT_DMB_TH) {

            for(i=0; i<(*accessInfoList)->scNum; i++){
                bctPara_rec = &(prtExt_rec->BroadcastPara->arr[i]);
                (*accessInfoList)->sessionParams[i].dvbAccess.meidaType        = bctPara_rec->attr.MediaType;
                (*accessInfoList)->sessionParams[i].dvbAccess.onID            = bctPara_rec->attr.onID;
                (*accessInfoList)->sessionParams[i].dvbAccess.protocolType    = bctPara_rec->attr.ProtocolType;
                (*accessInfoList)->sessionParams[i].dvbAccess.tsID            = bctPara_rec->attr.tsID;

                serCmpID = bctPara_rec->ServiceComponentID;
                (*accessInfoList)->sessionParams[i].dvbAccess.serviceComponentID = serCmpID->arr[0].val.ServiceComponentID;
            }


        } else if (rt->pvdr->protocol == SG_PROT_DAB) {
            for(i=0; i<(*accessInfoList)->scNum; i++){
                bctPara_rec = &(prtExt_rec->BroadcastPara->arr[i]);

                (*accessInfoList)->sessionParams[i].dabAccess.eID            = bctPara_rec->attr.eID;
                (*accessInfoList)->sessionParams[i].dabAccess.meidaType        = bctPara_rec->attr.MediaType;
                (*accessInfoList)->sessionParams[i].dabAccess.protocolType    = bctPara_rec->attr.ProtocolType;
                (*accessInfoList)->sessionParams[i].dabAccess.mode    = bctPara_rec->attr.TransportMode;

                chId = bctPara_rec->channelId;
                (*accessInfoList)->sessionParams[i].dabAccess.channelID = chId->arr[0].val.channelId;

                serCmpID = bctPara_rec->ServiceComponentID;
                (*accessInfoList)->sessionParams[i].dabAccess.serviceComponentID = serCmpID->arr[0].val.ServiceComponentID;
            }

        } else {
            if (rt->mgr->conf.prot == IV_FILEREV_PROTOCOL_FLUTEALC || rt->mgr->conf.prot == IV_FILEREV_PROTOCOL_BDIR) {
                AccessType_rec *acctype_arr;
                BroadcastServiceDelivery_rec *bsd_arr;
                SessionDescription_rec *sd_arr;

                //            if (!acc->arr->AccessType) {
                //                *ret = IVEC_FAILURE;
                //                return 0;
                //            }

                acctype_arr = acc->arr->AccessType->arr;
                if (!acctype_arr->BroadcastServiceDelivery) {
                    *ret = IVEC_FAILURE;
                    return 0;
                }

                bsd_arr = acctype_arr->BroadcastServiceDelivery->arr;
                if (!bsd_arr->SessionDescription) {
                    *ret = IVEC_FAILURE;
                    return 0;
                }

                sd_arr = bsd_arr->SessionDescription->arr;
                if (sd_arr->SDPRef) {
                    sg_sdp *sdp = (sg_sdp*)sg_root_find_file(rt, SDP, knil, sd_arr->SDPRef->arr->attr.idRef);
                    kchar path[256];

                    (*accessInfoList)->sessionParams[0].sdp.data = knil;
                    (*accessInfoList)->sessionParams[0].sdp.length = 0;
                    sg_root_get_zbd_path(sdp, path);
                    (*accessInfoList)->sessionParams[0].sdp.uri = kstr_dup(path);
                    (*accessInfoList)->accessType = IV_ACCESS_TYPE_IP;

                } else if (sd_arr->SDP) {
                    (*accessInfoList)->sessionParams[0].sdp.data = kstr_dup(sd_arr->SDP->arr->val.SDP);
                    (*accessInfoList)->sessionParams[0].sdp.length = strlen((*accessInfoList)->sessionParams[0].sdp.data);
                    (*accessInfoList)->sessionParams[0].sdp.uri = knil;
                    (*accessInfoList)->accessType = IV_ACCESS_TYPE_IP;

                } else {
                    *ret = IVEC_FAILURE;
                    return 0;
                }
            } else {
                *ret = IVEC_FAILURE;
                return 0;
            }
        }
        kmem_free(acclst);
        *ret = IVEC_SUCCESS;
        return 0;
    }

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;


        // VERSION 0.0.0.1
        // DO NOT SUPPORT MULTI Lingual and Single content access
        hdr = &fblk->fhdrs[SCH];
        entry = hdr->next;
        while (entry != hdr) {
            sch = FIELD_TO_STRUCTURE(entry, sg_sch, cls_le);
            entry = entry->next;

            if (!sch->arr->ServiceReference) {
                continue;
            }

            if (streq(sch->arr->ServiceReference->arr->attr.idRef, serviceID)) {
                if (!sch->arr->InteractivityDataReference) {

                    acclst = sg_root_get_acc_by_sch(rt, sch[0].attr.id);
                    // Only get ONE
                    if (acclst) {
                        acc = acclst[0];
                    }
                    if (acclst && acc) {
                        K_dlist_entry *entry, *hdr;

                        *AccessNum = 1;
                        if (rt->pvdr->protocol == SG_PROT_DMB_TH) {
                            if(knil != acc->arr->PrivateExt){
                                prtExt_rec = &(acc->arr->PrivateExt->arr[0]); // PrivateExt->ecnt always equals to 1

                                scNum = prtExt_rec->BroadcastPara->ecnt;
                                *accessInfoList = kmem_alloz(sizeof(IV_ACCESS_INFO) + (scNum - 1) * sizeof(IV_SESSION_PARAMS));
                                (*accessInfoList)->scNum = scNum;
                                (*accessInfoList)->accessType = IV_ACCESS_TYPE_DVB;
                            }
                            else{
                                *ret = IVEC_FAILURE;
                                return 0;
                            }
                        } else if (rt->pvdr->protocol == SG_PROT_DAB) {
                            if(knil != acc->arr->PrivateExt){
                                prtExt_rec = &(acc->arr->PrivateExt->arr[0]); // PrivateExt->ecnt always equals to 1

                                scNum = prtExt_rec->BroadcastPara->ecnt;
                                *accessInfoList = kmem_alloz(sizeof(IV_ACCESS_INFO) + (scNum - 1) * sizeof(IV_SESSION_PARAMS));
                                (*accessInfoList)->scNum = scNum;
                                (*accessInfoList)->accessType = IV_ACCESS_TYPE_DAB;
                            }
                            else{
                                *ret = IVEC_FAILURE;
                                return 0;
                            }
                        } else {
                            *accessInfoList = kmem_alloz(sizeof(IV_ACCESS_INFO));
                            (*accessInfoList)->accessType = IV_ACCESS_TYPE_IP;
                            (*accessInfoList)->scNum = 1;
                        }
                        (*accessInfoList)->frequency = ser->frequency;
                        (*accessInfoList)->defaultAccessFlag = 1;
                        (*accessInfoList)->validFrom = acc->attr.validFrom;
                        (*accessInfoList)->validTo = acc->attr.validTo;
                        (*accessInfoList)->usage = knil;

                        if (rt->pvdr->protocol == SG_PROT_DMB_TH) {
                            for(i=0; i<(*accessInfoList)->scNum; i++){
                                bctPara_rec = &(prtExt_rec->BroadcastPara->arr[i]);
                                (*accessInfoList)->sessionParams[i].dvbAccess.meidaType    = bctPara_rec->attr.MediaType;
                                (*accessInfoList)->sessionParams[i].dvbAccess.onID            = bctPara_rec->attr.onID;
                                (*accessInfoList)->sessionParams[i].dvbAccess.protocolType = bctPara_rec->attr.ProtocolType;
                                (*accessInfoList)->sessionParams[i].dvbAccess.tsID            = bctPara_rec->attr.tsID;

                                serCmpID = bctPara_rec->ServiceComponentID;
                                (*accessInfoList)->sessionParams[i].dvbAccess.serviceComponentID = serCmpID->arr[0].val.ServiceComponentID;
                            }

                        } else if (rt->pvdr->protocol == SG_PROT_DAB) {
                            for(i=0; i<(*accessInfoList)->scNum; i++){
                                bctPara_rec = &(prtExt_rec->BroadcastPara->arr[i]);

                                (*accessInfoList)->sessionParams[i].dabAccess.eID            = bctPara_rec->attr.eID;
                                (*accessInfoList)->sessionParams[i].dabAccess.meidaType        = bctPara_rec->attr.MediaType;
                                (*accessInfoList)->sessionParams[i].dabAccess.protocolType    = bctPara_rec->attr.ProtocolType;
                                (*accessInfoList)->sessionParams[i].dabAccess.mode    = bctPara_rec->attr.TransportMode;

                                chId = bctPara_rec->channelId;
                                (*accessInfoList)->sessionParams[i].dabAccess.channelID = chId->arr[0].val.channelId;

                                serCmpID = bctPara_rec->ServiceComponentID;
                                (*accessInfoList)->sessionParams[i].dabAccess.serviceComponentID = serCmpID->arr[0].val.ServiceComponentID;
                            }

                        } else {
                            if (rt->mgr->conf.prot == IV_FILEREV_PROTOCOL_FLUTEALC || rt->mgr->conf.prot == IV_FILEREV_PROTOCOL_BDIR) {
                                AccessType_rec *acctype_arr;
                                BroadcastServiceDelivery_rec *bsd_arr;
                                SessionDescription_rec *sd_arr;

                                if (!acc->arr->AccessType) {
                                    *ret = IVEC_FAILURE;
                                    return 0;
                                }

                                acctype_arr = acc->arr->AccessType->arr;
                                if (!acctype_arr->BroadcastServiceDelivery) {
                                    *ret = IVEC_FAILURE;
                                    return 0;
                                }

                                bsd_arr = acctype_arr->BroadcastServiceDelivery->arr;
                                if (!bsd_arr->SessionDescription) {
                                    *ret = IVEC_FAILURE;
                                    return 0;
                                }

                                sd_arr = bsd_arr->SessionDescription->arr;
                                if (sd_arr->SDPRef) {
                                    sg_sdp *sdp = (sg_sdp*)sg_root_find_file(rt, SDP, knil, sd_arr->SDPRef->arr->attr.idRef);
                                    kchar path[256];

                                    (*accessInfoList)->sessionParams[0].sdp.data = knil;
                                    (*accessInfoList)->sessionParams[0].sdp.length = 0;
                                    sg_root_get_zbd_path(sdp, path);
                                    (*accessInfoList)->sessionParams[0].sdp.uri = kstr_dup(path);
                                } else if (sd_arr->SDP) {
                                    (*accessInfoList)->sessionParams[0].sdp.data = kstr_dup(sd_arr->SDP->arr->val.SDP);
                                    (*accessInfoList)->sessionParams[0].sdp.length = strlen((*accessInfoList)->sessionParams[0].sdp.data);
                                    (*accessInfoList)->sessionParams[0].sdp.uri = knil;
                                } else {

                                }
                            } else {
                            }
                        }

                    }

                    kmem_free(acclst);
                    *ret = 0;
                    return 0;
                }
            }
        }
    }

    *ret = IVEC_SUCCESS;
    return 0;
}
kint ivisa_SG_GetServiceAccessInfo(kchar *serviceID, IV_ACCESS_INFO **accessInfoList, kint *AccessNum)
{
    sg_root *rt = __g_sg_mgr->cursg;
    kint ret;
    K_GetServiceAccessInfo gsai;
    gsai.serviceID = serviceID;
    gsai.accessInfoList = accessInfoList;
    gsai.AccessNum = AccessNum;

    kmsg_send(rt->worker_thread, KRTM_GET_SERVICE_ACCESS_INFO, rt, &gsai, knil, &ret);
    return ret;
}

typedef struct _K_GetProgList {
    kchar *serviceID;
    kuint startTime;
    kuint endTime;
    IV_PROGRAM_BASE_INFO**programList;
    kint *programNum;
} K_GetProgList;

kint om_GetProgramList(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    K_GetProgList *gpl = (K_GetProgList*)a_ar1;
    kint *ret = (kint*)a_ar3;

    kchar *serviceID = gpl->serviceID;
    kuint startTime = gpl->startTime;
    kuint endTime = gpl->endTime;
    IV_PROGRAM_BASE_INFO**programList = gpl->programList;
    kint *programNum = gpl->programNum;

    kchar *deflang;

    kuint i, cttStartTime, cttEndTime;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;

    sg_ser *ser;
    sg_sch *sch;
    sg_ctt *ctt, **cttlst;

    IV_PROGRAM_BASE_INFO *prog_bi;
    kint prog_cnt = 0, real_prog_cnt = 0;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        *ret = IVEC_FAILURE;
        return 0;
    }

    deflang = rt->mgr->conf.lang;
#if 0
    A. matchedSchedule = search_all_schedule(serviceID, has content reference);
    B. matchedSchedule::contentReference::PresentationWindow::StartTime/endTime/duration;
    C. getContent(matchedSchedule::contentReference) ==> content;
    D. content::name, content::programID, content::serviceContentProtection, content::Genre/parentRating;
#endif


    //found program list in CONTENT

    cttlst = sg_root_get_ctt_by_ser(rt, serviceID);
    if (cttlst) {
        int hasValidProgram = 0;
        for (prog_cnt = 0; cttlst[prog_cnt]; prog_cnt++);
        if (!prog_cnt) {
            *ret = IVEC_FAILURE;
            kmem_free(cttlst);
            return 0;
        }

        prog_bi = (IV_PROGRAM_BASE_INFO*)kmem_alloz(sizeof(IV_PROGRAM_BASE_INFO) * prog_cnt);
        *programList = prog_bi;

        prog_cnt = 0;
        while (ctt = cttlst[prog_cnt++]) {

            /* check and set the time */
            if (ctt->arr->StartTime) {
                hasValidProgram = 1;
                cttStartTime = kspt_dateTime_to_ntp(ctt->arr->StartTime->arr->val.StartTime);
            } else {
                //cttStartTime = 0;
                continue; //only a content, not a program
            }
            if (ctt->arr->EndTime) {
                hasValidProgram = 1;
                cttEndTime = kspt_dateTime_to_ntp(ctt->arr->EndTime->arr->val.EndTime);
            } else {
                //cttEndTime = 0xFFFFFFFF;
                continue;
            }

            if (!(cttStartTime < endTime && cttEndTime > startTime)) {
                /* klog(("Content time not right, skip current\n")); */
                kmem_free(prog_bi->programID);
                continue;
            }

            /* the REAL program inc */
            real_prog_cnt++;

            if (ctt->arr->StartTime) {
                prog_bi->startTime = cttStartTime;
            }
            if (ctt->arr->EndTime) {
                prog_bi->endTime = cttEndTime;
            }
            prog_bi->duration = prog_bi->endTime - prog_bi->startTime;

            if (ctt->arr->Name) {
                kuint j;
                // language
                for (j = 0; j < ctt->arr->Name->ecnt; j++) {
                    if (streq(deflang, ctt->arr->Name->arr[j].attr.xml_lang)) {
                        STR_SET(prog_bi->name, ctt->arr->Name->arr[j].val.Name);
                        break;
                    }
                }
                if (j == ctt->arr->Name->ecnt) {
                    STR_SET(prog_bi->name, ctt->arr->Name->arr[0].val.Name);
                }
            }

            STR_SET(prog_bi->programID, ctt->attr.id);

            if (ctt->arr->ParentalRating) {
                // XXX Multi
                STR_SET(prog_bi->parentRating, ctt->arr->ParentalRating->arr->val.ParentalRating);
            }
            if (ctt->arr->Genre) {
                // XXX Multi
                STR_SET(prog_bi->genre, ctt->arr->Genre->arr->val.Genre);
            }
            prog_bi->contentProtection = ctt->arr->attr.serviceContentProtection;

            prog_bi++;
        }

        *programNum = real_prog_cnt;
        if (!real_prog_cnt) {
            kmem_free(prog_bi);
            *programList = knil;
        }
        if(hasValidProgram){
            *ret = IVEC_SUCCESS;
            kmem_free(cttlst);
            return 0;
        }/*else{}//no invalid program in ctt,find by sch in following*/

        kmem_free(cttlst);
    }else{
        *programNum = 0;
        *programList = knil;
        *ret = IVEC_SUCCESS;
        return 0;
    }

    //if Content doesn't inlcude program(time) info
    //found programlist by SCH, Get the Count
    //now support only one contentReference vs one PresentationWindow
    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;


        // Get the Count
        hdr = &fblk->fhdrs[SCH];

        entry = hdr->next;
        while (entry != hdr) {
            sch = FIELD_TO_STRUCTURE(entry, sg_sch, cls_le);
            entry = entry->next;

            if (!sch->arr->ServiceReference) {
                continue;
            }

            if (!streq(sch->arr->ServiceReference->arr->attr.idRef, serviceID)) {
                continue;
            }

            if (!sch->arr->ContentReference) {
                continue;
            }

            prog_cnt += sch->arr->ContentReference->ecnt;
        }
    }

    // Fill the content
    //
    if (prog_cnt) {

        prog_bi = (IV_PROGRAM_BASE_INFO*)kmem_alloz(sizeof(IV_PROGRAM_BASE_INFO) * prog_cnt);
        *programList = prog_bi;

        fb_hdr = &rt->frqhdr;
        fb_entry = fb_hdr->next;
        while (fb_entry != fb_hdr) {
            fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
            fb_entry = fb_entry->next;

            hdr = &fblk->fhdrs[SCH];
            entry = hdr->next;
            while (entry != hdr) {
                sch = FIELD_TO_STRUCTURE(entry, sg_sch, cls_le);
                entry = entry->next;

                if (!sch->arr->ServiceReference) {
                    continue;
                }

                if (!streq(sch->arr->ServiceReference->arr->attr.idRef, serviceID)) {
                    continue;
                }

                if (!sch->arr->ContentReference) {
                    continue;
                }

                for (i = 0; i < sch->arr->ContentReference->ecnt; i++) {
                    ContentReference_rec *cr = &sch->arr->ContentReference->arr[i];
                    K_dlist_entry *le3, *hdr3;

                    if (cr->PresentationWindow) {
                        // Multi, now support only one contentReference vs one PresentationWindow
                        /* should check the time */
                        if (cr->PresentationWindow->arr->attr.startTime < endTime &&
                                cr->PresentationWindow->arr->attr.endTime > startTime) {
                            prog_bi->duration = cr->PresentationWindow->arr->attr.duration;
                            prog_bi->startTime = cr->PresentationWindow->arr->attr.startTime;
                            prog_bi->endTime = cr->PresentationWindow->arr->attr.endTime;

                        } else {

                            /* the time not right */
                            klog(("ContentReference exit but PresentationWindow not right, skip current\n"));

                            continue;
                        }
                    }

                    STR_SET(prog_bi->programID, cr->attr.idRef);

                    hdr3 = &fblk->fhdrs[CTT];
                    le3 = hdr3->next;
                    while (le3 != hdr3) {
                        ctt = FIELD_TO_STRUCTURE(le3, sg_ctt, cls_le);
                        le3 = le3->next;

                        if (streq(cr->attr.idRef, ctt->attr.id)) {

                            /* the REAL program inc */
                            real_prog_cnt++;

                            /* fill the name with right locale */
                            if (ctt->arr->Name) {
                                kuint j;
                                // language
                                for (j = 0; j < ctt->arr->Name->ecnt; j++) {
                                    if (streq(deflang, ctt->arr->Name->arr[j].attr.xml_lang)) {
                                        STR_SET(prog_bi->name, ctt->arr->Name->arr[j].val.Name);
                                        break;
                                    }
                                }
                                if (j == ctt->arr->Name->ecnt) {
                                    STR_SET(prog_bi->name, ctt->arr->Name->arr[0].val.Name);
                                }
                            }

                            STR_SET(prog_bi->programID, ctt->attr.id);

                            if (ctt->arr->ParentalRating) {
                                // XXX Multi
                                STR_SET(prog_bi->parentRating, ctt->arr->ParentalRating->arr->val.ParentalRating);
                            }
                            if (ctt->arr->Genre) {
                                // XXX Multi
                                STR_SET(prog_bi->genre, ctt->arr->Genre->arr->val.Genre);
                            }
                            prog_bi->contentProtection = ctt->arr->attr.serviceContentProtection;
                        }
                    }
                    prog_bi++;
                }
            }
        }

        *programNum = real_prog_cnt;
        if (!real_prog_cnt) {
            kmem_free(prog_bi);
            *programList = knil;
        }

    } else {
        *programNum = 0;
        *programList = knil;
    }
    *ret = IVEC_SUCCESS;
    return 0;

}
kint ivisa_SG_GetProgramList(kchar *serviceID, kuint startTime, kuint endTime, IV_PROGRAM_BASE_INFO**programList, kint *programNum)
{
    sg_root *rt = __g_sg_mgr->cursg;
    kint ret;
    K_GetProgList gpl;
    if (!rt) {
        return IVEC_FAILURE;
    }

    gpl.serviceID = serviceID;
    gpl.startTime = startTime;
    gpl.endTime = endTime;
    gpl.programList = programList;
    gpl.programNum = programNum;

    kmsg_send(rt->worker_thread, KRTM_GET_PROGRAM_LIST, rt, &gpl, 0, &ret);
    return ret;
}

kint om_GetProgramDescription(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kchar *programID = (kchar*)a_ar1;
    kchar **programDes = (kchar**)a_ar2;
    kint *ret = (kint*)a_ar3;

    kchar *deflang;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;
    sg_ctt *ctt;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        *ret = IVEC_FAILURE;
        return 0;
    }

    deflang = rt->mgr->conf.lang;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;



        hdr = &fblk->fhdrs[CTT];
        entry = hdr->next;
        while (entry != hdr) {
            ctt = FIELD_TO_STRUCTURE(entry, sg_ctt, cls_le);
            entry = entry->next;

            if (streq(programID, ctt->attr.id)) {
                if (ctt->arr->Description) {
                    // language
                    kuint j = 0;
                    for (j = 0; j < ctt->arr->Description->ecnt; j++) {
                        if (streq(deflang, ctt->arr->Description->arr[j].attr.xml_lang)) {
                            *programDes = kstr_dup(ctt->arr->Description->arr[j].val.Description);
                            break;
                        }
                    }
                    if (j == ctt->arr->Description->ecnt) {
                        *programDes = kstr_dup(ctt->arr->Description->arr[0].val.Description);
                    }
                } else {
                    *programDes = knil;
                }
                *ret = IVEC_SUCCESS;
                return 0;
            }
        }
    }
    *programDes = knil;
    *ret = IVEC_SUCCESS;
    return 0;
}
kint ivisa_SG_GetProgramDescription(kchar *programID, kchar **programDes)
{
    sg_root *rt = __g_sg_mgr->cursg;
    kint ret;
    if (!rt) {
        return IVEC_FAILURE;
    }
    kmsg_send(rt->worker_thread, KRTM_GET_PROG_DESC, rt, programID, programDes, &ret);
    return ret;
}

/**
 * @brief Set currrent service
 *
 * @warning this is the <b>ONLY</b> way to change current SG and current frequency, after scan
 *
 * @pre Should called after all the SG loaded, because this routine will not try to load
 * unloaed sg root
 */
kint ivisa_SG_SetCurrentService(kchar *serviceID)
{
    sg_ser *ser;
    K_dlist_entry *entry, *hdr;
    K_pvdr *pvdr;
    kuint frequency = 0;
    kint ret = IVEC_FAILURE;

    klog(("ivisa_SG_SetCurrentService:%s\n", serviceID));
    printf("ivisa_SG_SetCurrentService:%s\n", serviceID);

    if (!__g_sg_mgr->cursg) {
        kerror(("ivisa_SG_SetCurrentService: no cursg, scaning???\n"));
        return IVEC_FAILURE;
    }

    if (streq(__g_sg_mgr->cursg->curser, serviceID)) {
        klog(("ivisa_SG_SetCurrentService: (%s) already current!", serviceID));
        return IVEC_SUCCESS;
    }

    /*
     * scan all the provider to find the serviceID, and the current frequency
     */
    hdr = &__g_sg_mgr->provhdr;
    entry = hdr->next;
    while (entry != hdr) {
        pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
        entry = entry->next;

        if (!pvdr->root) {
            kerror(("Provider %d is not loaded yet\n", pvdr->id));
            continue;
        }

        ser = (sg_ser*)sg_root_find_file(pvdr->root, SER, knil, serviceID);
        if (ser) {
            __g_sg_mgr->cursg = pvdr->root;
            STR_SET(pvdr->root->curser, serviceID);

            /* OK, found the frequency */
            frequency = ser->frequency;
            ret = IVEC_SUCCESS;
            break;
        }
    }

    if (0 != frequency) {

        /* switch frequency for all root */
        hdr = &__g_sg_mgr->provhdr;
        entry = hdr->next;
        while (entry != hdr) {
            pvdr = FIELD_TO_STRUCTURE(entry, K_pvdr, le);
            entry = entry->next;

            if (pvdr->root) {
                sg_root_frequency_switch(pvdr->root, frequency);
            }
        }

        /*
         * try start auto update for all the provider
         * Must called after sg_root_frequency_switch()
         */
        if (kflg_chk(__g_sg_mgr->flg, MGR_AUTO_SYNC)) {
            ivisa_SG_EnableAutoUpdate(ktrue);
        }
    }

    return ret;
}

kint om_GetCurrentInteractivity(kbean a_tsk, kuint a_msg, kvoid *a_ar0, kvoid *a_ar1, kvoid *a_ar2, kvoid *a_ar3, kint a_rsn)
{
    sg_root *rt = (sg_root*)a_ar0;
    kchar *serviceID = (kchar*)a_ar1;
    IV_INTERACTIVITY_DIRECTORY **interactivity = (IV_INTERACTIVITY_DIRECTORY**)a_ar2;
    kint *ret = (kint*)a_ar3;

    sg_ser *ser;

    if (a_rsn) {
        klog(("dispatch for %d called for reason %d.\n", a_msg, a_rsn));
        *ret = IVEC_FAILURE;
        return 0;
    }

    ser = (sg_ser*)sg_root_find_file(rt, SER, knil, serviceID);
    if (!ser){
        *ret = IVEC_FAILURE;
        return 0;
    }

    klog(("get ser:%x, ser->cur_idir:%x, serid:%x\n", ser, ser->cur_idir, serviceID));
    if(    !ser->cur_idir){
        //*interactivity = knil;
#if 1 // for test
        K_dlist_entry *entry, *hdr;
        sg_imd *imd;
        kuint i, j, curtime = ksys_ntp_time() + __g_sg_mgr->env->time_diff;
        IV_INTERACTIVITY_DIRECTORY *idir;
        IV_INTERACTIVITY_MEDIA *im;
        IV_ITV_SMS *sms;
        IV_SMS_CHOICE *cur_choice, *ret_choice;

        IV_ITV_MEDIAOBJECTS *itv_mo = knil;
        IV_MEDIA_OBJECT *itv_file= knil;
        kint mediaNumber = 1;

        Object_rec *obj_arr;
        kint obj_cnt;

        klog(("no ser->cur_idir, use default\n"));

        mediaNumber = 1;
        obj_cnt = 2;

        // First, Get the mediaNumber for IV_INTERACTIVITY_DIRECTORY
        idir = (IV_INTERACTIVITY_DIRECTORY*)kmem_alloz(sizeof(IV_INTERACTIVITY_DIRECTORY) +
                (mediaNumber - 1) * sizeof(IV_INTERACTIVITY_MEDIA));

        // Allocate memory for idir
        idir->startTime = curtime - 60;
        idir->endTime = curtime + 300;
        idir->mediaNumber = mediaNumber;
        im = idir->interactivityMedia;
        im->type = IV_INTERACTIVITY_TYPE_MEDIAOBJ;
        im->relativePreference = 0;
        if(1){
            kchar tmpdir[IV_PATH_MAX_SIZE];

            itv_mo = kmem_alloz(sizeof(IV_ITV_MEDIAOBJECTS) + (obj_cnt - 1) * sizeof(IV_MEDIA_OBJECT));
            im->contents.mediaObjects = itv_mo;
            STR_SET(im->description, "Enjoy it...");


            itv_mo->moNum = obj_cnt;

            itv_mo->objects[0].flag = 1;
            sprintf(tmpdir,"%s%s",__g_sg_mgr->env->data_dir,"\\itv\\PlayList.htm");
            STR_SET(itv_mo->objects[0].contentLocation, tmpdir);
            //STR_SET(itv_mo->objects[0].contentLocation, "\\Program Files\\MobileTV\\itv\\PlayList.htm");
            STR_SET(itv_mo->objects[0].contentType, "text/htm");

            itv_mo->objects[1].flag = 0;
            sprintf(tmpdir,"%s%s",__g_sg_mgr->env->data_dir,"\\itv\\wait_ax.gif");
            STR_SET(itv_mo->objects[1].contentLocation, tmpdir);
            //STR_SET(itv_mo->objects[1].contentLocation, "\\Program Files\\MobileTV\\itv\\wait_ax.gif");
            STR_SET(itv_mo->objects[1].contentType, "image/gif");
        }
        if (!sg_imd_copy_idir(interactivity, idir,__g_sg_mgr->env->temp_dir,__g_sg_mgr->env->data_dir)) {
            //free idir
            for(i=0;i<obj_cnt;i++){
                kmem_free(itv_mo->objects[i].contentLocation);
                kmem_free(itv_mo->objects[i].contentType);
            }
            kmem_free(im->description);
            kmem_free(itv_mo);
            kmem_free(idir);

            *ret = IVEC_SUCCESS;
            return 0;
        }

        for(i=0;i<obj_cnt;i++){
            kmem_free(itv_mo->objects[i].contentLocation);
            kmem_free(itv_mo->objects[i].contentType);
        }
        kmem_free(im->description);
        kmem_free(itv_mo);
        kmem_free(idir);
        *ret = IVEC_FAILURE;
        return 0;
#endif
    }

    if (!sg_imd_copy_idir(interactivity, ser->cur_idir,__g_sg_mgr->env->temp_dir,__g_sg_mgr->env->data_dir)) {
        *ret = IVEC_SUCCESS;
        return 0;
    }
    *ret = IVEC_FAILURE;
    return 0;
}

kint ivisa_SG_GetCurrentInteractivity(kchar *serviceID, IV_INTERACTIVITY_DIRECTORY **interactivity)
{
    sg_root *rt = __g_sg_mgr->cursg;
    kint ret;
    if (!rt){
        return IVEC_FAILURE;
    }
    kmsg_send(rt->worker_thread, KRTM_GET_CURRENT_INTERACTIVITY, rt, serviceID, interactivity, &ret);
    return ret;
}

kint ivisa_SG_GetPurchaseItemList(kchar *serviceID, kuint startTime, kuint endTime, IV_PURCHASE_ITEM_BASE_INFO **purchaseItemList, kint *purchaseItemNum)
{
    kchar *deflang;
    sg_root *rt = __g_sg_mgr->cursg;
    sg_pci *pci;
    sg_sch *sch;

    IV_PURCHASE_ITEM_BASE_INFO    *pcibi;
    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;

    kuint itemCnt = 0, itemIndex = 0;

    kuint j = 0;

    if (!rt) {
        return IVEC_FAILURE;
    }
    deflang = rt->mgr->conf.lang;

    /* Get count */
    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[PCI];
        entry = hdr->next;
        while (entry != hdr) {
            pci = FIELD_TO_STRUCTURE(entry, sg_pci, cls_le);
            entry = entry->next;

            if (serviceID) {
                if (pci->arr->ServiceReference) {
                    kuint k;
                    for (k = 0; k < pci->arr->ServiceReference->ecnt; k++) {
                        ServiceReference_rec *sr = &pci->arr->ServiceReference->arr[k];

                        if (streq(sr->attr.idRef, serviceID)) {
                            itemCnt++;
                        }
                    }
                }

                // No according service found, try schedule
                if (pci->arr->ScheduleReference) {
                    kuint k;
                    for (k = 0; k < pci->arr->ScheduleReference->ecnt; k++) {
                        ScheduleReference_rec *sr = &pci->arr->ScheduleReference->arr[k];

                        K_dlist_entry *le1, *hdr1;
                        hdr1 = &fblk->fhdrs[SCH];
                        le1 = hdr1->next;
                        while (le1 != hdr1) {
                            sch = FIELD_TO_STRUCTURE(le1, sg_sch, cls_le);
                            le1 = le1->next;

                            if (streq(sch->attr.id, sr->attr.idRef)) {
                                if (sch->arr->ServiceReference) {
                                    if (streq(sch->arr->ServiceReference->arr->attr.idRef, serviceID) ) {
                                        itemCnt++;
                                    }
                                }
                            }
                        }
                    }
                }

                continue;
            }
        }
    }

    /* Fill Data */
    if (0 == itemCnt) {
        *purchaseItemList = knil;
        *purchaseItemNum = 0;
        return IVEC_SUCCESS;
    }

    *purchaseItemList = pcibi = kmem_alloz(sizeof(IV_PURCHASE_ITEM_BASE_INFO) * itemCnt);
    *purchaseItemNum = itemCnt;

    fb_hdr = &rt->frqhdr;
    fb_entry = fb_hdr->next;
    while (fb_entry != fb_hdr) {
        fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
        fb_entry = fb_entry->next;

        hdr = &fblk->fhdrs[PCI];
        entry = hdr->next;
        while (entry != hdr) {
            pci = FIELD_TO_STRUCTURE(entry, sg_pci, cls_le);
            entry = entry->next;

            if (serviceID) {
                if (pci->arr->ServiceReference) {
                    kuint k;
                    for (k = 0; k < pci->arr->ServiceReference->ecnt; k++) {
                        ServiceReference_rec *sr = &pci->arr->ServiceReference->arr[k];

                        if (streq(sr->attr.idRef, serviceID)) {
                            goto processit;
                        }
                    }
                }

                // No according service found, try schedule
                if (pci->arr->ScheduleReference) {
                    kuint k;
                    for (k = 0; k < pci->arr->ScheduleReference->ecnt; k++) {
                        ScheduleReference_rec *sr = &pci->arr->ScheduleReference->arr[k];

                        K_dlist_entry *le1, *hdr1;
                        hdr1 = &fblk->fhdrs[SCH];
                        le1 = hdr1->next;
                        while (le1 != hdr1) {
                            sch = FIELD_TO_STRUCTURE(le1, sg_sch, cls_le);
                            le1 = le1->next;

                            if (streq(sch->attr.id, sr->attr.idRef)) {
                                if (sch->arr->ServiceReference) {
                                    if (streq(sch->arr->ServiceReference->arr->attr.idRef, serviceID) ) {
                                        goto processit;
                                    }
                                }
                            }
                        }
                    }
                }

                continue;
            }
processit:
            pcibi[itemIndex].weight = pci->arr->attr.weight;
            pcibi[itemIndex].purchaseItemID = kstr_dup(pci->attr.id);

            if (pci->arr->Name) {
                // language
                for (j = 0; j < pci->arr->Name->ecnt; j++) {
                    if (streq(deflang, pci->arr->Name->arr[j].attr.xml_lang)) {
                        pcibi[itemIndex].name = pci->arr->Name->arr[j].val.Name;
                        break;
                    }
                }
                if (j == pci->arr->Name->ecnt) {
                    pcibi[itemIndex].name = kstr_dup(pci->arr->Name->arr[0].val.Name);
                }
            }

            if (pci->arr->StartTime) {
                strcpy(pcibi[itemIndex].startTime, pci->arr->StartTime->arr->val.StartTime);
            }
            if (pci->arr->EndTime) {
                strcpy(pcibi[itemIndex].endtime, pci->arr->EndTime->arr->val.EndTime);
            }

            itemIndex++;
        }
    }

    return IVEC_SUCCESS;
}

kint ivisa_SG_GetPurchaseItemDetails(kchar *purchaseItemID, IV_PURCHASE_ITEM_DETAIL_INFO *purchaseItemInfo)
{
    kchar *deflang;
    IV_PURCHASE_ITEM_DETAIL_INFO *pcii = purchaseItemInfo;
    IV_PURCHASE_ITEM_BASE_INFO    *pcibi = &pcii->purchaseItemBaseInfo;
    IV_PURCHASEITEM_CONTENT *pci_ctt;
    sg_root *rt = __g_sg_mgr->cursg;
    sg_pci *pci;
    sg_pcd *pcd;
    sg_pcc *pcc;
    sg_ser *ser;
    sg_sch *sch;
    sg_ctt *ctt;
    kuint i, j, index = 0;
    IV_PURCHASEITEM_CONTENT *content;

    K_dlist_entry *entry, *hdr, *fb_entry, *fb_hdr;
    K_freq *fblk;


    if (!rt) {
        return IVEC_FAILURE;
    }
    deflang = rt->mgr->conf.lang;

    pci = (sg_pci*)sg_root_find_file(rt, PCI, knil, purchaseItemID);
    if (!pci) {
        return IVEC_FAILURE;
    }

    // fill the base information
    pcibi->weight = pci->arr->attr.weight;
    pcibi->purchaseItemID = kstr_dup(pci->attr.id);

    if (pci->arr->Name) {
        // language
        for (i = 0; i < pci->arr->Name->ecnt; i++) {
            if (streq(deflang, pci->arr->Name->arr[i].attr.xml_lang)) {
                pcibi->name = pci->arr->Name->arr[i].val.Name;
                break;
            }
        }
        if (i == pci->arr->Name->ecnt) {
            pcibi->name = kstr_dup(pci->arr->Name->arr->val.Name);
        }
    }

    if (pci->arr->StartTime) {
        strcpy(pcibi->startTime, pci->arr->StartTime->arr->val.StartTime);
    }
    if (pci->arr->EndTime) {
        strcpy(pcibi->endtime, pci->arr->EndTime->arr->val.EndTime);
    }

    // fill the detailed information
    if (pci->arr->ParentalRating) {
        pcii->parentalRating = kstr_dup(pci->arr->ParentalRating->arr->val.ParentalRating);
    }

    if (pci->arr->Description) {
        // Description
        for (i = 0; i < pci->arr->Description->ecnt; i++) {
            if (streq(deflang, pci->arr->Description->arr[i].attr.xml_lang)) {
                pcii->description = kstr_dup(pci->arr->Description->arr[i].val.Description);
                break;
            }
        }
        if (i == pci->arr->Description->ecnt) {
            pcii->description = kstr_dup(pci->arr->Description->arr->val.Description);
        }
    }

    /*
     * Fill content
     */

    /* Get content count */
    pcii->contentNum = 0;
    if (pci->arr->ScheduleReference) {
        pcii->contentNum += pci->arr->ScheduleReference->ecnt;
    }
    if (pci->arr->ContentReference) {
        pcii->contentNum += pci->arr->ContentReference->ecnt;
    }
    if (pci->arr->ServiceReference) {
        pcii->contentNum += pci->arr->ServiceReference->ecnt;
    }
    if (pci->arr->PurchaseItemReference) {
        pcii->contentNum += pci->arr->PurchaseItemReference->ecnt;
    }

    content = kmem_alloz(sizeof(IV_PURCHASEITEM_CONTENT) * pcii->contentNum);
    pcii->content = content;

    /* fill content data */
    if (pci->arr->ScheduleReference) {
        for (i = 0; pci->arr->ScheduleReference->ecnt; i++) {
            ScheduleReference_rec *sr = &pci->arr->ScheduleReference->arr[i];

            fb_hdr = &rt->frqhdr;
            fb_entry = fb_hdr->next;
            while (fb_entry != fb_hdr) {
                K_dlist_entry *entry, *hdr;

                fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
                fb_entry = fb_entry->next;

                hdr = &fblk->fhdrs[SCH];
                entry = hdr->next;
                while (entry != hdr) {
                    sch = FIELD_TO_STRUCTURE(entry, sg_sch, cls_le);
                    entry = entry->next;

                    if (streq(sch->attr.id, sr->attr.idRef)) {
                        STR_SET(content[index].id, sch->attr.id);
                        // FIXME
                        content[index].name[0] = '\0';
                        content[index].type = IV_PURCHASE_TYPE_SCHEDULE;
                        index++;
                    }
                }
            }
        }
    }
    if (pci->arr->ContentReference) {
        for (i = 0; pci->arr->ContentReference->ecnt; i++) {
            ContentReference_rec *sr = &pci->arr->ContentReference->arr[i];

            K_dlist_entry *entry, *hdr;

            fb_hdr = &rt->frqhdr;
            fb_entry = fb_hdr->next;
            while (fb_entry != fb_hdr) {
                fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
                fb_entry = fb_entry->next;

                hdr = &fblk->fhdrs[CTT];
                entry = hdr->next;
                while (entry != hdr) {
                    ctt = FIELD_TO_STRUCTURE(entry, sg_ctt, cls_le);
                    entry = entry->next;

                    if (streq(ctt->attr.id, sr->attr.idRef)) {
                        STR_SET(content[index].id, ctt->attr.id);

                        if (ctt->arr->Name) {
                            // Name
                            for (j = 0; j < ctt->arr->Name->ecnt; j++) {
                                if (streq(deflang, ctt->arr->Name->arr[j].attr.xml_lang)) {
                                    content[index].name = kstr_dup(ctt->arr->Name->arr[j].val.Name);
                                    break;
                                }
                            }
                            if (j == pci->arr->Name->ecnt) {
                                content[index].name = kstr_dup(ctt->arr->Name->arr[0].val.Name);
                            }
                        }

                        content[index].type = IV_PURCHASE_TYPE_CONTENT;
                        index++;
                    }
                }
            }
        }
    }
    if (pci->arr->ServiceReference) {
        for (i = 0; i < pci->arr->ServiceReference->ecnt; i++) {
            ServiceReference_rec *sr = &pci->arr->ServiceReference->arr[i];

            K_dlist_entry *entry, *hdr;

            fb_hdr = &rt->frqhdr;
            fb_entry = fb_hdr->next;
            while (fb_entry != fb_hdr) {
                fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
                fb_entry = fb_entry->next;

                hdr = &fblk->fhdrs[SER];
                entry = hdr->next;
                while (entry != hdr) {
                    ser = FIELD_TO_STRUCTURE(entry, sg_ser, cls_le);
                    entry = entry->next;

                    if (streq(ser->attr.id, sr->attr.idRef)) {
                        STR_SET(content[index].id, ser->attr.id);

                        if (ser->arr->Name) {
                            // Name
                            for (j = 0; j < ser->arr->Name->ecnt; j++) {
                                if (streq(deflang, ser->arr->Name->arr[j].attr.xml_lang)) {
                                    content[index].name = kstr_dup(ser->arr->Name->arr[j].val.Name);
                                    break;
                                }
                            }
                            if (j == pci->arr->Name->ecnt) {
                                content[index].name = kstr_dup(ser->arr->Name->arr[0].val.Name);
                            }
                        }

                        content[index].type = IV_PURCHASE_TYPE_SERVICE;
                        index++;
                    }
                }
            }
        }
    }
    if (pci->arr->PurchaseItemReference) {
        for (i = 0; pci->arr->PurchaseItemReference->ecnt; i++) {
            PurchaseItemReference_rec *sr = &pci->arr->PurchaseItemReference->arr[i];

            K_dlist_entry *entry, *hdr;

            fb_hdr = &rt->frqhdr;
            fb_entry = fb_hdr->next;
            while (fb_entry != fb_hdr) {
                fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
                fb_entry = fb_entry->next;

                hdr = &fblk->fhdrs[PCI];
                entry = hdr->next;
                while (entry != hdr) {
                    pci = FIELD_TO_STRUCTURE(entry, sg_pci, cls_le);
                    entry = entry->next;

                    if (streq(pci->attr.id, sr->attr.idRef)) {
                        STR_SET(content[index].id, pci->attr.id);

                        if (pci->arr->Name) {
                            // Name
                            for (j = 0; j < pci->arr->Name->ecnt; j++) {
                                if (streq(deflang, pci->arr->Name->arr[j].attr.xml_lang)) {
                                    content[index].name = kstr_dup(pci->arr->Name->arr[j].val.Name);
                                    break;
                                }
                            }
                            if (j == pci->arr->Name->ecnt) {
                                content[index].name = kstr_dup(pci->arr->Name->arr[0].val.Name);
                            }
                        }

                        content[index].type = IV_PURCHASE_TYPE_PURCHASEITEM;
                        index++;
                    }
                }
            }
        }
    }


    /* PurchaseData */
    do {
        IV_PURCHASE_DATA *pd;
        kint purchaseDataNum = 0;
        K_dlist_entry *entry, *hdr;

        fb_hdr = &rt->frqhdr;
        fb_entry = fb_hdr->next;
        while (fb_entry != fb_hdr) {
            fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
            fb_entry = fb_entry->next;

            hdr = &fblk->fhdrs[PCD];
            entry = hdr->next;
            while (entry != hdr) {
                pcd = FIELD_TO_STRUCTURE(entry, sg_pcd, cls_le);
                entry = entry->next;

                if (pcd->arr->PurchaseChannelReference) {
                    if (streq(pcd->arr->PurchaseChannelReference->arr->attr.idRef, purchaseItemID)) {
                        purchaseDataNum++;
                    }
                }
            }
        }

        index = 0;

        if (0 == purchaseDataNum) {
            pcii->purchaseData = knil;
            pcii->purchaseDataNum = 0;
            break;
        }

        pd = kmem_alloz(sizeof(IV_PURCHASE_DATA) * purchaseDataNum);

        fb_hdr = &rt->frqhdr;
        fb_entry = fb_hdr->next;
        while (fb_entry != fb_hdr) {
            fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
            fb_entry = fb_entry->next;

            hdr = &fblk->fhdrs[PCD];
            entry = hdr->next;

            while (entry != hdr) {
                pcd = FIELD_TO_STRUCTURE(entry, sg_pcd, cls_le);
                entry = entry->next;

                if (pcd->arr->PurchaseChannelReference) {
                    if (streq(pcd->arr->PurchaseChannelReference->arr->attr.idRef, purchaseItemID)) {

                        if (pci->arr->Description) {
                            // description
                            for (j = 0; j < pci->arr->Description->ecnt; j++) {
                                if (streq(deflang, pci->arr->Description->arr[j].attr.xml_lang)) {
                                    STR_SET(pd[index].description, pci->arr->Description->arr[j].val.Description);
                                    break;
                                }
                            }
                            if (j == pci->arr->Name->ecnt) {
                                pd[index].description = kstr_dup(pci->arr->Description->arr[0].val.Description);
                            }
                        }

                        if (pcd->arr->PriceInfo) {
                            // Multi
                            pd[index].subscriptionType = pcd->arr->PriceInfo->arr->attr.subscriptionType;
                            if (pcd->arr->PriceInfo->arr->MonetaryPrice) {
                                strcpy(pd[index].price, pcd->arr->PriceInfo->arr->MonetaryPrice->arr->val.MonetaryPrice);
                            }
                            if (pcd->arr->PriceInfo->arr->MonetaryPrice) {
                                strcpy(pd[index].currency, pcd->arr->PriceInfo->arr->MonetaryPrice->arr->attr.currency);
                            }
                            if (pcd->arr->PriceInfo->arr->SubscriptionPeriod) {
                                strcpy(pd[index].subscriptionPeriod,  pcd->arr->PriceInfo->arr->SubscriptionPeriod->arr->val.SubscriptionPeriod);
                            }
                        }

                        do {
                            kint iid;
                            K_dlist_entry *entry, *hdr;
                            kint channelNum = 0;
                            IV_PURCHASE_CHANNEL *pc;

                            fb_hdr = &rt->frqhdr;
                            fb_entry = fb_hdr->next;
                            while (fb_entry != fb_hdr) {
                                fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
                                fb_entry = fb_entry->next;

                                // Get IV_PURCHASE_DATA::channelNum
                                hdr = &fblk->fhdrs[PCC];
                                entry = hdr->next;
                                while (entry != hdr) {
                                    pcc = FIELD_TO_STRUCTURE(entry, sg_pcc, cls_le);
                                    entry = entry->next;

                                    if (pcd->arr->PurchaseChannelReference) {
                                        if (!streq(pcc->attr.id, pcd->arr->PurchaseChannelReference->arr->attr.idRef)) {
                                            channelNum++;
                                        }
                                    }
                                }
                            }

                            iid = 0;

                            if (0 == channelNum) {
                                pd[index].channelList = knil;
                                pd[index].channelNum = 0;
                                break;
                            }
                            pc = kmem_alloz(sizeof(IV_PURCHASE_CHANNEL) * channelNum);

                            fb_hdr = &rt->frqhdr;
                            fb_entry = fb_hdr->next;
                            while (fb_entry != fb_hdr) {
                                fblk = FIELD_TO_STRUCTURE(fb_entry, K_freq, le);
                                fb_entry = fb_entry->next;

                                hdr = &fblk->fhdrs[PCC];
                                entry = hdr->next;
                                while (entry != hdr) {
                                    pcc = FIELD_TO_STRUCTURE(entry, sg_pcc, cls_le);
                                    entry = entry->next;

                                    if (pcd->arr->PurchaseChannelReference) {
                                        if (!streq(pcc->attr.id, pcd->arr->PurchaseChannelReference->arr->attr.idRef)) {

                                            if (pcc->arr->Name) {
                                                // language
                                                for (j = 0; j < pcc->arr->Name->ecnt; j++) {
                                                    if (streq(deflang, pcc->arr->Name->arr[j].attr.xml_lang)) {
                                                        STR_SET(pc[iid].name, pcc->arr->Name->arr[j].val.Name);
                                                        break;
                                                    }
                                                }
                                                if (j == pcc->arr->Name->ecnt) {
                                                    pc[iid].name = kstr_dup(pcc->arr->Name->arr[0].val.Name);
                                                }
                                            }

                                            if (pcc->arr->Description) {
                                                // language
                                                for (j = 0; j < pcc->arr->Description->ecnt; j++) {
                                                    if (streq(deflang, pcc->arr->Description->arr[j].attr.xml_lang)) {
                                                        STR_SET(pc[iid].description, pcc->arr->Description->arr[j].val.Description);
                                                        break;
                                                    }
                                                }
                                                if (j == pcc->arr->Description->ecnt) {
                                                    pc[iid].description = kstr_dup(pcc->arr->Description->arr[0].val.Description);
                                                }
                                            }

                                            // language
                                            //                            for (j = 0; j < pcc->arr->ContactInfo->ecnt; j++) {
                                            //                                if (streq(deflang, pcc->arr->ContactInfo->arr[j].attr.xml_lang)) {
                                            //                                    pc[iid].contactInfo = pcc->arr->ContactInfo->arr[j].val.ContactInfo;
                                            //                                    break;
                                            //                                }
                                            //                            }
                                            // if (j == pcc->arr->ContactInfo->ecnt) {
                                            //        pc[iid].contactInfo = kstr_dup(pcc->arr->ContactInfo->arr[0].val.ContactInfo);
                                            // }

                                            if (pcc->arr->PurchaseURL) {
                                                STR_SET(pc[iid].purchaseURL, pcc->arr->PurchaseURL->arr->val.PurchaseURL);
                                                STR_SET(pc[iid].portalURL, pcc->arr->PortalURL->arr->val.PortalURL);
                                            }

                                            iid++;
                                        }
                                    }
                                }
                            }
                            pd[index].channelList = pc;
                            pd[index].channelNum = channelNum;
                        } while (0);
                    }
                }
            }
        }

        pcii->purchaseData = pd;
        pcii->purchaseDataNum = purchaseDataNum;
    } while (0);

    return IVEC_SUCCESS;
}

kint ivisa_SG_GetCurrentNotification (IV_NOTIFICATION_INFO ** notificationInfo)
{

    return IVEC_SUCCESS;
    //sg_root_rtinfo *rti;
    //if (__g_sg_mgr && __g_sg_mgr->cursg && (rti = __g_sg_mgr->cursg->rti)) {
    //    *notificationInfo = kmem_alloz(sizeof(IV_NOTIFICATION_INFO));
    //    STR_SET((*notificationInfo)->description, curNotification.description);
    //    STR_SET((*notificationInfo)->requenstURL, curNotification.requenstURL);
    //    return IVEC_SUCCESS;
    //}
    //return IVEC_FAILURE;

}


kint ivisa_SG_MemFree(void *pData)
{
    kmem_free(pData);
    return IVEC_SUCCESS;
}

#endif

