/* vim:set et sw=4 sts=4 ff=unix: */
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <kstr.h>
#include <kdbg.h>
#include <kmem.h>
#include <xmldoc.h>

#include <kim.h>
#include "kmm.h"

/** \brief init and setup the module */
typedef void (*MM_HEY)(KIM *im);

/** \brief finalize and quit the module */
typedef void (*MM_BYE)(KIM *im);

/* XXX What is this? */
typedef void (*MM_OPT)(KIM *im);
typedef void (*MM_CFG)(KIM *im);

/** \brief Get GUID of a module */
typedef void (*MM_GUID)(KIM *im, kchar **retguid);

static kint kmm_call_entrance(kbean a_mm);

/*------------------------------------------------------------------------------
 * Type
 */
#define MT_COMP                     0x00            /**< component */
#define MT_PLUG                     0x01            /**< plugin */
#define MT_THEM                     0x02            /**< theme */

/*------------------------------------------------------------------------------
 * State
 */
#define MS_IS_DIS                   0x00            /**< disable */
#define MS_IS_ENA                   0x01            /**< enable */
#define MS_BB_ENA                   0x02            /**< 2b enable */
#define MS_BB_DIS                   0x03            /**< 2b disable */
#define MS_BB_DEL                   0x04            /**< 2b delete */
#define MS_BB_PUR                   0x05            /**< 2b purge */

#ifdef __WIN32__
#include <Windows.h>
#define KLoadLibrary(modname) LoadLibrary(modname)
#define KUnLoadLibrary(handle) FreeLibrary((HINSTANCE)handle)
#define KGetProcAddress(handle, name) GetProcAddress((HINSTANCE)handle, name)
#define KLastError() GetLastError()
#else
#include <dlfcn.h>
#include <errno.h>
#define KLoadLibrary(modname) dlopen(modname, RTLD_LAZY)
#define KUnLoadLibrary(handle) dlclose(handle)
#define KGetProcAddress(handle, name) dlsym(handle, name)
#define KLastError() errno
#endif

static kvoid kmm_clear_proc_cache(kbean a_mm);

kbean kmm_init(KIM *a_im)
{
    KMM *mm = (KMM*)kmem_alloz(sizeof(KMM));
    mm->im = a_im;
    mm->maxtype = -1;
    mm->loadorder = 0;

    kim_addint(a_im, "i.kmm.evt.modLoadStart", 1, 0, knil, knil);
    kim_addint(a_im, "i.kmm.evt.modLoadEnd", 1, 0, knil, knil);
    kim_addint(a_im, "i.kmm.evt.modUnloadStart", 1, 0, knil, knil);
    kim_addint(a_im, "i.kmm.evt.modUnloadEnd", 1, 0, knil, knil);

    return (kbean)mm;
}

kint kmm_final(kbean a_mm)
{
    KMM *mm = (KMM*)a_mm;

    kmm_clear_proc_cache(a_mm);
    kmm_unload_modules(a_mm);

    kim_delint(mm->im, "i.kmm.evt.modLoadStart");
    kim_delint(mm->im, "i.kmm.evt.modLoadEnd");
    kim_delint(mm->im, "i.kmm.evt.modUnloadStart");
    kim_delint(mm->im, "i.kmm.evt.modUnloadEnd");

    kmem_free(a_mm);
    return 0;
}

static kint kmm_get_mod_cnt(kchar *a_path)
{
    kchar fullpath[1024];
    KVFS_FINDDATA finfo;
    kbean fd = 0;
    kint modcnt = 0;

    sprintf(fullpath, "%s", a_path);

    fd = kvfs_findfirst(fullpath, &finfo);
    if (fd) {
        do {
            if (0 != strcmp(finfo.name, ".") && 0 !=strcmp(finfo.name, ".."))
                modcnt++;
        } while (-1 != kvfs_findnext(fd, &finfo));

        kvfs_findclose(fd);
    }
    return modcnt;
}

