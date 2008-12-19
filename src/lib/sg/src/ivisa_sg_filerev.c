//temporary substitute of FLUTE module //

#include <stdio.h>


#include    "ivisa_types.h"
#include    "ivisa_err_code.h"
#include    "ivisa_sg_filerev.h"


#define         MAX_IV_TIME_OUT        100000       // 3600*24=86400


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
kint Iv_FileRev_GetMoudleCap( IV_FILE_REV_PROTOCOL *pProType)
{

    return IVEC_FAILURE;
}

/****************************************************************************************************
  函数名:
  Iv_FileRev_StartSession
  函数原型:
  kint  Iv_FileRev_StartSession(void *vpEntryInfo , kuint uiSessionType)
  功能:
  启动数据对象接收的会话
  参数:
vpEntryInfo:        文件接收的入口参数指针,该指针类型为void *.文件接收模块根据所采用的接收
数据文件的协议,而把该参数转换成相应的指针类型.
如,采用MOT协议,该指针需转换为IV_MOT_SESSION_ENTRYINFO的指针类型,
采用FLUTE/ALC协议,该指针需转换为IV_FLUTE_ALC_SESSION_ENTRYINFO的指针
类型.

返回值:
非0  :               调用成功,且该值为标识该数据对象接收会话的句柄
0   :               调用失败
 *****************************************************************************************************/
kint  Iv_FileRev_StartSession(void *  vpEntryInfo , IV_SESSION_TYPE SessionType)
{

    return 0;
}

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
kint  Iv_FileRev_StopSession(  kint iHandle)
{
    return 0;

}

/****************************************************************************************************
  函数名:
  Iv_FileRev_SetObjectListFilter
  函数原型:
  kint  Iv_FileRev_SetObjectListFilter(    kint                       iHandle,
  Iv_ObjectList_CALLBACK         ObjectList_cbk,

  void *                      vpUserData1,
  void *                      vpUserData2)
  功能:
  设置在指定的会话里过滤数据对象列表的过滤条件
  参数:
iHandle:                标识一数据文件接收会话的句柄
ObjectList_cbk:         接收模块接收到数据接收对象列表后,向上回调的回调函数指针

vpUserData1:            接收模块需保存的用户数据的空类型指针,回调ObjectList_cbk
时需返回该指针值.
vpUserData2:            接收模块需保存的用户数据的空类型指针,回调ObjectList_cbk
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
        void *                  vpUserData3)
{
    return IVEC_FAILURE;

}

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
kint Iv_FileRev_StopObjectListFilter(  kint iHandle)
{

    return IVEC_FAILURE;
}

/****************************************************************************************************
  函数名:
  Iv_FileRev_SetTransferObjectFilter
  函数原型:
  kint Iv_FileRev_SetTransferObjectFilter( kint                        iHandle,
  kuint                       uiObjectID,
  kuint                       uiObjectIdMask,
  Iv_TransferObject_CALLBACK     TO_cbk,
  kchar*            pObjectName,
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

 *****************************************************************************************************/
kint Iv_FileRev_SetTransferObjectFilter(kint                        iHandle,
        kuint                       uiObjectID,
        kuint                       uiObjectIdMask,
        Iv_TransferObject_CALLBACK     TO_cbk,
        kchar*               pObjectName,
        kchar *                     sSavePathName,
        void *                      vpUserData0,
        void *                      vpUserData1,
        void *                      vpUserData2,
        void *                      vpUserData3)
{

    return 0;

}

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
kint Iv_FileRev_StopTransferObjectFilter(  kint iHandle, kint iObjectFilterHandel)
{

    return IVEC_FAILURE;

}

/****************************************************************************************************
  函数名:
  Iv_FlieRev_GetWebObject
  函数原型:
  kint Iv_FlieRev_GetWebObject(kchar *        pObjectUrl,
  Iv_TransferObject_CALLBACK     WebO_cbk,
  kchar *                     sSavePathName,
  kuint                       uiTimeout,
  void *                      vpUserData1,
  void *                      vpUserData2);
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
kint Iv_FlieRev_GetWebObject(    kchar *                    pObjectUrl,
        Iv_TransferObject_CALLBACK     WebO_cbk,
        kchar *                     sSavePathName,
        kuint                       uiTimeout,
        void *                      vpUserData0,
        void *                      vpUserData1,
        void *                      vpUserData2,
        void *                      vpUserData3)


{



    return IVEC_FAILURE;  // not surport

}


kint Iv_FileRev_MemoryStat()
{
    return 0;
}
