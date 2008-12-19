/*******************************************************************************************************/
/*                  iVisa ESG engine用户层API相关定义                                                  */
/*     Ver: 1.30                                                                                       */
/*     Date: 2008.03.18                                                                                */
/*******************************************************************************************************/



/*******************************************************************************************************/
/*                                  数据结构定义                                                       */
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
 * 频道
 */
typedef enum _IV_SERVICE_TYPE_
{

    IV_SERVICE_TYPE_UNSPECIFIED = 0,               // 0 – 未规定
    IV_SERVICE_TYPE_BASICTV,                       //1 – 基本电视节目
    IV_SERVICE_TYPE_BASICAUDIO,                    //2 – 基本音频广播
    IV_SERVICE_TYPE_RFU1,                          //3 – 保留
    IV_SERVICE_TYPE_CACHEDCAST,                    //4 - 缓冲用广播数据
    IV_SERVICE_TYPE_FILEDOWNLOAD,                  //5 – 数据广播
    IV_SERVICE_TYPE_SOFTWAREMGMT,                  //6 – 软件管理
    IV_SERVICE_TYPE_NOTIFICATION,                  //7 – 通告
    IV_SERVICE_TYPE_SERVICEGUIDE,                  //8 – 业务指南
    IV_SERVICE_TYPE_TERMINALPROV                   //9 – 终端配置
                                                //10 - 127  保留
                                                //128 -255 保留为私有
} IV_SERVICE_TYPE;

typedef struct _IV_SERVICE_BASE_INFO_
{
    kchar               *serviceID;             //指向频道ID(URI)的指针
    kchar               *name;                  //指向频道名的指针, UTF-8
    kuint               weight;                 //该频道在频道列表中的显示位置,按值递增的顺序排列,值最小的显示在最前面
    kuint               serviceProtection;      //该频道是否加密,0：未加密,其他值：加密[Bool]
    IV_SERVICE_TYPE     serviceType;   //频道类型,具体值定义见 IV_SERVICE_TYPE
    kchar               *MIMEType;              //频道业务的MIME类型;ServiceType = 数据广播时,该项表明具体的业务类别;为其他值时,此项值可为NULL。
} IV_SERVICE_BASE_INFO;






typedef enum _IV_MID_CHANNEL_ORIGIN_TYPE_
{
    IV_MID_CHANNEL_ORIGIN_TYPE_BROADCAST=0,             /*广播业务*/
    IV_MID_CHANNEL_ORIGIN_TYPE_WEB,                     /*双向业务*/
    IV_MID_CHANNEL_ORIGIN_TYPE_LOCATION                 /*本地业务*/

} IV_MID_CHANNEL_ORIGIN_TYPE;

typedef enum
{
	IV_MID_CHANNEL_TYPE_LIVE_BROADCAST=0,  	/*直播业务，无论是广播还是双向，无论音频广播还是视频节目*/
	IV_MID_CHANNEL_TYPE_VOD,				/* 点播业务，无论是广播还是双向*/
	IV_MID_CHANNEL_TYPE_DOWLOAD			/* 下载业务，无论是广播还是双向，无论是音视频类还是一般数据文件下载*/

}IV_MID_CHANNEL_TYPE;

typedef struct _IV_MID_CHANNEL_INFO_
{

	IV_MID_CHANNEL_ORIGIN_TYPE		ivChannelOrigin;	/*见IV_MID_CHANNEL_ORIGIN_TYPE*/
	IV_MID_CHANNEL_TYPE  			ivChannelType;      /*见IV_MID_CHANNEL_TYPE*/

	void 				*pChannelInfo;		/*根据不同的channel类型，该类型所指类型不同
												ivChannelType=IV_MID_CHANNEL_TYPE_LIVE_BROADCAST 时，该类型目前未定义，为NULL
												ivChannelType=IV_MID_CHANNEL_TYPE_VOD 时，该类型为char *,标识点播业务的分类ID,
																						即等同为SM_VodCateInfo结构中vodSortID值
																						(参见streaming_media.h中的定义)
												ivChannelType=IV_MID_CHANNEL_TYPE_DOWLOAD 时，该类型为char *，标识下载业务的分类ID,
																						即等同为SM_DownCateInfo结构中downSortID值
																						(参见streaming_media.h中的定义)

											*/

}IV_MID_CHANNEL_INFO;