kint kmm_load_modules(kbean a_mm)
{
    KMM *mm = (KMM*)a_mm;

    kchar *moddir, *language;
    kint index = 0, ii, type;

    KXmlDoc *doc = knil;
    KXmlNode *node;
    kchar *buf, *val;
    kint buflen;

    KVFS_FINDDATA finfo;
    kbean fd = 0;

    kchar fullpath[256];    /* for findfirst *.* */

    kchar ps[2] = { 0 };
    ps[0] = kvfs_path_sep();

    /* Check platform */
    kchar osver[128], *appname = knil, appver[128], *osid;
    kuint major, minor, rev, app_curver, app_maxver, app_minver, os_curver, os_maxver, os_minver;
    kint tcnt, kk;
    kbool ta_chkpass = kfalse;


    moddir = kim_getstr(mm->im, "s.env.path.moduleDir", knil);
    language = kim_getstr(mm->im, "s.env.language", knil);

    mm->mod.cnt = kmm_get_mod_cnt(moddir);
    if (mm->mod.cnt <= 0)
        return -1;

    kim_setint(mm->im, "i.kmm.evt.modLoadStart", 1, knil, knil);

    /* fill current os version */
    major = minor = rev = 0;
    sprintf(osver, "%s", kim_getstr(mm->im, "s.env.os.ver", knil));
    sscanf(osver, "%u.%u.%u", &major, &minor, &rev);
    os_curver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

    /* fill current os id */
    osid = kim_getstr(mm->im, "s.env.os.id", knil);

    /* current app version */
    appname = kim_getstr(mm->im, "s.env.app.name", knil);
    sprintf(appver, "%s", kim_getstr(mm->im, "s.env.app.ver", knil));
    sscanf(appver, "%u.%u.%u", &major, &minor, &rev);
    app_curver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

    /*
     * Some module has some DLL in it's own directory which can not be found
     * when load the module, it cause load failed.
     * So switch to modules dir when load
     */
    mm->mod.arr = (kmm_moditem*)kmem_alloz(sizeof(kmm_moditem) * mm->mod.cnt);

    sprintf(fullpath, "%s", moddir);

    fd = kvfs_findfirst(fullpath, &finfo);
    if (fd) {
        do {
            xmldoc_del(doc);
            doc = knil;
            if (0 != strcmp(finfo.name, ".") && 0 != strcmp(finfo.name, "..")) {
                kbean cfgfile;
                kchar cfgpath[1024], modpath[1024];
                sprintf(cfgpath, "%s%c%s%ccfg.xml", moddir, ps[0], finfo.name, ps[0]);
                sprintf(modpath, "%s%c%s", moddir, ps[0], finfo.name);

                klog(("cfgpath: %s\n", cfgpath));
                klog(("modpath: %s\n", modpath));

                buf = knil;
                buflen = 0x7FFFFFFF;
                if (!(cfgfile = kvfs_open(cfgpath, "rt", 0))) {
                    klog(("Access error [%s], ignored\n", cfgpath));
                    continue;
                }

                doc = xmldoc_new((KXmlDoc*)knil);
                kvfs_read(cfgfile, (void**)&buf, &buflen);
                kvfs_close(cfgfile);

                xmldoc_parse(doc, buf, buflen);
                kmem_free(buf);

                if (!(node = xmldoc_gotoNode(doc, "module", 0)))
                    continue;
                if (!(val = xmlnode_get_attr_value(node, "id")))
                    continue;

                mm->mod.arr[index].basepath = kstr_dup(modpath);

                mm->mod.arr[index].used = ktrue;
                mm->mod.arr[index].loaded = kfalse;
                mm->mod.arr[index].loadorder = -1;

                mm->mod.arr[index].id = kstr_dup(xmlnode_get_attr_value(node, "id"));
                mm->mod.arr[index].homeurl = kstr_dup(xmlnode_get_attr_value(node, "homepage"));
                mm->mod.arr[index].updturl = kstr_dup(xmlnode_get_attr_value(node, "updateurl"));
                mm->mod.arr[index].logourl = kstr_dup(xmlnode_get_attr_value(node, "logo"));
                mm->mod.arr[index].ver = kstr_dup(xmlnode_get_attr_value(node, "ver"));

                klog(("found plugin \"%s\" in \"%s\"\n",mm->mod.arr[index].id, mm->mod.arr[index].basepath));

                /* Fill name */
                kchar *modname = "";
                kint k, cnt = xmldoc_getNodeCnt(doc, "name");
                for (k = 0; k < cnt; k++) {
                    node = xmldoc_getNode(doc, "name", k);
                    if (!(val = xmlnode_get_attr_value(node, "lang")))
                        /* Save default name */
                        modname = node->text;
                    else if (language == val) {
                        mm->mod.arr[index].name = kstr_dup(node->text);
                        break;
                    }
                }
                if (k == cnt)
                    mm->mod.arr[index].name = kstr_dup(modname);

                /* Fill desc */
                kchar *moddesc = "";
                cnt = xmldoc_getNodeCnt(doc, "desc");
                for (k = 0; k < cnt; k++) {
                    node = xmldoc_getNode(doc, "desc", k);
                    if (!(val = xmlnode_get_attr_value(node, "lang")))
                        /* Save default desc */
                        moddesc = node->text;
                    else if (language == val) {
                        mm->mod.arr[index].desc = kstr_dup(node->text);
                        break;
                    }
                }
                if (k == cnt)
                    mm->mod.arr[index].desc = kstr_dup(moddesc);


                /* Fill type */
                if (!(node = xmldoc_getNode(doc, "type", 0)))
                    continue;

                mm->mod.arr[index].type = (kuchar)strtoul(node->text, 0, 10);
                if (mm->maxtype < mm->mod.arr[index].type)
                    mm->maxtype = mm->mod.arr[index].type;


                /* Fill state */
                if (!(node = xmldoc_getNode(doc, "state", 0)))
                    continue;

                mm->mod.arr[index].state = (kuchar)strtoul(node->text, 0, 10);

                if (MS_IS_DIS == mm->mod.arr[index].state) {
                    klog(("mm:: MS_IS_DIS module: %s\n", mm->mod.arr[index].id));
                } else if (MS_IS_ENA == mm->mod.arr[index].state) {
                    klog(("mm:: MS_IS_ENA module: %s\n", mm->mod.arr[index].id));
                } else if (MS_BB_ENA == mm->mod.arr[index].state) {
                    klog(("mm:: MS_BB_ENA module: %s\n", mm->mod.arr[index].id));
                    mm->mod.arr[index].state = MS_IS_ENA;
                    xmlnode_set_attr_value(node, "state", "0");
                    xmldoc_save(doc, cfgpath);
                } else if (MS_BB_DIS == mm->mod.arr[index].state) {
                    klog(("mm:: MS_BB_DIS module: %s\n", mm->mod.arr[index].id));
                    mm->mod.arr[index].state = MS_IS_DIS;
                    xmlnode_set_attr_value(node, "state", "1");
                    xmldoc_save(doc, cfgpath);
                } else if (MS_BB_DEL == mm->mod.arr[index].state) {
                    klog(("mm:: MS_BB_DEL module: %s\n", mm->mod.arr[index].id));
                } else if (MS_BB_PUR == mm->mod.arr[index].state) {
                    klog(("mm:: MS_BB_PUR module: %s\n", mm->mod.arr[index].id));
                } else {
                    klog(("mm:: bad type (%d) module: %s, set to be disable\n", mm->mod.arr[index].state, mm->mod.arr[index].id));
                    mm->mod.arr[index].state = MS_IS_DIS;
                }

                /*
                 * Check platform
                 */
                ta_chkpass = kfalse;

                /* check if all the platform valid */
                cnt = xmldoc_getNodeCnt(doc, "platform");
                for (k = 0; k < cnt; k++) {
                    node = xmldoc_gotoNode(doc, "platform", k);

                    /* check platform id */
                    if (!(val = xmlnode_get_attr_value(node, "id"))) {
                        kerror(("kmm_load_modules: no id attr\n"));
                        goto next_pl_loop;
                    }
                    if (kstr_icmp(val, osid, 0)) {
                        kerror(("kmm_load_modules: mismatch OS, cur is %s, wanted is %s\n", osid, val));
                        goto next_pl_loop;
                    }

                    /* wanted max version */
                    if (!(val = xmlnode_get_attr_value(node, "maxver"))) {
                        kerror(("kmm_load_modules: no maxver attr\n"));
                        goto next_pl_loop;
                    }
                    major = minor = rev = 0;
                    sscanf(val, "%u.%u.%u", &major, &minor, &rev);
                    os_maxver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

                    /* wanted min version */
                    if (!(val = xmlnode_get_attr_value(node, "minver"))) {
                        kerror(("kmm_load_modules: no minver attr\n"));
                        goto next_pl_loop;
                    }
                    major = minor = rev = 0;
                    sscanf(val, "%u.%u.%u", &major, &minor, &rev);
                    os_minver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

                    klog(("kmm_load_modules: os_curver:%08x, os_maxver:%08x, os_minver: %08x\n", os_curver, os_maxver, os_minver));
                    if (os_curver > os_maxver || os_curver < os_minver) {
                        kerror(("kmm_load_modules: version mismatched\n"));
                        goto next_pl_loop;
                    }

                    /* platform done, goto check target application */

                    /* Check Target Application */
                    tcnt = xmldoc_getNodeCnt(doc, "target");
                    for (kk = 0; kk < cnt; kk++) {
                        node = xmldoc_gotoNode(doc, "target", kk);

                        if (!(val = xmlnode_get_attr_value(node, "id")) || kstr_cmp(val, appname, 0))
                            goto next_ta_loop;

                        /* current wanted max app version */
                        if (!(val = xmlnode_get_attr_value(node, "maxver")))
                            goto next_ta_loop;

                        sscanf(val, "%u.%u.%u", &major, &minor, &rev);
                        app_maxver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

                        /* current wanted min app version */
                        if (!(val = xmlnode_get_attr_value(node, "minver")))
                            goto next_ta_loop;

                        sscanf(val, "%u.%u.%u", &major, &minor, &rev);
                        app_minver = ((major & 0xFF) << 24) + ((minor & 0xFF) << 16) + (rev & 0xFFFF);

                        klog(("kmm_load_modules: curver:%08x, maxver:%08x, minver: %08x\n", app_curver, app_maxver, app_minver));
                        if (app_curver >= app_minver && app_curver <= app_maxver)
                            ta_chkpass = ktrue;

next_ta_loop:   /* Next target */
                        node = xmldoc_gotoNode(doc, "..", 0);
                        if (ta_chkpass)
                            break;
                    }
next_pl_loop:   /* Next platform */
                    if (ta_chkpass)
                        break;

                    /*
                     * XXX if ta_chkpass, it will skip this, it make the node in the last, and also
                     * valid platform node, the next entrance also in this node
                     */
                    node = xmldoc_gotoNode(doc, "..", 0);
                }

                if ((MS_IS_ENA == mm->mod.arr[index].state) && (kfalse == ta_chkpass)) {
                    klog(("kmm_load_modules: %s check not passed\n", mm->mod.arr[index].id));
                    mm->mod.arr[index].state = MS_IS_DIS;
                }

                /* entrance for module */
                if (node = xmldoc_gotoNode(doc, "entrance", 0)) {
                    kchar buffer[1024];
#if defined(__UNIX__)
                    kchar *prefix = "lib", *ext = ".so";
#else
                    kchar *prefix = "", *ext = ".dll";
#endif
                    sprintf(buffer, "%s%c%s%c%s%s%s", moddir, ps[0], finfo.name, ps[0], prefix, node->text, ext);

                    mm->mod.arr[index].entry.name = kstr_dup(buffer);
                    mm->mod.arr[index].entry.bean = knil;
                    if (MS_IS_ENA == mm->mod.arr[index].state) {
                        kvfs_chdir(mm->mod.arr[index].basepath);
                        mm->mod.arr[index].entry.bean = KLoadLibrary(mm->mod.arr[index].entry.name);

                        kerror(("KMM:KLoadLibrary <%s> return <%08x> with GLE<%d>\n",
                                    mm->mod.arr[index].entry.name,
                                    mm->mod.arr[index].entry.bean,
                                    KLastError()));
                    }

                    node = xmldoc_gotoNode(doc, "..", 0);
                }

                index++;
                kassert(index <= mm->mod.cnt);
            }
        } while (-1 != kvfs_findnext(fd, &finfo));

        xmldoc_del(doc);
        kvfs_findclose(fd);
    }

    kmm_call_entrance(a_mm);

    klog(("<<< kmm_load_modules\n"));
    kim_setint(mm->im, "i.kmm.evt.modLoadEnd", 1, knil, knil);
    return 0;
}

static kint kmm_call_entrance(kbean a_mm)
{
    KMM *mm = (KMM*)a_mm;
    kint index = 0, type;

    MM_HEY hey = knil;
    MM_GUID guid = knil;
    kchar *retguid = knil;

    klog(("Call enterance mm_hey\n"));
    for (type = 0; type <= mm->maxtype; type++) {
        for (index = 0; index <mm->mod.cnt; index++) {

            if (kfalse == mm->mod.arr[index].used)
                continue;

            if (mm->mod.arr[index].loaded)
                continue;

            if (mm->mod.arr[index].type != type)
                continue;

            klog((">>> kmm:process:%s\n", mm->mod.arr[index].name));

            if (mm->mod.arr[index].entry.bean) {
                kvfs_chdir(mm->mod.arr[index].basepath);
                hey = (MM_BYE)KGetProcAddress(mm->mod.arr[index].entry.bean, "mm_hey");
                if (hey)
                    hey(mm->im);

                guid = (MM_GUID)KGetProcAddress(mm->mod.arr[index].entry.bean, "mm_guid");
                if (guid) {
                    retguid = knil;
                    guid(mm->im, &retguid);
                    mm->mod.arr[index].entry.soid = retguid;
                }
            }
            mm->mod.arr[index].loadorder = mm->loadorder++;
            mm->mod.arr[index].loaded = ktrue;
            klog(("<<< kmm:process:%s\n", mm->mod.arr[index].name));
        }
    }

    return 0;
}

