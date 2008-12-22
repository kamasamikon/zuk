#include <kdbg.h>
#include <kflg.h>

static kuint __g_kdbg_level = 0;                        /**< Global level, set by kdbg_init, default is no message */

static kbool __g_kdbg_inited = kfalse;                  /**< kdbg_init called or not */

static kchar __g_kdbg_cmdline[4096] = { 0 };            /**< command line for application, seperate by '\0' */
static kbool __g_kdbg_cmdline_inited = kfalse;          /**< command line gotten */

static kvoid kdbg_get_cmd_line()
{
    if (!__g_kdbg_cmdline_inited) {
        __g_kdbg_cmdline_inited = ktrue;

        memset(__g_kdbg_cmdline, 0, sizeof(__g_kdbg_cmdline));
        ksal_get_commandline(__g_kdbg_cmdline, sizeof(__g_kdbg_cmdline));
    }
}

/**
 * \brief Set parameters for debug message, should be called once in main
 * \param a_level ored of DBG_LOG, DBG_ERR, DBG_FAT, DBG_ASS, DBG_BRK and DBG_TIM
 * \return Debug message level after set.
 */
kuint kdbg_init()
{
    kint i, argc = 0;
    kchar *argv[2048];
    kchar prec = '\0', curc;

    if (__g_kdbg_inited) {
        return __g_kdbg_level;
    }

    __g_kdbg_inited = ktrue;

    kdbg_get_cmd_line();

    for (i = 0; i < sizeof(__g_kdbg_cmdline); i++) {
        curc = __g_kdbg_cmdline[i];

        if (prec == '\0' && curc != '\0') {
            argv[argc++] = __g_kdbg_cmdline + i;
        }

        prec = curc;
    }

    /* parse command line to get --log-level N and --assert-break y/n and --log-time y/n */
    for (i = 1; i < argc; i++) {
        if (0 == strcmp("--log-level", argv[i])) {
            if (i < argc - 1) {
                i++;
                kuint level = atoi(argv[i]);

                /* Init level */
                if (level & DBG_LOG) {
                    kflg_set(__g_kdbg_level, DBG_LOG);
                } else {
                    kflg_clr(__g_kdbg_level, DBG_LOG);
                }
                if (level & DBG_ERR) {
                    kflg_set(__g_kdbg_level, DBG_ERR);
                } else {
                    kflg_clr(__g_kdbg_level, DBG_ERR);
                }
                if (level & DBG_FAT) {
                    kflg_set(__g_kdbg_level, DBG_FAT);
                } else {
                    kflg_clr(__g_kdbg_level, DBG_FAT);
                }
                if (level & DBG_ASS) {
                    kflg_set(__g_kdbg_level, DBG_ASS);
                } else {
                    kflg_clr(__g_kdbg_level, DBG_ASS);
                }
            }
        } else if (0 == strcmp("--log-time", argv[i])) {
            if (i < argc - 1) {
                i++;
                if ('y' == argv[i][0]) {
                    kflg_set(__g_kdbg_level, DBG_TIM);
                } else {
                    kflg_clr(__g_kdbg_level, DBG_TIM);
                }
            }
        } else if (0 == strcmp("--assert-break", argv[i])) {

            if (i < argc - 1) {
                i++;
                if ('y' == argv[i][0]) {
                    kflg_set(__g_kdbg_level, DBG_BRK);
                } else {
                    kflg_clr(__g_kdbg_level, DBG_BRK);
                }
            }
        }
    }

    return __g_kdbg_level;
}

/**
 * \brief Current debug message level.
 *
 * This will check the command line to do more on each file.
 *
 * The command line format is --ll-N :file1.ext:file2.ext:file:
 *      N is ored DBG_LOG, DBG_ERR, DBG_ASS, DBG_BRK and DBG_TIM
 *      file.ext is the file name. If exist dup name file, the level set to all.
 */
kuint kdbg_getlevel(const kchar *a_file)
{
    kint i, argc = 0;
    kchar *argv[2048], file[256];
    kchar prec = '\0', curc;

    kuint cur_level = __g_kdbg_level;

    kdbg_get_cmd_line();

    for (i = 0; i < sizeof(__g_kdbg_cmdline); i++) {
        curc = __g_kdbg_cmdline[i];

        if (prec == '\0' && curc != '\0') {
            argv[argc++] = __g_kdbg_cmdline + i;
        }

        prec = curc;
    }

    sprintf(file, ":%s:", a_file);
    /* get level for current file */
    for (i = 1; i < argc; i++) {
        if (0 == strncmp("--ll-", argv[i], 5)) {
            if ((i < argc - 1) && strstr(argv[i + 1], file)) {
                cur_level = atoi(argv[i] + 5);
            }
        }
    }

    return cur_level;
}

