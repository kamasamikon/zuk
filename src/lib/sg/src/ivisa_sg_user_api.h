/*******************************************************************************************************/
/*                  iVisa ESG engine�û���API��ض���                                                  */
/*     Ver: 1.30                                                                                       */
/*     Date: 2008.03.18                                                                                */
/*******************************************************************************************************/



/*******************************************************************************************************/
/*                                  ���ݽṹ����                                                       */
/*******************************************************************************************************/
#ifndef  __IV_SG_USE_API___
#define  __IV_SG_USE_API___

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ivisa_types.h"
#include "ivisa_err_code.h"
#include "ivisa_sg_comm.h"

/*
 * Ƶ��
 */
typedef enum _IV_SERVICE_TYPE_
{

    IV_SERVICE_TYPE_UNSPECIFIED = 0,               // 0 �C δ�涨
    IV_SERVICE_TYPE_BASICTV,                       //1 �C �������ӽ�Ŀ
    IV_SERVICE_TYPE_BASICAUDIO,                    //2 �C ������Ƶ�㲥
    IV_SERVICE_TYPE_RFU1,                          //3 �C ����
    IV_SERVICE_TYPE_CACHEDCAST,                    //4 - �����ù㲥����
    IV_SERVICE_TYPE_FILEDOWNLOAD,                  //5 �C ���ݹ㲥
    IV_SERVICE_TYPE_SOFTWAREMGMT,                  //6 �C �������
    IV_SERVICE_TYPE_NOTIFICATION,                  //7 �C ͨ��
    IV_SERVICE_TYPE_SERVICEGUIDE,                  //8 �C ҵ��ָ��
    IV_SERVICE_TYPE_TERMINALPROV                   //9 �C �ն�����
                                                //10 - 127  ����
                                                //128 -255 ����Ϊ˽��
} IV_SERVICE_TYPE;

typedef struct _IV_SERVICE_BASE_INFO_
{
    kchar               *serviceID;             //ָ��Ƶ��ID(URI)��ָ��
    kchar               *name;                  //ָ��Ƶ������ָ��, UTF-8
    kuint               weight;                 //��Ƶ����Ƶ���б��е���ʾλ��,��ֵ������˳������,ֵ��С����ʾ����ǰ��
    kuint               serviceProtection;      //��Ƶ���Ƿ����,0��δ����,����ֵ������[Bool]
    IV_SERVICE_TYPE     serviceType;   //Ƶ������,����ֵ����� IV_SERVICE_TYPE
    kchar               *MIMEType;              //Ƶ��ҵ���MIME����;ServiceType = ���ݹ㲥ʱ,������������ҵ�����;Ϊ����ֵʱ,����ֵ��ΪNULL��
} IV_SERVICE_BASE_INFO;






typedef enum _IV_MID_CHANNEL_ORIGIN_TYPE_
{
    IV_MID_CHANNEL_ORIGIN_TYPE_BROADCAST=0,             /*�㲥ҵ��*/
    IV_MID_CHANNEL_ORIGIN_TYPE_WEB,                     /*˫��ҵ��*/
    IV_MID_CHANNEL_ORIGIN_TYPE_LOCATION                 /*����ҵ��*/

} IV_MID_CHANNEL_ORIGIN_TYPE;

typedef enum
{
	IV_MID_CHANNEL_TYPE_LIVE_BROADCAST=0,  	/*ֱ��ҵ�������ǹ㲥����˫��������Ƶ�㲥������Ƶ��Ŀ*/
	IV_MID_CHANNEL_TYPE_VOD,				/* �㲥ҵ�������ǹ㲥����˫��*/
	IV_MID_CHANNEL_TYPE_DOWLOAD			/* ����ҵ�������ǹ㲥����˫������������Ƶ�໹��һ�������ļ�����*/

}IV_MID_CHANNEL_TYPE;