typedef struct _IV_SERVICE_DETAIL_INFO_
{
    IV_SERVICE_BASE_INFO serviceBaseInfo;      //Service基本数据
    kchar           *description;           //指向频道描述的指针
    kchar           *parentRating;          //指向频道内容分类等级(字符串)的指针
    kchar           *genre;                 //指向频道流派的字符串(字符串)的指针
    kchar           *globalServiceID;       //指向该频道全球唯一ID(URI)的指针
} IV_SERVICE_DETAIL_INFO;

typedef struct _IV_FILE_DATA_
{
    void            *data;                  //指向该信息数据的指针
    kuint           length;                 //数据的有效长度
    kchar           *uri;                   //当data为NULL时可以从uri获取。uri可为本地路径或Web链接。
} IV_FILE_DATA;


/* Preview 数据 */
typedef enum _IV_PREVIEW_TYPE_
{
    IV_PREVIEW_TYPE_LOGO_SMALL = 0,            //LOGO的小图片
    IV_PREVIEW_TYPE_LOGO_LARGE,                //频道标志的大图片
    IV_PREVIEW_TYPE_SERVICE_PREVIEW,           //频道节目预览信息,为图片或者文本
    IV_PREVIEW_TYPE_SERVICE_SWITCH,            //从一个频道切换到另一个频道的等待期间显示的预览信息,为图片或者文本
    IV_PREVIEW_TYPE_ALT_BLACKOUT,              //当屏幕出现黑屏（如无有效内容信号时）显示用图片或者文本
    IV_PREVIEW_TYPE_SERVICE_PROMOTION          //频道的推广信息,为图片或者文本
} IV_PREVIEW_TYPE;

typedef struct _IV_PREVIEW_INFO_
{
    IV_FILE_DATA       file;                    //只有 file.data为空时使用 uri项获取文件数据
    kchar           *MIMEType;                  //数据的MIME 类型(文本时为UTF-8)
} IV_PREVIEW_INFO;



/*
 * 内容获取参数(Access)
 */

 typedef enum _IV_ACCESS_TYPE_
{
    IV_ACCESS_TYPE_IP = 0,                      //0 – 基于IP的广播系统，Acess的类型
    IV_ACCESS_TYPE_DAB,                         //1 – 基本DAB（音视频业务非IP）的广播系统，Acess的类型
    IV_ACCESS_TYPE_DVB,							//2 – 基于DVB(音视频业务非IP) 的广播系统
	IV_ACCESS_TYPE_WEB							//3 – 基于WEB的音视频业务(通过URL获取业务)
} IV_ACCESS_TYPE;

 typedef union _IV_SESSION_PARAMS_{
	 IV_FILE_DATA         sdp;               //buf.data为NULL时使用uri获取SDP。用于IPDC or DAB-IP.
	 IV_DAB_SC_ENTRY   dabAccess;   			//当广播体制为DAB时使用。
	 IV_DVB_SC_ENTRY   dvbAccess;   			//当广播体制为DVB时使用。
 } IV_SESSION_PARAMS;

typedef struct _IV_ACCESS_INFO_
{
    kuint           	frequency;              //该Service所在的频点值,以Hz为单位
    kchar               *usage;                 //该Access对应会话的描述,UTF-8字符串,当有多个会话存在时,应将此项显示给用户供用户选择
    kint                defaultAccessFlag;      //该Acess是否为缺省,默认显示缺省Access指向的信息 。其他通过菜单供用户选择
    kuint               validFrom;              //有效期开始时间 UTC.
    kuint               validTo;                //有效期终止时间 UTC
    IV_ACCESS_TYPE      accessType;             // accessType＝IV_ACCESS_TYPE_IP，sessionParams的类型为IV_FILE_DATA
                                                // accessType＝IV_ACCESS_TYPE_DAB，sessionParams的类型为IV_DAB_SC_ENTRY
                                                // accessType＝IV_ACCESS_TYPE_DVB，sessionParams的类型为IV_DVB_SC_ENTRY
                                                // accessType＝IV_ACCESS_TYPE_WEB，sessionParams的类型为char *url

    kuint				scNum;					//该access中业务分量的个数，当为SDP时，此值应置为1， 当为IV_DAB_SC_ENTRY时，应为该
                                                // 业务实际所用IV_DAB_SC_ENTRY的个数。
    union {
        IV_FILE_DATA         sdp;               //buf.data为NULL时使用uri获取SDP。用于IPDC or DAB-IP.
        IV_DAB_SC_ENTRY   dabAccess;   			//当广播体制为DAB时使用。
        IV_DVB_SC_ENTRY   dvbAccess;   			//当广播体制为DVB时使用。
		kchar				*url;				//当音视频业务通过WEB时
    } sessionParams[1];

} IV_ACCESS_INFO;


