/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__)
#define EXPORT_FUN __declspec(dllexport)
#else
#define EXPORT_FUN
#endif

#endif /* __SYS_CFG_H__ */
