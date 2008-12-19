#ifndef  __KMC_DEFINES_H__
#define  __KMC_DEFINES_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <ktypes.h>
#include <time.h>
    /*
     * Ƶ��
     */
    typedef enum _KMC_SERVICE_TYPE_
    {

        KMC_SERVICE_TYPE_UNSPECIFIED = 0,               // 0 �C δ�涨
        KMC_SERVICE_TYPE_BASICTV,                       //1 �C �������ӽ�Ŀ
        KMC_SERVICE_TYPE_BASICAUDIO,                    //2 �C ������Ƶ�㲥
        KMC_SERVICE_TYPE_RFU1,                          //3 �C ����
        KMC_SERVICE_TYPE_CACHEDCAST,                    //4 - �����ù㲥����
        KMC_SERVICE_TYPE_FILEDOWNLOAD,                  //5 �C ���ݹ㲥
        KMC_SERVICE_TYPE_SOFTWAREMGMT,                  //6 �C �������
        KMC_SERVICE_TYPE_NOTIFICATION,                  //7 �C ͨ��
        KMC_SERVICE_TYPE_SERVICEGUIDE,                  //8 �C ҵ��ָ��
        KMC_SERVICE_TYPE_TERMINALPROV                   //9 �C �ն�����
            //10 - 127  ����
            //128 -255 ����Ϊ˽��
    } KMC_SERVICE_TYPE;

    typedef struct _KMC_SERVICE_BASE_INFO_
    {
        kchar               *serviceID;             //ָ��Ƶ��ID(URI)��ָ��
        kchar               *name;                  //ָ��Ƶ������ָ��, UTF-8
        kuint               weight;                 //��Ƶ����Ƶ���б��е���ʾλ��,��ֵ������˳������,ֵ��С����ʾ����ǰ��
        kuint               serviceProtection;      //��Ƶ���Ƿ����,0��δ����,����ֵ������[Bool]
        KMC_SERVICE_TYPE     serviceType;   //Ƶ������,����ֵ����� KMC_SERVICE_TYPE
        kchar               *MIMEType;              //Ƶ��ҵ���MIME����;ServiceType = ���ݹ㲥ʱ,������������ҵ�����;Ϊ����ֵʱ,����ֵ��ΪNULL��

        kchar                 *pURL;                    // for flash play

        time_t              startTime;              // ���ڲ��ܻ�ÿ�ʼʱ����������ֵ����Ϊ0
        time_t              endTime;                // ���ڲ��ܻ�ý���ʱ����������ֵ����Ϊ��ʼʱ�䣨0�����Ͻ�Ŀʱ����������ܻ��ʱ������Ϊ-1
    } KMC_SERVICE_BASE_INFO;


    typedef enum _KMC_CHANNEL_ORIGIN_TYPE_
    {
        KMC_CHANNEL_ORIGIN_TYPE_BROADCAST=0,             /*�㲥ҵ��*/
        KMC_CHANNEL_ORIGIN_TYPE_WEB,                     /*˫��ҵ��*/
        KMC_CHANNEL_ORIGIN_TYPE_LOCATION                 /*����ҵ��*/

    } KMC_CHANNEL_ORIGIN_TYPE;

    typedef enum
    {
        KMC_CHANNEL_OPERATION_TYPE_LIVING_BROADCAST=0,      /* ֱ��ҵ�������ǹ㲥����˫��������Ƶ�㲥������Ƶ��Ŀ*/
        KMC_CHANNEL_OPERATION_TYPE_VOD= 1,                    /* �㲥ҵ�������ǹ㲥����˫��*/
        KMC_CHANNEL_OPERATION_TYPE_DOWLOAD=2,                /* ����ҵ�������ǹ㲥����˫������������Ƶ�໹��һ�������ļ�����*/
        KMC_CHANNEL_OPERATION_TYPE_SHARE= 4,                /* ��Ƶ����ҵ�������ǹ㲥����˫��*/
        KMC_CHANNEL_OPERATION_TYPE_RECORD=8                 /* ¼��ҵ��*/
    }KMC_CHANNEL_OPERATION_TYPE;


    typedef struct _KMC_CHANNEL_CATE_INFO_
    {
        kchar *pSortId;
        kchar *pSortName;

    }KMC_CHANNEL_CATE_INFO;


    typedef struct _KMC_CHANNEL_ATTRIBUTE_INFO_
    {

        KMC_CHANNEL_ORIGIN_TYPE                    ivChannelOrigin;    /*��KMC_CHANNEL_ORIGIN_TYPE*/
        KMC_CHANNEL_OPERATION_TYPE              ivChannelType;      /*��KMC_CHANNEL_OPERATION_TYPE*/

        void                                     *pChannelInfo;        /*���ݲ�ͬ��channel���ͣ���������ָ���Ͳ�ͬ
                                                                         ivChannelType=KMC_CHANNEL_TYPE_LIVE_BROADCAST / KMC_CHANNEL_OPERATION_TYPE_Recordedʱ��������Ŀǰδ���壬ΪNULL
                                                                         ivChannelType=KMC_CHANNEL_OPERATION_TYPE_DOWLOAD ʱ��������ΪKMC_CHANNEL_CATE_INFO ָ��
                                                                         ivChannelType=KMC_CHANNEL_TYPE_VOD ʱ��������ΪKMC_CHANNEL_CATE_INFO ָ��
                                                                         ivChannelType=KMC_CHANNEL_OPERATION_TYPE_VIDEO_SHARE ʱ��������ΪKMC_CHANNEL_CATE_INFO ָ��

*/

    }KMC_CHANNEL_ATTRIBUTE_INFO;

    typedef struct _KMC_ChannelInfo_
    {
        KMC_SERVICE_BASE_INFO serviceBaseInfo;      //Service��������
        kchar           *description;           //ָ��Ƶ��������ָ��
        kchar           *parentRating;          //ָ��Ƶ�����ݷ���ȼ�(�ַ���)��ָ��
        kchar           *genre;                 //ָ��Ƶ�����ɵ��ַ���(�ַ���)��ָ��
        kchar           *globalServiceID;       //ָ���Ƶ��ȫ��ΨһID(URI)��ָ��
        kchar            *logo;                    //ָ���Ƶ��logo�ı���URI��Ӧ�ø��ݺ�׺��ʶ���ļ�����

        KMC_CHANNEL_ATTRIBUTE_INFO  ivChannelInfo;        /*��ʱ���ӽṹ����� KMC_CHANNEL_INFO ����*/


    } KMC_ChannelInfo;

    typedef struct _KMC_FILE_DATA_
    {
        void            *data;                  //ָ�����Ϣ���ݵ�ָ��
        kuint           length;                 //���ݵ���Ч����
        kchar           *uri;                   //��dataΪNULLʱ���Դ�uri��ȡ��uri��Ϊ����·����Web���ӡ�
    } KMC_FILE_DATA;


    /* Preview ���� */
    typedef enum _KMC_PREVIEW_TYPE_
    {
        KMC_PREVIEW_TYPE_LOGO_SMALL = 0,            //LOGO��СͼƬ
        KMC_PREVIEW_TYPE_LOGO_LARGE,                //Ƶ����־�Ĵ�ͼƬ
        KMC_PREVIEW_TYPE_SERVICE_PREVIEW,           //Ƶ����ĿԤ����Ϣ,ΪͼƬ�����ı�
        KMC_PREVIEW_TYPE_SERVICE_SWITCH,            //��һ��Ƶ���л�����һ��Ƶ���ĵȴ��ڼ���ʾ��Ԥ����Ϣ,ΪͼƬ�����ı�
        KMC_PREVIEW_TYPE_ALT_BLACKOUT,              //����Ļ���ֺ�����������Ч�����ź�ʱ����ʾ��ͼƬ�����ı�
        KMC_PREVIEW_TYPE_SERVICE_PROMOTION          //Ƶ�����ƹ���Ϣ,ΪͼƬ�����ı�
    } KMC_PREVIEW_TYPE;

    typedef struct _KMC_PreviewInfo_
    {
        KMC_FILE_DATA       file;                    //ֻ�� file.dataΪ��ʱʹ�� uri���ȡ�ļ�����
        kchar           *MIMEType;                  //���ݵ�MIME ����(�ı�ʱΪUTF-8)
    } KMC_PreviewInfo;

    /*
     * ?��??
     */
    typedef struct _KMC_ProgInfo_
    {
        kchar           *name;                      //��Ŀ����
        kchar           *programID;                 //��ĿID
        kuint           startTime;                  //��Ŀ��ʼʱ��(UTC)
        kuint           endTime;                    //��Ŀ����ʱ��(UTC)
        kuint           duration;                   //��Ŀʱ��(��)
        kchar           *parentRating;              //��Ŀ�������ݷ���ȼ�,�ַ���
        kchar           *genre;                     //��Ŀ��������,�ַ���,��'Jazz'
        kuint           contentProtection;          //�����Ƿ����,boolֵ
    } KMC_ProgInfo;



    /*
     * ���Ż�������
     */
    typedef struct _KMC_SMS_CHOICE_
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
    } KMC_SMS_CHOICE;

    typedef struct _KMC_ITV_SMS_TEMPLATE_
    {
        kchar           *text;      //���Ͷ������ݵ��ı�ǰ׺���û����Ͷ��ŵ����ݽ�Ϊtext+��ѡ���sms-body�ı�(smsURI��).
        kint            choiceNum;    //ѡ��ĸ���
        KMC_SMS_CHOICE  choices[1];
    } KMC_ITV_SMS;


    /*
     * Email ��������ģ��
     */
    typedef struct _KMC_ITV_EMAIL_TEMPLATE_
    {
        kchar   *to;                //�����ͷ���ַ
        kchar   *cc;                //���ͷ���ַ
        kchar   *bcc;                //���ͷ���ַ
        kchar   *subject;            //����
        kchar   *body;                //Ԥ������
    } KMC_ITV_EMAIL;

    /*
     * �㲥�ļ�����
     */
    typedef struct _KMC_MEDIA_OBJECT_
    {
        kint        flag;                   //1��ʾ���ļ�Ϊ���(����ҳ),0Ϊ��ͨ�ļ�������ֵԤ����
        kchar       *contentLocation;       //�ļ��������·��,���ܰ���$(SG_SWAP_DIRECTORY)��
        kchar       *contentType;           //MIME����
    } KMC_MEDIA_OBJECT;

    typedef struct _KMC_ITV_MEDIAOBJECTS_
    {
        kint                moNum;            //�����ļ��ĸ���
        KMC_MEDIA_OBJECT    objects[1];            //�ļ��б�
    } KMC_ITV_MEDIAOBJECTS;


    /*
     * ��������
     */
    typedef enum _KMC_INTERACTIVITY_TYPE_
    {
        KMC_INTERACTIVITY_TYPE_SMS = 0,         //��������
        KMC_INTERACTIVITY_TYPE_EMAIL,              //Email
        KMC_INTERACTIVITY_TYPE_VOICECALL,       //�绰
        KMC_INTERACTIVITY_TYPE_WEBLINK,         //��ҳ����
        KMC_INTERACTIVITY_TYPE_MEDIAOBJ         //�ļ��㲥
    } KMC_INTERACTIVITY_TYPE;

    typedef struct _KMC_INTERACTIVITY_MEDIA_
    {
        kuchar          type;                       //����������, KMC_INTERACTIVITY_TYPE
        kchar           *description;               //��������
        kuchar          relativePreference;         //��ͬʱ�ж������ʱ,����������ʾ˳��,������˳����ʾ,��ֵС������ʾ

        union {
            KMC_ITV_SMS                *sms;
            KMC_ITV_EMAIL           email;
            kchar               phoneNumber[20];
            kchar               *webLink;           //��ҳ���ӻ���ʱ,��ҳ���ڵ�URL,Ӧ��ͨ��������ȡ�û�����ҳ
            KMC_ITV_MEDIAOBJECTS    *mediaObjects;
        } contents;

    } KMC_INTERACTIVITY_MEDIA;

    /*
     * ����Ŀ¼
     */
    typedef struct _KMC_ItvInfo_
    {
        kuint               startTime;                //������ʾ��ʼʱ��
        kuint               endTime;                //������ʾ��ֹʱ��
        kint                mediaNumber;            //��������
        KMC_INTERACTIVITY_MEDIA  interactivityMedia[1];    //�����б�
    } KMC_ItvInfo;


    /*
     * ??1o
     */
    typedef enum _KMC_PURCHASE_TYPE_
    {
        KMC_PURCHASE_TYPE_SERVICE,                 //Ƶ��
        KMC_PURCHASE_TYPE_SCHEDULE,                //��Ŀ
        KMC_PURCHASE_TYPE_CONTENT,                //����
        KMC_PURCHASE_TYPE_PURCHASEITEM            //����һ��������

    } KMC_PURCHASE_TYPE;

    typedef struct _KMC_PURCHASE_ITEM_BASE_INFO_
    {
        kchar           *name;                      //�ö����������,UTF-8 �ı�
        kchar           *purchaseItemID;            //�ö������ID,ΪglobalPurchaseItemID
        kint            weight;                     //��ʾ˳��,��ֵ������˳������,ֵ��С����ʾ����ǰ��
        kchar           startTime[20];              //�ɿ�ʼ������ʱ��,��(YYYY:MM:DD:hh:mm:ss)����ʽ��ʾ,������ʾ
        kchar           endtime[20];                //��ֹ������ʱ��,��(YYYY:MM:DD:hh:mm:ss)����ʽ��ʾ,������ʾ
    } KMC_PURCHASE_ITEM_BASE_INFO;


    typedef struct _KMC_PURCHASEITEM_CONTENT_
    {
        kchar           *id;
        kchar           *name;                      //�������ݵ�����,UTF-8 �ı�
        KMC_PURCHASE_TYPE    type;                       //�������ݵ�����,��IV_PURCHASE_TYPE����
    } KMC_PURCHASEITEM_CONTENT;

    typedef struct _KMC_PURCHASE_CHANNEL_
    {
        kchar           *name;                      //�ö���ͨ��������,UTF-8 �ı�
        kchar           *description;               //����ͨ�����,UTF-8 �ı�
        kchar           *contactInfo;               //����ͨ����ϵ��ʽ,UTF-8 �ı�
        kchar           *purchaseURL;               //����ͨ��URL,UTF-8 �ı�
        kchar           *portalURL;                 //����ͨ��Portal URL,UTF-8 �ı�
    } KMC_PURCHASE_CHANNEL;


    typedef enum _KMC_SUBSCRIPTION_TYPE_
    {
        KMC_SUBSCRIPTION_TYPE_SINGLE = 0,               //���ζ���,����һ����SubscriptionPeriodָ����ʱ������Ч,����ΪPriceָ������Ϣ
        KMC_SUBSCRIPTION_TYPE_CONTINUOUS                //��������,����һ�ν�������Ч,ֱ���û�ȡ������Ϊֹ,��ÿ��SubscriptionPeriodָ����ʱ��,����ΪPriceָ������Ϣ
    } KMC_SUBSCRIPTION_TYPE;


    typedef struct _KMC_PURCHASE_DATA_                  //����������
    {
        kchar           *description;               //������������,UTF-8 �ı�
        kuint           subscriptionType;           //��������,����ֵ��IV_SUBSCRIPTION_TYPE
        kchar           price[8];                   //�۸�(���ػ��ҵ�λ),С�������λ������,��ASCII����ʽ��ʾ,�确00010.00'����10.00
        kchar           currency[4];                //��������,���չ��ʱ�׼���ҵ�λ���ַ���,�确CNY��Ϊ�����
        kchar           subscriptionPeriod[4];      //��������,UTF-8 �ı�, ��:P1Y=1��,P2M= 2��,P1H= 1Сʱ
        kint            channelNum;                 //�ö����������Ӧ�Ĺ���ͨ������
        KMC_PURCHASE_CHANNEL    *channelList;           //�ö����������Ӧ�Ĺ���ͨ�����б�
    } KMC_PURCHASE_DATA;


    typedef struct _KMC_PURCHASE_ITEM_DETAIL_INFO_
    {
        KMC_PURCHASE_ITEM_BASE_INFO purchaseItemBaseInfo;    //�������������
        kchar               *description;               //��������,UTF-8 �ı�
        kchar               *parentalRating;            //���ݷ���ȼ�,�ַ���
        kint                contentNum;                 //�ö������������������
        KMC_PURCHASEITEM_CONTENT    *content;               //�ö���������������б�
        kint                purchaseDataNum;            //�ö������Ӧ�Ķ�������������
        KMC_PURCHASE_DATA       *purchaseData;              //�ö������Ӧ�Ķ����������б�
    } KMC_PURCHASE_ITEM_DETAIL_INFO;


    /*
     * SG Provider��Ϣ
     */
    typedef struct _KMC_SG_PROVIDER_
    {
        kuint           id;                                //SG�ṩ�̱�ʶ
        kchar           *name;                            //SG�ṩ������
        kchar           *uri;                            //SG�ṩ��ΨһID
        kchar           *infoUrl;                        //SG�ṩ��web��ַ
        kchar           *auxData;                        //��������

        KMC_PreviewInfo    logo;                            //SG�ṩ��logo����
    } KMC_SG_PROVIDER;


#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* __KMC_DEFINES_H__ */

