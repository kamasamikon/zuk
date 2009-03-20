/* vim:set et sw=4 sts=4: */

#include "esgfile.h"
#include "ksupport.h"
#include <kmem.h>

#include "ivisa_sg_config.h"

#include "sg_com.h"
#include "sg_s2n_fun.h"
#include "s2n_macro.h"

S2N_FUN_DECL(Version)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Version)
/*X2S_GETNODECNT(Version)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Version)
X2S_SET_CNT(Version)*/
S2N_SET_NCNT(Version)
S2N_FOREACH_NCNT(Version)
S2N_CURLY_BRACE_L
S2N_SET_REC(Version)
// X2S_BRK_IF_BAD_NODE(Version)
S2N_DEL_VAL(kpchar, Version)
// X2S_GOTO_PRT_NODE(Version)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Version)
S2N_DEL_SELF(Version)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Version)


S2N_FUN_DECL(BDSType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BDSType)
/*X2S_GETNODECNT(BDSType)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BDSType)
X2S_SET_CNT(BDSType)*/
S2N_SET_NCNT(BDSType)
S2N_FOREACH_NCNT(BDSType)
S2N_CURLY_BRACE_L
S2N_SET_REC(BDSType)
// X2S_BRK_IF_BAD_NODE(BDSType)
S2N_DEL_SUB(Type, Type)
S2N_DEL_SUB(Version, Version)
// X2S_GOTO_PRT_NODE(BDSType)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BDSType)
S2N_DEL_SELF(BDSType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BDSType)


S2N_FUN_DECL(SDP)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SDP)
/*X2S_GETNODECNT(SDP)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(SDP)
X2S_SET_CNT(SDP)*/
S2N_SET_NCNT(SDP)
S2N_FOREACH_NCNT(SDP)
S2N_CURLY_BRACE_L
S2N_SET_REC(SDP)
// X2S_BRK_IF_BAD_NODE(SDP)
S2N_DEL_VAL(kpchar, SDP)
// X2S_GOTO_PRT_NODE(SDP)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SDP)
S2N_DEL_SELF(SDP)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SDP)


S2N_FUN_DECL(SDPRef)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SDPRef)
/*X2S_GETNODECNT(SDPRef)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(SDPRef)
X2S_SET_CNT(SDPRef)*/
S2N_SET_NCNT(SDPRef)
S2N_FOREACH_NCNT(SDPRef)
S2N_CURLY_BRACE_L
S2N_SET_REC(SDPRef)
// X2S_BRK_IF_BAD_NODE(SDPRef)
S2N_DEL_ATT(kpchar, uri)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(SDPRef)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SDPRef)
S2N_DEL_SELF(SDPRef)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SDPRef)


S2N_FUN_DECL(USBDRef)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(USBDRef)
/*X2S_GETNODECNT(USBDRef)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(USBDRef)
X2S_SET_CNT(USBDRef)*/
S2N_SET_NCNT(USBDRef)
S2N_FOREACH_NCNT(USBDRef)
S2N_CURLY_BRACE_L
S2N_SET_REC(USBDRef)
// X2S_BRK_IF_BAD_NODE(USBDRef)
S2N_DEL_ATT(kpchar, uri)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(USBDRef)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(USBDRef)
S2N_DEL_SELF(USBDRef)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(USBDRef)


S2N_FUN_DECL(ADPRef)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ADPRef)
/*X2S_GETNODECNT(ADPRef)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ADPRef)
X2S_SET_CNT(ADPRef)*/
S2N_SET_NCNT(ADPRef)
S2N_FOREACH_NCNT(ADPRef)
S2N_CURLY_BRACE_L
S2N_SET_REC(ADPRef)
// X2S_BRK_IF_BAD_NODE(ADPRef)
S2N_DEL_ATT(kpchar, uri)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(ADPRef)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ADPRef)
S2N_DEL_SELF(ADPRef)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ADPRef)


S2N_FUN_DECL(SessionDescription)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SessionDescription)
/*X2S_GETNODECNT(SessionDescription)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(SessionDescription)
X2S_SET_CNT(SessionDescription)*/
S2N_SET_NCNT(SessionDescription)
S2N_FOREACH_NCNT(SessionDescription)
S2N_CURLY_BRACE_L
S2N_SET_REC(SessionDescription)
// X2S_BRK_IF_BAD_NODE(SessionDescription)
S2N_DEL_SUB(SDP, SDP)
S2N_DEL_SUB(SDPRef, SDPRef)
S2N_DEL_SUB(USBDRef, USBDRef)
S2N_DEL_SUB(ADPRef, ADPRef)
// X2S_GOTO_PRT_NODE(SessionDescription)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SessionDescription)
S2N_DEL_SELF(SessionDescription)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SessionDescription)


S2N_FUN_DECL(File)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(File)
/*X2S_GETNODECNT(File)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(File)
X2S_SET_CNT(File)*/
S2N_SET_NCNT(File)
S2N_FOREACH_NCNT(File)
S2N_CURLY_BRACE_L
S2N_SET_REC(File)
// X2S_BRK_IF_BAD_NODE(File)
S2N_DEL_ATT(kpchar, Content_Location)
//S2N_DEL_ATT(kint, TOI)
//S2N_DEL_ATT(kulong, Content_Length)
//S2N_DEL_ATT(kulong, Transfer_Length)
S2N_DEL_ATT(kpchar, Content_Type)
S2N_DEL_ATT(kpchar, Content_Encoding)
S2N_DEL_ATT(kpchar, Content_MD5)
//S2N_DEL_ATT(kuchar, FEC_OTI_FEC_Encoding_ID)
//S2N_DEL_ATT(kulong, FEC_OTI_FEC_Instance_ID)
//S2N_DEL_ATT(kulong, FEC_OTI_Maximum_Source_Block_Length)
//S2N_DEL_ATT(kulong, FEC_OTI_Encoding_Symbol_Length)
//S2N_DEL_ATT(kulong, FEC_OTI_Max_Number_of_Encoding_Symbols)
S2N_DEL_ATT(kpchar, FEC_OTI_Scheme_Specific_Info)
// X2S_GOTO_PRT_NODE(File)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(File)
S2N_DEL_SELF(File)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(File)


S2N_FUN_DECL(FileDescription)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(FileDescription)
/*X2S_GETNODECNT(FileDescription)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(FileDescription)
X2S_SET_CNT(FileDescription)*/
S2N_SET_NCNT(FileDescription)
S2N_FOREACH_NCNT(FileDescription)
S2N_CURLY_BRACE_L
S2N_SET_REC(FileDescription)
// X2S_BRK_IF_BAD_NODE(FileDescription)
S2N_DEL_ATT(kpchar, Content_Type)
S2N_DEL_ATT(kpchar, Content_Encoding)
//S2N_DEL_ATT(kuchar, FEC_OTI_FEC_Encoding_ID)
//S2N_DEL_ATT(kulong, FEC_OTI_FEC_Instance_ID)
//S2N_DEL_ATT(kulong, FEC_OTI_Maximum_Source_Block_Length)
//S2N_DEL_ATT(kulong, FEC_OTI_Encoding_Symbol_Length)
//S2N_DEL_ATT(kulong, FEC_OTI_Max_Number_of_Encoding_Symbols)
//S2N_DEL_ATT(kchar, FEC_OTI_Scheme_Specific_Info)
S2N_DEL_SUB(File, File)
// X2S_GOTO_PRT_NODE(FileDescription)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(FileDescription)
S2N_DEL_SELF(FileDescription)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(FileDescription)


S2N_FUN_DECL(BroadcastServiceDelivery)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BroadcastServiceDelivery)
/*X2S_GETNODECNT(BroadcastServiceDelivery)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BroadcastServiceDelivery)
X2S_SET_CNT(BroadcastServiceDelivery)*/
S2N_SET_NCNT(BroadcastServiceDelivery)
S2N_FOREACH_NCNT(BroadcastServiceDelivery)
S2N_CURLY_BRACE_L
S2N_SET_REC(BroadcastServiceDelivery)
// X2S_BRK_IF_BAD_NODE(BroadcastServiceDelivery)
S2N_DEL_SUB(BDSType, BDSType)
S2N_DEL_SUB(SessionDescription, SessionDescription)
S2N_DEL_SUB(FileDescription, FileDescription)
// X2S_GOTO_PRT_NODE(BroadcastServiceDelivery)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BroadcastServiceDelivery)
S2N_DEL_SELF(BroadcastServiceDelivery)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BroadcastServiceDelivery)


S2N_FUN_DECL(AccessType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AccessType)
/*X2S_GETNODECNT(AccessType)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AccessType)
X2S_SET_CNT(AccessType)*/
S2N_SET_NCNT(AccessType)
S2N_FOREACH_NCNT(AccessType)
S2N_CURLY_BRACE_L
S2N_SET_REC(AccessType)
// X2S_BRK_IF_BAD_NODE(AccessType)
S2N_DEL_SUB(BroadcastServiceDelivery, BroadcastServiceDelivery)
// X2S_GOTO_PRT_NODE(AccessType)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AccessType)
S2N_DEL_SELF(AccessType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AccessType)


S2N_FUN_DECL(KeyManagementSystem)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(KeyManagementSystem)
/*X2S_GETNODECNT(KeyManagementSystem)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(KeyManagementSystem)
X2S_SET_CNT(KeyManagementSystem)*/
S2N_SET_NCNT(KeyManagementSystem)
S2N_FOREACH_NCNT(KeyManagementSystem)
S2N_CURLY_BRACE_L
S2N_SET_REC(KeyManagementSystem)
// X2S_BRK_IF_BAD_NODE(KeyManagementSystem)
//S2N_DEL_ATT(kchar, kmsType)
//S2N_DEL_ATT(kchar, protectionType)
S2N_DEL_SUB(ProtectionKeyID, ProtectionKeyID)
S2N_DEL_SUB(PermissionsIssuerURI, PermissionsIssuerURI)
//S2N_DEL_SUB(TerminalBindingKeyID, TerminalBindingKeyID)
// X2S_GOTO_PRT_NODE(KeyManagementSystem)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(KeyManagementSystem)
S2N_DEL_SELF(KeyManagementSystem)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(KeyManagementSystem)


S2N_FUN_DECL(EncryptionType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(EncryptionType)
/*X2S_GETNODECNT(EncryptionType)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(EncryptionType)
X2S_SET_CNT(EncryptionType)*/
S2N_SET_NCNT(EncryptionType)
S2N_FOREACH_NCNT(EncryptionType)
S2N_CURLY_BRACE_L
S2N_SET_REC(EncryptionType)
// X2S_BRK_IF_BAD_NODE(EncryptionType)
//S2N_DEL_VAL(kchar, EncryptionType)
// X2S_GOTO_PRT_NODE(EncryptionType)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(EncryptionType)
S2N_DEL_SELF(EncryptionType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(EncryptionType)


S2N_FUN_DECL(DownloadFile)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(DownloadFile)
/*X2S_GETNODECNT(DownloadFile)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(DownloadFile)
X2S_SET_CNT(DownloadFile)*/
S2N_SET_NCNT(DownloadFile)
S2N_FOREACH_NCNT(DownloadFile)
S2N_CURLY_BRACE_L
S2N_SET_REC(DownloadFile)
// X2S_BRK_IF_BAD_NODE(DownloadFile)
S2N_DEL_SUB(MIMEType, MIMEType)
// X2S_GOTO_PRT_NODE(DownloadFile)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(DownloadFile)
S2N_DEL_SELF(DownloadFile)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(DownloadFile)


S2N_FUN_DECL(TerminalCapabilityRequirement)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TerminalCapabilityRequirement)
/*X2S_GETNODECNT(TerminalCapabilityRequirement)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TerminalCapabilityRequirement)
X2S_SET_CNT(TerminalCapabilityRequirement)*/
S2N_SET_NCNT(TerminalCapabilityRequirement)
S2N_FOREACH_NCNT(TerminalCapabilityRequirement)
S2N_CURLY_BRACE_L
S2N_SET_REC(TerminalCapabilityRequirement)
// X2S_BRK_IF_BAD_NODE(TerminalCapabilityRequirement)
S2N_DEL_SUB(Video, Video)
S2N_DEL_SUB(Audio, Audio)
S2N_DEL_SUB(DownloadFile, DownloadFile)
// X2S_GOTO_PRT_NODE(TerminalCapabilityRequirement)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TerminalCapabilityRequirement)
S2N_DEL_SELF(TerminalCapabilityRequirement)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TerminalCapabilityRequirement)


S2N_FUN_DECL(BandwidthRequirement)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BandwidthRequirement)
/*X2S_GETNODECNT(BandwidthRequirement)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BandwidthRequirement)
X2S_SET_CNT(BandwidthRequirement)*/
S2N_SET_NCNT(BandwidthRequirement)
S2N_FOREACH_NCNT(BandwidthRequirement)
S2N_CURLY_BRACE_L
S2N_SET_REC(BandwidthRequirement)
// X2S_BRK_IF_BAD_NODE(BandwidthRequirement)
//S2N_DEL_VAL(kchar, BandwidthRequirement)
// X2S_GOTO_PRT_NODE(BandwidthRequirement)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BandwidthRequirement)
S2N_DEL_SELF(BandwidthRequirement)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BandwidthRequirement)