/*
 * 节目
 */
typedef struct _IV_PROGRAM_BASE_INFO_
{
    kchar           *name;                      //节目名称
    kchar           *programID;                 //节目ID
    kuint           startTime;                  //节目开始时间(UTC)
    kuint           endTime;                    //节目结束时间(UTC)
    kuint           duration;                   //节目时长(秒)
    kchar           *parentRating;              //节目所属内容分类等级,字符串
    kchar           *genre;                     //节目所属流派,字符串,如'Jazz'
    kuint           contentProtection;          //内容是否加密,bool值
} IV_PROGRAM_BASE_INFO;



/*
 * 短信互动内容
 */
typedef struct _IV_SMS_CHOICE_
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
} IV_SMS_CHOICE;

typedef struct _IV_ITV_SMS_TEMPLATE_
{
    kchar           *text;      //发送短信内容的文本前缀。用户发送短信的内容将为text+所选项的sms-body文本(smsURI中).
    kint            choiceNum;	//选项的个数
    IV_SMS_CHOICE  choices[1];
} IV_ITV_SMS;


/*
 * Email 互动内容模板
 */
typedef struct _IV_ITV_EMAIL_TEMPLATE_
{
    kchar   *to;				//主发送方地址
    kchar   *cc;				//抄送方地址
    kchar   *bcc;				//抄送方地址
    kchar   *subject;			//标题
    kchar   *body;				//预置内容
} IV_ITV_EMAIL;

/*
 * 广播文件互动
 */
typedef struct _IV_MEDIA_OBJECT_
{
    kint        flag;                   //1表示该文件为入口(如首页),0为普通文件。其他值预留。
    kchar       *contentLocation;       //文件所在相对路径,可能包含$(SG_SWAP_DIRECTORY)宏
    kchar       *contentType;           //MIME类型
} IV_MEDIA_OBJECT;

typedef struct _IV_ITV_MEDIAOBJECTS_
{
    kint                moNum;			//包含文件的个数
    IV_MEDIA_OBJECT    objects[1];			//文件列表
} IV_ITV_MEDIAOBJECTS;


/*
 * 互动描述
 */
typedef enum _IV_INTERACTIVITY_TYPE_
{
    IV_INTERACTIVITY_TYPE_SMS = 0,     	//短信类型
    IV_INTERACTIVITY_TYPE_EMAIL,      		//Email
    IV_INTERACTIVITY_TYPE_VOICECALL,   	//电话
    IV_INTERACTIVITY_TYPE_WEBLINK,     	//网页链接
    IV_INTERACTIVITY_TYPE_MEDIAOBJ     	//文件广播
} IV_INTERACTIVITY_TYPE;

typedef struct _IV_INTERACTIVITY_MEDIA_
{
    kuchar          type;                       //互动的类型, IV_INTERACTIVITY_TYPE
    kchar           *description;               //互动描述
    kuchar          relativePreference;         //当同时有多个互动时,各互动的显示顺序,按递增顺序显示,即值小的先显示

    union {
        IV_ITV_SMS    			*sms;
        IV_ITV_EMAIL           email;
        kchar               phoneNumber[20];
        kchar               *webLink;           //网页连接互动时,网页所在的URL,应该通过该连接取得互动网页
        IV_ITV_MEDIAOBJECTS    *mediaObjects;
    } contents;

} IV_INTERACTIVITY_MEDIA;

/*
 * 互动目录
 */
typedef struct _IV_INTERACTIVITY_DIRECTORY_
{
    kuint               startTime;				//互动显示开始时间
    kuint               endTime;				//互动显示终止时间
    kint                mediaNumber;			//互动个数
    IV_INTERACTIVITY_MEDIA  interactivityMedia[1];	//互动列表
} IV_INTERACTIVITY_DIRECTORY;

/*
 * 紧急信息
 */
typedef struct _IV_NOTIFICATION_INFO_
{
    kchar       *nid;       	        //该信息的唯一ID,字符串
    kchar       emergencyType;          //紧急事件的类型
    kchar       emergencyLevel;         //紧急事件的级别
    kchar       *publisherName;         //发行机构名称（最大255Byte）
    kuint       validTo;                //该消息有效期的截止时间(UTC)
    kchar       *title;                 //紧急消息标题
    kchar       *content;               //紧急消息内容
    kchar       *targetArea;            //该紧急消息针对的目标区域，所为空表示针对所有区域。
                                        //区域标示符由网络运营商统一分配
    IV_ACCESS_INFO extDataAccess;       //附加数据通道信息
} IV_NOTIFICATION_INFO;