typedef struct _IV_MID_CHANNEL_INFO_
{

	IV_MID_CHANNEL_ORIGIN_TYPE		ivChannelOrigin;	/*��IV_MID_CHANNEL_ORIGIN_TYPE*/
	IV_MID_CHANNEL_TYPE  			ivChannelType;      /*��IV_MID_CHANNEL_TYPE*/

	void 				*pChannelInfo;		/*���ݲ�ͬ��channel���ͣ���������ָ���Ͳ�ͬ
												ivChannelType=IV_MID_CHANNEL_TYPE_LIVE_BROADCAST ʱ��������Ŀǰδ���壬ΪNULL
												ivChannelType=IV_MID_CHANNEL_TYPE_VOD ʱ��������Ϊchar *,��ʶ�㲥ҵ��ķ���ID,
																						����ͬΪSM_VodCateInfo�ṹ��vodSortIDֵ
																						(�μ�streaming_media.h�еĶ���)
												ivChannelType=IV_MID_CHANNEL_TYPE_DOWLOAD ʱ��������Ϊchar *����ʶ����ҵ��ķ���ID,
																						����ͬΪSM_DownCateInfo�ṹ��downSortIDֵ
																						(�μ�streaming_media.h�еĶ���)

											*/

}IV_MID_CHANNEL_INFO;




typedef struct _IV_SERVICE_DETAIL_INFO_
{
    IV_SERVICE_BASE_INFO serviceBaseInfo;      //Service��������
    kchar           *description;           //ָ��Ƶ��������ָ��
    kchar           *parentRating;          //ָ��Ƶ�����ݷ���ȼ�(�ַ���)��ָ��
    kchar           *genre;                 //ָ��Ƶ�����ɵ��ַ���(�ַ���)��ָ��
    kchar           *globalServiceID;       //ָ���Ƶ��ȫ��ΨһID(URI)��ָ��
} IV_SERVICE_DETAIL_INFO;

typedef struct _IV_FILE_DATA_
{
    void            *data;                  //ָ�����Ϣ���ݵ�ָ��
    kuint           length;                 //���ݵ���Ч����
    kchar           *uri;                   //��dataΪNULLʱ���Դ�uri��ȡ��uri��Ϊ����·����Web���ӡ�
} IV_FILE_DATA;


/* Preview ���� */
typedef enum _IV_PREVIEW_TYPE_
{
    IV_PREVIEW_TYPE_LOGO_SMALL = 0,            //LOGO��СͼƬ
    IV_PREVIEW_TYPE_LOGO_LARGE,                //Ƶ����־�Ĵ�ͼƬ
    IV_PREVIEW_TYPE_SERVICE_PREVIEW,           //Ƶ����ĿԤ����Ϣ,ΪͼƬ�����ı�
    IV_PREVIEW_TYPE_SERVICE_SWITCH,            //��һ��Ƶ���л�����һ��Ƶ���ĵȴ��ڼ���ʾ��Ԥ����Ϣ,ΪͼƬ�����ı�
    IV_PREVIEW_TYPE_ALT_BLACKOUT,              //����Ļ���ֺ�����������Ч�����ź�ʱ����ʾ��ͼƬ�����ı�
    IV_PREVIEW_TYPE_SERVICE_PROMOTION          //Ƶ�����ƹ���Ϣ,ΪͼƬ�����ı�
} IV_PREVIEW_TYPE;

typedef struct _IV_PREVIEW_INFO_
{
    IV_FILE_DATA       file;                    //ֻ�� file.dataΪ��ʱʹ�� uri���ȡ�ļ�����
    kchar           *MIMEType;                  //���ݵ�MIME ����(�ı�ʱΪUTF-8)
} IV_PREVIEW_INFO;



/*
 * ���ݻ�ȡ����(Access)
 */

 typedef enum _IV_ACCESS_TYPE_
{
    IV_ACCESS_TYPE_IP = 0,                      //0 �C ����IP�Ĺ㲥ϵͳ��Acess������
    IV_ACCESS_TYPE_DAB,                         //1 �C ����DAB������Ƶҵ���IP���Ĺ㲥ϵͳ��Acess������
    IV_ACCESS_TYPE_DVB,							//2 �C ����DVB(����Ƶҵ���IP) �Ĺ㲥ϵͳ
	IV_ACCESS_TYPE_WEB							//3 �C ����WEB������Ƶҵ��(ͨ��URL��ȡҵ��)
} IV_ACCESS_TYPE;

 typedef union _IV_SESSION_PARAMS_{
	 IV_FILE_DATA         sdp;               //buf.dataΪNULLʱʹ��uri��ȡSDP������IPDC or DAB-IP.
	 IV_DAB_SC_ENTRY   dabAccess;   			//���㲥����ΪDABʱʹ�á�
	 IV_DVB_SC_ENTRY   dvbAccess;   			//���㲥����ΪDVBʱʹ�á�
 } IV_SESSION_PARAMS;