S2N_FUN_DECL(ServiceClass)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceClass)
/*X2S_GETNODECNT(ServiceClass)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceClass)
X2S_SET_CNT(ServiceClass)*/
S2N_SET_NCNT(ServiceClass)
S2N_FOREACH_NCNT(ServiceClass)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceClass)
// X2S_BRK_IF_BAD_NODE(ServiceClass)
S2N_DEL_VAL(kchar, ServiceClass)
// X2S_GOTO_PRT_NODE(ServiceClass)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceClass)
S2N_DEL_SELF(ServiceClass)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceClass)

#if 1
S2N_FUN_DECL(Access)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Access)
/*X2S_GETNODECNT(Access)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Access)
X2S_SET_CNT(Access)*/
S2N_SET_NCNT(Access)
S2N_FOREACH_NCNT(Access)
S2N_CURLY_BRACE_L
S2N_SET_REC(Access)
// X2S_BRK_IF_BAD_NODE(Access)
S2N_DEL_SUB(AccessType, AccessType)
S2N_DEL_SUB(KeyManagementSystem, KeyManagementSystem)
S2N_DEL_SUB(EncryptionType, EncryptionType)
S2N_DEL_SUB(ServiceReference, ServiceReference)
S2N_DEL_SUB(ScheduleReference, ScheduleReference)
S2N_DEL_SUB(TerminalCapabilityRequirement, TerminalCapabilityRequirement)
S2N_DEL_SUB(BandwidthRequirement, BandwidthRequirement)
S2N_DEL_SUB(ServiceClass, ServiceClass)
S2N_DEL_SUB(PreviewDataReference, PreviewDataReference)
S2N_DEL_SUB(NotificationReception, NotificationReception)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(Access)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Access)
S2N_DEL_SELF(Access)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Access)
#endif

S2N_FUN_DECL(Type)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Type)
/*X2S_GETNODECNT(Type)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Type)
X2S_SET_CNT(Type)*/
S2N_SET_NCNT(Type)
S2N_FOREACH_NCNT(Type)
S2N_CURLY_BRACE_L
S2N_SET_REC(Type)
// X2S_BRK_IF_BAD_NODE(Type)
//S2N_DEL_VAL(kuchar, Type)
// X2S_GOTO_PRT_NODE(Type)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Type)
S2N_DEL_SELF(Type)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Type)


S2N_FUN_DECL(Name)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Name)
/*X2S_GETNODECNT(Name)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Name)
X2S_SET_CNT(Name)*/
S2N_SET_NCNT(Name)
S2N_FOREACH_NCNT(Name)
S2N_CURLY_BRACE_L
S2N_SET_REC(Name)
// X2S_BRK_IF_BAD_NODE(Name)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, Name)
// X2S_GOTO_PRT_NODE(Name)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Name)
S2N_DEL_SELF(Name)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Name)


S2N_FUN_DECL(StartTime)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(StartTime)
/*X2S_GETNODECNT(StartTime)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(StartTime)
X2S_SET_CNT(StartTime)*/
S2N_SET_NCNT(StartTime)
S2N_FOREACH_NCNT(StartTime)
S2N_CURLY_BRACE_L
S2N_SET_REC(StartTime)
// X2S_BRK_IF_BAD_NODE(StartTime)
S2N_DEL_VAL(kpchar, StartTime)
// X2S_GOTO_PRT_NODE(StartTime)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(StartTime)
S2N_DEL_SELF(StartTime)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(StartTime)


S2N_FUN_DECL(EndTime)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(EndTime)
/*X2S_GETNODECNT(EndTime)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(EndTime)
X2S_SET_CNT(EndTime)*/
S2N_SET_NCNT(EndTime)
S2N_FOREACH_NCNT(EndTime)
S2N_CURLY_BRACE_L
S2N_SET_REC(EndTime)
// X2S_BRK_IF_BAD_NODE(EndTime)
S2N_DEL_VAL(kpchar, EndTime)
// X2S_GOTO_PRT_NODE(EndTime)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(EndTime)
S2N_DEL_SELF(EndTime)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(EndTime)


S2N_FUN_DECL(Description)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Description)
/*X2S_GETNODECNT(Description)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Description)
X2S_SET_CNT(Description)*/
S2N_SET_NCNT(Description)
S2N_FOREACH_NCNT(Description)
S2N_CURLY_BRACE_L
S2N_SET_REC(Description)
// X2S_BRK_IF_BAD_NODE(Description)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_ATT(kpchar, text)
S2N_DEL_VAL(kpchar, Description)
// X2S_GOTO_PRT_NODE(Description)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Description)
S2N_DEL_SELF(Description)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Description)


S2N_FUN_DECL(Extension)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Extension)
/*X2S_GETNODECNT(Extension)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Extension)
X2S_SET_CNT(Extension)*/
S2N_SET_NCNT(Extension)
S2N_FOREACH_NCNT(Extension)
S2N_CURLY_BRACE_L
S2N_SET_REC(Extension)
// X2S_BRK_IF_BAD_NODE(Extension)
S2N_DEL_ATT(kpchar, url)
S2N_DEL_SUB(Description, Description)
// X2S_GOTO_PRT_NODE(Extension)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Extension)
S2N_DEL_SELF(Extension)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Extension)


S2N_FUN_DECL(PreviewDataReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PreviewDataReference)
/*X2S_GETNODECNT(PreviewDataReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PreviewDataReference)
X2S_SET_CNT(PreviewDataReference)*/
S2N_SET_NCNT(PreviewDataReference)
S2N_FOREACH_NCNT(PreviewDataReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(PreviewDataReference)
// X2S_BRK_IF_BAD_NODE(PreviewDataReference)
S2N_DEL_ATT(kpchar, idRef)
//S2N_DEL_ATT(kuchar, usage)
// X2S_GOTO_PRT_NODE(PreviewDataReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PreviewDataReference)
S2N_DEL_SELF(PreviewDataReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PreviewDataReference)


S2N_FUN_DECL(Genre)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Genre)
/*X2S_GETNODECNT(Genre)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Genre)
X2S_SET_CNT(Genre)*/
S2N_SET_NCNT(Genre)
S2N_FOREACH_NCNT(Genre)
S2N_CURLY_BRACE_L
S2N_SET_REC(Genre)
// X2S_BRK_IF_BAD_NODE(Genre)
S2N_DEL_ATT(kpchar, type)
S2N_DEL_ATT(kpchar, href)
S2N_DEL_VAL(kpchar, Genre)
// X2S_GOTO_PRT_NODE(Genre)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Genre)
S2N_DEL_SELF(Genre)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Genre)


S2N_FUN_DECL(TextLanguage)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TextLanguage)
/*X2S_GETNODECNT(TextLanguage)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TextLanguage)
X2S_SET_CNT(TextLanguage)*/
S2N_SET_NCNT(TextLanguage)
S2N_FOREACH_NCNT(TextLanguage)
S2N_CURLY_BRACE_L
S2N_SET_REC(TextLanguage)
// X2S_BRK_IF_BAD_NODE(TextLanguage)
S2N_DEL_ATT(kpchar, id)
S2N_DEL_ATT(kpchar, languageSDPTag)
S2N_DEL_VAL(kpchar, TextLanguage)
// X2S_GOTO_PRT_NODE(TextLanguage)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TextLanguage)
S2N_DEL_SELF(TextLanguage)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TextLanguage)


S2N_FUN_DECL(ParentalRating)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ParentalRating)
/*X2S_GETNODECNT(ParentalRating)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ParentalRating)
X2S_SET_CNT(ParentalRating)*/
S2N_SET_NCNT(ParentalRating)
S2N_FOREACH_NCNT(ParentalRating)
S2N_CURLY_BRACE_L
S2N_SET_REC(ParentalRating)
// X2S_BRK_IF_BAD_NODE(ParentalRating)
S2N_DEL_ATT(kpchar, ratingSystem)
S2N_DEL_VAL(kpchar, ParentalRating)
// X2S_GOTO_PRT_NODE(ParentalRating)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ParentalRating)
S2N_DEL_SELF(ParentalRating)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ParentalRating)


S2N_FUN_DECL(TargetUserProfile)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TargetUserProfile)
/*X2S_GETNODECNT(TargetUserProfile)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TargetUserProfile)
X2S_SET_CNT(TargetUserProfile)*/
S2N_SET_NCNT(TargetUserProfile)
S2N_FOREACH_NCNT(TargetUserProfile)
S2N_CURLY_BRACE_L
S2N_SET_REC(TargetUserProfile)
// X2S_BRK_IF_BAD_NODE(TargetUserProfile)
S2N_DEL_ATT(kpchar, attributeName)
S2N_DEL_ATT(kpchar, attributeValue)
// X2S_GOTO_PRT_NODE(TargetUserProfile)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TargetUserProfile)
S2N_DEL_SELF(TargetUserProfile)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TargetUserProfile)


S2N_FUN_DECL(ProtectionKeyID)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ProtectionKeyID)
/*X2S_GETNODECNT(ProtectionKeyID)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ProtectionKeyID)
X2S_SET_CNT(ProtectionKeyID)*/
S2N_SET_NCNT(ProtectionKeyID)
S2N_FOREACH_NCNT(ProtectionKeyID)
S2N_CURLY_BRACE_L
S2N_SET_REC(ProtectionKeyID)
// X2S_BRK_IF_BAD_NODE(ProtectionKeyID)
//S2N_DEL_ATT(kuchar, type)
S2N_DEL_VAL(kuchar, ProtectionKeyID)
// X2S_GOTO_PRT_NODE(ProtectionKeyID)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ProtectionKeyID)
S2N_DEL_SELF(ProtectionKeyID)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ProtectionKeyID)


S2N_FUN_DECL(AudioLanguage)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AudioLanguage)
/*X2S_GETNODECNT(AudioLanguage)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AudioLanguage)
X2S_SET_CNT(AudioLanguage)*/
S2N_SET_NCNT(AudioLanguage)
S2N_FOREACH_NCNT(AudioLanguage)
S2N_CURLY_BRACE_L
S2N_SET_REC(AudioLanguage)
// X2S_BRK_IF_BAD_NODE(AudioLanguage)
S2N_DEL_ATT(kpchar, id)
S2N_DEL_ATT(kpchar, languageSDPTag)
S2N_DEL_VAL(kpchar, AudioLanguage)
// X2S_GOTO_PRT_NODE(AudioLanguage)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AudioLanguage)
S2N_DEL_SELF(AudioLanguage)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AudioLanguage)


S2N_FUN_DECL(IPBroadcastDelivery)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(IPBroadcastDelivery)
/*X2S_GETNODECNT(IPBroadcastDelivery)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(IPBroadcastDelivery)
X2S_SET_CNT(IPBroadcastDelivery)*/
S2N_SET_NCNT(IPBroadcastDelivery)
S2N_FOREACH_NCNT(IPBroadcastDelivery)
S2N_CURLY_BRACE_L
S2N_SET_REC(IPBroadcastDelivery)
// X2S_BRK_IF_BAD_NODE(IPBroadcastDelivery)
//S2N_DEL_ATT(kuint, port)
S2N_DEL_ATT(kpchar, address)
// X2S_GOTO_PRT_NODE(IPBroadcastDelivery)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(IPBroadcastDelivery)
S2N_DEL_SELF(IPBroadcastDelivery)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(IPBroadcastDelivery)


S2N_FUN_DECL(RequestURL)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(RequestURL)
/*X2S_GETNODECNT(RequestURL)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(RequestURL)
X2S_SET_CNT(RequestURL)*/
S2N_SET_NCNT(RequestURL)
S2N_FOREACH_NCNT(RequestURL)
S2N_CURLY_BRACE_L
S2N_SET_REC(RequestURL)
// X2S_BRK_IF_BAD_NODE(RequestURL)
S2N_DEL_VAL(kpchar, RequestURL)
// X2S_GOTO_PRT_NODE(RequestURL)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(RequestURL)
S2N_DEL_SELF(RequestURL)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(RequestURL)


S2N_FUN_DECL(PollURL)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PollURL)
/*X2S_GETNODECNT(PollURL)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PollURL)
X2S_SET_CNT(PollURL)*/
S2N_SET_NCNT(PollURL)
S2N_FOREACH_NCNT(PollURL)
S2N_CURLY_BRACE_L
S2N_SET_REC(PollURL)
// X2S_BRK_IF_BAD_NODE(PollURL)
S2N_DEL_VAL(kpchar, PollURL)
// X2S_GOTO_PRT_NODE(PollURL)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PollURL)
S2N_DEL_SELF(PollURL)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PollURL)


S2N_FUN_DECL(NotificationReception)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(NotificationReception)
/*X2S_GETNODECNT(NotificationReception)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(NotificationReception)
X2S_SET_CNT(NotificationReception)*/
S2N_SET_NCNT(NotificationReception)
S2N_FOREACH_NCNT(NotificationReception)
S2N_CURLY_BRACE_L
S2N_SET_REC(NotificationReception)
// X2S_BRK_IF_BAD_NODE(NotificationReception)
S2N_DEL_SUB(IPBroadcastDelivery, IPBroadcastDelivery)
S2N_DEL_SUB(RequestURL, RequestURL)
S2N_DEL_SUB(PollURL, PollURL)
// X2S_GOTO_PRT_NODE(NotificationReception)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(NotificationReception)
S2N_DEL_SELF(NotificationReception)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(NotificationReception)


S2N_FUN_DECL(PurchaseItemReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PurchaseItemReference)
/*X2S_GETNODECNT(PurchaseItemReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PurchaseItemReference)
X2S_SET_CNT(PurchaseItemReference)*/
S2N_SET_NCNT(PurchaseItemReference)
S2N_FOREACH_NCNT(PurchaseItemReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(PurchaseItemReference)
// X2S_BRK_IF_BAD_NODE(PurchaseItemReference)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(PurchaseItemReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PurchaseItemReference)
S2N_DEL_SELF(PurchaseItemReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PurchaseItemReference)


S2N_FUN_DECL(PresentationWindowIDRef)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PresentationWindowIDRef)
/*X2S_GETNODECNT(PresentationWindowIDRef)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PresentationWindowIDRef)
X2S_SET_CNT(PresentationWindowIDRef)*/
S2N_SET_NCNT(PresentationWindowIDRef)
S2N_FOREACH_NCNT(PresentationWindowIDRef)
S2N_CURLY_BRACE_L
S2N_SET_REC(PresentationWindowIDRef)
// X2S_BRK_IF_BAD_NODE(PresentationWindowIDRef)
//S2N_DEL_VAL(kuint, PresentationWindowIDRef)
// X2S_GOTO_PRT_NODE(PresentationWindowIDRef)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PresentationWindowIDRef)
S2N_DEL_SELF(PresentationWindowIDRef)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PresentationWindowIDRef)


S2N_FUN_DECL(cc)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(cc)
/*X2S_GETNODECNT(cc)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(cc)
X2S_SET_CNT(cc)*/
S2N_SET_NCNT(cc)
S2N_FOREACH_NCNT(cc)
S2N_CURLY_BRACE_L
S2N_SET_REC(cc)
// X2S_BRK_IF_BAD_NODE(cc)
//S2N_DEL_VAL(kushort, cc)
// X2S_GOTO_PRT_NODE(cc)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(cc)
S2N_DEL_SELF(cc)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(cc)


S2N_FUN_DECL(name_area)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(name_area)
/*X2S_GETNODECNT(name_area)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(name_area)
X2S_SET_CNT(name_area)*/
S2N_SET_NCNT(name_area)
S2N_FOREACH_NCNT(name_area)
S2N_CURLY_BRACE_L
S2N_SET_REC(name_area)
// X2S_BRK_IF_BAD_NODE(name_area)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, name_area)
// X2S_GOTO_PRT_NODE(name_area)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(name_area)
S2N_DEL_SELF(name_area)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(name_area)


S2N_FUN_DECL(ZipCode)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ZipCode)
/*X2S_GETNODECNT(ZipCode)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ZipCode)
X2S_SET_CNT(ZipCode)*/
S2N_SET_NCNT(ZipCode)
S2N_FOREACH_NCNT(ZipCode)
S2N_CURLY_BRACE_L
S2N_SET_REC(ZipCode)
// X2S_BRK_IF_BAD_NODE(ZipCode)
S2N_DEL_VAL(kpchar, ZipCode)
// X2S_GOTO_PRT_NODE(ZipCode)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ZipCode)
S2N_DEL_SELF(ZipCode)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ZipCode)


S2N_FUN_DECL(PP2CellID)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PP2CellID)
/*X2S_GETNODECNT(PP2CellID)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PP2CellID)
X2S_SET_CNT(PP2CellID)*/
S2N_SET_NCNT(PP2CellID)
S2N_FOREACH_NCNT(PP2CellID)
S2N_CURLY_BRACE_L
S2N_SET_REC(PP2CellID)
// X2S_BRK_IF_BAD_NODE(PP2CellID)
//S2N_DEL_VAL(kint, PP2CellID)
// X2S_GOTO_PRT_NODE(PP2CellID)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PP2CellID)
S2N_DEL_SELF(PP2CellID)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PP2CellID)


S2N_FUN_DECL(CellArea)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(CellArea)
/*X2S_GETNODECNT(CellArea)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(CellArea)
X2S_SET_CNT(CellArea)*/
S2N_SET_NCNT(CellArea)
S2N_FOREACH_NCNT(CellArea)
S2N_CURLY_BRACE_L
S2N_SET_REC(CellArea)
// X2S_BRK_IF_BAD_NODE(CellArea)
//S2N_DEL_ATT(kushort, value)
S2N_DEL_SUB(PP2CellID, PP2CellID)
// X2S_GOTO_PRT_NODE(CellArea)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(CellArea)
S2N_DEL_SELF(CellArea)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(CellArea)


S2N_FUN_DECL(CellTargetArea)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(CellTargetArea)
/*X2S_GETNODECNT(CellTargetArea)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(CellTargetArea)
X2S_SET_CNT(CellTargetArea)*/
S2N_SET_NCNT(CellTargetArea)
S2N_FOREACH_NCNT(CellTargetArea)
S2N_CURLY_BRACE_L
S2N_SET_REC(CellTargetArea)
// X2S_BRK_IF_BAD_NODE(CellTargetArea)
//S2N_DEL_ATT(kuchar, type)
S2N_DEL_SUB(CellArea, CellArea)
// X2S_GOTO_PRT_NODE(CellTargetArea)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(CellTargetArea)
S2N_DEL_SELF(CellTargetArea)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(CellTargetArea)


S2N_FUN_DECL(TargetArea)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TargetArea)
/*X2S_GETNODECNT(TargetArea)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TargetArea)
X2S_SET_CNT(TargetArea)*/
S2N_SET_NCNT(TargetArea)
S2N_FOREACH_NCNT(TargetArea)
S2N_CURLY_BRACE_L
S2N_SET_REC(TargetArea)
// X2S_BRK_IF_BAD_NODE(TargetArea)
S2N_DEL_SUB(shape, shape)
S2N_DEL_SUB(cc, cc)
S2N_DEL_SUB(name_area, name_area)
S2N_DEL_SUB(ZipCode, ZipCode)
S2N_DEL_SUB(CellTargetArea, CellTargetArea)
// X2S_GOTO_PRT_NODE(TargetArea)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TargetArea)
S2N_DEL_SELF(TargetArea)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TargetArea)


S2N_FUN_DECL(hor_acc)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(hor_acc)
/*X2S_GETNODECNT(hor_acc)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(hor_acc)
X2S_SET_CNT(hor_acc)*/
S2N_SET_NCNT(hor_acc)
S2N_FOREACH_NCNT(hor_acc)
S2N_CURLY_BRACE_L
S2N_SET_REC(hor_acc)
// X2S_BRK_IF_BAD_NODE(hor_acc)
S2N_DEL_VAL(kpchar, hor_acc)
// X2S_GOTO_PRT_NODE(hor_acc)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(hor_acc)
S2N_DEL_SELF(hor_acc)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(hor_acc)


S2N_FUN_DECL(BroadcastArea)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BroadcastArea)
/*X2S_GETNODECNT(BroadcastArea)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BroadcastArea)
X2S_SET_CNT(BroadcastArea)*/
S2N_SET_NCNT(BroadcastArea)
S2N_FOREACH_NCNT(BroadcastArea)
S2N_CURLY_BRACE_L
S2N_SET_REC(BroadcastArea)
// X2S_BRK_IF_BAD_NODE(BroadcastArea)
//S2N_DEL_ATT(kbool, polarity)
S2N_DEL_SUB(TargetArea, TargetArea)
S2N_DEL_SUB(hor_acc, hor_acc)
// X2S_GOTO_PRT_NODE(BroadcastArea)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BroadcastArea)
S2N_DEL_SELF(BroadcastArea)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BroadcastArea)


S2N_FUN_DECL(Country)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Country)
/*X2S_GETNODECNT(Country)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Country)
X2S_SET_CNT(Country)*/
S2N_SET_NCNT(Country)
S2N_FOREACH_NCNT(Country)
S2N_CURLY_BRACE_L
S2N_SET_REC(Country)
// X2S_BRK_IF_BAD_NODE(Country)
S2N_DEL_VAL(kpchar, Country)
// X2S_GOTO_PRT_NODE(Country)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Country)
S2N_DEL_SELF(Country)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Country)


S2N_FUN_DECL(Language)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Language)
/*X2S_GETNODECNT(Language)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Language)
X2S_SET_CNT(Language)*/
S2N_SET_NCNT(Language)
S2N_FOREACH_NCNT(Language)
S2N_CURLY_BRACE_L
S2N_SET_REC(Language)
// X2S_BRK_IF_BAD_NODE(Language)
S2N_DEL_VAL(kpchar, Language)
// X2S_GOTO_PRT_NODE(Language)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Language)
S2N_DEL_SELF(Language)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Language)


S2N_FUN_DECL(PreviewDataIDRef)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PreviewDataIDRef)
/*X2S_GETNODECNT(PreviewDataIDRef)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PreviewDataIDRef)
X2S_SET_CNT(PreviewDataIDRef)*/
S2N_SET_NCNT(PreviewDataIDRef)
S2N_FOREACH_NCNT(PreviewDataIDRef)
S2N_CURLY_BRACE_L
S2N_SET_REC(PreviewDataIDRef)
// X2S_BRK_IF_BAD_NODE(PreviewDataIDRef)
S2N_DEL_VAL(kpchar, PreviewDataIDRef)
// X2S_GOTO_PRT_NODE(PreviewDataIDRef)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PreviewDataIDRef)
S2N_DEL_SELF(PreviewDataIDRef)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PreviewDataIDRef)


S2N_FUN_DECL(TermsOfUseText)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TermsOfUseText)
/*X2S_GETNODECNT(TermsOfUseText)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TermsOfUseText)
X2S_SET_CNT(TermsOfUseText)*/
S2N_SET_NCNT(TermsOfUseText)
S2N_FOREACH_NCNT(TermsOfUseText)
S2N_CURLY_BRACE_L
S2N_SET_REC(TermsOfUseText)
// X2S_BRK_IF_BAD_NODE(TermsOfUseText)
S2N_DEL_VAL(kpchar, TermsOfUseText)
// X2S_GOTO_PRT_NODE(TermsOfUseText)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TermsOfUseText)
S2N_DEL_SELF(TermsOfUseText)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TermsOfUseText)


S2N_FUN_DECL(TermsOfUse)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TermsOfUse)
/*X2S_GETNODECNT(TermsOfUse)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TermsOfUse)
X2S_SET_CNT(TermsOfUse)*/
S2N_SET_NCNT(TermsOfUse)
S2N_FOREACH_NCNT(TermsOfUse)
S2N_CURLY_BRACE_L
S2N_SET_REC(TermsOfUse)
// X2S_BRK_IF_BAD_NODE(TermsOfUse)
//S2N_DEL_ATT(kuchar, type)
S2N_DEL_ATT(kpchar, id)
//S2N_DEL_ATT(kbool, userConsentRequired)
S2N_DEL_SUB(Country, Country)
S2N_DEL_SUB(Language, Language)
S2N_DEL_SUB(PreviewDataIDRef, PreviewDataIDRef)
S2N_DEL_SUB(TermsOfUseText, TermsOfUseText)
// X2S_GOTO_PRT_NODE(TermsOfUse)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TermsOfUse)
S2N_DEL_SELF(TermsOfUse)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TermsOfUse)

S2N_FUN_DECL(ServiceComponentID)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceComponentID)
/*X2S_GETNODECNT(ServiceComponentID)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceComponentID)
X2S_SET_CNT(ServiceComponentID)*/
S2N_SET_NCNT(ServiceComponentID)
S2N_FOREACH_NCNT(ServiceComponentID)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceComponentID)
// X2S_BRK_IF_BAD_NODE(ServiceComponentID)
// S2N_DEL_VAL(kuint, ServiceComponentID)
// X2S_GOTO_PRT_NODE(ServiceComponentID)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceComponentID)
S2N_DEL_SELF(ServiceComponentID)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceComponentID)


S2N_FUN_DECL(channelId)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(channelId)
/*X2S_GETNODECNT(channelId)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(channelId)
X2S_SET_CNT(channelId)*/
S2N_SET_NCNT(channelId)
S2N_FOREACH_NCNT(channelId)
S2N_CURLY_BRACE_L
S2N_SET_REC(channelId)
// X2S_BRK_IF_BAD_NODE(channelId)
// X2S_GOTO_PRT_NODE(channelId)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(channelId)
S2N_DEL_SELF(channelId)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(channelId)


S2N_FUN_DECL(BroadcastPara)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BroadcastPara)
/*X2S_GETNODECNT(BroadcastPara)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BroadcastPara)
X2S_SET_CNT(BroadcastPara)*/
S2N_SET_NCNT(BroadcastPara)
S2N_FOREACH_NCNT(BroadcastPara)
S2N_CURLY_BRACE_L
S2N_SET_REC(BroadcastPara)
// X2S_BRK_IF_BAD_NODE(BroadcastPara)
S2N_DEL_SUB(channelId, channelId)
S2N_DEL_SUB(ServiceComponentID, ServiceComponentID)
// X2S_GOTO_PRT_NODE(BroadcastPara)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BroadcastPara)
S2N_DEL_SELF(BroadcastPara)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BroadcastPara)


S2N_FUN_DECL(PrivateExt)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PrivateExt)
/*X2S_GETNODECNT(PrivateExt)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PrivateExt)
X2S_SET_CNT(PrivateExt)*/
S2N_SET_NCNT(PrivateExt)
S2N_FOREACH_NCNT(PrivateExt)
S2N_CURLY_BRACE_L
S2N_SET_REC(PrivateExt)
// X2S_BRK_IF_BAD_NODE(PrivateExt)
S2N_DEL_VAL(kpchar, PrivateExt)
S2N_DEL_SUB(BroadcastPara, BroadcastPara)
// X2S_GOTO_PRT_NODE(PrivateExt)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PrivateExt)
S2N_DEL_SELF(PrivateExt)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PrivateExt)


S2N_FUN_DECL(ServiceReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceReference)
/*X2S_GETNODECNT(ServiceReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceReference)
X2S_SET_CNT(ServiceReference)*/
S2N_SET_NCNT(ServiceReference)
S2N_FOREACH_NCNT(ServiceReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceReference)
// X2S_BRK_IF_BAD_NODE(ServiceReference)
S2N_DEL_ATT(kpchar, idRef)
S2N_DEL_ATT(kpchar, audioLanguageIDRef)
S2N_DEL_ATT(kpchar, textLanguageIDRef)
//S2N_DEL_ATT(kushort, weight)
// X2S_GOTO_PRT_NODE(ServiceReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceReference)
S2N_DEL_SELF(ServiceReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceReference)


S2N_FUN_DECL(Length)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Length)
/*X2S_GETNODECNT(Length)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Length)
X2S_SET_CNT(Length)*/
S2N_SET_NCNT(Length)
S2N_FOREACH_NCNT(Length)
S2N_CURLY_BRACE_L
S2N_SET_REC(Length)
// X2S_BRK_IF_BAD_NODE(Length)
//S2N_DEL_VAL(kuint, Length)
// X2S_GOTO_PRT_NODE(Length)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Length)
S2N_DEL_SELF(Length)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Length)

#if 1
S2N_FUN_DECL(Content)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Content)
/*X2S_GETNODECNT(Content)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Content)
X2S_SET_CNT(Content)*/
S2N_SET_NCNT(Content)
S2N_FOREACH_NCNT(Content)
S2N_CURLY_BRACE_L
S2N_SET_REC(Content)
// X2S_BRK_IF_BAD_NODE(Content)
S2N_DEL_ATT(kpchar, globalContentID)
//S2N_DEL_ATT(kbool, emergency)
//S2N_DEL_ATT(kbool, serviceContentProtection)
S2N_DEL_ATT(kpchar, baseCID)
S2N_DEL_SUB(ServiceReference, ServiceReference)
S2N_DEL_SUB(ProtectionKeyID, ProtectionKeyID)
S2N_DEL_SUB(Name, Name)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(StartTime, StartTime)
S2N_DEL_SUB(EndTime, EndTime)
S2N_DEL_SUB(AudioLanguage, AudioLanguage)
S2N_DEL_SUB(TextLanguage, TextLanguage)
S2N_DEL_SUB(Length, Length)
S2N_DEL_SUB(ParentalRating, ParentalRating)
S2N_DEL_SUB(TargetUserProfile, TargetUserProfile)
S2N_DEL_SUB(Genre, Genre)
S2N_DEL_SUB(Extension, Extension)
S2N_DEL_SUB(PreviewDataReference, PreviewDataReference)
S2N_DEL_SUB(BroadcastArea, BroadcastArea)
S2N_DEL_SUB(TermsOfUse, TermsOfUse)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(Content)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Content)
S2N_DEL_SELF(Content)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Content)
#endif

S2N_FUN_DECL(InteractivityType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(InteractivityType)
/*X2S_GETNODECNT(InteractivityType)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(InteractivityType)
X2S_SET_CNT(InteractivityType)*/
S2N_SET_NCNT(InteractivityType)
S2N_FOREACH_NCNT(InteractivityType)
S2N_CURLY_BRACE_L
S2N_SET_REC(InteractivityType)
// X2S_BRK_IF_BAD_NODE(InteractivityType)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, InteractivityType)
// X2S_GOTO_PRT_NODE(InteractivityType)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(InteractivityType)
S2N_DEL_SELF(InteractivityType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(InteractivityType)


S2N_FUN_DECL(InteractivityWindow)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(InteractivityWindow)
/*X2S_GETNODECNT(InteractivityWindow)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(InteractivityWindow)
X2S_SET_CNT(InteractivityWindow)*/
S2N_SET_NCNT(InteractivityWindow)
S2N_FOREACH_NCNT(InteractivityWindow)
S2N_CURLY_BRACE_L
S2N_SET_REC(InteractivityWindow)
// X2S_BRK_IF_BAD_NODE(InteractivityWindow)
//S2N_DEL_ATT(kuint, startTime)
//S2N_DEL_ATT(kuint, endTime)
S2N_DEL_VAL(kpchar, InteractivityWindow)
// X2S_GOTO_PRT_NODE(InteractivityWindow)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(InteractivityWindow)
S2N_DEL_SELF(InteractivityWindow)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(InteractivityWindow)


S2N_FUN_DECL(InteractiveDelivery)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(InteractiveDelivery)
/*X2S_GETNODECNT(InteractiveDelivery)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(InteractiveDelivery)
X2S_SET_CNT(InteractiveDelivery)*/
S2N_SET_NCNT(InteractiveDelivery)
S2N_FOREACH_NCNT(InteractiveDelivery)
S2N_CURLY_BRACE_L
S2N_SET_REC(InteractiveDelivery)
// X2S_BRK_IF_BAD_NODE(InteractiveDelivery)
//S2N_DEL_ATT(kchar, interactivtyMediaURL)
//S2N_DEL_ATT(kbool, pushDelivery)
S2N_DEL_VAL(kpchar, InteractiveDelivery)
// X2S_GOTO_PRT_NODE(InteractiveDelivery)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(InteractiveDelivery)
S2N_DEL_SELF(InteractiveDelivery)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(InteractiveDelivery)


S2N_FUN_DECL(BackOffTiming)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BackOffTiming)
/*X2S_GETNODECNT(BackOffTiming)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BackOffTiming)
X2S_SET_CNT(BackOffTiming)*/
S2N_SET_NCNT(BackOffTiming)
S2N_FOREACH_NCNT(BackOffTiming)
S2N_CURLY_BRACE_L
S2N_SET_REC(BackOffTiming)
// X2S_BRK_IF_BAD_NODE(BackOffTiming)
//S2N_DEL_ATT(kint, offsetTime)
//S2N_DEL_ATT(kint, randomTime)
// X2S_GOTO_PRT_NODE(BackOffTiming)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BackOffTiming)
S2N_DEL_SELF(BackOffTiming)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BackOffTiming)


#if 1
S2N_FUN_DECL(InteractivityData)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(InteractivityData)
/*X2S_GETNODECNT(InteractivityData)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(InteractivityData)
X2S_SET_CNT(InteractivityData)*/
S2N_SET_NCNT(InteractivityData)
S2N_FOREACH_NCNT(InteractivityData)
S2N_CURLY_BRACE_L
S2N_SET_REC(InteractivityData)
// X2S_BRK_IF_BAD_NODE(InteractivityData)
//S2N_DEL_ATT(kbool, preListenIndicator)
S2N_DEL_ATT(kpchar, interactivityMediaDocumentPointer)
S2N_DEL_SUB(InteractivityType, InteractivityType)
S2N_DEL_SUB(ServiceReference, ServiceReference)
S2N_DEL_SUB(ContentReference, ContentReference)
S2N_DEL_SUB(ScheduleReference, ScheduleReference)
S2N_DEL_SUB(InteractivityWindow, InteractivityWindow)
S2N_DEL_SUB(InteractiveDelivery, InteractiveDelivery)
S2N_DEL_SUB(Extension, Extension)
S2N_DEL_SUB(BackOffTiming, BackOffTiming)
S2N_DEL_SUB(TermsOfUse, TermsOfUse)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(InteractivityData)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(InteractivityData)
S2N_DEL_SELF(InteractivityData)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(InteractivityData)
#endif

S2N_FUN_DECL(ContactInfo)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ContactInfo)
/*X2S_GETNODECNT(ContactInfo)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ContactInfo)
X2S_SET_CNT(ContactInfo)*/
S2N_SET_NCNT(ContactInfo)
S2N_FOREACH_NCNT(ContactInfo)
S2N_CURLY_BRACE_L
S2N_SET_REC(ContactInfo)
// X2S_BRK_IF_BAD_NODE(ContactInfo)
S2N_DEL_VAL(kpchar, ContactInfo)
// X2S_GOTO_PRT_NODE(ContactInfo)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ContactInfo)
S2N_DEL_SELF(ContactInfo)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ContactInfo)


S2N_FUN_DECL(PortalURL)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PortalURL)
/*X2S_GETNODECNT(PortalURL)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PortalURL)
X2S_SET_CNT(PortalURL)*/
S2N_SET_NCNT(PortalURL)
S2N_FOREACH_NCNT(PortalURL)
S2N_CURLY_BRACE_L
S2N_SET_REC(PortalURL)
// X2S_BRK_IF_BAD_NODE(PortalURL)
//S2N_DEL_ATT(kuchar, supportedService)
S2N_DEL_VAL(kpchar, PortalURL)
// X2S_GOTO_PRT_NODE(PortalURL)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PortalURL)
S2N_DEL_SELF(PortalURL)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PortalURL)


S2N_FUN_DECL(PurchaseURL)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PurchaseURL)
/*X2S_GETNODECNT(PurchaseURL)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PurchaseURL)
X2S_SET_CNT(PurchaseURL)*/
S2N_SET_NCNT(PurchaseURL)
S2N_FOREACH_NCNT(PurchaseURL)
S2N_CURLY_BRACE_L
S2N_SET_REC(PurchaseURL)
// X2S_BRK_IF_BAD_NODE(PurchaseURL)
S2N_DEL_VAL(kpchar, PurchaseURL)
// X2S_GOTO_PRT_NODE(PurchaseURL)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PurchaseURL)
S2N_DEL_SELF(PurchaseURL)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PurchaseURL)

#if 1
S2N_FUN_DECL(PurchaseChannel)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PurchaseChannel)
/*X2S_GETNODECNT(PurchaseChannel)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PurchaseChannel)
X2S_SET_CNT(PurchaseChannel)*/
S2N_SET_NCNT(PurchaseChannel)
S2N_FOREACH_NCNT(PurchaseChannel)
S2N_CURLY_BRACE_L
S2N_SET_REC(PurchaseChannel)
// X2S_BRK_IF_BAD_NODE(PurchaseChannel)
S2N_DEL_ATT(kpchar, rightsIssuerURI)
S2N_DEL_SUB(Name, Name)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(ContactInfo, ContactInfo)
S2N_DEL_SUB(PortalURL, PortalURL)
S2N_DEL_SUB(PurchaseURL, PurchaseURL)
S2N_DEL_SUB(Extension, Extension)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(PurchaseChannel)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PurchaseChannel)
S2N_DEL_SELF(PurchaseChannel)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PurchaseChannel)
#endif

S2N_FUN_DECL(MonetaryPrice)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(MonetaryPrice)
/*X2S_GETNODECNT(MonetaryPrice)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(MonetaryPrice)
X2S_SET_CNT(MonetaryPrice)*/
S2N_SET_NCNT(MonetaryPrice)
S2N_FOREACH_NCNT(MonetaryPrice)
S2N_CURLY_BRACE_L
S2N_SET_REC(MonetaryPrice)
// X2S_BRK_IF_BAD_NODE(MonetaryPrice)
S2N_DEL_ATT(kpchar, currency)
S2N_DEL_VAL(kpchar, MonetaryPrice)
// X2S_GOTO_PRT_NODE(MonetaryPrice)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(MonetaryPrice)
S2N_DEL_SELF(MonetaryPrice)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(MonetaryPrice)


S2N_FUN_DECL(TotalNumberToken)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TotalNumberToken)
/*X2S_GETNODECNT(TotalNumberToken)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TotalNumberToken)
X2S_SET_CNT(TotalNumberToken)*/
S2N_SET_NCNT(TotalNumberToken)
S2N_FOREACH_NCNT(TotalNumberToken)
S2N_CURLY_BRACE_L
S2N_SET_REC(TotalNumberToken)
// X2S_BRK_IF_BAD_NODE(TotalNumberToken)
//S2N_DEL_ATT(kuchar, tokenType)
//S2N_DEL_ATT(kushort, consumptionAmount)
//S2N_DEL_ATT(kuchar, consumptionUnit)
//S2N_DEL_ATT(kushort, maxReplay)
//S2N_DEL_VAL(kushort, TotalNumberToken)
// X2S_GOTO_PRT_NODE(TotalNumberToken)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TotalNumberToken)
S2N_DEL_SELF(TotalNumberToken)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TotalNumberToken)


S2N_FUN_DECL(SubscriptionPeriod)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SubscriptionPeriod)
/*X2S_GETNODECNT(SubscriptionPeriod)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(SubscriptionPeriod)
X2S_SET_CNT(SubscriptionPeriod)*/
S2N_SET_NCNT(SubscriptionPeriod)
S2N_FOREACH_NCNT(SubscriptionPeriod)
S2N_CURLY_BRACE_L
S2N_SET_REC(SubscriptionPeriod)
// X2S_BRK_IF_BAD_NODE(SubscriptionPeriod)
S2N_DEL_VAL(kpchar, SubscriptionPeriod)
// X2S_GOTO_PRT_NODE(SubscriptionPeriod)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SubscriptionPeriod)
S2N_DEL_SELF(SubscriptionPeriod)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SubscriptionPeriod)


S2N_FUN_DECL(PriceInfo)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PriceInfo)
/*X2S_GETNODECNT(PriceInfo)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PriceInfo)
X2S_SET_CNT(PriceInfo)*/
S2N_SET_NCNT(PriceInfo)
S2N_FOREACH_NCNT(PriceInfo)
S2N_CURLY_BRACE_L
S2N_SET_REC(PriceInfo)
// X2S_BRK_IF_BAD_NODE(PriceInfo)
//S2N_DEL_ATT(kuchar, subscriptionType)
S2N_DEL_SUB(MonetaryPrice, MonetaryPrice)
S2N_DEL_SUB(TotalNumberToken, TotalNumberToken)
S2N_DEL_SUB(SubscriptionPeriod, SubscriptionPeriod)
// X2S_GOTO_PRT_NODE(PriceInfo)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PriceInfo)
S2N_DEL_SELF(PriceInfo)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PriceInfo)


S2N_FUN_DECL(Title)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Title)
/*X2S_GETNODECNT(Title)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Title)
X2S_SET_CNT(Title)*/
S2N_SET_NCNT(Title)
S2N_FOREACH_NCNT(Title)
S2N_CURLY_BRACE_L
S2N_SET_REC(Title)
// X2S_BRK_IF_BAD_NODE(Title)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, Title)
// X2S_GOTO_PRT_NODE(Title)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Title)
S2N_DEL_SELF(Title)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Title)


S2N_FUN_DECL(PromotionExtension)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PromotionExtension)
/*X2S_GETNODECNT(PromotionExtension)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PromotionExtension)
X2S_SET_CNT(PromotionExtension)*/
S2N_SET_NCNT(PromotionExtension)
S2N_FOREACH_NCNT(PromotionExtension)
S2N_CURLY_BRACE_L
S2N_SET_REC(PromotionExtension)
// X2S_BRK_IF_BAD_NODE(PromotionExtension)
S2N_DEL_ATT(kpchar, url)
S2N_DEL_SUB(Description, Description)
// X2S_GOTO_PRT_NODE(PromotionExtension)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PromotionExtension)
S2N_DEL_SELF(PromotionExtension)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PromotionExtension)


S2N_FUN_DECL(PromotionInfo)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PromotionInfo)
/*X2S_GETNODECNT(PromotionInfo)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PromotionInfo)
X2S_SET_CNT(PromotionInfo)*/
S2N_SET_NCNT(PromotionInfo)
S2N_FOREACH_NCNT(PromotionInfo)
S2N_CURLY_BRACE_L
S2N_SET_REC(PromotionInfo)
// X2S_BRK_IF_BAD_NODE(PromotionInfo)
S2N_DEL_ATT(kpchar, id)
//S2N_DEL_ATT(kuint, validFrom)
//S2N_DEL_ATT(kuint, validTo)
S2N_DEL_SUB(Title, Title)
S2N_DEL_SUB(TargetUserProfile, TargetUserProfile)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(PromotionExtension, PromotionExtension)
// X2S_GOTO_PRT_NODE(PromotionInfo)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PromotionInfo)
S2N_DEL_SELF(PromotionInfo)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PromotionInfo)


S2N_FUN_DECL(PurchaseChannelReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PurchaseChannelReference)
/*X2S_GETNODECNT(PurchaseChannelReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PurchaseChannelReference)
X2S_SET_CNT(PurchaseChannelReference)*/
S2N_SET_NCNT(PurchaseChannelReference)
S2N_FOREACH_NCNT(PurchaseChannelReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(PurchaseChannelReference)
// X2S_BRK_IF_BAD_NODE(PurchaseChannelReference)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(PurchaseChannelReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PurchaseChannelReference)
S2N_DEL_SELF(PurchaseChannelReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PurchaseChannelReference)


S2N_FUN_DECL(PreviwDataReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PreviwDataReference)
/*X2S_GETNODECNT(PreviwDataReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PreviwDataReference)
X2S_SET_CNT(PreviwDataReference)*/
S2N_SET_NCNT(PreviwDataReference)
S2N_FOREACH_NCNT(PreviwDataReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(PreviwDataReference)
// X2S_BRK_IF_BAD_NODE(PreviwDataReference)
S2N_DEL_ATT(kpchar, idRef)
//S2N_DEL_ATT(kuchar, usage)
S2N_DEL_VAL(kpchar, PreviwDataReference)
// X2S_GOTO_PRT_NODE(PreviwDataReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PreviwDataReference)
S2N_DEL_SELF(PreviwDataReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PreviwDataReference)


#if 1
S2N_FUN_DECL(PurchaseData)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PurchaseData)
/*X2S_GETNODECNT(PurchaseData)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PurchaseData)
X2S_SET_CNT(PurchaseData)*/
S2N_SET_NCNT(PurchaseData)
S2N_FOREACH_NCNT(PurchaseData)
S2N_CURLY_BRACE_L
S2N_SET_REC(PurchaseData)
// X2S_BRK_IF_BAD_NODE(PurchaseData)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(PriceInfo, PriceInfo)
S2N_DEL_SUB(PromotionInfo, PromotionInfo)
S2N_DEL_SUB(Extension, Extension)
S2N_DEL_SUB(PurchaseItemReference, PurchaseItemReference)
S2N_DEL_SUB(PurchaseChannelReference, PurchaseChannelReference)
S2N_DEL_SUB(PreviwDataReference, PreviwDataReference)
S2N_DEL_SUB(TermsOfUse, TermsOfUse)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(PurchaseData)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PurchaseData)
S2N_DEL_SELF(PurchaseData)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PurchaseData)
#endif


S2N_FUN_DECL(DistributionWindowID)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(DistributionWindowID)
/*X2S_GETNODECNT(DistributionWindowID)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(DistributionWindowID)
X2S_SET_CNT(DistributionWindowID)*/
S2N_SET_NCNT(DistributionWindowID)
S2N_FOREACH_NCNT(DistributionWindowID)
S2N_CURLY_BRACE_L
S2N_SET_REC(DistributionWindowID)
// X2S_BRK_IF_BAD_NODE(DistributionWindowID)
// S2N_DEL_VAL(kuint, DistributionWindowID)
// X2S_GOTO_PRT_NODE(DistributionWindowID)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(DistributionWindowID)
S2N_DEL_SELF(DistributionWindowID)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(DistributionWindowID)


S2N_FUN_DECL(ScheduleReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ScheduleReference)
/*X2S_GETNODECNT(ScheduleReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ScheduleReference)
X2S_SET_CNT(ScheduleReference)*/
S2N_SET_NCNT(ScheduleReference)
S2N_FOREACH_NCNT(ScheduleReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(ScheduleReference)
// X2S_BRK_IF_BAD_NODE(ScheduleReference)
S2N_DEL_ATT(kpchar, idRef)
S2N_DEL_SUB(PresentationWindowIDRef, PresentationWindowIDRef)
S2N_DEL_SUB(DistributionWindowID, DistributionWindowID)
// X2S_GOTO_PRT_NODE(ScheduleReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ScheduleReference)
S2N_DEL_SELF(ScheduleReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ScheduleReference)


S2N_FUN_DECL(Dependencies)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Dependencies)
/*X2S_GETNODECNT(Dependencies)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Dependencies)
X2S_SET_CNT(Dependencies)*/
S2N_SET_NCNT(Dependencies)
S2N_FOREACH_NCNT(Dependencies)
S2N_CURLY_BRACE_L
S2N_SET_REC(Dependencies)
// X2S_BRK_IF_BAD_NODE(Dependencies)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(Dependencies)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Dependencies)
S2N_DEL_SELF(Dependencies)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Dependencies)


S2N_FUN_DECL(Exclusions)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Exclusions)
/*X2S_GETNODECNT(Exclusions)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Exclusions)
X2S_SET_CNT(Exclusions)*/
S2N_SET_NCNT(Exclusions)
S2N_FOREACH_NCNT(Exclusions)
S2N_CURLY_BRACE_L
S2N_SET_REC(Exclusions)
// X2S_BRK_IF_BAD_NODE(Exclusions)
S2N_DEL_ATT(kpchar, idRef)
// X2S_GOTO_PRT_NODE(Exclusions)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Exclusions)
S2N_DEL_SELF(Exclusions)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Exclusions)

#if 1
S2N_FUN_DECL(PurchaseItem)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PurchaseItem)
/*X2S_GETNODECNT(PurchaseItem)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PurchaseItem)
X2S_SET_CNT(PurchaseItem)*/
S2N_SET_NCNT(PurchaseItem)
S2N_FOREACH_NCNT(PurchaseItem)
S2N_CURLY_BRACE_L
S2N_SET_REC(PurchaseItem)
// X2S_BRK_IF_BAD_NODE(PurchaseItem)
S2N_DEL_ATT(kpchar, globalPurchaseItemID)
//S2N_DEL_ATT(kuint, binaryPurchaseItemID)
//S2N_DEL_ATT(kushort, weight)
//S2N_DEL_ATT(kbool, closed)
S2N_DEL_SUB(ServiceReference, ServiceReference)
S2N_DEL_SUB(ScheduleReference, ScheduleReference)
S2N_DEL_SUB(ContentReference, ContentReference)
S2N_DEL_SUB(PurchaseItemReference, PurchaseItemReference)
S2N_DEL_SUB(ProtectionKeyID, ProtectionKeyID)
S2N_DEL_SUB(Name, Name)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(StartTime, StartTime)
S2N_DEL_SUB(EndTime, EndTime)
S2N_DEL_SUB(Extension, Extension)
S2N_DEL_SUB(ParentalRating, ParentalRating)
S2N_DEL_SUB(Dependencies, Dependencies)
S2N_DEL_SUB(Exclusions, Exclusions)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(PurchaseItem)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PurchaseItem)
S2N_DEL_SELF(PurchaseItem)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PurchaseItem)
#endif


S2N_FUN_DECL(SMIL)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SMIL)
/*X2S_GETNODECNT(SMIL)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(SMIL)
X2S_SET_CNT(SMIL)*/
S2N_SET_NCNT(SMIL)
S2N_FOREACH_NCNT(SMIL)
S2N_CURLY_BRACE_L
S2N_SET_REC(SMIL)
// X2S_BRK_IF_BAD_NODE(SMIL)
//S2N_DEL_ATT(kuchar, type)
S2N_DEL_VAL(kpchar, SMIL)
// X2S_GOTO_PRT_NODE(SMIL)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SMIL)
S2N_DEL_SELF(SMIL)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SMIL)


S2N_FUN_DECL(VideoURI)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(VideoURI)
/*X2S_GETNODECNT(VideoURI)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(VideoURI)
X2S_SET_CNT(VideoURI)*/
S2N_SET_NCNT(VideoURI)
S2N_FOREACH_NCNT(VideoURI)
S2N_CURLY_BRACE_L
S2N_SET_REC(VideoURI)
// X2S_BRK_IF_BAD_NODE(VideoURI)
S2N_DEL_VAL(kpchar, VideoURI)
// X2S_GOTO_PRT_NODE(VideoURI)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(VideoURI)
S2N_DEL_SELF(VideoURI)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(VideoURI)


S2N_FUN_DECL(MIMEType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(MIMEType)
/*X2S_GETNODECNT(MIMEType)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(MIMEType)
X2S_SET_CNT(MIMEType)*/
S2N_SET_NCNT(MIMEType)
S2N_FOREACH_NCNT(MIMEType)
S2N_CURLY_BRACE_L
S2N_SET_REC(MIMEType)
// X2S_BRK_IF_BAD_NODE(MIMEType)
S2N_DEL_ATT(kpchar, codec)
S2N_DEL_VAL(kpchar, MIMEType)
// X2S_GOTO_PRT_NODE(MIMEType)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(MIMEType)
S2N_DEL_SELF(MIMEType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(MIMEType)


S2N_FUN_DECL(AlternativeText)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AlternativeText)
/*X2S_GETNODECNT(AlternativeText)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AlternativeText)
X2S_SET_CNT(AlternativeText)*/
S2N_SET_NCNT(AlternativeText)
S2N_FOREACH_NCNT(AlternativeText)
S2N_CURLY_BRACE_L
S2N_SET_REC(AlternativeText)
// X2S_BRK_IF_BAD_NODE(AlternativeText)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, AlternativeText)
// X2S_GOTO_PRT_NODE(AlternativeText)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AlternativeText)
S2N_DEL_SELF(AlternativeText)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AlternativeText)


S2N_FUN_DECL(AlternativePicture)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AlternativePicture)
/*X2S_GETNODECNT(AlternativePicture)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AlternativePicture)
X2S_SET_CNT(AlternativePicture)*/
S2N_SET_NCNT(AlternativePicture)
S2N_FOREACH_NCNT(AlternativePicture)
S2N_CURLY_BRACE_L
S2N_SET_REC(AlternativePicture)
// X2S_BRK_IF_BAD_NODE(AlternativePicture)
S2N_DEL_VAL(kpchar, AlternativePicture)
// X2S_GOTO_PRT_NODE(AlternativePicture)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AlternativePicture)
S2N_DEL_SELF(AlternativePicture)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AlternativePicture)


S2N_FUN_DECL(Bitrate)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Bitrate)
/*X2S_GETNODECNT(Bitrate)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Bitrate)
X2S_SET_CNT(Bitrate)*/
S2N_SET_NCNT(Bitrate)
S2N_FOREACH_NCNT(Bitrate)
S2N_CURLY_BRACE_L
S2N_SET_REC(Bitrate)
// X2S_BRK_IF_BAD_NODE(Bitrate)
//S2N_DEL_ATT(kushort, average)
//S2N_DEL_ATT(kushort, maximum)
// X2S_GOTO_PRT_NODE(Bitrate)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Bitrate)
S2N_DEL_SELF(Bitrate)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Bitrate)


S2N_FUN_DECL(Resolution)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Resolution)
/*X2S_GETNODECNT(Resolution)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Resolution)
X2S_SET_CNT(Resolution)*/
S2N_SET_NCNT(Resolution)
S2N_FOREACH_NCNT(Resolution)
S2N_CURLY_BRACE_L
S2N_SET_REC(Resolution)
// X2S_BRK_IF_BAD_NODE(Resolution)
//S2N_DEL_ATT(kushort, horizontal)
//S2N_DEL_ATT(kuchar, vertical)
//S2N_DEL_ATT(kint, temporal)
// X2S_GOTO_PRT_NODE(Resolution)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Resolution)
S2N_DEL_SELF(Resolution)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Resolution)


S2N_FUN_DECL(MinimumBufferSize)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(MinimumBufferSize)
/*X2S_GETNODECNT(MinimumBufferSize)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(MinimumBufferSize)
X2S_SET_CNT(MinimumBufferSize)*/
S2N_SET_NCNT(MinimumBufferSize)
S2N_FOREACH_NCNT(MinimumBufferSize)
S2N_CURLY_BRACE_L
S2N_SET_REC(MinimumBufferSize)
// X2S_BRK_IF_BAD_NODE(MinimumBufferSize)
//S2N_DEL_VAL(kuint, MinimumBufferSize)
// X2S_GOTO_PRT_NODE(MinimumBufferSize)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(MinimumBufferSize)
S2N_DEL_SELF(MinimumBufferSize)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(MinimumBufferSize)


S2N_FUN_DECL(Complexity)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Complexity)
/*X2S_GETNODECNT(Complexity)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Complexity)
X2S_SET_CNT(Complexity)*/
S2N_SET_NCNT(Complexity)
S2N_FOREACH_NCNT(Complexity)
S2N_CURLY_BRACE_L
S2N_SET_REC(Complexity)
// X2S_BRK_IF_BAD_NODE(Complexity)
S2N_DEL_SUB(Bitrate, Bitrate)
S2N_DEL_SUB(Resolution, Resolution)
S2N_DEL_SUB(MinimumBufferSize, MinimumBufferSize)
// X2S_GOTO_PRT_NODE(Complexity)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Complexity)
S2N_DEL_SELF(Complexity)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Complexity)


S2N_FUN_DECL(Video)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Video)
/*X2S_GETNODECNT(Video)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Video)
X2S_SET_CNT(Video)*/
S2N_SET_NCNT(Video)
S2N_FOREACH_NCNT(Video)
S2N_CURLY_BRACE_L
S2N_SET_REC(Video)
// X2S_BRK_IF_BAD_NODE(Video)
S2N_DEL_SUB(VideoURI, VideoURI)
S2N_DEL_SUB(MIMEType, MIMEType)
S2N_DEL_SUB(AlternativeText, AlternativeText)
S2N_DEL_SUB(AlternativePicture, AlternativePicture)
S2N_DEL_SUB(Complexity, Complexity)
// X2S_GOTO_PRT_NODE(Video)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Video)
S2N_DEL_SELF(Video)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Video)


S2N_FUN_DECL(AudioURI)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AudioURI)
/*X2S_GETNODECNT(AudioURI)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AudioURI)
X2S_SET_CNT(AudioURI)*/
S2N_SET_NCNT(AudioURI)
S2N_FOREACH_NCNT(AudioURI)
S2N_CURLY_BRACE_L
S2N_SET_REC(AudioURI)
// X2S_BRK_IF_BAD_NODE(AudioURI)
S2N_DEL_VAL(kpchar, AudioURI)
// X2S_GOTO_PRT_NODE(AudioURI)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AudioURI)
S2N_DEL_SELF(AudioURI)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AudioURI)


S2N_FUN_DECL(Audio)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Audio)
/*X2S_GETNODECNT(Audio)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Audio)
X2S_SET_CNT(Audio)*/
S2N_SET_NCNT(Audio)
S2N_FOREACH_NCNT(Audio)
S2N_CURLY_BRACE_L
S2N_SET_REC(Audio)
// X2S_BRK_IF_BAD_NODE(Audio)
S2N_DEL_SUB(AudioURI, AudioURI)
S2N_DEL_SUB(MIMEType, MIMEType)
S2N_DEL_SUB(AlternativeText, AlternativeText)
S2N_DEL_SUB(AlternativePicture, AlternativePicture)
S2N_DEL_SUB(Complexity, Complexity)
// X2S_GOTO_PRT_NODE(Audio)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Audio)
S2N_DEL_SELF(Audio)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Audio)


S2N_FUN_DECL(PictureURI)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PictureURI)
/*X2S_GETNODECNT(PictureURI)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PictureURI)
X2S_SET_CNT(PictureURI)*/
S2N_SET_NCNT(PictureURI)
S2N_FOREACH_NCNT(PictureURI)
S2N_CURLY_BRACE_L
S2N_SET_REC(PictureURI)
// X2S_BRK_IF_BAD_NODE(PictureURI)
S2N_DEL_VAL(kpchar, PictureURI)
// X2S_GOTO_PRT_NODE(PictureURI)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PictureURI)
S2N_DEL_SELF(PictureURI)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PictureURI)


S2N_FUN_DECL(PictureData)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PictureData)
/*X2S_GETNODECNT(PictureData)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PictureData)
X2S_SET_CNT(PictureData)*/
S2N_SET_NCNT(PictureData)
S2N_FOREACH_NCNT(PictureData)
S2N_CURLY_BRACE_L
S2N_SET_REC(PictureData)
// X2S_BRK_IF_BAD_NODE(PictureData)
S2N_DEL_VAL(kpchar, PictureData)
// X2S_GOTO_PRT_NODE(PictureData)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PictureData)
S2N_DEL_SELF(PictureData)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PictureData)


S2N_FUN_DECL(Picture)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Picture)
/*X2S_GETNODECNT(Picture)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Picture)
X2S_SET_CNT(Picture)*/
S2N_SET_NCNT(Picture)
S2N_FOREACH_NCNT(Picture)
S2N_CURLY_BRACE_L
S2N_SET_REC(Picture)
// X2S_BRK_IF_BAD_NODE(Picture)
S2N_DEL_SUB(PictureURI, PictureURI)
S2N_DEL_SUB(PictureData, PictureData)
S2N_DEL_SUB(MIMEType, MIMEType)
S2N_DEL_SUB(AlternativeText, AlternativeText)
// X2S_GOTO_PRT_NODE(Picture)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Picture)
S2N_DEL_SELF(Picture)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Picture)


S2N_FUN_DECL(Text)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Text)
/*X2S_GETNODECNT(Text)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Text)
X2S_SET_CNT(Text)*/
S2N_SET_NCNT(Text)
S2N_FOREACH_NCNT(Text)
S2N_CURLY_BRACE_L
S2N_SET_REC(Text)
// X2S_BRK_IF_BAD_NODE(Text)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, Text)
// X2S_GOTO_PRT_NODE(Text)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Text)
S2N_DEL_SELF(Text)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Text)


S2N_FUN_DECL(AccessReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AccessReference)
/*X2S_GETNODECNT(AccessReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AccessReference)
X2S_SET_CNT(AccessReference)*/
S2N_SET_NCNT(AccessReference)
S2N_FOREACH_NCNT(AccessReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(AccessReference)
// X2S_BRK_IF_BAD_NODE(AccessReference)
S2N_DEL_ATT(kpchar, idRef)
//S2N_DEL_ATT(kuchar, usage)
S2N_DEL_VAL(kpchar, AccessReference)
// X2S_GOTO_PRT_NODE(AccessReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AccessReference)
S2N_DEL_SELF(AccessReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AccessReference)

#if 1
S2N_FUN_DECL(PreviewData)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PreviewData)
/*X2S_GETNODECNT(PreviewData)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PreviewData)
X2S_SET_CNT(PreviewData)*/
S2N_SET_NCNT(PreviewData)
S2N_FOREACH_NCNT(PreviewData)
S2N_CURLY_BRACE_L
S2N_SET_REC(PreviewData)
// X2S_BRK_IF_BAD_NODE(PreviewData)
S2N_DEL_SUB(SMIL, SMIL)
S2N_DEL_SUB(Video, Video)
S2N_DEL_SUB(Audio, Audio)
S2N_DEL_SUB(Picture, Picture)
S2N_DEL_SUB(Text, Text)
S2N_DEL_SUB(AccessReference, AccessReference)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(PreviewData)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PreviewData)
S2N_DEL_SELF(PreviewData)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PreviewData)
#endif

S2N_FUN_DECL(AutoStart)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AutoStart)
/*X2S_GETNODECNT(AutoStart)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AutoStart)
X2S_SET_CNT(AutoStart)*/
S2N_SET_NCNT(AutoStart)
S2N_FOREACH_NCNT(AutoStart)
S2N_CURLY_BRACE_L
S2N_SET_REC(AutoStart)
// X2S_BRK_IF_BAD_NODE(AutoStart)
//S2N_DEL_VAL(kuint, AutoStart)
// X2S_GOTO_PRT_NODE(AutoStart)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AutoStart)
S2N_DEL_SELF(AutoStart)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AutoStart)


S2N_FUN_DECL(DistributionWindow)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(DistributionWindow)
/*X2S_GETNODECNT(DistributionWindow)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(DistributionWindow)
X2S_SET_CNT(DistributionWindow)*/
S2N_SET_NCNT(DistributionWindow)
S2N_FOREACH_NCNT(DistributionWindow)
S2N_CURLY_BRACE_L
S2N_SET_REC(DistributionWindow)
// X2S_BRK_IF_BAD_NODE(DistributionWindow)
//S2N_DEL_ATT(kuint, startTime)
//S2N_DEL_ATT(kuint, endTime)
//S2N_DEL_ATT(kuint, duration)
//S2N_DEL_ATT(kuint, id)
// X2S_GOTO_PRT_NODE(DistributionWindow)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(DistributionWindow)
S2N_DEL_SELF(DistributionWindow)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(DistributionWindow)


S2N_FUN_DECL(InteractivityDataReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(InteractivityDataReference)
/*X2S_GETNODECNT(InteractivityDataReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(InteractivityDataReference)
X2S_SET_CNT(InteractivityDataReference)*/
S2N_SET_NCNT(InteractivityDataReference)
S2N_FOREACH_NCNT(InteractivityDataReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(InteractivityDataReference)
// X2S_BRK_IF_BAD_NODE(InteractivityDataReference)
S2N_DEL_ATT(kpchar, idRef)
S2N_DEL_SUB(AutoStart, AutoStart)
S2N_DEL_SUB(DistributionWindow, DistributionWindow)
// X2S_GOTO_PRT_NODE(InteractivityDataReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(InteractivityDataReference)
S2N_DEL_SELF(InteractivityDataReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(InteractivityDataReference)


S2N_FUN_DECL(PresentationWindow)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PresentationWindow)
/*X2S_GETNODECNT(PresentationWindow)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PresentationWindow)
X2S_SET_CNT(PresentationWindow)*/
S2N_SET_NCNT(PresentationWindow)
S2N_FOREACH_NCNT(PresentationWindow)
S2N_CURLY_BRACE_L
S2N_SET_REC(PresentationWindow)
// X2S_BRK_IF_BAD_NODE(PresentationWindow)
//S2N_DEL_ATT(kuint, startTime)
//S2N_DEL_ATT(kuint, endTime)
//S2N_DEL_ATT(kuint, duration)
//S2N_DEL_ATT(kuint, id)
// X2S_GOTO_PRT_NODE(PresentationWindow)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PresentationWindow)
S2N_DEL_SELF(PresentationWindow)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PresentationWindow)


S2N_FUN_DECL(ContentReference)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ContentReference)
/*X2S_GETNODECNT(ContentReference)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ContentReference)
X2S_SET_CNT(ContentReference)*/
S2N_SET_NCNT(ContentReference)
S2N_FOREACH_NCNT(ContentReference)
S2N_CURLY_BRACE_L
S2N_SET_REC(ContentReference)
// X2S_BRK_IF_BAD_NODE(ContentReference)
S2N_DEL_ATT(kpchar, idRef)
S2N_DEL_SUB(PresentationWindow, PresentationWindow)
S2N_DEL_SUB(DistributionWindow, DistributionWindow)
// X2S_GOTO_PRT_NODE(ContentReference)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ContentReference)
S2N_DEL_SELF(ContentReference)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ContentReference)

#if 1
S2N_FUN_DECL(Schedule)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Schedule)
/*X2S_GETNODECNT(Schedule)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Schedule)
X2S_SET_CNT(Schedule)*/
S2N_SET_NCNT(Schedule)
S2N_FOREACH_NCNT(Schedule)
S2N_CURLY_BRACE_L
S2N_SET_REC(Schedule)
// X2S_BRK_IF_BAD_NODE(Schedule)
//S2N_DEL_ATT(kbool, defaultSchedule)
//S2N_DEL_ATT(kbool, onDemand)
S2N_DEL_SUB(ServiceReference, ServiceReference)
S2N_DEL_SUB(InteractivityDataReference, InteractivityDataReference)
S2N_DEL_SUB(ContentReference, ContentReference)
S2N_DEL_SUB(PreviewDataReference, PreviewDataReference)
S2N_DEL_SUB(TermsOfUse, TermsOfUse)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(Schedule)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Schedule)
S2N_DEL_SELF(Schedule)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Schedule)
#endif


S2N_FUN_DECL(BSMFilterCode)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BSMFilterCode)
/*X2S_GETNODECNT(BSMFilterCode)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BSMFilterCode)
X2S_SET_CNT(BSMFilterCode)*/
S2N_SET_NCNT(BSMFilterCode)
S2N_FOREACH_NCNT(BSMFilterCode)
S2N_CURLY_BRACE_L
S2N_SET_REC(BSMFilterCode)
// X2S_BRK_IF_BAD_NODE(BSMFilterCode)
//S2N_DEL_VAL(kuint, BSMFilterCode)
// X2S_GOTO_PRT_NODE(BSMFilterCode)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BSMFilterCode)
S2N_DEL_SELF(BSMFilterCode)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BSMFilterCode)


S2N_FUN_DECL(RoamingRule)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(RoamingRule)
/*X2S_GETNODECNT(RoamingRule)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(RoamingRule)
X2S_SET_CNT(RoamingRule)*/
S2N_SET_NCNT(RoamingRule)
S2N_FOREACH_NCNT(RoamingRule)
S2N_CURLY_BRACE_L
S2N_SET_REC(RoamingRule)
// X2S_BRK_IF_BAD_NODE(RoamingRule)
S2N_DEL_VAL(kpchar, RoamingRule)
// X2S_GOTO_PRT_NODE(RoamingRule)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(RoamingRule)
S2N_DEL_SELF(RoamingRule)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(RoamingRule)


S2N_FUN_DECL(BSMSelector)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BSMSelector)
/*X2S_GETNODECNT(BSMSelector)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BSMSelector)
X2S_SET_CNT(BSMSelector)*/
S2N_SET_NCNT(BSMSelector)
S2N_FOREACH_NCNT(BSMSelector)
S2N_CURLY_BRACE_L
S2N_SET_REC(BSMSelector)
// X2S_BRK_IF_BAD_NODE(BSMSelector)
S2N_DEL_ATT(kpchar, id)
S2N_DEL_ATT(kpchar, roamingRuleRequestAddress)
S2N_DEL_SUB(BSMFilterCode, BSMFilterCode)
S2N_DEL_SUB(Name, Name)
S2N_DEL_SUB(RoamingRule, RoamingRule)
// X2S_GOTO_PRT_NODE(BSMSelector)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BSMSelector)
S2N_DEL_SELF(BSMSelector)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BSMSelector)


S2N_FUN_DECL(BSMList)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(BSMList)
/*X2S_GETNODECNT(BSMList)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(BSMList)
X2S_SET_CNT(BSMList)*/
S2N_SET_NCNT(BSMList)
S2N_FOREACH_NCNT(BSMList)
S2N_CURLY_BRACE_L
S2N_SET_REC(BSMList)
// X2S_BRK_IF_BAD_NODE(BSMList)
S2N_DEL_SUB(BSMSelector, BSMSelector)
// X2S_GOTO_PRT_NODE(BSMList)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(BSMList)
S2N_DEL_SELF(BSMList)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(BSMList)


S2N_FUN_DECL(TimeGroupingCriteria)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TimeGroupingCriteria)
/*X2S_GETNODECNT(TimeGroupingCriteria)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TimeGroupingCriteria)
X2S_SET_CNT(TimeGroupingCriteria)*/
S2N_SET_NCNT(TimeGroupingCriteria)
S2N_FOREACH_NCNT(TimeGroupingCriteria)
S2N_CURLY_BRACE_L
S2N_SET_REC(TimeGroupingCriteria)
// X2S_BRK_IF_BAD_NODE(TimeGroupingCriteria)
//S2N_DEL_ATT(kuint, startTime)
//S2N_DEL_ATT(kuint, endTime)
// X2S_GOTO_PRT_NODE(TimeGroupingCriteria)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TimeGroupingCriteria)
S2N_DEL_SELF(TimeGroupingCriteria)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TimeGroupingCriteria)


S2N_FUN_DECL(GenreGroupingCriteria)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(GenreGroupingCriteria)
/*X2S_GETNODECNT(GenreGroupingCriteria)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(GenreGroupingCriteria)
X2S_SET_CNT(GenreGroupingCriteria)*/
S2N_SET_NCNT(GenreGroupingCriteria)
S2N_FOREACH_NCNT(GenreGroupingCriteria)
S2N_CURLY_BRACE_L
S2N_SET_REC(GenreGroupingCriteria)
// X2S_BRK_IF_BAD_NODE(GenreGroupingCriteria)
S2N_DEL_ATT(kpchar, type)
S2N_DEL_ATT(kpchar, href)
S2N_DEL_VAL(kpchar, GenreGroupingCriteria)
// X2S_GOTO_PRT_NODE(GenreGroupingCriteria)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(GenreGroupingCriteria)
S2N_DEL_SELF(GenreGroupingCriteria)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(GenreGroupingCriteria)


S2N_FUN_DECL(ServiceCriteria)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceCriteria)
/*X2S_GETNODECNT(ServiceCriteria)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceCriteria)
X2S_SET_CNT(ServiceCriteria)*/
S2N_SET_NCNT(ServiceCriteria)
S2N_FOREACH_NCNT(ServiceCriteria)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceCriteria)
// X2S_BRK_IF_BAD_NODE(ServiceCriteria)
S2N_DEL_VAL(kpchar, ServiceCriteria)
// X2S_GOTO_PRT_NODE(ServiceCriteria)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceCriteria)
S2N_DEL_SELF(ServiceCriteria)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceCriteria)


S2N_FUN_DECL(GroupingCriteria)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(GroupingCriteria)
/*X2S_GETNODECNT(GroupingCriteria)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(GroupingCriteria)
X2S_SET_CNT(GroupingCriteria)*/
S2N_SET_NCNT(GroupingCriteria)
S2N_FOREACH_NCNT(GroupingCriteria)
S2N_CURLY_BRACE_L
S2N_SET_REC(GroupingCriteria)
// X2S_BRK_IF_BAD_NODE(GroupingCriteria)
S2N_DEL_SUB(TimeGroupingCriteria, TimeGroupingCriteria)
S2N_DEL_SUB(GenreGroupingCriteria, GenreGroupingCriteria)
S2N_DEL_SUB(BSMSelector, BSMSelector)
S2N_DEL_SUB(ServiceCriteria, ServiceCriteria)
// X2S_GOTO_PRT_NODE(GroupingCriteria)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(GroupingCriteria)
S2N_DEL_SELF(GroupingCriteria)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(GroupingCriteria)


S2N_FUN_DECL(Transport)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Transport)
/*X2S_GETNODECNT(Transport)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Transport)
X2S_SET_CNT(Transport)*/
S2N_SET_NCNT(Transport)
S2N_FOREACH_NCNT(Transport)
S2N_CURLY_BRACE_L
S2N_SET_REC(Transport)
// X2S_BRK_IF_BAD_NODE(Transport)
S2N_DEL_ATT(kpchar, ipAddress)
//S2N_DEL_ATT(kuint, port)
S2N_DEL_ATT(kpchar, srcIpAddress)
//S2N_DEL_ATT(kushort, transmissionSessionID)
//S2N_DEL_ATT(kulong, versionIDLength)
//S2N_DEL_ATT(kbool, hasFDT)
// X2S_GOTO_PRT_NODE(Transport)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Transport)
S2N_DEL_SELF(Transport)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Transport)


S2N_FUN_DECL(AlternativeAccessURL)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(AlternativeAccessURL)
/*X2S_GETNODECNT(AlternativeAccessURL)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(AlternativeAccessURL)
X2S_SET_CNT(AlternativeAccessURL)*/
S2N_SET_NCNT(AlternativeAccessURL)
S2N_FOREACH_NCNT(AlternativeAccessURL)
S2N_CURLY_BRACE_L
S2N_SET_REC(AlternativeAccessURL)
// X2S_BRK_IF_BAD_NODE(AlternativeAccessURL)
S2N_DEL_VAL(kpchar, AlternativeAccessURL)
// X2S_GOTO_PRT_NODE(AlternativeAccessURL)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(AlternativeAccessURL)
S2N_DEL_SELF(AlternativeAccessURL)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(AlternativeAccessURL)


S2N_FUN_DECL(Fragment)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Fragment)
/*X2S_GETNODECNT(Fragment)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Fragment)
X2S_SET_CNT(Fragment)*/
S2N_SET_NCNT(Fragment)
S2N_FOREACH_NCNT(Fragment)
S2N_CURLY_BRACE_L
S2N_SET_REC(Fragment)
// X2S_BRK_IF_BAD_NODE(Fragment)
//S2N_DEL_ATT(kuint, transportID)
S2N_DEL_ATT(kpchar, id)
//S2N_DEL_ATT(kuint, version)
//S2N_DEL_ATT(kuint, validFrom)
//S2N_DEL_ATT(kuint, validTo)
//S2N_DEL_ATT(kuchar, fragmentEncoding)
//S2N_DEL_ATT(kuchar, fragmentType)
S2N_DEL_SUB(GroupingCriteria, GroupingCriteria)
// X2S_GOTO_PRT_NODE(Fragment)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Fragment)
S2N_DEL_SELF(Fragment)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Fragment)


S2N_FUN_DECL(ServiceGuideDeliveryUnit)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceGuideDeliveryUnit)
/*X2S_GETNODECNT(ServiceGuideDeliveryUnit)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceGuideDeliveryUnit)
X2S_SET_CNT(ServiceGuideDeliveryUnit)*/
S2N_SET_NCNT(ServiceGuideDeliveryUnit)
S2N_FOREACH_NCNT(ServiceGuideDeliveryUnit)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceGuideDeliveryUnit)
// X2S_BRK_IF_BAD_NODE(ServiceGuideDeliveryUnit)
//S2N_DEL_ATT(kuint, transportObjectID)
S2N_DEL_ATT(kpchar, contentLocation)
//S2N_DEL_ATT(kuint, validFrom)
//S2N_DEL_ATT(kuint, validTo)
S2N_DEL_SUB(Fragment, Fragment)
// X2S_GOTO_PRT_NODE(ServiceGuideDeliveryUnit)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceGuideDeliveryUnit)
S2N_DEL_SELF(ServiceGuideDeliveryUnit)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceGuideDeliveryUnit)


S2N_FUN_DECL(DescriptorEntry)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(DescriptorEntry)
/*X2S_GETNODECNT(DescriptorEntry)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(DescriptorEntry)
X2S_SET_CNT(DescriptorEntry)*/
S2N_SET_NCNT(DescriptorEntry)
S2N_FOREACH_NCNT(DescriptorEntry)
S2N_CURLY_BRACE_L
S2N_SET_REC(DescriptorEntry)
// X2S_BRK_IF_BAD_NODE(DescriptorEntry)
S2N_DEL_SUB(GroupingCriteria, GroupingCriteria)
S2N_DEL_SUB(Transport, Transport)
S2N_DEL_SUB(AlternativeAccessURL, AlternativeAccessURL)
S2N_DEL_SUB(ServiceGuideDeliveryUnit, ServiceGuideDeliveryUnit)
// X2S_GOTO_PRT_NODE(DescriptorEntry)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(DescriptorEntry)
S2N_DEL_SELF(DescriptorEntry)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(DescriptorEntry)


S2N_FUN_DECL(ServiceGuideDeliveryDescriptor)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceGuideDeliveryDescriptor)
/*X2S_GETNODECNT(ServiceGuideDeliveryDescriptor)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceGuideDeliveryDescriptor)
X2S_SET_CNT(ServiceGuideDeliveryDescriptor)*/
S2N_SET_NCNT(ServiceGuideDeliveryDescriptor)
S2N_FOREACH_NCNT(ServiceGuideDeliveryDescriptor)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceGuideDeliveryDescriptor)
// X2S_BRK_IF_BAD_NODE(ServiceGuideDeliveryDescriptor)
//S2N_DEL_ATT(kpchar, id)
//S2N_DEL_ATT(kuint, version)
S2N_DEL_SUB(NotificationReception, NotificationReception)
S2N_DEL_SUB(BSMList, BSMList)
S2N_DEL_SUB(DescriptorEntry, DescriptorEntry)
// X2S_GOTO_PRT_NODE(ServiceGuideDeliveryDescriptor)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceGuideDeliveryDescriptor)
S2N_DEL_SELF(ServiceGuideDeliveryDescriptor)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceGuideDeliveryDescriptor)


S2N_FUN_DECL(ServiceType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ServiceType)
/*X2S_GETNODECNT(ServiceType)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(ServiceType)
X2S_SET_CNT(ServiceType)*/
S2N_SET_NCNT(ServiceType)
S2N_FOREACH_NCNT(ServiceType)
S2N_CURLY_BRACE_L
S2N_SET_REC(ServiceType)
// X2S_BRK_IF_BAD_NODE(ServiceType)
//S2N_DEL_VAL(kuchar, ServiceType)
// X2S_GOTO_PRT_NODE(ServiceType)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ServiceType)
S2N_DEL_SELF(ServiceType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ServiceType)


S2N_FUN_DECL(TerminalBindingKeyID)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(TerminalBindingKeyID)
/*X2S_GETNODECNT(TerminalBindingKeyID)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(TerminalBindingKeyID)
X2S_SET_CNT(TerminalBindingKeyID)*/
S2N_SET_NCNT(TerminalBindingKeyID)
S2N_FOREACH_NCNT(TerminalBindingKeyID)
S2N_CURLY_BRACE_L
S2N_SET_REC(TerminalBindingKeyID)
// X2S_BRK_IF_BAD_NODE(TerminalBindingKeyID)
S2N_DEL_ATT(kpchar, tbkPermissionsIssuerURI)
// X2S_GOTO_PRT_NODE(TerminalBindingKeyID)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(TerminalBindingKeyID)
S2N_DEL_SELF(TerminalBindingKeyID)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(TerminalBindingKeyID)


S2N_FUN_DECL(GlobalServiceID)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(GlobalServiceID)
/*X2S_GETNODECNT(GlobalServiceID)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(GlobalServiceID)
X2S_SET_CNT(GlobalServiceID)*/
S2N_SET_NCNT(GlobalServiceID)
S2N_FOREACH_NCNT(GlobalServiceID)
S2N_CURLY_BRACE_L
S2N_SET_REC(GlobalServiceID)
// X2S_BRK_IF_BAD_NODE(GlobalServiceID)
S2N_DEL_VAL(kpchar, GlobalServiceID)
// X2S_GOTO_PRT_NODE(GlobalServiceID)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(GlobalServiceID)
S2N_DEL_SELF(GlobalServiceID)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(GlobalServiceID)


S2N_FUN_DECL(PermissionsIssuerURI)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PermissionsIssuerURI)
/*X2S_GETNODECNT(PermissionsIssuerURI)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(PermissionsIssuerURI)
X2S_SET_CNT(PermissionsIssuerURI)*/
S2N_SET_NCNT(PermissionsIssuerURI)
S2N_FOREACH_NCNT(PermissionsIssuerURI)
S2N_CURLY_BRACE_L
S2N_SET_REC(PermissionsIssuerURI)
// X2S_BRK_IF_BAD_NODE(PermissionsIssuerURI)
//S2N_DEL_ATT(kbool, type)
// X2S_GOTO_PRT_NODE(PermissionsIssuerURI)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PermissionsIssuerURI)
S2N_DEL_SELF(PermissionsIssuerURI)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PermissionsIssuerURI)


S2N_FUN_DECL(shape)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(shape)
/*X2S_GETNODECNT(shape)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(shape)
X2S_SET_CNT(shape)*/
S2N_SET_NCNT(shape)
S2N_FOREACH_NCNT(shape)
S2N_CURLY_BRACE_L
S2N_SET_REC(shape)
// X2S_BRK_IF_BAD_NODE(shape)
// X2S_GOTO_PRT_NODE(shape)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(shape)
S2N_DEL_SELF(shape)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(shape)

#if 1
S2N_FUN_DECL(Service)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Service)
/*X2S_GETNODECNT(Service)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(Service)
X2S_SET_CNT(Service)*/
S2N_SET_NCNT(Service)
S2N_FOREACH_NCNT(Service)
S2N_CURLY_BRACE_L
S2N_SET_REC(Service)
// X2S_BRK_IF_BAD_NODE(Service)
//S2N_DEL_ATT(kushort, weight)
//S2N_DEL_ATT(kbool, serviceContentProtection)
S2N_DEL_ATT(kpchar, baseCID)
//S2N_DEL_ATT(kbool, emergency)
S2N_DEL_SUB(ProtectionKeyID, ProtectionKeyID)
S2N_DEL_SUB(ServiceType, ServiceType)
S2N_DEL_SUB(GlobalServiceID, GlobalServiceID)
S2N_DEL_SUB(Name, Name)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(AudioLanguage, AudioLanguage)
S2N_DEL_SUB(TextLanguage, TextLanguage)
S2N_DEL_SUB(ParentalRating, ParentalRating)
S2N_DEL_SUB(TargetUserProfile, TargetUserProfile)
S2N_DEL_SUB(Genre, Genre)
S2N_DEL_SUB(Extension, Extension)
S2N_DEL_SUB(PreviewDataReference, PreviewDataReference)
S2N_DEL_SUB(BroadcastArea, BroadcastArea)
S2N_DEL_SUB(TermsOfUse, TermsOfUse)
S2N_DEL_SUB(PrivateExt, PrivateExt)
// X2S_GOTO_PRT_NODE(Service)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Service)
S2N_DEL_SELF(Service)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Service)
#endif




S2N_FUN_DECL(ActionDescriptor)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ActionDescriptor)
S2N_SET_NCNT(ActionDescriptor)
S2N_FOREACH_NCNT(ActionDescriptor)
S2N_CURLY_BRACE_L
S2N_SET_REC(ActionDescriptor)
S2N_DEL_ATT(kpchar, onTimeOutPointer)
S2N_DEL_ATT(kpchar, onActionPointer)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ActionDescriptor)
S2N_DEL_SELF(ActionDescriptor)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ActionDescriptor)


S2N_FUN_DECL(PartType)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PartType)
S2N_SET_NCNT(PartType)
S2N_FOREACH_NCNT(PartType)
S2N_CURLY_BRACE_L
S2N_SET_REC(PartType)
S2N_DEL_VAL(kpchar, PartType)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PartType)
S2N_DEL_SELF(PartType)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PartType)


S2N_FUN_DECL(Object)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Object)
S2N_SET_NCNT(Object)
S2N_FOREACH_NCNT(Object)
S2N_CURLY_BRACE_L
S2N_SET_REC(Object)
S2N_DEL_ATT(kpchar, Content_Location)
S2N_DEL_ATT(kpchar, Content_Type)
S2N_DEL_SUB(PartType, PartType)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Object)
S2N_DEL_SELF(Object)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Object)


S2N_FUN_DECL(ChoiceText)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(ChoiceText)
S2N_SET_NCNT(ChoiceText)
S2N_FOREACH_NCNT(ChoiceText)
S2N_CURLY_BRACE_L
S2N_SET_REC(ChoiceText)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_VAL(kpchar, ChoiceText)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(ChoiceText)
S2N_DEL_SELF(ChoiceText)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(ChoiceText)


S2N_FUN_DECL(SelectChoice)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SelectChoice)
S2N_SET_NCNT(SelectChoice)
S2N_FOREACH_NCNT(SelectChoice)
S2N_CURLY_BRACE_L
S2N_SET_REC(SelectChoice)
S2N_DEL_ATT(kpchar, smsURI)
S2N_DEL_SUB(ChoiceText, ChoiceText)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SelectChoice)
S2N_DEL_SELF(SelectChoice)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SelectChoice)


S2N_FUN_DECL(SMSTemplate)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(SMSTemplate)
S2N_SET_NCNT(SMSTemplate)
S2N_FOREACH_NCNT(SMSTemplate)
S2N_CURLY_BRACE_L
S2N_SET_REC(SMSTemplate)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(SelectChoice, SelectChoice)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(SMSTemplate)
S2N_DEL_SELF(SMSTemplate)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(SMSTemplate)


S2N_FUN_DECL(MessageBody)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(MessageBody)
S2N_SET_NCNT(MessageBody)
S2N_FOREACH_NCNT(MessageBody)
S2N_CURLY_BRACE_L
S2N_SET_REC(MessageBody)
S2N_DEL_VAL(kpchar, MessageBody)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(MessageBody)
S2N_DEL_SELF(MessageBody)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(MessageBody)


S2N_FUN_DECL(EmailTemplate)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(EmailTemplate)
S2N_SET_NCNT(EmailTemplate)
S2N_FOREACH_NCNT(EmailTemplate)
S2N_CURLY_BRACE_L
S2N_SET_REC(EmailTemplate)
S2N_DEL_ATT(kpchar, toHeader)
S2N_DEL_ATT(kpchar, ccHeader)
S2N_DEL_ATT(kpchar, bccHeader)
S2N_DEL_ATT(kpchar, subjectHeader)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(MessageBody, MessageBody)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(EmailTemplate)
S2N_DEL_SELF(EmailTemplate)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(EmailTemplate)


S2N_FUN_DECL(PhoneNumber)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(PhoneNumber)
S2N_SET_NCNT(PhoneNumber)
S2N_FOREACH_NCNT(PhoneNumber)
S2N_CURLY_BRACE_L
S2N_SET_REC(PhoneNumber)
S2N_DEL_VAL(kpchar, PhoneNumber)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(PhoneNumber)
S2N_DEL_SELF(PhoneNumber)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(PhoneNumber)



S2N_FUN_DECL(VoiceCall)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(VoiceCall)
S2N_SET_NCNT(VoiceCall)
S2N_FOREACH_NCNT(VoiceCall)
S2N_CURLY_BRACE_L
S2N_SET_REC(VoiceCall)
S2N_DEL_SUB(Description, Description)
S2N_DEL_SUB(PhoneNumber, PhoneNumber)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(VoiceCall)
S2N_DEL_SELF(VoiceCall)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(VoiceCall)


S2N_FUN_DECL(Weblink)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(Weblink)
S2N_SET_NCNT(Weblink)
S2N_FOREACH_NCNT(Weblink)
S2N_CURLY_BRACE_L
S2N_SET_REC(Weblink)
S2N_DEL_ATT(kpchar, webURL)
S2N_DEL_SUB(Description, Description)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(Weblink)
S2N_DEL_SELF(Weblink)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(Weblink)


S2N_FUN_DECL(MediaObjectSet)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(MediaObjectSet)
S2N_SET_NCNT(MediaObjectSet)
S2N_FOREACH_NCNT(MediaObjectSet)
S2N_CURLY_BRACE_L
S2N_SET_REC(MediaObjectSet)
S2N_DEL_ATT(kpchar, xml_lang)
S2N_DEL_ATT(kpchar, Content_Type)
S2N_DEL_ATT(kpchar, Content_Location)
S2N_DEL_SUB(Description, Description)
// S2N_DEL_SUB(File, File)
S2N_DEL_SUB(Object, Object)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(MediaObjectSet)
S2N_DEL_SELF(MediaObjectSet)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(MediaObjectSet)


S2N_FUN_DECL(MediaObjectGroup)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(MediaObjectGroup)
S2N_SET_NCNT(MediaObjectGroup)
S2N_FOREACH_NCNT(MediaObjectGroup)
S2N_CURLY_BRACE_L
S2N_SET_REC(MediaObjectGroup)
S2N_DEL_ATT(kpchar, id)
S2N_DEL_SUB(ActionDescriptor, ActionDescriptor)
S2N_DEL_SUB(BackOffTiming, BackOffTiming)
S2N_DEL_SUB(MediaObjectSet, MediaObjectSet)
S2N_DEL_SUB(SMSTemplate, SMSTemplate)
S2N_DEL_SUB(EmailTemplate, EmailTemplate)
S2N_DEL_SUB(VoiceCall, VoiceCall)
S2N_DEL_SUB(Weblink, Weblink)
S2N_DEL_SUB(AlternativeText, AlternativeText)
S2N_CURLY_BRACE_R
S2N_DEL_ARR(MediaObjectGroup)
S2N_DEL_SELF(MediaObjectGroup)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(MediaObjectGroup)


S2N_FUN_DECL(InteractivityMediaDocument)
S2N_CURLY_BRACE_L
S2N_DECL_VAR(InteractivityMediaDocument)
/*X2S_GETNODECNT(InteractivityMediaDocument)
S2N_CURLY_BRACE_L
X2S_ALLOC_T(InteractivityMediaDocument)
X2S_SET_CNT(InteractivityMediaDocument)*/
S2N_SET_NCNT(InteractivityMediaDocument)
S2N_FOREACH_NCNT(InteractivityMediaDocument)
S2N_CURLY_BRACE_L
S2N_SET_REC(InteractivityMediaDocument)
// X2S_BRK_IF_BAD_NODE(InteractivityMediaDocument)
S2N_DEL_ATT(kpchar, groupID)
//S2N_DEL_ATT(kushort, groupPosition)
S2N_DEL_SUB(MediaObjectGroup, MediaObjectGroup)
// X2S_GOTO_PRT_NODE(InteractivityMediaDocument)
// S2N_CURLY_BRACE_R
S2N_CURLY_BRACE_R
S2N_DEL_ARR(InteractivityMediaDocument)
S2N_DEL_SELF(InteractivityMediaDocument)
S2N_FUN_RET
S2N_CURLY_BRACE_R
S2N_END_FUNCTION(InteractivityMediaDocument)



