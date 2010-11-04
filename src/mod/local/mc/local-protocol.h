/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __KMC_LOCAL_PROTOCOL_H__
#define __KMC_LOCAL_PROTOCOL_H__

#include <kmccontainer.h>

class KMC_LocalProtocol : public KMediaProtocol
{
public:
	KMC_LocalProtocol(KIM *a_im, KMediaContainer *a_parentContainer, char *a_name, int a_type);
	virtual ~KMC_LocalProtocol(void);

	virtual kbool start(void);
	virtual kbool stop(void);

	virtual int scanDevice(void);
};

#endif /* __KMC_LOCAL_PROTOCOL_H__ */
