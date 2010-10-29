/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __K_MEM_H__
#define __K_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include <ksal.h>

kint kmem_init(kuint a_flg);
kint kmem_final(kvoid);

kvoid *kmem_alloc(kuint a_size);
kvoid *kmem_alloz(kuint a_size);
kvoid kmem_free(kvoid *a_ptr);

#define kmem_free_s(p) do { if (p) kmem_free(p); } while (0)
#define kmem_free_z(p) do { kmem_free(p); p = knil; } while (0)
#define kmem_free_zs(p) do { if (p) { kmem_free(p); p = knil; } } while (0)

kvoid *kmem_realloc(kvoid *a_ptr, kuint a_size);
kvoid *kmem_move(kvoid *a_addr1, kvoid *a_addr2, kuint a_num);

#ifdef __cplusplus
}
#endif
#endif /* __K_MEM_H__ */