kint kmm_unload_modules(kbean a_mm)
{
    KMM *mm = (KMM*)a_mm;
    kint loadOrder, index;

    klog(("into kmm_unload_modules\n"));

    kim_setint(mm->im, "i.kmm.evt.modUnloadStart", 1, knil, knil);

    for (loadOrder = mm->loadorder - 1; loadOrder >= 0; loadOrder--) {

        for (index = 0; index <mm->mod.cnt; index++) {

            if (kfalse == mm->mod.arr[index].used)
                continue;
            if ((mm->mod.arr[index].loadorder != -1) && (mm->mod.arr[index].loadorder != loadOrder))
                continue;

            klog(("unloading module <%s>\n", mm->mod.arr[index].name));
            if (mm->mod.arr[index].loaded)
                if (mm->mod.arr[index].entry.bean) {
                    MM_BYE bye= (MM_BYE)KGetProcAddress(mm->mod.arr[index].entry.bean, "mm_bye");
                    if (bye)
                        bye(mm->im);
                }

            if (mm->mod.arr[index].used) {
                mm->mod.arr[index].used = kfalse;
                mm->mod.arr[index].loaded = kfalse;

                if (mm->mod.arr[index].entry.bean)
                    KUnLoadLibrary(mm->mod.arr[index].entry.bean);
            }

            kmem_free_s(mm->mod.arr[index].entry.name);

            kmem_free_s(mm->mod.arr[index].basepath);
            kmem_free_s(mm->mod.arr[index].desc);
            kmem_free_s(mm->mod.arr[index].homeurl);
            kmem_free_s(mm->mod.arr[index].id);
            kmem_free_s(mm->mod.arr[index].logourl);
            kmem_free_s(mm->mod.arr[index].name);
            kmem_free_s(mm->mod.arr[index].updturl);
            kmem_free_s(mm->mod.arr[index].ver);

            break;
        }
    }

    mm->mod.cnt = 0;
    if (mm->mod.arr) {
        kmem_free(mm->mod.arr);
        mm->mod.arr = knil;
    }

    kim_setint(mm->im, "i.kmm.evt.modUnloadEnd", 1, knil, knil);

    klog(("leave kmm_unload_modules\n"));
    return 0;
}