typedef struct _IV_ACCESS_INFO_
{
    kuint           	frequency;              //��Service���ڵ�Ƶ��ֵ,��HzΪ��λ
    kchar               *usage;                 //��Access��Ӧ�Ự������,UTF-8�ַ���,���ж���Ự����ʱ,Ӧ��������ʾ���û����û�ѡ��
    kint                defaultAccessFlag;      //��Acess�Ƿ�Ϊȱʡ,Ĭ����ʾȱʡAccessָ�����Ϣ ������ͨ���˵����û�ѡ��
    kuint               validFrom;              //��Ч�ڿ�ʼʱ�� UTC.
    kuint               validTo;                //��Ч����ֹʱ�� UTC
    IV_ACCESS_TYPE      accessType;             // accessType��IV_ACCESS_TYPE_IP��sessionParams������ΪIV_FILE_DATA
                                                // accessType��IV_ACCESS_TYPE_DAB��sessionParams������ΪIV_DAB_SC_ENTRY
                                                // accessType��IV_ACCESS_TYPE_DVB��sessionParams������ΪIV_DVB_SC_ENTRY
                                                // accessType��IV_ACCESS_TYPE_WEB��sessionParams������Ϊchar *url

    kuint				scNum;					//��access��ҵ������ĸ�������ΪSDPʱ����ֵӦ��Ϊ1�� ��ΪIV_DAB_SC_ENTRYʱ��ӦΪ��
                                                // ҵ��ʵ������IV_DAB_SC_ENTRY�ĸ�����
    union {
        IV_FILE_DATA         sdp;               //buf.dataΪNULLʱʹ��uri��ȡSDP������IPDC or DAB-IP.
        IV_DAB_SC_ENTRY   dabAccess;   			//���㲥����ΪDABʱʹ�á�
        IV_DVB_SC_ENTRY   dvbAccess;   			//���㲥����ΪDVBʱʹ�á�
		kchar				*url;				//������Ƶҵ��ͨ��WEBʱ
    } sessionParams[1];

} IV_ACCESS_INFO;


/*
 * ��Ŀ
 */
typedef struct _IV_PROGRAM_BASE_INFO_
{
    kchar           *name;                      //��Ŀ����
    kchar           *programID;                 //��ĿID
    kuint           startTime;                  //��Ŀ��ʼʱ��(UTC)
    kuint           endTime;                    //��Ŀ����ʱ��(UTC)
    kuint           duration;                   //��Ŀʱ��(��)
    kchar           *parentRating;              //��Ŀ�������ݷ���ȼ�,�ַ���
    kchar           *genre;                     //��Ŀ��������,�ַ���,��'Jazz'
    kuint           contentProtection;          //�����Ƿ����,boolֵ
} IV_PROGRAM_BASE_INFO;



/*
 * ���Ż�������
 */
typedef struct _IV_SMS_CHOICE_
{
    kchar       *smsURI;        //sms: URIs, ���԰����궨�� $userid,$deviceid,$userinputs��,�﷨����
            /*---------------------------------------------------------------------
               sms-uri               =  scheme ":" scheme-specific-part
               scheme                =  "sms"
               scheme-specific-part  =  1*( sms-recipient ) [ sms-body ]
               sms-recipient         =  gstn-phone sms-qualifier
                                        [ "," sms-recipient ]
               sms-qualifier         =  *( smsc-qualifier / pid-qualifier )
               smsc-qualifier        =  ";smsc=" SMSC-sub-addr
               pid-qualifier         =  ";pid=" PID-sub-addr
               sms-body              =  "?body=" *urlc
            ------------------------------------------------------------------------*/
    kchar       *choiceText;    //����ѡ�������ʾ,������ȫ�������趨�йء�
} IV_SMS_CHOICE;

typedef struct _IV_ITV_SMS_TEMPLATE_
{
    kchar           *text;      //���Ͷ������ݵ��ı�ǰ׺���û����Ͷ��ŵ����ݽ�Ϊtext+��ѡ���sms-body�ı�(smsURI��).
    kint            choiceNum;	//ѡ��ĸ���
    IV_SMS_CHOICE  choices[1];
} IV_ITV_SMS;


/*
 * Email ��������ģ��
 */
typedef struct _IV_ITV_EMAIL_TEMPLATE_
{
    kchar   *to;				//�����ͷ���ַ
    kchar   *cc;				//���ͷ���ַ
    kchar   *bcc;				//���ͷ���ַ
    kchar   *subject;			//����
    kchar   *body;				//Ԥ������
} IV_ITV_EMAIL;

/*
 * �㲥�ļ�����
 */
typedef struct _IV_MEDIA_OBJECT_
{
    kint        flag;                   //1��ʾ���ļ�Ϊ���(����ҳ),0Ϊ��ͨ�ļ�������ֵԤ����
    kchar       *contentLocation;       //�ļ��������·��,���ܰ���$(SG_SWAP_DIRECTORY)��
    kchar       *contentType;           //MIME����
} IV_MEDIA_OBJECT;

typedef struct _IV_ITV_MEDIAOBJECTS_
{
    kint                moNum;			//�����ļ��ĸ���
    IV_MEDIA_OBJECT    objects[1];			//�ļ��б�
} IV_ITV_MEDIAOBJECTS;


/*
 * ��������
 */
typedef enum _IV_INTERACTIVITY_TYPE_
{
    IV_INTERACTIVITY_TYPE_SMS = 0,     	//��������
    IV_INTERACTIVITY_TYPE_EMAIL,      		//Email
    IV_INTERACTIVITY_TYPE_VOICECALL,   	//�绰
    IV_INTERACTIVITY_TYPE_WEBLINK,     	//��ҳ����
    IV_INTERACTIVITY_TYPE_MEDIAOBJ     	//�ļ��㲥
} IV_INTERACTIVITY_TYPE;

typedef struct _IV_INTERACTIVITY_MEDIA_
{
    kuchar          type;                       //����������, IV_INTERACTIVITY_TYPE
    kchar           *description;               //��������
    kuchar          relativePreference;         //��ͬʱ�ж������ʱ,����������ʾ˳��,������˳����ʾ,��ֵС������ʾ

    union {
        IV_ITV_SMS    			*sms;
        IV_ITV_EMAIL           email;
        kchar               phoneNumber[20];
        kchar               *webLink;           //��ҳ���ӻ���ʱ,��ҳ���ڵ�URL,Ӧ��ͨ��������ȡ�û�����ҳ
        IV_ITV_MEDIAOBJECTS    *mediaObjects;
    } contents;

} IV_INTERACTIVITY_MEDIA;

/*
 * ����Ŀ¼
 */
typedef struct _IV_INTERACTIVITY_DIRECTORY_
{
    kuint               startTime;				//������ʾ��ʼʱ��
    kuint               endTime;				//������ʾ��ֹʱ��
    kint                mediaNumber;			//��������
    IV_INTERACTIVITY_MEDIA  interactivityMedia[1];	//�����б�
} IV_INTERACTIVITY_DIRECTORY;

/*
 * ������Ϣ
 */
typedef struct _IV_NOTIFICATION_INFO_
{
    kchar       *nid;       	        //����Ϣ��ΨһID,�ַ���
    kchar       emergencyType;          //�����¼�������
    kchar       emergencyLevel;         //�����¼��ļ���
    kchar       *publisherName;         //���л������ƣ����255Byte��
    kuint       validTo;                //����Ϣ��Ч�ڵĽ�ֹʱ��(UTC)
    kchar       *title;                 //������Ϣ����
    kchar       *content;               //������Ϣ����
    kchar       *targetArea;            //�ý�����Ϣ��Ե�Ŀ��������Ϊ�ձ�ʾ�����������
                                        //�����ʾ����������Ӫ��ͳһ����
    IV_ACCESS_INFO extDataAccess;       //��������ͨ����Ϣ
} IV_NOTIFICATION_INFO;


