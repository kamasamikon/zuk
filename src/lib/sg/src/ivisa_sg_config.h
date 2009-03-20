/* vim:set et sw=4 sts=4: */
/*******************************************************************************************************/
/*                  iVisa ESG engine配置项目定义                                                  */
/*     Ver: 1.30                                                                                       */
/*     Date: 2008.03.18                                                                                  */
/*******************************************************************************************************/
#ifndef _IV_SG_CONFIG__
#define _IV_SG_CONFIG__

//#define     IV_SG_WITH_FILE_SYSTEM        1       //是否有文件系统,YES = 有,NO = 没有文件系统
//#define     IV_SG_ROOT_DIRECTORY          "d:\\epg\\working"
//                                                //设置ESG engine在文件系统中的工作目录
//#define     IV_SG_SWAP_DIRECTORY          "d:\\epg\\sharing"
//                                                //设置ESG engine在文件系统中的共享工作目录

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
