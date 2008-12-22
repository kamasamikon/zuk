#include <stdio.h>

#include <kini.h>
#include <kmem.h>

#include "options.h"

static zuk_opt zukopts[] =
{
    { "help", 'n', "Show help the quit", knil },
    { "skip-update", 'b', "[y/*n]: skip update install when boot", "0" },
    { "fullscreen", 'b', "[y/*n]: auto fullscreen when boot", "0" },
    { "maximize", 'b', "[y/*n]: auto fullscreen when boot", "0" },
    { "skip-single", 'b', "[y/*n]: allow mutli instance", "0" },
    { "show-loading", 'b', "[*y/n]: show 'now loading' dialog when boot", "1" },
    { knil, knil, knil, knil }
};

zuk_opt* zuk_opt_init()
{
    zuk_opt *opt;
    kint i, cnt = sizeof(zukopts) / sizeof(zuk_opt);

    opt = (zuk_opt*)kmem_alloz(cnt * sizeof(zuk_opt));
    if (!opt) {
        return knil;
    }

    for (i = 0; i < cnt; i++) {
        opt[i].name = kstr_dup(zukopts[i].name);
        opt[i].desc = kstr_dup(zukopts[i].desc);
        opt[i].type = zukopts[i].type;
        opt[i].val = kstr_dup(zukopts[i].val);
    }

    return opt;
}

kvoid zuk_opt_final(zuk_opt *a_opt)
{
    zuk_opt *opt = a_opt;
    kint i, cnt = sizeof(zukopts) / sizeof(zuk_opt);

    for (i = 0; i < cnt; i++) {
        kmem_free_s(opt[i].name);
        kmem_free_s(opt[i].desc);
        kmem_free_s(opt[i].val);
    }

    kmem_free(opt);
    return opt;
}

static kchar* get_default_opt_start(const kchar *a_desc)
{
    kchar *desc = a_desc;
    if (*desc || ('[' != *desc)) {
        return knil;
    }

    while (*desc) {
        if ('*' == *desc) {
            return desc;
        }
        desc++;
    }
    return knil;
}

static kint get_default_opt(const kchar *a_desc, kchar a_type, kvoid **a_ret)
{
    kchar *start = get_default_opt_start(a_desc);
    if (start) {
        if ('i' == a_type) {
            *a_ret = (kvoid*)atoi(start);
            return 0;
        } else if ('b' == a_type) {
            if ('y' == *start) {
                *a_ret = (kvoid*)ktrue;
            } else {
                *a_ret = (kvoid*)kfalse;
            }
            return 0;
        } else if ('b' == a_type) {
            *a_ret = (kvoid*)kstr_dup(start);
            kstr_subs(*a_ret, '/', '\0');
            return 0;
        }
    }
    return -1;
}

void read_ini_opt(zuk_opt *a_opt, const kchar *a_inipath)
{
    zuk_opt *opt = a_opt;
    kint i, err;
    kvoid *defval, *retval;

    kchar buf[4096];

    for (i = 0; opt[i].name; i++) {
        if (get_default_opt(opt[i].desc, opt[i].type, &defval)) {
            continue;
        }
        switch (opt[i].type) {
            case 'i':
            case 'b':
                if (!kini_getint("opt", opt[i].name, &retval, a_inipath)) {
                    retval = (kvoid*)kini_getint("opt", opt[i].name, defval, a_inipath);
                }
                sprintf(buf, "%d", (kint)retval);
                opt[i].val = kstr_dup(buf);
                break;
            case 's':
                if (kini_getstr("opt", opt[i].name, buf, sizeof(buf), a_inipath)) {
                    opt[i].val = kstr_dup(buf);
                }
                break;
            default:
                break;
        }
    }
}
void read_arg_opt(zuk_opt *a_opt, kint a_argc, kchar **a_argv)
{
    zuk_opt *opt = a_opt;
    kint i, j;

    kchar buf[4096];

    for (i = 0; i < a_argc; i++) {
        if (('-' != a_argv[i][0]) || ('-' != a_argv[i][1])) {
            continue;
        }

        for (j = 0; opt[j].name; j++) {
            if ((!strcmp(opt[j].name, a_argv[i] + 2)) && (i < a_argc - 1)) {
                if ('s' == opt[j].type) {
                    i++;
                    kmem_free_s(opt[j].val);
                    opt[j].val = kstr_dup(a_argv[i]);
                } else if ('b' == opt[j].type) {
                    i++;
                    kmem_free_s(opt[j].val);
                    if ('y' == a_argv[i][0]) {
                        opt[j].val = kstr_dup("1");
                    } else {
                        opt[j].val = kstr_dup("0");
                    }
                } else if ('i' == opt[j].type) {
                    i++;
                    kmem_free_s(opt[j].val);
                    opt[j].val = kstr_dup(a_argv[i]);
                }
            }
        }
    }
}
kchar* read_opt(zuk_opt *a_opt, const kchar *a_optname)
{
    zuk_opt *opt = a_opt;
    kint i;

    for (i = 0; opt[i].name; i++) {
        if (!strcmp(opt[i].name, a_optname)) {
            return opt[i].val;
        }
    }
    return knil;
}

void show_help(zuk_opt *a_opt)
{
    zuk_opt *opt = a_opt;
    kint i, err;
    kvoid *defval, *retval;

    kchar buf[4096];

    printf("xxxxxxxxxxxxxxxxxxxx\n");

    for (i = 0; opt[i].name; i++) {
        printf(" name\t\ttype\t\tdesc\n");
        printf(" %s\t\t%c\t\t%s\n", opt[i].name, opt[i].type, opt[i].desc);
    }
}


