/* vim:set noet ts=8 sw=8 sts=8 ff=unix: */

#ifndef __KMC_MCSKEL_DEVICE_H__
#define __KMC_MCSKEL_DEVICE_H__

#include <kmccontainer.h>
#include "MCSkel-protocol.h"

class KMC_MCSkelDevice : public KMediaDevice
{
public:
	KMC_MCSkelDevice(KMC_MCSkelProtocol *a_parentProtocal, char *a_name);
	virtual ~ KMC_MCSkelDevice(void);

	virtual char *getHash(void) = 0;

	virtual kbool start(void);
	virtual kbool stop(void);

	virtual int remove(void) {
		return EC_NOT_SUPPORT;
	}
	/**  0 <= a_amp < 100 */ virtual int setSignalAmp(int a_amp) {
		return EC_NOT_SUPPORT;
	}
	virtual int getSignalAmp(int *a_pamp) {
		return EC_NOT_SUPPORT;
	}

	virtual int updateChannelList(void) {
		return EC_NOT_SUPPORT;
	}
	virtual int cancelUpdateChannelList(void) {
		return EC_NOT_SUPPORT;
	}

private:
};

#endif /* __KMC_MCSKEL_DEVICE_H__ */