/*
 * ����
 */
typedef enum _IV_PURCHASE_TYPE_
{
    IV_PURCHASE_TYPE_SERVICE,             	//Ƶ��
    IV_PURCHASE_TYPE_SCHEDULE,            	//��Ŀ
    IV_PURCHASE_TYPE_CONTENT,            	//����
    IV_PURCHASE_TYPE_PURCHASEITEM			//����һ��������

} IV_PURCHASE_TYPE;

typedef struct _IV_PURCHASE_ITEM_BASE_INFO_
{
    kchar           *name;                      //�ö����������,UTF-8 �ı�
    kchar           *purchaseItemID;            //�ö������ID,ΪglobalPurchaseItemID
    kint            weight;                     //��ʾ˳��,��ֵ������˳������,ֵ��С����ʾ����ǰ��
    kchar           startTime[20];              //�ɿ�ʼ������ʱ��,��(YYYY:MM:DD:hh:mm:ss)����ʽ��ʾ,������ʾ
    kchar           endtime[20];                //��ֹ������ʱ��,��(YYYY:MM:DD:hh:mm:ss)����ʽ��ʾ,������ʾ
} IV_PURCHASE_ITEM_BASE_INFO;


typedef struct _IV_PURCHASEITEM_CONTENT_
{
    kchar           *id;
    kchar           *name;                      //�������ݵ�����,UTF-8 �ı�
    IV_PURCHASE_TYPE	type;                       //�������ݵ�����,��IV_PURCHASE_TYPE����
} IV_PURCHASEITEM_CONTENT;

typedef struct _IV_PURCHASE_CHANNEL_
{
    kchar           *name;                      //�ö���ͨ��������,UTF-8 �ı�
    kchar           *description;               //����ͨ�����,UTF-8 �ı�
    kchar           *contactInfo;               //����ͨ����ϵ��ʽ,UTF-8 �ı�
    kchar           *purchaseURL;               //����ͨ��URL,UTF-8 �ı�
    kchar           *portalURL;                 //����ͨ��Portal URL,UTF-8 �ı�
} IV_PURCHASE_CHANNEL;


typedef enum _IV_SUBSCRIPTION_TYPE
{
    IV_SUBSCRIPTION_TYPE_SINGLE = 0,               //���ζ���,����һ����SubscriptionPeriodָ����ʱ������Ч,����ΪPriceָ������Ϣ
    IV_SUBSCRIPTION_TYPE_CONTINUOUS                //��������,����һ�ν�������Ч,ֱ���û�ȡ������Ϊֹ,��ÿ��SubscriptionPeriodָ����ʱ��,����ΪPriceָ������Ϣ
} IV_SUBSCRIPTION_TYPE;


typedef struct _IV_PURCHASE_DATA_                  //����������
{
    kchar           *description;               //������������,UTF-8 �ı�
    kuint           subscriptionType;           //��������,����ֵ��IV_SUBSCRIPTION_TYPE
    kchar           price[8];                   //�۸�(���ػ��ҵ�λ),С�������λ������,��ASCII����ʽ��ʾ,�确00010.00'����10.00
    kchar           currency[4];                //��������,���չ��ʱ�׼���ҵ�λ���ַ���,�确CNY��Ϊ�����
    kchar           subscriptionPeriod[4];      //��������,UTF-8 �ı�, ��:P1Y=1��,P2M= 2��,P1H= 1Сʱ
    kint            channelNum;                 //�ö����������Ӧ�Ĺ���ͨ������
    IV_PURCHASE_CHANNEL    *channelList;           //�ö����������Ӧ�Ĺ���ͨ�����б�
} IV_PURCHASE_DATA;


