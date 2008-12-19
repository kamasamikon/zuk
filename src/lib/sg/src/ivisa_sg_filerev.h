#ifndef  _DAV_FILE_RECEIVE_API_H_
#define  _DAV_FILE_RECEIVE_API_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ivisa_types.h"
#include "ivisa_err_code.h"
#include "ivisa_sg_comm.h"


    /*******************************************************************************************************/
    /*                                  枚举类型定义                                                       */
    /*******************************************************************************************************/
    typedef enum
    {
        IV_FILEREV_PROTOCOL_FLUTEALC=0,     /* FLUTE /ALC  文件接收协议标识类型*/
        IV_FILEREV_PROTOCOL_MOT=1,             /* MOT 文件接收协议标识类型*/
        IV_FILEREV_PROTOCOL_BDIR=2,             /* MOT 文件接收协议标识类型*/
    }IV_FILE_REV_PROTOCOL;


    typedef enum
    {
        IV_SESSION_WITHDIR_TYPE=0,            /* 该session中有文件列表，如FLUTE/MOT Directory Mode session*/
        IV_SESSION_WITHOUTDIR_TYPE            /* 该session中没有文件列表，如ALC/MOT Header Mode session*/
    } IV_SESSION_TYPE;

    /*******************************************************************************************************/
    /*                                  数据结构定义                                                       */
    /*******************************************************************************************************/

    typedef struct _IV_OBJECT_INFO_
    {
        kchar           *pObjectName;       /*  标识对象名称,必需为非空*/
        kuint           uiObjectID;         /*  对象的ID标识，应该是传输对象的完整TransportObjectID*/
        kint            iObjectVersion;     /*  对象的版本:
                                                >=0,合法版本号,
                                                -1,不存在版本号
                                                <-1,非法版本号*/
        kint            iObjectVersionLength;     /*  对象版本的字节长度,如模块采用MOT协议,该属性可忽略,并设为-1 */
        kuint           uiContentLength;    /*  对象没有经过编码的数据长度(单位为字节)*/
        kuint           uiTransferLength;   /*  对象的传输时的数据长度(单位为字节)*/
        kchar           *pContentEncoding;  /*  对象的编码类型,用字符串方式标识;对象没有经过编码
                                                该值为空如果传输层中表示对象编码的类型为整型数据,
                                                也转换为字符串方式标识.如,"gzip"表示经过对象在传输
                                                前经过了Gzip方式的压缩*/

        kchar           *pContentType;      /*  对象的数据类型,用MIME方式来表示;对象没有该项说明时为
                                                空来标识 */
        kchar           *pContentMD5;       /*  对象的MD5信息,无此信息时,为空来标识*/

    }IV_OBJECT_INFO;


    typedef struct  _IV_OBJECT_LIST_ATTRIBUTE_
    {
        kuint    uiExpiresTime;      /* NTP时间,标识对象列表的到期时间 */
        kuint    uiObjectListVersion;/* 对象列表的版本 */
        kint       iComplete_flag;     /* 对象列表的complete属性:
                                          1,表示该对象列表内容在该会话期间是完整的
                                          0,表示该对象列表内容在该会话期间不是完整的
                                          -1,表示该属性没有出现*/
        kint    iFullFDT_flag;      /* 对象列表的FullFDT属性:
                                       1,表示该对象列表内容在列表的有效时间内完整的
                                       0,表示该对象列表内容在列表的有效时间内不是完整的
                                       -1,表示该属性没有出现*/

    }IV_OBJECT_LIST_ATTRIBUTE;


    /*******************************************************************************************************/
    /*                                  回调函数定义                                                       */
    /*******************************************************************************************************/
    /*****************************************************************************************************
      回调函数模型名
      Iv_ObjectList_CALLBACK
      功能:
      由底层单向数据接收模块,在接收完或接收失败对象列表后调用
      参数:
iHandle:                标识一数据文件接收会话的句柄
iStatus:                接收对象列表的结果:
IVEC_SUCCESS,表示接收成功
IVEC_FAILURE,表示接收失败
pObjectList_attibute:   IV_OBJECT_LIST_ATTRIBUTE类型指针,如果接收模块采用MOT时,该值可
设置NULL

pInfoHead:              指向IV_OBJECT_INFO类型数据块的头指针地址
iObjectNum              IV_OBJECT_LIST__INFO类型数据块的个数,即对象的个数
vpUserData0~3:          用户数据的空类型指针,由Iv_FileRev_SetObjectListFilter设置下来的
参数值

返回值:
IVEC_SUCCESS :调用成功
IVEC_BADPARAM :参数错误
IVEC_FAILURE:调用失败
     *****************************************************************************************************/
    typedef kint (*Iv_ObjectList_CALLBACK)(kint                        iHandle,
            kint                         iStatus,
            IV_OBJECT_LIST_ATTRIBUTE   * pObjectList_attibute,
            IV_OBJECT_INFO              * pInfoLisHead,
            kint                         iObjectNum,
            void *                       vpUserData0,
            void *                       vpUserData1,
            void *                       vpUserData2,
            void *                       vpUserData3);

    /*****************************************************************************************************
      回调函数模型名
      Iv_TransferObject_CALLBACK
      功能:
      由底层单向数据接收模块,在接收完或接收失败对象后调用
      参数:
iHandle:                标识一数据文件接收会话的句柄
iStatus:                接收对象列表的结果:
IVEC_SUCCESS,表示接收成功
IVEC_FAILURE,表示接收失败
uiObjectID:             接收数据对象的标识, 该值由Iv_FileRev_SetTransferObjectFilter()传入，
对于由Iv_FlieRev_GetWebObject()设置接收的对象，该值应该置为0。

pObjectName:            接收数据对象的文件名，由Iv_FileRev_SetTransferObjectFilter()传入。
对于由Iv_FlieRev_GetWebObject()设置接收的对象，该值应该置为NULL。

pObjectUrl:             标识数据对象的URL，由Iv_FlieRev_GetWebObject()传入，对于由
Iv_FileRev_SetTransferObjectFilter()设置接收的对象，此项应该置为NULL。

iObjectVersion:         接收数据对象的版本号,如传输层中没有出现版本号,用-1表示.
pDateBuffer:            无符号字节型指针,为对象数据块的头地址指针;
如果,为非内存模式时,该值为空(即Iv_FileRev_SetTransferObjectFilter的
sSavePathName值不为空)

uiObjectLength:         32为无符号整型,表示对象的传输长度
pSavePathName           对象的存储路径,为内存模式时,该值为空
pContentEncoding        对象的编码类型,用字符串方式标识;对象没有经过编码(或没有在传输层中出现
表示对象的编码属性)该值为空;如果传输层中有表示对象编码的类型为整型数据,
也转换为字符串方式标识;如,"gzip"表示经过对象在传输前经过了Gzip方式的压缩

vpUserData0~3:          用户数据的空类型指针,Iv_FileRev_SetTransferObjectFilter或Iv_FlieRev_GetWebObject
设置下来的参数值

返回值:
IVEC_SUCCESS  :调用成功
IVEC_BADPARAM :参数错误
IVEC_FAILURE  :调用失败

     *****************************************************************************************************/
    typedef kint (*Iv_TransferObject_CALLBACK)(    kint                iHandle,
            kint                iStatus,
            kuint               uiObjectID,
            kchar*                pObjectName,
            kchar *             pObjectUrl,
            kint                iObjectVersion,
            kuchar *            pDataBuffer,
            kuint               uiObjectLength,
            kchar *             pSavePathName,
            kchar *             pContentEncoding,
            void *              vpUserData0,
            void *              vpUserData1,
            void *              vpUserData2,
            void *              vpUserData3);




    /*******************************************************************************************************/
    /*                                  数据接收模块API函数定义                                             */
    /*******************************************************************************************************/

    /****************************************************************************************************
      函数名:
      Iv_FileRev_GetMoudleCap
      函数原型:
      kint Iv_FileRev_GetMoudleCap( IV_FILE_REV_PROTOCOL *pProType)
      功能:
      获取文件接收模块的接收功能
      参数:
pProType:           接收模块采用协议类型的指针

返回值:
IVEC_SUCCESS    :      调用成功
IVEC_BADPARAM   :      输入参数有误
IVEC_FAILURE    :      调用失败
     *****************************************************************************************************/
    kint Iv_FileRev_GetMoudleCap( IV_FILE_REV_PROTOCOL *pProType);


    /****************************************************************************************************
      函数名:
      Iv_FileRev_StartSession
      函数原型:
      kint  Iv_FileRev_StartSession(void *vpEntryInfo, kuint uiSessionType)
      功能:
      启动数据对象接收的会话
      参数:
vpEntryInfo:        文件接收的入口参数指针,该指针类型为void *.文件接收模块根据所采用的接收
数据文件的协议,而把该参数转换成相应的指针类型.
如,采用MOT协议,该指针需转换为IV_DAB_SESSION_ENTRYINFO的指针类型,
采用FLUTE/ALC协议,该指针需转换为IV_FLUTE_ALC_SESSION_ENTRYINFO的指针
类型.
uiSessionType:        该Session的类型，见IV_SESSION_TYPE定义

返回值:
非0  :               调用成功,且该值为标识该数据对象接收会话的句柄
0   :               调用失败
     *****************************************************************************************************/
    kint  Iv_FileRev_StartSession(  void *  vpEntryInfo, IV_SESSION_TYPE SessionType);



    /****************************************************************************************************
      函数名:
      Iv_FileRev_StopSession
      函数原型:
      kint  Iv_FileRev_StopSession(  kint iHandle)
      功能:
      停止数据对象接收的会话
      参数:
iHandle:   标识数据对象接收会话的句柄

返回值:
IVEC_SUCCESS  :       调用成功
IVEC_BADPARAM :       输入参数有误
IVEC_FAILURE  :       调用失败
     *****************************************************************************************************/
    kint  Iv_FileRev_StopSession(  kint iHandle);

    /****************************************************************************************************
      函数名:
      Iv_FileRev_SetObjectListFilter
      函数原型:
      kint  Iv_FileRev_SetObjectListFilter(    kint                       iHandle,
      Iv_ObjectList_CALLBACK         ObjectList_cbk,
      void *                      vpUserData0,
      void *                      vpUserData1,
      void *                      vpUserData2,
      void *                      vpUserData3)
      功能:
      设置在指定的会话里过滤数据对象列表的过滤条件
      参数:
iHandle:                标识一数据文件接收会话的句柄
ObjectList_cbk:         接收模块接收到数据接收对象列表后,向上回调的回调函数指针
vpUserData0~3:          接收模块需保存的用户数据的空类型指针,回调ObjectList_cbk
时需返回该指针值.

返回值:
IVEC_SUCCESS  :      调用成功
IVEC_BADPARAM :      输入参数有误
IVEC_FAILURE  :      调用失败
     *****************************************************************************************************/
    kint  Iv_FileRev_SetObjectListFilter( kint                  iHandle,
            Iv_ObjectList_CALLBACK     ObjectList_cbk,
            void *                  vpUserData0,
            void *                  vpUserData1,
            void *                  vpUserData2,
            void *                  vpUserData3);


    /****************************************************************************************************
      函数名:
      Iv_FileRev_StopObjectListFilter
      函数原型:
      kint Iv_FileRev_StopObjectListFilter(kint iHandle)
      功能:
      在指定的会话里,停止过滤数据对象列表
      参数:
iHandle:        标识数据对象接收会话的句柄

返回值:
IVEC_SUCCESS  :      调用成功
IVEC_BADPARAM :      输入参数有误
IVEC_FAILURE  :      调用失败
     *****************************************************************************************************/
    kint Iv_FileRev_StopObjectListFilter(  kint iHandle);


    /****************************************************************************************************
      函数名:
      Iv_FileRev_SetTransferObjectFilter
      函数原型:
      kint Iv_FileRev_SetTransferObjectFilter( kint                        iHandle,
      kuint                       uiObjectID,
      kuint                       uiObjectIdMask,
      Iv_TransferObject_CALLBACK     TO_cbk,
      kchar*                         pObjectName,
      kchar *                     sSavePathName,
      void *                      vpUserData0,
      void *                      vpUserData1,
      void *                      vpUserData2,
      void *                      vpUserData3)
      功能:
      设置在指定的会话里过滤指定对象的过滤条件
      参数:
iHandle:                标识数据文件接收会话的句柄
uiObjectID:             接收数据对象的标识，如果pObjectName值为NULL，应根据此值收取文件。
uiObjectIdMask:         uiObjectID的比特Mask.这个比特Mask同uiObjectID一到用来设置
过滤对象标识的范围.如果uiObjectIdMask中的比特位为"1",则对
于uiObjectID比特位加入过滤条件,如果uiObjectIdMask中的比特
位为"0",则相应的uiObjectID比特位不进入过滤条件.
如:uiObjectID=0x34abcd08,uiObjectIdMask=0xfffffff0;
则符合过滤条件的ID就为:[0x34abcd00,0x34abcd0f]共16个ID
pObjectName:            接收数据对象的文件名，如果该值不为NULL，应根据该值收取文件，uiObjectID无效。


TO_cbk:                 接收模块接收到数据接收对象列表后,向上回调的回调函数指针
sSavePathName           字符串指针,同时标识对象的存储模式:
NULL:   内存模式
非NULL: 硬盘存储模式,该字符串表示对象的存储的全路径
vpUserData0~3:          接收模块需保存的用户数据的空类型指针,回调TO_cbk时需返回
该指针值.
返回值:
非0  :                   调用成功,且该值为标识接收该对象的句柄
0   :                   调用失败
注    : 对于具有文件列表的session,此函数可在调用Iv_FileRev_SetTransferObjectFilter() 之前被调用
     *****************************************************************************************************/
    kint Iv_FileRev_SetTransferObjectFilter(    kint                        iHandle,
            kuint                       uiObjectID,
            kuint                       uiObjectIdMask,
            Iv_TransferObject_CALLBACK     TO_cbk,
            kchar*                         pObjectName,
            kchar *                     sSavePathName,
            void *                      vpUserData0,
            void *                      vpUserData1,
            void *                      vpUserData2,
            void *                      vpUserData3);


    /****************************************************************************************************
      函数名:
      Iv_FileRev_StopTransferObjectFilter
      函数原型:
      kint Iv_FileRev_StopTransferObjectFilter(kint iHandle,kint iObjectFilterHandel)
      功能:
      在指定的会话里,停止过滤指定的数据对象
      参数:
iHandle:                标识数据对象接收会话的句柄
iObjectFilterHandel:    标识数据对象的句柄
返回值:
IVEC_SUCCESS  :      调用成功
IVEC_BADPARAM :      输入参数有误
IVEC_FAILURE  :      调用失败
     *******************************************************************************************************/
    kint Iv_FileRev_StopTransferObjectFilter(  kint iHandle, kint iObjectFilterHandel);




    /****************************************************************************************************
      函数名:
      Iv_FlieRev_GetWebObject
      函数原型:
      kint Iv_FlieRev_GetWebObject(kchar *        pObjectUrl,
      Iv_TransferObject_CALLBACK     WebO_cbk,
      kchar *                     sSavePathName,
      kuint                       uiTimeout,
      void *                      vpUserData0,
      void *                      vpUserData1,
      void *                      vpUserData2,
      void *                      vpUserData3);
      功能:
      通过双向获取指定的数据对象
      参数:
pObjectUrl:             标识数据对象的URL
WebO_cbk:               双向接收模块接收到数据接收对象列表后,向上回调的回调函数指针
sSavePathName           字符串指针,同时标识对象的存储模式:
NULL:   内存模式,把接收到的对象放入内存
非NULL: 硬盘存储模式,该字符串表示对象的存储的全路径,把接收到
数据对象存入指定的路径.
uiTimeout:              双向接收数据模块接收对象的时间超时长度,该值的单位为秒
vpUserData1:            双向接收模块需保存的用户数据的空类型指针,WebO_cbk时需返回该
指针值.
vpUserData2:            双向接收模块需保存的用户数据的空类型指针,WebO_cbk时需返回该
指针值.
返回值:
IVEC_SUCCESS  :      调用成功
IVEC_BADPARAM :      输入参数有误
IVEC_FAILURE  :      调用失败
     *****************************************************************************************************/
    kint Iv_FlieRev_GetWebObject(kchar *                    pObjectUrl,
            Iv_TransferObject_CALLBACK     WebO_cbk,
            kchar *                     sSavePathName,
            kuint                       uiTimeout,
            void *                      vpUserData0,
            void *                      vpUserData1,
            void *                      vpUserData2,
            void *                      vpUserData3);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif
