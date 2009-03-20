/* vim:set et sw=4 sts=4: */
#ifndef __K_FLG_H__
#define __K_FLG_H__


#ifdef __cplusplus
extern "C" {
#endif

#define kflg_set(flg, bits)     ((flg) |= (bits))
#define kflg_clr(flg, bits)     ((flg) &= ~(bits))
#define kflg_chk(flg, bits)     ((flg) & (bits))

#ifdef __cplusplus
}
#endif


#endif /* __K_FLG_H__ */

