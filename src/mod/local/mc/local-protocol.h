/* vim:set et sw=4 sts=4 ff=unix: */
#ifndef __KMC_LOCAL_PROTOCOL_H__
#define __KMC_LOCAL_PROTOCOL_H__

#include <kmccontainer.h>

class KMC_LocalProtocol : public KMediaProtocal
{
public:
    KMC_LocalProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type);
    virtual ~KMC_LocalProtocol(void);

    virtual char* getHash(void);

    virtual kbool start(void);
    virtual kbool stop(void);

    virtual int scanDevice(void);
};

#endif /* __KMC_LOCAL_PROTOCOL_H__ */

