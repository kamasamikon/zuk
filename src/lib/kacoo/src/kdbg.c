/* vim:set et sw=4 sts=4: */
#include <stdio.h>
#include <memory.h>
#include <kdbg.h>
#include <kflg.h>

static kuint __g_kdbg_level = 0;                        /**< Global level, set by kdbg_init, default is no message */

static kbool __g_kdbg_inited = kfalse;                  /**< kdbg_init called or not */

static kchar __g_kdbg_cmdline[4096] = { 0 };            /**< command line for application, seperate by '\0' */
static kbool __g_kdbg_cmdline_inited = kfalse;          /**< command line gotten */


/**
 * \brief Set parameters for debug message, should be called once in main
 * \param a_level ored of DBG_LOG, DBG_ERR and DBG_FAT
 * \return Debug message level after set.
 */
kuint kdbg_init(kuint a_level)
{
    if (!__g_kdbg_inited) {
        __g_kdbg_inited = ktrue;

        /* Init level */
        if (a_level & DBG_LOG)
            kflg_set(__g_kdbg_level, DBG_LOG);

        if (a_level & DBG_ERR)
            kflg_set(__g_kdbg_level, DBG_ERR);

        if (a_level & DBG_FAT)
            kflg_set(__g_kdbg_level, DBG_FAT);

        if (a_level & DBG_TIME)
            kflg_set(__g_kdbg_level, DBG_TIME);
    }

    return __g_kdbg_level;
}

static kvoid kdbg_get_cmd_line()
{
    if (!__g_kdbg_cmdline_inited) {
        __g_kdbg_cmdline_inited = ktrue;

        memset(__g_kdbg_cmdline, 0, sizeof(__g_kdbg_cmdline));
        ksal_get_commandline(__g_kdbg_cmdline, sizeof(__g_kdbg_cmdline));
    }
}

/**
 * \brief Current debug message level.
 *
 * This will check the command line to do more on each file.
 *
 * The command line format is --ll-N :file1.ext:file2.ext:file:
 *      N is ored DBG_LOG, DBG_ERR and DBG_FAT
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

        if (prec == '\0' && curc != '\0')
            argv[argc++] = __g_kdbg_cmdline + i;

        prec = curc;
    }

    sprintf(file, ":%s:", a_file);
    /* get level for current file */
    for (i = 0; i < argc; i++)
        if (0 == strncmp("--ll-", argv[i], 5))
            if ((i < argc - 1) && strstr(argv[i + 1], file)) {
                cur_level = atoi(argv[i] + 5);
                break;
            }

    return cur_level;
}

