/* vim:set et sw=4 sts=4: */
/*******************************************************************************************************/
/*                  iVisa ESG engine������Ŀ����                                                  */
/*     Ver: 1.30                                                                                       */
/*     Date: 2008.03.18                                                                                  */
/*******************************************************************************************************/
#ifndef _IV_SG_CONFIG__
#define _IV_SG_CONFIG__

//#define     IV_SG_WITH_FILE_SYSTEM        1       //�Ƿ����ļ�ϵͳ,YES = ��,NO = û���ļ�ϵͳ
//#define     IV_SG_ROOT_DIRECTORY          "d:\\epg\\working"
//                                                //����ESG engine���ļ�ϵͳ�еĹ���Ŀ¼
//#define     IV_SG_SWAP_DIRECTORY          "d:\\epg\\sharing"
//                                                //����ESG engine���ļ�ϵͳ�еĹ�����Ŀ¼

//enum _IV_BROADCAST_TYPE_ {
//            IV_BROADCAST_TYPE_IPDC_OVER_DVBH,
//            IV_BROADCAST_TYPE_DAB_IP,
//            IV_BROADCAST_TYPE_DAB
//};
//#define     IV_SG_BROADCAST_TYPE          BROADCAST_TYPE_DAB

//#define        IV_DEBUG                      1


/* Platform specific configurations */
#define     IV_SG_BIG_ENDIAN              0       //Define this macro for Big Endian CPU

#define        IV_PATH_MAX_SIZE            256

//#define     PROT_DMBTH
//#define     PROT_DAB
//#define        PROT_IP

#endif