typedef struct _IV_PURCHASE_ITEM_DETAIL_INFO_
{
	IV_PURCHASE_ITEM_BASE_INFO purchaseItemBaseInfo;	//�������������
    kchar               *description;               //��������,UTF-8 �ı�
    kchar               *parentalRating;            //���ݷ���ȼ�,�ַ���
    kint                contentNum;                 //�ö������������������
    IV_PURCHASEITEM_CONTENT    *content;               //�ö���������������б�
    kint                purchaseDataNum;            //�ö������Ӧ�Ķ�������������
    IV_PURCHASE_DATA       *purchaseData;              //�ö������Ӧ�Ķ����������б�
} IV_PURCHASE_ITEM_DETAIL_INFO;


/*
 * SG Provider��Ϣ
 */
typedef struct _IV_SG_PROVIDER_
{
    kuint           id;								//SG�ṩ�̱�ʶ
    kchar           *name;							//SG�ṩ������
    kchar           *uri;							//SG�ṩ��ΨһID
    kchar           *infoUrl;						//SG�ṩ��web��ַ
    kchar           *auxData;						//��������
    IV_PREVIEW_INFO    logo;							//SG�ṩ��logo����
} IV_SG_PROVIDER;

typedef enum _IV_SG_CALLBACK_TYPE
{
    IV_SG_CALLBACK_SCAN = 0,    					// ɨ��������
    IV_SG_CALLBACK_INTERACTIVITY, 					// ���µĻ���
    IV_SG_CALLBACK_NOTIFICATION,                    // ���µ���Ϣ
    IV_SG_CALLBACK_SERVICE_UPDATE,  				// Ƶ����Ϣ����
    IV_SG_CALLBACK_PROGRAM_UPDATE,  				// ��Ŀ����Ϣ����
    IV_SG_CALLBACK_ACCESS_UPDATE,					// access��Ϣ����
    IV_SG_CALLBACK_PURCHASE_UPDATE,					// ������Ϣ����
} IV_SG_CALLBACK_TYPE;



typedef kint (*IVISA_SG_CALLBACK)(IV_SG_CALLBACK_TYPE infoType, void *para1, void *para2,void *para3, void *para4);
/*****************************************************************************************************
    ���ܣ�SG�¼�����ص����������û������
    ������
    infoType: �Ѹ�����Ϣ�����ͣ�����ivisa_sg_user_api.h����IV_SG_CALLBACK_TYPE���塣
    para1~para4�� ���������õ��Ĳ���(ע�� Ӧ�ò�Ӧ�ø�����Ҫ��������ת��)��

        1��infoType == IV_SG_CALLBACK_SCAN
            ��Ӧ�ò����ivisa_sg_StartScan�󣬻ᷢ���˻ص���ֻ�е�״̬Ϊ�������/��ʱ�˳�/ʧ��ʱ�����ߵ���ivisa_SG_StopScan()����ֵΪ�ɹ�ʱ����������һ��ɨ�� ivisa_SG_StartScan();

            para1:
            SGɨ��������״̬���䶨�����£�
            		para1== IVEC_INPROGRESS��������
            		para1== IVEC_SUCCESS���������
            		para1== IVEC_TIMEOUT����ʱ�˳�
            		para1== IVEC_FAILURE: ��������ԭ���¸���ʧ��
            		����ֵ������
            para2: 	��ǰ���յ��ļ�����������Ϊ��ǰƵ��SG����״̬��ʾ��ԭʼ����
            para3��	��ǰ�ѷ�����Ҫ��ȡ���ļ�����������Ϊ��ǰƵ��SG����״̬��ʾ��ԭʼ����
            para4: 	�û��ڵ��� ivisa_SG_StartScan()ʱ������Զ������
        2��infoType == IV_SG_CALLBACK_INTERACTIVITY
        	���յ������ͻص��������para1��ָ����ַ��� == ��ǰƵ����ServiceID��Ӧ�õ���ivisa_sg_GetCurrentInteractivity()��ȡ��Ƶ�����µĻ�������ʾ���û���
            para1��	������ӦServiceID
            para2: 	����
            para3: 	����
            para4: 	����
        3��infoType == IV_SG_CALLBACK_NOTIFICATION
        	���յ������ͻص���Ӧ�õ���ivisa_sg_GetCurrentNotification()��ȡ��ǰ����֪ͨ��Ϣ����ʾ���û���
            para1��	����
            para2: 	����
            para3: 	����
            para4: 	����
        4��infoType == IV_SG_CALLBACK_SERVICE_UPDATE
        	���յ������ͻص���Ӧ�õ���ivisa_sg_GetServiceList()��ȡ����Ƶ����Ϣ��
            para1��	����
            para2: 	����
            para3: 	����
            para4: 	����
        5��infoType == IV_SG_CALLBACK_PROGRAM_UPDATE
        	���յ������ͻص�������ҪӦ�õ���ivisa_sg_GetProgramList()��ȡ���½�Ŀ����Ϣ��
            para1��	����
            para2: 	����
            para3: 	����
            para4: 	����
        6��infoType == IV_SG_CALLBACK_ACCESS_UPDATE
        	���յ������ͻص�������ҪӦ�øõ���ivisa_SG_GetServiceAccessInfo()��ȡ������Ϣ��
            para1��	����
            para2: 	����
            para3: 	����
            para4: 	����
        7��infoType == IV_SG_CALLBACK_PURCHASE_UPDATE
        	���յ������ͻص�������ҪӦ�õ���ivisa_SG_GetPurchaseItemList ()��ȡ������Ϣ��
            para1��	����
            para2: 	����
            para3: 	����
            para4: 	����
    [����ֵ]:
            �ɹ�ʱ����0������ʱ���أ�1��

*******************************************************************************************************/

