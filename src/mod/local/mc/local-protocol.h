/* vim:set et sw=4 sts=4: */
#ifndef __KMC_LOCAL_PROTOCOL_H__
#define __KMC_LOCAL_PROTOCOL_H__

#include <kmccontainer.h>

class KMC_LocalProtocol
{
public:
    KMC_LocalProtocol(KMediaContainer* a_parentContainer, char* a_name, int a_type);
    virtual ~KMC_LocalProtocol(void);

    virtual char* getHash(void) = 0;

    virtual kbool start(void) { kuint of = flg; kflg_set(flg, MC_PRO_FLG_STARTED); return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false; }
    virtual kbool stop(void) { kuint of = flg; kflg_clr(flg, MC_PRO_FLG_STARTED); return kflg_chk(of, MC_PRO_FLG_STARTED) ? true : false; }

    virtual int scanDevice(void);
};

#endif /* __KMC_LOCAL_PROTOCOL_H__ */

