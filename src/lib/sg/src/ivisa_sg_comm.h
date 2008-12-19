/*******************************************************************************************************/
/*                  iVisa ESG engine�������ݽṹ����                                                   */
/*     Ver: 1.30                                                                                       */
/*     Date: 2008.03.18                                                                                */
/*******************************************************************************************************/



/*******************************************************************************************************/
/*                                  ���ݽṹ����                                                       */
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
    IV_DAB_FIDC_MODE = 0,                      /*ҵ����FIDC�д���*/
    IV_DAB_STREAM_MODE = 1,							/*ҵ��ʹ����ģʽ����*/
    IV_DAB_PACKET_MODE = 2,							/*ҵ��ʹ�ð�ģʽ����*/
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
typedef struct _IV_DAB_SESSION_ENTRYINFO_	/*MOT �ļ������� */
{
    kuint           eID;					/*��session���ڵ�ensemble*/
    IV_DAB_DATA_MODE   mode;                /*��Access����Ӧҵ������Ĵ���ģʽ��*/
    kuint           serviceComponentID;     /*��ҵ�������ID*/
                        	/*---------------------------------------------------------
                            mode = IV_DAB_FIDC_MODE��  ��ֵΪFIDCId
                            mode = IV_DAB_STREAM_MODE����ֵΪSubChId
                            mode = IV_DAB_PACKET_MODE����ֵΪSCId;
                        	---------------------------------------------------------*/
} IV_DAB_SESSION_ENTRYINFO;


//for DAB service's access, to use by IV_ACCESS_INFO
typedef struct _IV_DAB_SC_ENTRY_			
{
    kuint           eID;                    /*�÷������ڵ�Ensemble ID*/
    kuint			meidaType;				/*��ҵ����������ͣ���Ӧ�ò���� 
                                                0x00010000: DAB��Ƶ
                                                0x00010001: ��Slideshow��DAB��Ƶ
                                                0x00010002: ��DLS��DAB��Ƶ
                                                0x00010003: ��Slideshow��DLS��DAB��Ƶ
                                                0x00020000: ��Ƶ
                                                0x00030000: BWS
                                                0x00040000: TTI/Stock
                                                0x00050000: �ļ��㲥
                                                0x00060000: ����֪ͨ
                                            */

    IV_DAB_PROTOCOL_TYPE protocolType;		/*����Э������*/ 
    kuint			channelID;				/*����protocolType==IV_DAB_MPEG2TSʱ��Ч��˵����TS���е�ID*/
    IV_DAB_DATA_MODE   mode;                /*��Access����Ӧҵ������Ĵ���ģʽ��*/
    kuint           serviceComponentID;     /*��ҵ�������ID*/
                        	/*---------------------------------------------------------
                            mode = IV_DAB_FIDC_MODE��  ��ֵΪFIDCId
                            mode = IV_DAB_STREAM_MODE����ֵΪSubChId
                            mode = IV_DAB_PACKET_MODE����ֵΪSCId;
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
    kuint           onID;                   /*��ҵ���������ԭʼ����ID*/
    kushort         tsID;                   /*��ҵ���������TS��ID */
    kuint			meidaType;				/*��ҵ����������ͣ���Ӧ�ò���� 
                                                0x01000000: Ԥ��
                                                0x01010000: ISO-11172 MPEG-1 ��Ƶ
                                                0x01020000: ISO-13818 MPEG-2 ��Ƶ
                                                0x01030000: ISO-11172 MPEG-1��Ƶ
                                                0x01040000: ISO-13818 MPEG-2��Ƶ
                                                0x01050000: ISO-13818 ˽������
                                                0x01060000: PES˽������
                                                0x01070000: ISO-13522 MHEG
                                                0x01080000: ISO-13818 DSM-CC
                                                0x01090000: ITU-T H.222 
                                                ������ֵ����

                                            */

    IV_DVB_PROTOCOL_TYPE protocolType;		/*����Э������*/ 
    kuint           serviceComponentID;     /*��ҵ�������PID,ֵ��ΧΪ0~0xFFFE��0xFFFFԤ��*/
} IV_DVB_SC_ENTRY;



/*-----------------------------------------------------------------------------------------------------*/
//for IPDC or DAB-IP's SG/datacast file receiver
typedef struct _IV_IP_ADDRESS_
{	
	kint           iIPv4_flag;				/*  IP���ͱ�ʶ: 
	                                                  0,��ʾIPv6����;
	                                                  1,��ʾIPv4����*/
    kuchar 	       ubIP[16];				/*  ��ַΪIPv4ʱ,����λ��ǰ�洢��ͷ4���ֽڵ�Ԫ,
												��ַΪIPv6ʱ,����λ��ǰ�洢��16���ֽڵ�Ԫ    */                                 
}IV_IP_ADDRESS;

typedef struct _IV_ALC_CHANNEL_INFO_
{
	IV_IP_ADDRESS		target_address;  	/* Ŀ���鲥��ַ*/
	kushort 		    u16TargetPort;	 	/* Ŀ��˿ں�*/
} IV_ALC_CHANNEL_INFO;


typedef  struct _IV_FLUTE_ALC_SESSION_ENTRYINFO_
{
    kuint           			frequency;              /*��session���ڵ�Ƶ��ֵ,��HzΪ��λ*/    
	kuint                       uiTSI;              	/*����Ự��ʶ */
	IV_IP_ADDRESS				source_address;  		/* Դ��ַ,�����Ϊ��SSMģʽʱ,�ó�Ա�ɺ���*/
	kint			    		iSSM_flag;       		/* SSMģʽ��ʶ: 
			                                           		0,��ʶ��SSMģʽ,��ASMģʽ;
	       			                                    	1,��ʶSSMģʽ*/
	kint			            iAlcChannelNum;     	/*ALCƵ���ĸ��� */
	IV_ALC_CHANNEL_INFO        	pAlcChannelHead[1];   	/*ALCƵ���б��ͷָ���ַ */
}IV_FLUTE_ALC_SESSION_ENTRYINFO;


#ifdef __cplusplus
}
#endif // __cplusplus


#endif
