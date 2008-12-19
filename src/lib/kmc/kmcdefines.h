#ifndef  __KMC_DEFINES_H__
#define  __KMC_DEFINES_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <ktypes.h>
#include <time.h>
    /*
     * 频道
     */
    typedef enum _KMC_SERVICE_TYPE_
    {

        KMC_SERVICE_TYPE_UNSPECIFIED = 0,               // 0 – 未规定
        KMC_SERVICE_TYPE_BASICTV,                       //1 – 基本电视节目
        KMC_SERVICE_TYPE_BASICAUDIO,                    //2 – 基本音频广播
        KMC_SERVICE_TYPE_RFU1,                          //3 – 保留
        KMC_SERVICE_TYPE_CACHEDCAST,                    //4 - 缓冲用广播数据
        KMC_SERVICE_TYPE_FILEDOWNLOAD,                  //5 – 数据广播
        KMC_SERVICE_TYPE_SOFTWAREMGMT,                  //6 – 软件管理
        KMC_SERVICE_TYPE_NOTIFICATION,                  //7 – 通告
        KMC_SERVICE_TYPE_SERVICEGUIDE,                  //8 – 业务指南
        KMC_SERVICE_TYPE_TERMINALPROV                   //9 – 终端配置
            //10 - 127  保留
            //128 -255 保留为私有
    } KMC_SERVICE_TYPE;

    typedef struct _KMC_SERVICE_BASE_INFO_
    {
        kchar               *serviceID;             //指向频道ID(URI)的指针
        kchar               *name;                  //指向频道名的指针, UTF-8
        kuint               weight;                 //该频道在频道列表中的显示位置,按值递增的顺序排列,值最小的显示在最前面
        kuint               serviceProtection;      //该频道是否加密,0：未加密,其他值：加密[Bool]
        KMC_SERVICE_TYPE     serviceType;   //频道类型,具体值定义见 KMC_SERVICE_TYPE
        kchar               *MIMEType;              //频道业务的MIME类型;ServiceType = 数据广播时,该项表明具体的业务类别;为其他值时,此项值可为NULL。

        kchar                 *pURL;                    // for flash play

        time_t              startTime;              // 对于不能获得开始时间的情况，该值设置为0
        time_t              endTime;                // 对于不能获得结束时间的情况，该值设置为开始时间（0）加上节目时长，如果不能获得时长，置为-1
    } KMC_SERVICE_BASE_INFO;


    typedef enum _KMC_CHANNEL_ORIGIN_TYPE_
    {
        KMC_CHANNEL_ORIGIN_TYPE_BROADCAST=0,             /*广播业务*/
        KMC_CHANNEL_ORIGIN_TYPE_WEB,                     /*双向业务*/
        KMC_CHANNEL_ORIGIN_TYPE_LOCATION                 /*本地业务*/

    } KMC_CHANNEL_ORIGIN_TYPE;

    typedef enum
    {
        KMC_CHANNEL_OPERATION_TYPE_LIVING_BROADCAST=0,      /* 直播业务，无论是广播还是双向，无论音频广播还是视频节目*/
        KMC_CHANNEL_OPERATION_TYPE_VOD= 1,                    /* 点播业务，无论是广播还是双向*/
        KMC_CHANNEL_OPERATION_TYPE_DOWLOAD=2,                /* 下载业务，无论是广播还是双向，无论是音视频类还是一般数据文件下载*/
        KMC_CHANNEL_OPERATION_TYPE_SHARE= 4,                /* 视频分享业务，无论是广播还是双向*/
        KMC_CHANNEL_OPERATION_TYPE_RECORD=8                 /* 录制业务*/
    }KMC_CHANNEL_OPERATION_TYPE;


    typedef struct _KMC_CHANNEL_CATE_INFO_
    {
        kchar *pSortId;
        kchar *pSortName;

    }KMC_CHANNEL_CATE_INFO;


    typedef struct _KMC_CHANNEL_ATTRIBUTE_INFO_
    {

        KMC_CHANNEL_ORIGIN_TYPE                    ivChannelOrigin;    /*见KMC_CHANNEL_ORIGIN_TYPE*/
        KMC_CHANNEL_OPERATION_TYPE              ivChannelType;      /*见KMC_CHANNEL_OPERATION_TYPE*/

        void                                     *pChannelInfo;        /*根据不同的channel类型，该类型所指类型不同
                                                                         ivChannelType=KMC_CHANNEL_TYPE_LIVE_BROADCAST / KMC_CHANNEL_OPERATION_TYPE_Recorded时，该类型目前未定义，为NULL
                                                                         ivChannelType=KMC_CHANNEL_OPERATION_TYPE_DOWLOAD 时，该类型为KMC_CHANNEL_CATE_INFO 指针
                                                                         ivChannelType=KMC_CHANNEL_TYPE_VOD 时，该类型为KMC_CHANNEL_CATE_INFO 指针
                                                                         ivChannelType=KMC_CHANNEL_OPERATION_TYPE_VIDEO_SHARE 时，该类型为KMC_CHANNEL_CATE_INFO 指针

*/

    }KMC_CHANNEL_ATTRIBUTE_INFO;

    typedef struct _KMC_ChannelInfo_
    {
        KMC_SERVICE_BASE_INFO serviceBaseInfo;      //Service基本数据
        kchar           *description;           //指向频道描述的指针
        kchar           *parentRating;          //指向频道内容分类等级(字符串)的指针
        kchar           *genre;                 //指向频道流派的字符串(字符串)的指针
        kchar           *globalServiceID;       //指向该频道全球唯一ID(URI)的指针
        kchar            *logo;                    //指向该频道logo的本地URI，应用根据后缀来识别文件类型

        KMC_CHANNEL_ATTRIBUTE_INFO  ivChannelInfo;        /*临时增加结构，详见 KMC_CHANNEL_INFO 定义*/


    } KMC_ChannelInfo;

    typedef struct _KMC_FILE_DATA_
    {
        void            *data;                  //指向该信息数据的指针
        kuint           length;                 //数据的有效长度
        kchar           *uri;                   //当data为NULL时可以从uri获取。uri可为本地路径或Web链接。
    } KMC_FILE_DATA;


    /* Preview 数据 */
    typedef enum _KMC_PREVIEW_TYPE_
    {
        KMC_PREVIEW_TYPE_LOGO_SMALL = 0,            //LOGO的小图片
        KMC_PREVIEW_TYPE_LOGO_LARGE,                //频道标志的大图片
        KMC_PREVIEW_TYPE_SERVICE_PREVIEW,           //频道节目预览信息,为图片或者文本
        KMC_PREVIEW_TYPE_SERVICE_SWITCH,            //从一个频道切换到另一个频道的等待期间显示的预览信息,为图片或者文本
        KMC_PREVIEW_TYPE_ALT_BLACKOUT,              //当屏幕出现黑屏（如无有效内容信号时）显示用图片或者文本
        KMC_PREVIEW_TYPE_SERVICE_PROMOTION          //频道的推广信息,为图片或者文本
    } KMC_PREVIEW_TYPE;

    typedef struct _KMC_PreviewInfo_
    {
        KMC_FILE_DATA       file;                    //只有 file.data为空时使用 uri项获取文件数据
        kchar           *MIMEType;                  //数据的MIME 类型(文本时为UTF-8)
    } KMC_PreviewInfo;

    /*
     * ?ú??
     */
    typedef struct _KMC_ProgInfo_
    {
        kchar           *name;                      //节目名称
        kchar           *programID;                 //节目ID
        kuint           startTime;                  //节目开始时间(UTC)
        kuint           endTime;                    //节目结束时间(UTC)
        kuint           duration;                   //节目时长(秒)
        kchar           *parentRating;              //节目所属内容分类等级,字符串
        kchar           *genre;                     //节目所属流派,字符串,如'Jazz'
        kuint           contentProtection;          //内容是否加密,bool值
    } KMC_ProgInfo;



    /*
     * 短信互动内容
     */
    typedef struct _KMC_SMS_CHOICE_
    {
        kchar       *smsURI;        //sms: URIs, 可以包含宏定义 $userid,$deviceid,$userinputs等,语法如下
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
        kchar       *choiceText;    //用于选择项的显示,语言与全局语言设定有关。
    } KMC_SMS_CHOICE;

    typedef struct _KMC_ITV_SMS_TEMPLATE_
    {
        kchar           *text;      //发送短信内容的文本前缀。用户发送短信的内容将为text+所选项的sms-body文本(smsURI中).
        kint            choiceNum;    //选项的个数
        KMC_SMS_CHOICE  choices[1];
    } KMC_ITV_SMS;


    /*
     * Email 互动内容模板
     */
    typedef struct _KMC_ITV_EMAIL_TEMPLATE_
    {
        kchar   *to;                //主发送方地址
        kchar   *cc;                //抄送方地址
        kchar   *bcc;                //抄送方地址
        kchar   *subject;            //标题
        kchar   *body;                //预置内容
    } KMC_ITV_EMAIL;

    /*
     * 广播文件互动
     */
    typedef struct _KMC_MEDIA_OBJECT_
    {
        kint        flag;                   //1表示该文件为入口(如首页),0为普通文件。其他值预留。
        kchar       *contentLocation;       //文件所在相对路径,可能包含$(SG_SWAP_DIRECTORY)宏
        kchar       *contentType;           //MIME类型
    } KMC_MEDIA_OBJECT;

    typedef struct _KMC_ITV_MEDIAOBJECTS_
    {
        kint                moNum;            //包含文件的个数
        KMC_MEDIA_OBJECT    objects[1];            //文件列表
    } KMC_ITV_MEDIAOBJECTS;


    /*
     * 互动描述
     */
    typedef enum _KMC_INTERACTIVITY_TYPE_
    {
        KMC_INTERACTIVITY_TYPE_SMS = 0,         //短信类型
        KMC_INTERACTIVITY_TYPE_EMAIL,              //Email
        KMC_INTERACTIVITY_TYPE_VOICECALL,       //电话
        KMC_INTERACTIVITY_TYPE_WEBLINK,         //网页链接
        KMC_INTERACTIVITY_TYPE_MEDIAOBJ         //文件广播
    } KMC_INTERACTIVITY_TYPE;

    typedef struct _KMC_INTERACTIVITY_MEDIA_
    {
        kuchar          type;                       //互动的类型, KMC_INTERACTIVITY_TYPE
        kchar           *description;               //互动描述
        kuchar          relativePreference;         //当同时有多个互动时,各互动的显示顺序,按递增顺序显示,即值小的先显示

        union {
            KMC_ITV_SMS                *sms;
            KMC_ITV_EMAIL           email;
            kchar               phoneNumber[20];
            kchar               *webLink;           //网页连接互动时,网页所在的URL,应该通过该连接取得互动网页
            KMC_ITV_MEDIAOBJECTS    *mediaObjects;
        } contents;

    } KMC_INTERACTIVITY_MEDIA;

    /*
     * 互动目录
     */
    typedef struct _KMC_ItvInfo_
    {
        kuint               startTime;                //互动显示开始时间
        kuint               endTime;                //互动显示终止时间
        kint                mediaNumber;            //互动个数
        KMC_INTERACTIVITY_MEDIA  interactivityMedia[1];    //互动列表
    } KMC_ItvInfo;


    /*
     * ??1o
     */
    typedef enum _KMC_PURCHASE_TYPE_
    {
        KMC_PURCHASE_TYPE_SERVICE,                 //频道
        KMC_PURCHASE_TYPE_SCHEDULE,                //节目
        KMC_PURCHASE_TYPE_CONTENT,                //内容
        KMC_PURCHASE_TYPE_PURCHASEITEM            //另外一个购买项

    } KMC_PURCHASE_TYPE;

    typedef struct _KMC_PURCHASE_ITEM_BASE_INFO_
    {
        kchar           *name;                      //该订购项的名称,UTF-8 文本
        kchar           *purchaseItemID;            //该订购项的ID,为globalPurchaseItemID
        kint            weight;                     //显示顺序,按值递增的顺序排列,值最小的显示在最前面
        kchar           startTime[20];              //可开始订购的时间,以(YYYY:MM:DD:hh:mm:ss)的形式表示,用于显示
        kchar           endtime[20];                //终止订购的时间,以(YYYY:MM:DD:hh:mm:ss)的形式表示,用于显示
    } KMC_PURCHASE_ITEM_BASE_INFO;


    typedef struct _KMC_PURCHASEITEM_CONTENT_
    {
        kchar           *id;
        kchar           *name;                      //该项内容的名称,UTF-8 文本
        KMC_PURCHASE_TYPE    type;                       //该项内容的类型,见IV_PURCHASE_TYPE定义
    } KMC_PURCHASEITEM_CONTENT;

    typedef struct _KMC_PURCHASE_CHANNEL_
    {
        kchar           *name;                      //该订购通道的名称,UTF-8 文本
        kchar           *description;               //订购通道简介,UTF-8 文本
        kchar           *contactInfo;               //订购通道联系方式,UTF-8 文本
        kchar           *purchaseURL;               //订购通道URL,UTF-8 文本
        kchar           *portalURL;                 //订购通道Portal URL,UTF-8 文本
    } KMC_PURCHASE_CHANNEL;


    typedef enum _KMC_SUBSCRIPTION_TYPE_
    {
        KMC_SUBSCRIPTION_TYPE_SINGLE = 0,               //单次订购,订购一次在SubscriptionPeriod指定的时间内有效,费用为Price指定的信息
        KMC_SUBSCRIPTION_TYPE_CONTINUOUS                //连续订购,订购一次将持续有效,直到用户取消订购为止,在每个SubscriptionPeriod指定的时间,费用为Price指定的信息
    } KMC_SUBSCRIPTION_TYPE;


    typedef struct _KMC_PURCHASE_DATA_                  //订购数据项
    {
        kchar           *description;               //订购数据项简介,UTF-8 文本
        kuint           subscriptionType;           //订购类型,具体值见IV_SUBSCRIPTION_TYPE
        kchar           price[8];                   //价格(本地货币单位),小数点后两位的数字,以ASCII码形式表示,如‘00010.00'代表10.00
        kchar           currency[4];                //货币种类,按照国际标准货币单位的字符串,如‘CNY’为人民币
        kchar           subscriptionPeriod[4];      //订购周期,UTF-8 文本, 如:P1Y=1年,P2M= 2月,P1H= 1小时
        kint            channelNum;                 //该订购数据项对应的购买通道数量
        KMC_PURCHASE_CHANNEL    *channelList;           //该订购数据项对应的购买通道项列表
    } KMC_PURCHASE_DATA;


    typedef struct _KMC_PURCHASE_ITEM_DETAIL_INFO_
    {
        KMC_PURCHASE_ITEM_BASE_INFO purchaseItemBaseInfo;    //订购项基本数据
        kchar               *description;               //订购项简介,UTF-8 文本
        kchar               *parentalRating;            //内容分类等级,字符串
        kint                contentNum;                 //该订购项包含的内容数量
        KMC_PURCHASEITEM_CONTENT    *content;               //该订购项包含的内容列表
        kint                purchaseDataNum;            //该订购项对应的订购数据项数量
        KMC_PURCHASE_DATA       *purchaseData;              //该订购项对应的订购数据项列表
    } KMC_PURCHASE_ITEM_DETAIL_INFO;


    /*
     * SG Provider信息
     */
    typedef struct _KMC_SG_PROVIDER_
    {
        kuint           id;                                //SG提供商标识
        kchar           *name;                            //SG提供商名字
        kchar           *uri;                            //SG提供商唯一ID
        kchar           *infoUrl;                        //SG提供商web地址
        kchar           *auxData;                        //附加数据

        KMC_PreviewInfo    logo;                            //SG提供商logo数据
    } KMC_SG_PROVIDER;


#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* __KMC_DEFINES_H__ */

