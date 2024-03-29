/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __KMC_MCSKEL_PROTOCOL_H__
#define __KMC_MCSKEL_PROTOCOL_H__

#include <kmccontainer.h>

class KMC_MCSkelProtocol : public KMediaProtocol
{
public:
	KMC_MCSkelProtocol(KMediaContainer *a_parentContainer, char *a_name, int a_type);
	virtual ~ KMC_MCSkelProtocol(void);

	virtual char *getHash(void) = 0;

	virtual kbool start(void);
	virtual kbool stop(void);

	virtual int scanDevice(void);
};

#endif /* __KMC_MCSKEL_PROTOCOL_H__ */