/*
 * 订购
 */
typedef enum _IV_PURCHASE_TYPE_
{
    IV_PURCHASE_TYPE_SERVICE,             	//频道
    IV_PURCHASE_TYPE_SCHEDULE,            	//节目
    IV_PURCHASE_TYPE_CONTENT,            	//内容
    IV_PURCHASE_TYPE_PURCHASEITEM			//另外一个购买项

} IV_PURCHASE_TYPE;

typedef struct _IV_PURCHASE_ITEM_BASE_INFO_
{
    kchar           *name;                      //该订购项的名称,UTF-8 文本
    kchar           *purchaseItemID;            //该订购项的ID,为globalPurchaseItemID
    kint            weight;                     //显示顺序,按值递增的顺序排列,值最小的显示在最前面
    kchar           startTime[20];              //可开始订购的时间,以(YYYY:MM:DD:hh:mm:ss)的形式表示,用于显示
    kchar           endtime[20];                //终止订购的时间,以(YYYY:MM:DD:hh:mm:ss)的形式表示,用于显示
} IV_PURCHASE_ITEM_BASE_INFO;


typedef struct _IV_PURCHASEITEM_CONTENT_
{
    kchar           *id;
    kchar           *name;                      //该项内容的名称,UTF-8 文本
    IV_PURCHASE_TYPE	type;                       //该项内容的类型,见IV_PURCHASE_TYPE定义
} IV_PURCHASEITEM_CONTENT;

typedef struct _IV_PURCHASE_CHANNEL_
{
    kchar           *name;                      //该订购通道的名称,UTF-8 文本
    kchar           *description;               //订购通道简介,UTF-8 文本
    kchar           *contactInfo;               //订购通道联系方式,UTF-8 文本
    kchar           *purchaseURL;               //订购通道URL,UTF-8 文本
    kchar           *portalURL;                 //订购通道Portal URL,UTF-8 文本
} IV_PURCHASE_CHANNEL;


typedef enum _IV_SUBSCRIPTION_TYPE
{
    IV_SUBSCRIPTION_TYPE_SINGLE = 0,               //单次订购,订购一次在SubscriptionPeriod指定的时间内有效,费用为Price指定的信息
    IV_SUBSCRIPTION_TYPE_CONTINUOUS                //连续订购,订购一次将持续有效,直到用户取消订购为止,在每个SubscriptionPeriod指定的时间,费用为Price指定的信息
} IV_SUBSCRIPTION_TYPE;


typedef struct _IV_PURCHASE_DATA_                  //订购数据项
{
    kchar           *description;               //订购数据项简介,UTF-8 文本
    kuint           subscriptionType;           //订购类型,具体值见IV_SUBSCRIPTION_TYPE
    kchar           price[8];                   //价格(本地货币单位),小数点后两位的数字,以ASCII码形式表示,如‘00010.00'代表10.00
    kchar           currency[4];                //货币种类,按照国际标准货币单位的字符串,如‘CNY’为人民币
    kchar           subscriptionPeriod[4];      //订购周期,UTF-8 文本, 如:P1Y=1年,P2M= 2月,P1H= 1小时
    kint            channelNum;                 //该订购数据项对应的购买通道数量
    IV_PURCHASE_CHANNEL    *channelList;           //该订购数据项对应的购买通道项列表
} IV_PURCHASE_DATA;


typedef struct _IV_PURCHASE_ITEM_DETAIL_INFO_
{
	IV_PURCHASE_ITEM_BASE_INFO purchaseItemBaseInfo;	//订购项基本数据
    kchar               *description;               //订购项简介,UTF-8 文本
    kchar               *parentalRating;            //内容分类等级,字符串
    kint                contentNum;                 //该订购项包含的内容数量
    IV_PURCHASEITEM_CONTENT    *content;               //该订购项包含的内容列表
    kint                purchaseDataNum;            //该订购项对应的订购数据项数量
    IV_PURCHASE_DATA       *purchaseData;              //该订购项对应的订购数据项列表
} IV_PURCHASE_ITEM_DETAIL_INFO;


/*
 * SG Provider信息
 */
