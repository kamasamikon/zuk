#ifndef  _DAV_FILE_RECEIVE_API_H_
#define  _DAV_FILE_RECEIVE_API_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "ivisa_types.h"
#include "ivisa_err_code.h"
#include "ivisa_sg_comm.h"


    /*******************************************************************************************************/
    /*                                  ö�����Ͷ���                                                       */
    /*******************************************************************************************************/
    typedef enum
    {
        IV_FILEREV_PROTOCOL_FLUTEALC=0,     /* FLUTE /ALC  �ļ�����Э���ʶ����*/
        IV_FILEREV_PROTOCOL_MOT=1,             /* MOT �ļ�����Э���ʶ����*/
        IV_FILEREV_PROTOCOL_BDIR=2,             /* MOT �ļ�����Э���ʶ����*/
    }IV_FILE_REV_PROTOCOL;


    typedef enum
    {
        IV_SESSION_WITHDIR_TYPE=0,            /* ��session�����ļ��б���FLUTE/MOT Directory Mode session*/
        IV_SESSION_WITHOUTDIR_TYPE            /* ��session��û���ļ��б���ALC/MOT Header Mode session*/
    } IV_SESSION_TYPE;

    /*******************************************************************************************************/
    /*                                  ���ݽṹ����                                                       */
    /*******************************************************************************************************/

    typedef struct _IV_OBJECT_INFO_
    {
        kchar           *pObjectName;       /*  ��ʶ��������,����Ϊ�ǿ�*/
        kuint           uiObjectID;         /*  �����ID��ʶ��Ӧ���Ǵ�����������TransportObjectID*/
        kint            iObjectVersion;     /*  ����İ汾:
                                                >=0,�Ϸ��汾��,
                                                -1,�����ڰ汾��
                                                <-1,�Ƿ��汾��*/
        kint            iObjectVersionLength;     /*  ����汾���ֽڳ���,��ģ�����MOTЭ��,�����Կɺ���,����Ϊ-1 */
        kuint           uiContentLength;    /*  ����û�о�����������ݳ���(��λΪ�ֽ�)*/
        kuint           uiTransferLength;   /*  ����Ĵ���ʱ�����ݳ���(��λΪ�ֽ�)*/
        kchar           *pContentEncoding;  /*  ����ı�������,���ַ�����ʽ��ʶ;����û�о�������
                                                ��ֵΪ�����������б�ʾ������������Ϊ��������,
                                                Ҳת��Ϊ�ַ�����ʽ��ʶ.��,"gzip"��ʾ���������ڴ���
                                                ǰ������Gzip��ʽ��ѹ��*/

        kchar           *pContentType;      /*  �������������,��MIME��ʽ����ʾ;����û�и���˵��ʱΪ
                                                ������ʶ */
        kchar           *pContentMD5;       /*  �����MD5��Ϣ,�޴���Ϣʱ,Ϊ������ʶ*/

    }IV_OBJECT_INFO;


    typedef struct  _IV_OBJECT_LIST_ATTRIBUTE_
    {
        kuint    uiExpiresTime;      /* NTPʱ��,��ʶ�����б�ĵ���ʱ�� */
        kuint    uiObjectListVersion;/* �����б�İ汾 */
        kint       iComplete_flag;     /* �����б��complete����:
                                          1,��ʾ�ö����б������ڸûỰ�ڼ���������
                                          0,��ʾ�ö����б������ڸûỰ�ڼ䲻��������
                                          -1,��ʾ������û�г���*/
        kint    iFullFDT_flag;      /* �����б��FullFDT����:
                                       1,��ʾ�ö����б��������б����Чʱ����������
                                       0,��ʾ�ö����б��������б����Чʱ���ڲ���������
                                       -1,��ʾ������û�г���*/

    }IV_OBJECT_LIST_ATTRIBUTE;


    /*******************************************************************************************************/
    /*                                  �ص���������                                                       */
    /*******************************************************************************************************/
    /*****************************************************************************************************
      �ص�����ģ����
      Iv_ObjectList_CALLBACK
      ����:
      �ɵײ㵥�����ݽ���ģ��,�ڽ���������ʧ�ܶ����б�����
      ����:
iHandle:                ��ʶһ�����ļ����ջỰ�ľ��
iStatus:                ���ն����б�Ľ��:
IVEC_SUCCESS,��ʾ���ճɹ�
IVEC_FAILURE,��ʾ����ʧ��
pObjectList_attibute:   IV_OBJECT_LIST_ATTRIBUTE����ָ��,�������ģ�����MOTʱ,��ֵ��
����NULL

pInfoHead:              ָ��IV_OBJECT_INFO�������ݿ��ͷָ���ַ
iObjectNum              IV_OBJECT_LIST__INFO�������ݿ�ĸ���,������ĸ���
vpUserData0~3:          �û����ݵĿ�����ָ��,��Iv_FileRev_SetObjectListFilter����������
����ֵ

����ֵ:
IVEC_SUCCESS :���óɹ�
IVEC_BADPARAM :��������
IVEC_FAILURE:����ʧ��
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
      �ص�����ģ����
      Iv_TransferObject_CALLBACK
      ����:
      �ɵײ㵥�����ݽ���ģ��,�ڽ���������ʧ�ܶ�������
      ����:
iHandle:                ��ʶһ�����ļ����ջỰ�ľ��
iStatus:                ���ն����б�Ľ��:
IVEC_SUCCESS,��ʾ���ճɹ�
IVEC_FAILURE,��ʾ����ʧ��
uiObjectID:             �������ݶ���ı�ʶ, ��ֵ��Iv_FileRev_SetTransferObjectFilter()���룬
������Iv_FlieRev_GetWebObject()���ý��յĶ��󣬸�ֵӦ����Ϊ0��

pObjectName:            �������ݶ�����ļ�������Iv_FileRev_SetTransferObjectFilter()���롣
������Iv_FlieRev_GetWebObject()���ý��յĶ��󣬸�ֵӦ����ΪNULL��

pObjectUrl:             ��ʶ���ݶ����URL����Iv_FlieRev_GetWebObject()���룬������
Iv_FileRev_SetTransferObjectFilter()���ý��յĶ��󣬴���Ӧ����ΪNULL��

iObjectVersion:         �������ݶ���İ汾��,�紫�����û�г��ְ汾��,��-1��ʾ.
pDateBuffer:            �޷����ֽ���ָ��,Ϊ�������ݿ��ͷ��ַָ��;
���,Ϊ���ڴ�ģʽʱ,��ֵΪ��(��Iv_FileRev_SetTransferObjectFilter��
sSavePathNameֵ��Ϊ��)

uiObjectLength:         32Ϊ�޷�������,��ʾ����Ĵ��䳤��
pSavePathName           ����Ĵ洢·��,Ϊ�ڴ�ģʽʱ,��ֵΪ��
pContentEncoding        ����ı�������,���ַ�����ʽ��ʶ;����û�о�������(��û���ڴ�����г���
��ʾ����ı�������)��ֵΪ��;�����������б�ʾ������������Ϊ��������,
Ҳת��Ϊ�ַ�����ʽ��ʶ;��,"gzip"��ʾ���������ڴ���ǰ������Gzip��ʽ��ѹ��

vpUserData0~3:          �û����ݵĿ�����ָ��,Iv_FileRev_SetTransferObjectFilter��Iv_FlieRev_GetWebObject
���������Ĳ���ֵ

����ֵ:
IVEC_SUCCESS  :���óɹ�
IVEC_BADPARAM :��������
IVEC_FAILURE  :����ʧ��

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
    /*                                  ���ݽ���ģ��API��������                                             */
    /*******************************************************************************************************/

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
    kint Iv_FileRev_GetMoudleCap( IV_FILE_REV_PROTOCOL *pProType);


    /****************************************************************************************************
      ������:
      Iv_FileRev_StartSession
      ����ԭ��:
      kint  Iv_FileRev_StartSession(void *vpEntryInfo, kuint uiSessionType)
      ����:
      �������ݶ�����յĻỰ
      ����:
vpEntryInfo:        �ļ����յ���ڲ���ָ��,��ָ������Ϊvoid *.�ļ�����ģ����������õĽ���
�����ļ���Э��,���Ѹò���ת������Ӧ��ָ������.
��,����MOTЭ��,��ָ����ת��ΪIV_DAB_SESSION_ENTRYINFO��ָ������,
����FLUTE/ALCЭ��,��ָ����ת��ΪIV_FLUTE_ALC_SESSION_ENTRYINFO��ָ��
����.
uiSessionType:        ��Session�����ͣ���IV_SESSION_TYPE����

����ֵ:
��0  :               ���óɹ�,�Ҹ�ֵΪ��ʶ�����ݶ�����ջỰ�ľ��
0   :               ����ʧ��
     *****************************************************************************************************/
    kint  Iv_FileRev_StartSession(  void *  vpEntryInfo, IV_SESSION_TYPE SessionType);



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
    kint  Iv_FileRev_StopSession(  kint iHandle);

    /****************************************************************************************************
      ������:
      Iv_FileRev_SetObjectListFilter
      ����ԭ��:
      kint  Iv_FileRev_SetObjectListFilter(    kint                       iHandle,
      Iv_ObjectList_CALLBACK         ObjectList_cbk,
      void *                      vpUserData0,
      void *                      vpUserData1,
      void *                      vpUserData2,
      void *                      vpUserData3)
      ����:
      ������ָ���ĻỰ��������ݶ����б�Ĺ�������
      ����:
iHandle:                ��ʶһ�����ļ����ջỰ�ľ��
ObjectList_cbk:         ����ģ����յ����ݽ��ն����б��,���ϻص��Ļص�����ָ��
vpUserData0~3:          ����ģ���豣����û����ݵĿ�����ָ��,�ص�ObjectList_cbk
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
            void *                  vpUserData3);


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
    kint Iv_FileRev_StopObjectListFilter(  kint iHandle);


    /****************************************************************************************************
      ������:
      Iv_FileRev_SetTransferObjectFilter
      ����ԭ��:
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
ע    : ���ھ����ļ��б��session,�˺������ڵ���Iv_FileRev_SetTransferObjectFilter() ֮ǰ������
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
    kint Iv_FileRev_StopTransferObjectFilter(  kint iHandle, kint iObjectFilterHandel);




    /****************************************************************************************************
      ������:
      Iv_FlieRev_GetWebObject
      ����ԭ��:
      kint Iv_FlieRev_GetWebObject(kchar *        pObjectUrl,
      Iv_TransferObject_CALLBACK     WebO_cbk,
      kchar *                     sSavePathName,
      kuint                       uiTimeout,
      void *                      vpUserData0,
      void *                      vpUserData1,
      void *                      vpUserData2,
      void *                      vpUserData3);
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
