//temporary substitute of FLUTE module //

#include <stdio.h>


#include    "ivisa_types.h"
#include    "ivisa_err_code.h"
#include    "ivisa_sg_filerev.h"


#define         MAX_IV_TIME_OUT        100000       // 3600*24=86400


/****************************************************************************************************
  ������:
  Iv_FileRev_GetMoudleCap
  ����ԭ��:
  kint Iv_FileRev_GetMoudleCap( IV_FILE_REV_PROTOCOL *pProType)
  ����:
  ��ȡ�ļ�����ģ��Ľ��չ���
  ����:
pProType:           ����ģ�����Э�����͵�ָ��

����ֵ:
IVEC_SUCCESS    :      ���óɹ�
IVEC_BADPARAM   :      �����������
IVEC_FAILURE    :      ����ʧ��
 *****************************************************************************************************/
kint Iv_FileRev_GetMoudleCap( IV_FILE_REV_PROTOCOL *pProType)
{

    return IVEC_FAILURE;
}

/****************************************************************************************************
  ������:
  Iv_FileRev_StartSession
  ����ԭ��:
  kint  Iv_FileRev_StartSession(void *vpEntryInfo , kuint uiSessionType)
  ����:
  �������ݶ�����յĻỰ
  ����:
vpEntryInfo:        �ļ����յ���ڲ���ָ��,��ָ������Ϊvoid *.�ļ�����ģ����������õĽ���
�����ļ���Э��,���Ѹò���ת������Ӧ��ָ������.
��,����MOTЭ��,��ָ����ת��ΪIV_MOT_SESSION_ENTRYINFO��ָ������,
����FLUTE/ALCЭ��,��ָ����ת��ΪIV_FLUTE_ALC_SESSION_ENTRYINFO��ָ��
����.

����ֵ:
��0  :               ���óɹ�,�Ҹ�ֵΪ��ʶ�����ݶ�����ջỰ�ľ��
0   :               ����ʧ��
 *****************************************************************************************************/
kint  Iv_FileRev_StartSession(void *  vpEntryInfo , IV_SESSION_TYPE SessionType)
{

    return 0;
}

/****************************************************************************************************
  ������:
  Iv_FileRev_StopSession
  ����ԭ��:
  kint  Iv_FileRev_StopSession(  kint iHandle)
  ����:
  ֹͣ���ݶ�����յĻỰ
  ����:
iHandle:   ��ʶ���ݶ�����ջỰ�ľ��

����ֵ:
IVEC_SUCCESS  :       ���óɹ�
IVEC_BADPARAM :       �����������
IVEC_FAILURE  :       ����ʧ��
 *****************************************************************************************************/
kint  Iv_FileRev_StopSession(  kint iHandle)
{
    return 0;

}

/****************************************************************************************************
  ������:
  Iv_FileRev_SetObjectListFilter
  ����ԭ��:
  kint  Iv_FileRev_SetObjectListFilter(    kint                       iHandle,
  Iv_ObjectList_CALLBACK         ObjectList_cbk,

  void *                      vpUserData1,
  void *                      vpUserData2)
  ����:
  ������ָ���ĻỰ��������ݶ����б�Ĺ�������
  ����:
iHandle:                ��ʶһ�����ļ����ջỰ�ľ��
ObjectList_cbk:         ����ģ����յ����ݽ��ն����б��,���ϻص��Ļص�����ָ��

vpUserData1:            ����ģ���豣����û����ݵĿ�����ָ��,�ص�ObjectList_cbk
ʱ�践�ظ�ָ��ֵ.
vpUserData2:            ����ģ���豣����û����ݵĿ�����ָ��,�ص�ObjectList_cbk
ʱ�践�ظ�ָ��ֵ.

����ֵ:
IVEC_SUCCESS  :      ���óɹ�
IVEC_BADPARAM :      �����������
IVEC_FAILURE  :      ����ʧ��
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
  ������:
  Iv_FileRev_StopObjectListFilter
  ����ԭ��:
  kint Iv_FileRev_StopObjectListFilter(kint iHandle)
  ����:
  ��ָ���ĻỰ��,ֹͣ�������ݶ����б�
  ����:
iHandle:        ��ʶ���ݶ�����ջỰ�ľ��

����ֵ:
IVEC_SUCCESS  :      ���óɹ�
IVEC_BADPARAM :      �����������
IVEC_FAILURE  :      ����ʧ��
 *****************************************************************************************************/
kint Iv_FileRev_StopObjectListFilter(  kint iHandle)
{

    return IVEC_FAILURE;
}

/****************************************************************************************************
  ������:
  Iv_FileRev_SetTransferObjectFilter
  ����ԭ��:
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
  ����:
  ������ָ���ĻỰ�����ָ������Ĺ�������
  ����:
iHandle:                ��ʶ�����ļ����ջỰ�ľ��
uiObjectID:             �������ݶ���ı�ʶ�����pObjectNameֵΪNULL��Ӧ���ݴ�ֵ��ȡ�ļ���
uiObjectIdMask:         uiObjectID�ı���Mask.�������MaskͬuiObjectIDһ����������
���˶����ʶ�ķ�Χ.���uiObjectIdMask�еı���λΪ"1",���
��uiObjectID����λ�����������,���uiObjectIdMask�еı���
λΪ"0",����Ӧ��uiObjectID����λ�������������.
��:uiObjectID=0x34abcd08,uiObjectIdMask=0xfffffff0;
����Ϲ���������ID��Ϊ:[0x34abcd00,0x34abcd0f]��16��ID
pObjectName:            �������ݶ�����ļ����������ֵ��ΪNULL��Ӧ���ݸ�ֵ��ȡ�ļ���uiObjectID��Ч��


TO_cbk:                 ����ģ����յ����ݽ��ն����б��,���ϻص��Ļص�����ָ��
sSavePathName           �ַ���ָ��,ͬʱ��ʶ����Ĵ洢ģʽ:
NULL:   �ڴ�ģʽ
��NULL: Ӳ�̴洢ģʽ,���ַ�����ʾ����Ĵ洢��ȫ·��
vpUserData0~3:          ����ģ���豣����û����ݵĿ�����ָ��,�ص�TO_cbkʱ�践��
��ָ��ֵ.
����ֵ:
��0  :                   ���óɹ�,�Ҹ�ֵΪ��ʶ���ոö���ľ��
0   :                   ����ʧ��

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
  ������:
  Iv_FileRev_StopTransferObjectFilter
  ����ԭ��:
  kint Iv_FileRev_StopTransferObjectFilter(kint iHandle,kint iObjectFilterHandel)
  ����:
  ��ָ���ĻỰ��,ֹͣ����ָ�������ݶ���
  ����:
iHandle:                ��ʶ���ݶ�����ջỰ�ľ��
iObjectFilterHandel:    ��ʶ���ݶ���ľ��
����ֵ:
IVEC_SUCCESS  :      ���óɹ�
IVEC_BADPARAM :      �����������
IVEC_FAILURE  :      ����ʧ��
 *******************************************************************************************************/
kint Iv_FileRev_StopTransferObjectFilter(  kint iHandle, kint iObjectFilterHandel)
{

    return IVEC_FAILURE;

}

/****************************************************************************************************
  ������:
  Iv_FlieRev_GetWebObject
  ����ԭ��:
  kint Iv_FlieRev_GetWebObject(kchar *        pObjectUrl,
  Iv_TransferObject_CALLBACK     WebO_cbk,
  kchar *                     sSavePathName,
  kuint                       uiTimeout,
  void *                      vpUserData1,
  void *                      vpUserData2);
  ����:
  ͨ��˫���ȡָ�������ݶ���
  ����:
pObjectUrl:             ��ʶ���ݶ����URL
WebO_cbk:               ˫�����ģ����յ����ݽ��ն����б��,���ϻص��Ļص�����ָ��
sSavePathName           �ַ���ָ��,ͬʱ��ʶ����Ĵ洢ģʽ:
NULL:   �ڴ�ģʽ,�ѽ��յ��Ķ�������ڴ�
��NULL: Ӳ�̴洢ģʽ,���ַ�����ʾ����Ĵ洢��ȫ·��,�ѽ��յ�
���ݶ������ָ����·��.
uiTimeout:              ˫���������ģ����ն����ʱ�䳬ʱ����,��ֵ�ĵ�λΪ��
vpUserData1:            ˫�����ģ���豣����û����ݵĿ�����ָ��,WebO_cbkʱ�践�ظ�
ָ��ֵ.
vpUserData2:            ˫�����ģ���豣����û����ݵĿ�����ָ��,WebO_cbkʱ�践�ظ�
ָ��ֵ.
����ֵ:
IVEC_SUCCESS  :      ���óɹ�
IVEC_BADPARAM :      �����������
IVEC_FAILURE  :      ����ʧ��
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