static kvoid* kmm_find_proc(kbean a_mm, kchar *a_soid, kchar *a_cname)
{
    KMM *mm = (KMM*)a_mm;
    kvoid* proc = knil;
    kint index;

    for (index = 0; index < mm->mod.cnt; index++)
        if (mm->mod.arr[index].entry.bean && mm->mod.arr[index].entry.soid &&
                !strcmp(mm->mod.arr[index].entry.soid, a_soid)) {
            proc = (kvoid*)KGetProcAddress(mm->mod.arr[index].entry.bean, a_cname);
            return proc;
        }

    kerror(("kmm_find_proc: guid:%s, cmd:%s not found!\n", a_soid, a_cname));
    return knil;
}

static kvoid kmm_clear_proc_cache(kbean a_mm)
{
    KMM *mm = (KMM*)a_mm;

    kuint i;
    for (i = 0; i < mm->his.cnt; i++) {
        kmem_free_s(mm->his.arr[i].soid);
        kmem_free_s(mm->his.arr[i].name);
    }
}

/**
 * \brief Add jc command history entry to list, grow the list if neccessary
 *
 * \warning No duplicate check, so \e Must call it when kmm_find_proc_in_cache failed.
 */
static kvoid kmm_add_proc_to_cache(kbean a_mm, kchar *a_soid, kchar *a_cname, WASP_JC a_proc)
{
    KMM *mm = (KMM*)a_mm;
    const int grow_step = 20;

    kuint i;
    kvoid *arr;
    for (i = 0; i < mm->his.cnt; i++)
        if (!mm->his.arr[i].soid)
            /* found a unused entry */
            break;

    /* not found empty entry, grow the list */
    if (i == mm->his.cnt) {
        mm->his.cnt += grow_step;

        arr = kmem_alloz(mm->his.cnt * sizeof(kmm_jc_his_entry));
        if (mm->his.arr) {
            memcpy(arr, mm->his.arr, (mm->his.cnt - grow_step) * sizeof(kmm_jc_his_entry));
            kmem_free(mm->his.arr);
        }
        mm->his.arr = (kmm_jc_his_entry*)arr;
    }

    mm->his.arr[i].soid = kstr_dup(a_soid);
    mm->his.arr[i].name = kstr_dup(a_cname);
    mm->his.arr[i].proc = a_proc;
}
static WASP_JC kmm_find_proc_in_cache(kbean a_mm, kchar *a_soid, kchar *a_cname)
{
    KMM *mm = (KMM*)a_mm;
    kchar *soid, *name;
    kuint i;
    for (i = 0; i < mm->his.cnt; i++) {
        soid = mm->his.arr[i].soid;
        name = mm->his.arr[i].name;

        if (soid && name && !kstr_icmp(soid, a_soid, 0) && !strcmp(name, a_cname))
            return mm->his.arr[i].proc;
    }
    return knil;
}

/**
 * \brief Call function in dll for so.
 *
 * \param a_cname Command's name.
 * \param a_soid Dll's or SO's guid.
 *
 * \return 0 for success, -1 for error(not found).
 */
kint kmm_jc_cmd(kbean a_mm, kchar *a_cname, kchar *a_soid, kchar *a_ur0, kchar *a_ur1, kchar *a_ur2, kchar *a_ur3, kchar **a_result)
{
    KMM *mm = (KMM*)a_mm;
    WASP_JC wasp_jc;

    klog(("jc:guid%s, fun:%s\n", a_soid, a_cname));
    if (!a_cname)
        return -1;

    wasp_jc = (WASP_JC)kmm_find_proc_in_cache(a_mm, a_soid, a_cname);
    if (!wasp_jc) {
        wasp_jc = (WASP_JC)kmm_find_proc(a_mm, a_soid, a_cname);
        if (wasp_jc)
            kmm_add_proc_to_cache(a_mm, a_soid, a_cname, wasp_jc);
    }

    if (knil != wasp_jc) {
        wasp_jc(mm->im, a_ur0, a_ur1, a_ur2, a_ur3, a_result);
        return 0;
    } else
        kerror(("-- command :%s not found --\n", a_cname));

    return -1;
}