/*******************************************************************************************************/
/*                                  �ӿں�������                                                       */
/*******************************************************************************************************/
kint ivisa_SG_Init(kuint autoUpdateTime, IVISA_SG_CALLBACK evtCallback);
kint ivisa_SG_Start(kvoid);
kint ivisa_SG_Finalize(kvoid);
kint ivisa_SG_SetEnvironment(kchar *key, kchar *val);
kint ivisa_SG_EnableAutoUpdate(kbool enable);
kint ivisa_SG_GetVersion(kuint *majorVer, kuint *minorVer, kuint *revision);
kint ivisa_SG_GetProviderList(IV_SG_PROVIDER **providerInfo, kuint *providerNum);
kint ivisa_SG_SetProvider(kuint providerID);
kint ivisa_SG_Cleanup(kuint providerID, kuint frequency);
kint ivisa_SG_StartScan(kint frequency, kchar tmode, kuint timeout, kvoid *userdata);
kint ivisa_SG_StopScan(kvoid);
kint ivisa_SG_SetLanguage(kchar *language);
kint ivisa_SG_GetServiceList(IV_SERVICE_BASE_INFO ** serviceList, kint *serviceNum);
kint ivisa_SG_GetServiceDetailInfo(kchar *serviceID, IV_SERVICE_DETAIL_INFO * serviceInfo);
kint ivisa_SG_GetServicePreviewInfo (kchar *serviceID, IV_PREVIEW_TYPE infoType, IV_PREVIEW_INFO *previewInfo);
kint ivisa_SG_GetServiceAccessInfo(kchar *serviceID, IV_ACCESS_INFO ** accessInfoList, kint *AccessNum);
kint ivisa_SG_GetProgramList(kchar *serviceID, kuint startTime,kuint endTime, IV_PROGRAM_BASE_INFO** programList, kint *programNum);
kint ivisa_SG_GetProgramDescription (kchar * programID, kchar ** programDes);
kint ivisa_SG_SetCurrentService (kchar * serviceID);
kint ivisa_SG_GetCurrentInteractivity (kchar *serviceID, IV_INTERACTIVITY_DIRECTORY **interactivity);
kint ivisa_SG_GetCurrentNotification (IV_NOTIFICATION_INFO ** notificationInfo);
kint ivisa_SG_GetPurchaseItemList (kchar *serviceID, kuint startTime, kuint endTime, IV_PURCHASE_ITEM_BASE_INFO** purchaseItemList, kint *purchaseItemNum);
kint ivisa_SG_GetPurchaseItemDetails (kchar *purchaseItemID, IV_PURCHASE_ITEM_DETAIL_INFO * purchaseItemInfo);
kint ivisa_SG_MemFree(void *pData);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