typedef struct _IV_SG_PROVIDER_
{
    kuint           id;								//SG提供商标识
    kchar           *name;							//SG提供商名字
    kchar           *uri;							//SG提供商唯一ID
    kchar           *infoUrl;						//SG提供商web地址
    kchar           *auxData;						//附加数据
    IV_PREVIEW_INFO    logo;							//SG提供商logo数据
} IV_SG_PROVIDER;

typedef enum _IV_SG_CALLBACK_TYPE
{
    IV_SG_CALLBACK_SCAN = 0,    					// 扫描结果更新
    IV_SG_CALLBACK_INTERACTIVITY, 					// 有新的互动
    IV_SG_CALLBACK_NOTIFICATION,                    // 有新的消息
    IV_SG_CALLBACK_SERVICE_UPDATE,  				// 频道信息更新
    IV_SG_CALLBACK_PROGRAM_UPDATE,  				// 节目单信息更新
    IV_SG_CALLBACK_ACCESS_UPDATE,					// access信息更新
    IV_SG_CALLBACK_PURCHASE_UPDATE,					// 订购信息更新
} IV_SG_CALLBACK_TYPE;



typedef kint (*IVISA_SG_CALLBACK)(IV_SG_CALLBACK_TYPE infoType, void *para1, void *para2,void *para3, void *para4);
/*****************************************************************************************************
    功能：SG事件处理回调函数，由用户层完成
    参数：
    infoType: 已更新信息的类型，见’ivisa_sg_user_api.h’中IV_SG_CALLBACK_TYPE定义。
    para1~para4： 该类型所用到的参数(注： 应用层应该根据需要进行类型转换)。

        1）infoType == IV_SG_CALLBACK_SCAN
            当应用层调用ivisa_sg_StartScan后，会发生此回调，只有当状态为更新完成/超时退出/失败时，或者调用ivisa_SG_StopScan()返回值为成功时才能启动下一次扫描 ivisa_SG_StartScan();

            para1:
            SG扫描结果更新状态，其定义如下：
            		para1== IVEC_INPROGRESS：更新中
            		para1== IVEC_SUCCESS：更新完成
            		para1== IVEC_TIMEOUT：超时退出
            		para1== IVEC_FAILURE: 由于其他原因导致更新失败
            		其他值，保留
            para2: 	当前已收到文件个数，可作为当前频点SG更新状态显示用原始数据
            para3：	当前已发现需要收取的文件个数，可作为当前频点SG更新状态显示用原始数据
            para4: 	用户在调用 ivisa_SG_StartScan()时传入的自定义参数
        2）infoType == IV_SG_CALLBACK_INTERACTIVITY
        	当收到此类型回调后，如果该para1所指向的字符串 == 当前频道的ServiceID，应该调用ivisa_sg_GetCurrentInteractivity()获取该频道最新的互动并显示给用户。
            para1：	互动对应ServiceID
            para2: 	保留
            para3: 	保留
            para4: 	保留
        3）infoType == IV_SG_CALLBACK_NOTIFICATION
        	当收到此类型回调后，应该调用ivisa_sg_GetCurrentNotification()获取当前最新通知信息并显示给用户。
            para1：	保留
            para2: 	保留
            para3: 	保留
            para4: 	保留
        4）infoType == IV_SG_CALLBACK_SERVICE_UPDATE
        	当收到此类型回调后，应该调用ivisa_sg_GetServiceList()获取最新频道信息。
            para1：	保留
            para2: 	保留
            para3: 	保留
            para4: 	保留
        5）infoType == IV_SG_CALLBACK_PROGRAM_UPDATE
        	当收到此类型回调后，如需要应该调用ivisa_sg_GetProgramList()获取最新节目单信息。
            para1：	保留
            para2: 	保留
            para3: 	保留
            para4: 	保留
        6）infoType == IV_SG_CALLBACK_ACCESS_UPDATE
        	当收到此类型回调后，如需要应该该调用ivisa_SG_GetServiceAccessInfo()获取最新信息。
            para1：	保留
            para2: 	保留
            para3: 	保留
            para4: 	保留
        7）infoType == IV_SG_CALLBACK_PURCHASE_UPDATE
        	当收到此类型回调后，如需要应该调用ivisa_SG_GetPurchaseItemList ()获取最新信息。
            para1：	保留
            para2: 	保留
            para3: 	保留
            para4: 	保留
    [返回值]:
            成功时返回0，错误时返回－1。

*******************************************************************************************************/

/*******************************************************************************************************/
/*                                  接口函数定义                                                       */
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
