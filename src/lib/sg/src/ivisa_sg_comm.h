/*******************************************************************************************************/
/*                  iVisa ESG engine共用数据结构定义                                                   */
/*     Ver: 1.30                                                                                       */
/*     Date: 2008.03.18                                                                                */
/*******************************************************************************************************/



/*******************************************************************************************************/
/*                                  数据结构定义                                                       */
/*******************************************************************************************************/
#ifndef  __SG_COMM___
#define  __SG_COMM___

#include "ivisa_types.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*-----------------------------------------------------------------------------------------------------*/
// for DAB

#define	IV_DAB_BS_DEF_EID			0
#define	IV_DAB_BS_DEF_SCID			63
#define	IV_DAB_SGDD_DEF_SCID		62

typedef enum _IV_DAB_DATA_MODE_
{
    IV_DAB_FIDC_MODE = 0,                      /*业务在FIDC中传输*/
    IV_DAB_STREAM_MODE = 1,							/*业务使用流模式传输*/
    IV_DAB_PACKET_MODE = 2,							/*业务使用包模式传输*/
	IV_DAB_X_PAD = 3								/*x-pad for audio */
} IV_DAB_DATA_MODE;   


typedef enum _IV_DAB_PROTOCOL_TYPE_
{
    IV_DAB_TDC = 0,                      
    IV_DAB_MPEG2TS = 1,                    
    IV_DAB_IP_TUNELING = 2,                     
    IV_DAB_EWS = 3,
    IV_DAB_DATA_GROUP = 4,
    IV_DAB_MOT_DIRECTORY = 5,
    IV_DAB_MOT_HEADER = 6 
} IV_DAB_PROTOCOL_TYPE;   
          
//for DAB SG/datacast file receiver                
typedef struct _IV_DAB_SESSION_ENTRYINFO_	/*MOT 文件接收用 */
{
    kuint           eID;					/*该session所在的ensemble*/
    IV_DAB_DATA_MODE   mode;                /*该Access所对应业务分量的传输模式义*/
    kuint           serviceComponentID;     /*该业务分量的ID*/
                        	/*---------------------------------------------------------
                            mode = IV_DAB_FIDC_MODE：  该值为FIDCId
                            mode = IV_DAB_STREAM_MODE：该值为SubChId
                            mode = IV_DAB_PACKET_MODE：该值为SCId;
                        	---------------------------------------------------------*/
} IV_DAB_SESSION_ENTRYINFO;


//for DAB service's access, to use by IV_ACCESS_INFO
typedef struct _IV_DAB_SC_ENTRY_			
{
    kuint           eID;                    /*该分量所在的Ensemble ID*/
    kuint			meidaType;				/*该业务分量的类型，由应用层解释 
                                                0x00010000: DAB音频
                                                0x00010001: 带Slideshow的DAB音频
                                                0x00010002: 带DLS的DAB音频
                                                0x00010003: 带Slideshow和DLS的DAB音频
                                                0x00020000: 视频
                                                0x00030000: BWS
                                                0x00040000: TTI/Stock
                                                0x00050000: 文件广播
                                                0x00060000: 紧急通知
                                            */

    IV_DAB_PROTOCOL_TYPE protocolType;		/*传输协议类型*/ 
    kuint			channelID;				/*仅当protocolType==IV_DAB_MPEG2TS时有效，说明在TS流中的ID*/
    IV_DAB_DATA_MODE   mode;                /*该Access所对应业务分量的传输模式义*/
    kuint           serviceComponentID;     /*该业务分量的ID*/
                        	/*---------------------------------------------------------
                            mode = IV_DAB_FIDC_MODE：  该值为FIDCId
                            mode = IV_DAB_STREAM_MODE：该值为SubChId
                            mode = IV_DAB_PACKET_MODE：该值为SCId;
                        	---------------------------------------------------------*/
} IV_DAB_SC_ENTRY;

/*-----------------------------------------------------------------------------------------------------*/
// for DVB

typedef enum _IV_DVB_PROTOCOL_TYPE_
{
    IV_DVB_PES = 15,                      
    IV_DVB_DATA_PIPE = 16,                    
    IV_DVB_DSM_CC_DC = 17,                     
    IV_DVB_DSM_CC_OC = 18 
} IV_DVB_PROTOCOL_TYPE;   
          
//for DVB service's access, to use by IV_ACCESS_INFO
typedef struct _IV_DVB_SC_ENTRY_			
{
    kuint           onID;                   /*该业务分量所在原始网络ID*/
    kushort         tsID;                   /*该业务分量所作TS流ID */
    kuint			meidaType;				/*该业务分量的类型，由应用层解释 
                                                0x01000000: 预留
                                                0x01010000: ISO-11172 MPEG-1 视频
                                                0x01020000: ISO-13818 MPEG-2 视频
                                                0x01030000: ISO-11172 MPEG-1音频
                                                0x01040000: ISO-13818 MPEG-2音频
                                                0x01050000: ISO-13818 私有数据
                                                0x01060000: PES私有数据
                                                0x01070000: ISO-13522 MHEG
                                                0x01080000: ISO-13818 DSM-CC
                                                0x01090000: ITU-T H.222 
                                                …其余值保留

                                            */

    IV_DVB_PROTOCOL_TYPE protocolType;		/*传输协议类型*/ 
    kuint           serviceComponentID;     /*该业务分量的PID,值范围为0~0xFFFE，0xFFFF预留*/
} IV_DVB_SC_ENTRY;



/*-----------------------------------------------------------------------------------------------------*/
//for IPDC or DAB-IP's SG/datacast file receiver
typedef struct _IV_IP_ADDRESS_
{	
	kint           iIPv4_flag;				/*  IP类型标识: 
	                                                  0,表示IPv6类型;
	                                                  1,表示IPv4类型*/
    kuchar 	       ubIP[16];				/*  地址为IPv4时,按高位在前存储在头4个字节单元,
												地址为IPv6时,按高位在前存储在16个字节单元    */                                 
}IV_IP_ADDRESS;

typedef struct _IV_ALC_CHANNEL_INFO_
{
	IV_IP_ADDRESS		target_address;  	/* 目标组播地址*/
	kushort 		    u16TargetPort;	 	/* 目标端口号*/
} IV_ALC_CHANNEL_INFO;


typedef  struct _IV_FLUTE_ALC_SESSION_ENTRYINFO_
{
    kuint           			frequency;              /*该session所在的频点值,以Hz为单位*/    
	kuint                       uiTSI;              	/*传输会话标识 */
	IV_IP_ADDRESS				source_address;  		/* 源地址,如果果为非SSM模式时,该成员可忽略*/
	kint			    		iSSM_flag;       		/* SSM模式标识: 
			                                           		0,标识非SSM模式,即ASM模式;
	       			                                    	1,标识SSM模式*/
	kint			            iAlcChannelNum;     	/*ALC频道的个数 */
	IV_ALC_CHANNEL_INFO        	pAlcChannelHead[1];   	/*ALC频道列表的头指针地址 */
}IV_FLUTE_ALC_SESSION_ENTRYINFO;


#ifdef __cplusplus
}
#endif // __cplusplus


#endif
