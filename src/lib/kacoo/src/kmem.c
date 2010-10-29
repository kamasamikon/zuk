/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#include <memory.h>
#include <kmem.h>

kint kmem_init(kuint a_flg)
{
	return ksal_mem_init(a_flg);
}

kint kmem_final(kvoid)
{
	return ksal_mem_final();
}

kvoid *kmem_alloc(kuint a_size)
{
	return ksal_mem_alloc(a_size);
}

kvoid *kmem_alloz(kuint a_size)
{
	kvoid *ptr = ksal_mem_alloc(a_size);
	if (ptr)
		memset(ptr, 0, a_size);
	return ptr;
}

kvoid kmem_free(kvoid *a_ptr)
{
	ksal_mem_free(a_ptr);
}

kvoid *kmem_realloc(kvoid *a_ptr, kuint a_size)
{
	kvoid *nptr = kmem_alloc(a_size);
	if (nptr) {
		memcpy(nptr, a_ptr, a_size);
		kmem_free(a_ptr);
	}
	return nptr;
}

kvoid *kmem_move(kvoid *a_addr1, kvoid *a_addr2, kuint a_num)
{
	kchar *s1, *s2;

	s1 = a_addr1;
	s2 = a_addr2;
	if ((s2 < s1) && (s2 + a_num > s1))
		goto back;

	while (a_num > 0) {
		*s1++ = *s2++;
		a_num--;
	}
	return a_addr1;

back:
	s1 += a_num;
	s2 += a_num;
	while (a_num > 0) {
		*--s1 = *--s2;
		a_num--;
	}
	return a_addr1;
}
