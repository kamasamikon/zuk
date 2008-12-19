#ifndef __K_OPTS_H__
#define __K_OPTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>

typedef struct _zuk_opt {
    kchar *name;                            /**< --name is opt name */
    kchar type;                             /**< n:nul, i:int, b:bool, s:string */
    kchar *desc;                            /**< description */
    kchar *val;                             /**< all value is string */
} zuk_opt;

zuk_opt* zuk_opt_init();
kvoid zuk_opt_final(zuk_opt *a_opt);

void read_ini_opt(zuk_opt *a_opt, const kchar *a_inipath);
void read_arg_opt(zuk_opt *a_opt, kint a_argc, kchar **a_argv);

kchar* read_opt(zuk_opt *a_opt, const kchar *a_optname);

void show_help(zuk_opt *a_opt);

#ifdef __cplusplus
}
#endif

#endif /* __K_OPTS_H__ */

