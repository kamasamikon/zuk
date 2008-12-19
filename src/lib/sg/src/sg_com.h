#ifndef __SG_COM_H__
#define __SG_COM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "esgfile.h"
#include <ktypes.h>
#include "ivisa_sg_config.h"

    typedef struct
    {
        struct {
            kuchar Type;
        } val;
    } Type_rec;

    typedef struct
    {
        kuint ecnt;
        Type_rec *arr;
    } Type_t;

    typedef struct {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            kchar *Name;
        } val;
    } Name_rec;

    typedef struct {
        kuint ecnt;
        Name_rec *arr;
    } Name_t;

    typedef struct
    {
        struct {
            kchar *StartTime;
        } val;
    } StartTime_rec;

    typedef struct
    {
        kuint ecnt;
        StartTime_rec *arr;
    } StartTime_t;

    typedef struct
    {
        struct {
            kchar *EndTime;
        } val;
    } EndTime_rec;

    typedef struct
    {
        kuint ecnt;
        EndTime_rec *arr;
    } EndTime_t;

    typedef struct {
        struct {
            kchar *xml_lang;
            kchar *text;
        } attr;
        struct {
            kchar *Description;
        } val;
    } Description_rec;

    typedef struct {
        kuint ecnt;
        Description_rec *arr;
    } Description_t;

    typedef struct {
        struct {
            kchar *url;
        } attr;
        Description_t *Description;
    } Extension_rec;

    typedef struct {
        kuint ecnt;
        Extension_rec *arr;
    } Extension_t;

    typedef struct {
        struct {
            kchar *idRef;
            // 0. unspecified
            // 1. Service-by-Service Switching
            // 2. Service Guide Browsing
            // 3. Service Preview
            // 4. Barker
            // 5. Alternative to blackout
            // 6-127. reserved for future use
            // 128-255. reserved for proprietary use
            kuchar usage;
        } attr;
    } PreviewDataReference_rec;

    typedef struct {
        kuint ecnt;
        PreviewDataReference_rec *arr;
    } PreviewDataReference_t;

    typedef struct {
        struct {
            kchar *type;     // "main", "secondary", "other"
            kchar *href;
        } attr;
        struct {
            kchar *Genre;
        } val;
    } Genre_rec;

    typedef struct {
        kuint ecnt;
        Genre_rec *arr;
    } Genre_t;

    typedef struct {
        struct {
            kchar *id;
            kchar *languageSDPTag;
        } attr;
        struct {
            kchar *TextLanguage;
        } val;
    } TextLanguage_rec;

    typedef struct {
        kuint ecnt;
        TextLanguage_rec *arr;
    } TextLanguage_t;

    typedef struct
    {
        struct {
            kchar *ratingSystem;
        } attr;
        struct {
            kchar *ParentalRating;
        } val;
    } ParentalRating_rec;

    typedef struct
    {
        kuint ecnt;
        ParentalRating_rec *arr;
    } ParentalRating_t;

    typedef struct {
        struct {
            kchar *attributeName;
            kchar *attributeValue;
        } attr;
    } TargetUserProfile_rec;

    typedef struct {
        kuint ecnt;
        TargetUserProfile_rec *arr;
    } TargetUserProfile_t;

    typedef struct {
        struct {
            // Type of ProtectionKeyID:
            // 0: ProtectionKeyID = Key Domain ID
            // concatenated with SEK/PEK ID, where both
            // values are as used in the Smartcard Profile
            // [BCAST10-ServContProt]
            // 1-127 Reserved for future use
            // 128-255 Reserved for proprietary use
            kuchar type;
        } attr;
        struct {
            // base64Binay
            kchar *ProtectionKeyID;
        } val;
    } ProtectionKeyID_rec;

    typedef struct {
        kuint ecnt;
        ProtectionKeyID_rec *arr;
    } ProtectionKeyID_t;

    typedef struct {
        struct {
            kchar *id;
            kchar *languageSDPTag;
        } attr;
        struct {
            // FIXME: for multilanguage
            kchar *AudioLanguage;
        } val;
    } AudioLanguage_rec;

    typedef struct {
        kuint ecnt;
        AudioLanguage_rec *arr;
    } AudioLanguage_t;

    typedef struct {
        struct {
            kuint port;
            kchar *address;
        } attr;
    } IPBroadcastDelivery_rec;

    typedef struct {
        kuint ecnt;
        IPBroadcastDelivery_rec *arr;
    } IPBroadcastDelivery_t;

    typedef struct {
        struct {
            kchar *RequestURL;
        } val;
    } RequestURL_rec;

    typedef struct {
        kuint ecnt;
        RequestURL_rec *arr;
    } RequestURL_t;

    typedef struct {
        struct {
            kchar *PollURL;
        } val;
    } PollURL_rec;

    typedef struct {
        kuint ecnt;
        PollURL_rec *arr;
    } PollURL_t;

    typedef struct {
        struct {
            kuint channelId;
        } val;
    } channelId_rec;

    typedef struct {
        kuint ecnt;
        channelId_rec *arr;
    } channelId_t;

    typedef struct {
        struct {
            kuint ServiceComponentID;
        } val;
    } ServiceComponentID_rec;

    typedef struct {
        kuint ecnt;
        ServiceComponentID_rec *arr;
    } ServiceComponentID_t;

    typedef struct {
        ServiceComponentID_t *ServiceComponentID;
    } BroadcastDelivery_rec;

    typedef struct {
        kuint ecnt;
        BroadcastDelivery_rec *arr;
    } BroadcastDelivery_t;

    typedef struct {
        struct {
            kushort onID;
            kushort tsID;
            kuint MediaType;
            kuchar ProtocolType;


            kuint eID;
            kuchar TransportMode;
        } attr;

        channelId_t *channelId;
        ServiceComponentID_t *ServiceComponentID;
    } BroadcastPara_rec;

    typedef struct {
        kuint ecnt;
        BroadcastPara_rec *arr;
    } BroadcastPara_t;

    typedef struct {
        Description_t *Description;
        //IPBroadcastDelivery_t *IPBroadcastDelivery;
        RequestURL_t *RequestURL;
        PollURL_t *PollURL;
        BroadcastDelivery_t *BroadcastDelivery;
        IPBroadcastDelivery_t *IPBroadcastDelivery;
    } NotificationReception_rec;

    typedef struct {
        kuint ecnt;
        NotificationReception_rec *arr;
    } NotificationReception_t;

    typedef struct
    {
        struct {
            kchar *idRef;
        } attr;
    } PurchaseItemReference_rec;

    typedef struct
    {
        kuint ecnt;
        PurchaseItemReference_rec *arr;
    } PurchaseItemReference_t;

    typedef struct
    {
        struct {
            kuint PresentationWindowIDRef;
        } val;
    } PresentationWindowIDRef_rec;

    typedef struct
    {
        kuint ecnt;
        PresentationWindowIDRef_rec *arr;
    } PresentationWindowIDRef_t;

    typedef struct {
        struct {
            kuint TBD;
        } val;
    } shape_rec;

    typedef struct {
        // FIXME: Shapes used to represent a geographic area that describes (as specified in the [OMA MLP])
        kuint ecnt;
        shape_rec *arr;
    } shape_t;

    typedef struct {
        struct {
            // FIXME: Country code, 1-3 digits e.g. 355 for Albania (as  specified in the [OMA MLP])
            kushort cc;
        } val;
    } cc_rec;

    typedef struct {
        kuint ecnt;
        cc_rec *arr;
    } cc_t;

    typedef struct {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            kchar *name_area;
        } val;
    } name_area_rec;

    typedef struct {
        kuint ecnt;
        name_area_rec *arr;
    } name_area_t;

    typedef struct {
        struct {
            kchar *ZipCode;
        } val;
    } ZipCode_rec;

    typedef struct {
        kuint ecnt;
        ZipCode_rec *arr;
    } ZipCode_t;

    typedef struct {
        struct {
            // If type = 4, the value is Sector_ID as defined in
            // [3GPP2 C.S0024-A]
            // If type = 5, 6, 7 or 8, the value is BASE ID as
            // defined in [3GPP2 C.S0002-0]
            // 3GPP2 terminals SHALL support this element.

            // positiveInteger
            kint PP2CellID;
        } val;
    } PP2CellID_rec;

    typedef struct {
        kuint ecnt;
        PP2CellID_rec *arr;
    } PP2CellID_t;

    typedef struct {
        struct {
            kushort value;
        } attr;
        PP2CellID_t *PP2CellID;
    } CellArea_rec;

    typedef struct {
        kuint ecnt;
        CellArea_rec *arr;
    } CellArea_t;

    typedef struct {
        struct {
            // 0 每  Unspecified
            // 1 - 3GPP Cell Global Identifier as defined in
            // 3GPP TS 23.003
            // 2 每 3GPP Routing Area Identifier (RAI) as
            // defined in 3GPP TS 23.003
            // 3 每 3GPP Location Area Identifier (LAI) as
            // defined in 3GPP TS 23.003
            // 4 每 3GPP Service Area Identifier (SAI) as
            // defined in 3GPP TS 23.003
            // 5 每 3GPP MBMS Service Area Identity (MBMS
            // SAI) as defined in 3GPP TS 23.003
            // 6 每 3GPP2 Subnet ID as defined in [3GPP2
            // C.S0054-0]
            // 7 每 3GPP2 SID as defined in [3GPP2 C.S0005-
            // D]
            // 8 每 3GPP2 SID+NID as defined in [3GPP2
            // C.S0005-D]
            // 9 每 3GPP2 SID+NID+PZID as defined in
            // [3GPP2 C.S0005-D]
            // 10 每 3GPP2 SID+PZID as defined in [3GPP2
            // C.S0005-D]
            // 11 每 DVB-H Cell ID  (specified in section 6.3.4.1
            // of [BCAST10-DVBH-IPDC-Adaptation] )
            // 12 - 127  reserved for future use
            // 128 -255 reserved for proprietary use
            kuchar type;
        } attr;
        CellArea_t *CellArea;
    } CellTargetArea_rec;

    typedef struct {
        kuint ecnt;
        CellTargetArea_rec *arr;
    } CellTargetArea_t;

    typedef struct {
        shape_t *shape;
        cc_t *cc;
        name_area_t *name_area;
        ZipCode_t *ZipCode;
        CellTargetArea_t *CellTargetArea;
    } TargetArea_rec;

    typedef struct {
        kuint ecnt;
        TargetArea_rec *arr;
    } TargetArea_t;

    typedef struct {
        struct {
            // Horizontal accuracy in meters (as specified in the [OMA MLP])
            kchar *hor_acc;
        } val;
    } hor_acc_rec;

    typedef struct {
        kuint ecnt;
        hor_acc_rec *arr;
    } hor_acc_t;

    typedef struct {
        struct {
            kbool polarity;
        } attr;
        TargetArea_t *TargetArea;
        hor_acc_t *hor_acc;
    } BroadcastArea_rec;

    typedef struct {
        kuint ecnt;
        BroadcastArea_rec *arr;
    } BroadcastArea_t;

    typedef struct {
        struct {
            // List of countries for which the Terms of Use is
            // applicable. Each value is a three character string
            // according to ISO 3166-1 alpha-3
            kchar *Country;
        } val;
    } Country_rec;

    typedef struct {
        kuint ecnt;
        Country_rec *arr;
    } Country_t;

    typedef struct {
        struct {
            // Language in which the Terms of Use is given.
            // Value is a three character string according to ISO
            // 639-2 alpha standard for language codes.
            kchar *Language;
        } val;
    } Language_rec;

    typedef struct {
        kuint ecnt;
        Language_rec *arr;
    } Language_t;

    typedef struct {
        struct {
            kchar *PreviewDataIDRef;
        } val;
    } PreviewDataIDRef_rec;

    typedef struct {
        kuint ecnt;
        PreviewDataIDRef_rec *arr;
    } PreviewDataIDRef_t;

    typedef struct {
        struct {
            kchar *TermsOfUseText;
        } val;
    } TermsOfUseText_rec;

    typedef struct {
        kuint ecnt;
        TermsOfUseText_rec *arr;
    } TermsOfUseText_t;

    typedef struct {
        struct {
            kuchar type;
            kchar *id;
            kbool userConsentRequired;
        } attr;

        Country_t *Country;
        Language_t *Language;
        PreviewDataIDRef_t *PreviewDataIDRef;
        TermsOfUseText_t *TermsOfUseText;
    } TermsOfUse_rec;

    typedef struct {
        kuint ecnt;
        TermsOfUse_rec *arr;
    } TermsOfUse_t;

    typedef struct {
        struct {
            kchar *PrivateExt;
        } val;
        BroadcastPara_t *BroadcastPara;            // for Access
    } PrivateExt_rec;

    typedef struct {
        kuint ecnt;
        PrivateExt_rec *arr;
    } PrivateExt_t;

    typedef struct {
        struct {
            // 0 - unspecified
            // 1 - Basic TV
            // 2 - Basic Radio
            // 3 - RI services
            // 4 - Cachecast
            // 5 - File download services
            // 6 - Software management services
            // 7 - Notification
            // 8 每 Service Guide
            // 9 - Terminal Provisioning services
            // 10 - 127  reserved for future use
            // 128 -255 reserved for proprietary use
            kuchar ServiceType;
        } val;
    } ServiceType_rec;

    typedef struct {
        kuint ecnt;
        ServiceType_rec *arr;
    } ServiceType_t;

    typedef struct {
        struct {
            kchar *GlobalServiceID;
        } val;
    } GlobalServiceID_rec;

    typedef struct {
        kuint ecnt;
        GlobalServiceID_rec *arr;
    } GlobalServiceID_t;


    typedef enum {
        mobileCountryCode,
        mobileNetworkCode,
        networkSubsetCode,
        networkSubsetCodeRangeStart,
        networkSubsetCodeRangeEnd,
        serviceProviderCode,
        corporateCode,
        nonSmartCardCode
    } enumBSMFilterCode;

    typedef struct {
        struct {
            kuint BSMFilterCode;
        } val;
    } BSMFilterCode_rec;

    typedef struct {
        kuint ecnt;
        BSMFilterCode_rec *arr;
    } BSMFilterCode_t;

    typedef struct {
        struct {
            kchar *RoamingRule;
        } val;
    } RoamingRule_rec;

    typedef struct {
        kuint ecnt;
        RoamingRule_rec *arr;
    } RoamingRule_t;

    typedef struct {
        struct {
            kchar *id;
            kchar *roamingRuleRequestAddress;
        } attr;
        BSMFilterCode_t *BSMFilterCode;
        Name_t *Name;
        RoamingRule_t *RoamingRule;
    } BSMSelector_rec;

    typedef struct {
        kuint ecnt;
        BSMSelector_rec *arr;
    } BSMSelector_t;

    typedef struct {
        BSMSelector_t *BSMSelector;
    } BSMList_rec;

    typedef struct {
        kuint ecnt;
        BSMList_rec *arr;
    } BSMList_t;

    typedef struct {
        struct {
            kuint startTime;
            kuint endTime;
        } attr;
    } TimeGroupingCriteria_rec;

    typedef struct {
        kuint ecnt;
        TimeGroupingCriteria_rec *arr;
    } TimeGroupingCriteria_t;

    typedef struct {
        struct {
            kchar *type;
            kchar *href;
        } attr;
        struct {
            kchar *GenreGroupingCriteria;
        } val;
    } GenreGroupingCriteria_rec;

    typedef struct {
        kuint ecnt;
        GenreGroupingCriteria_rec *arr;
    } GenreGroupingCriteria_t;

    typedef struct {
        struct {
            kchar *ServiceCriteria;
        } val;
    } ServiceCriteria_rec;

    typedef struct {
        kuint ecnt;
        ServiceCriteria_rec *arr;
    } ServiceCriteria_t;

    typedef struct {
        TimeGroupingCriteria_t *TimeGroupingCriteria;
        GenreGroupingCriteria_t *GenreGroupingCriteria;
        BSMSelector_t *BSMSelector;
        ServiceCriteria_t *ServiceCriteria;
    } GroupingCriteria_rec;

    typedef struct {
        kuint ecnt;
        GroupingCriteria_rec *arr;
    } GroupingCriteria_t;

    typedef struct {
        struct {
            kchar *ipAddress;
            kuint port;
            kchar *srcIpAddress;
            kushort transmissionSessionID;
            kulong versionIDLength;
            kbool hasFDT;

            kuint eID;
            kuchar TransportMode;

        } attr;
        ServiceComponentID_t *ServiceComponentID;
    } Transport_rec;

    typedef struct {
        kuint ecnt;
        Transport_rec *arr;
    } Transport_t;

    typedef struct {
        struct {
            kchar *AlternativeAccessURL;
        } val;
    } AlternativeAccessURL_rec;

    typedef struct {
        kuint ecnt;
        AlternativeAccessURL_rec *arr;
    } AlternativeAccessURL_t;

    typedef struct {
        struct {
            kuint transportID;
            kchar *id;
            kuint version;
            kuint validFrom;
            kuint validTo;
            kuchar fragmentEncoding;
            kuchar fragmentType;
        } attr;

        GroupingCriteria_t *GroupingCriteria;
    } Fragment_rec;

    typedef struct {
        kuint ecnt;
        Fragment_rec *arr;
    } Fragment_t;

    typedef struct {
        struct {
            kuint transportObjectID;
            kchar *contentLocation;
            kuint validFrom;
            kuint validTo;
        } attr;

        Fragment_t *Fragment;
    } ServiceGuideDeliveryUnit_rec;

    typedef struct {
        kuint ecnt;
        ServiceGuideDeliveryUnit_rec *arr;
    } ServiceGuideDeliveryUnit_t;

    typedef struct {
        GroupingCriteria_t *GroupingCriteria;
        Transport_t *Transport;
        AlternativeAccessURL_t *AlternativeAccessURL;
        ServiceGuideDeliveryUnit_t *ServiceGuideDeliveryUnit;
    } DescriptorEntry_rec;

    typedef struct {
        kuint ecnt;
        DescriptorEntry_rec *arr;
    } DescriptorEntry_t;

    typedef struct
    {
        struct {
            kuchar type;
        } attr;
        struct {
            kchar *SMIL;
        } val;
    } SMIL_rec;

    typedef struct
    {
        kuint ecnt;
        SMIL_rec *arr;
    } SMIL_t;

    typedef struct
    {
        struct {
            kchar *VideoURI;
        } val;
    } VideoURI_rec;

    typedef struct
    {
        kuint ecnt;
        VideoURI_rec *arr;
    } VideoURI_t;

    typedef struct
    {
        struct {
            kchar *codec;
        } attr;
        struct {
            kchar *MIMEType;
        } val;
    } MIMEType_rec;

    typedef struct
    {
        kuint ecnt;
        MIMEType_rec *arr;
    } MIMEType_t;

    typedef struct
    {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            kchar *AlternativeText;
        } val;
    } AlternativeText_rec;

    typedef struct
    {
        kuint ecnt;
        AlternativeText_rec *arr;
    } AlternativeText_t;

    typedef struct
    {
        struct {
            kchar *AlternativePicture;
        } val;
    } AlternativePicture_rec;

    typedef struct
    {
        kuint ecnt;
        AlternativePicture_rec *arr;
    } AlternativePicture_t;

    typedef struct {
        struct {
            kushort average;
            kushort maximum;
        } attr;
    } Bitrate_rec;

    typedef struct {
        kuint ecnt;
        Bitrate_rec *arr;
    } Bitrate_t;

    typedef struct
    {
        struct {
            kushort horizontal;
            kushort vertical;
            kint temporal;
        } attr;
    } Resolution_rec;

    typedef struct
    {
        kuint ecnt;
        Resolution_rec *arr;
    } Resolution_t;
    typedef struct
    {
        struct {
            kuint MinimumBufferSize;
        } val;
    } MinimumBufferSize_rec;

    typedef struct
    {
        kuint ecnt;
        MinimumBufferSize_rec *arr;
    } MinimumBufferSize_t;

    typedef struct {
        Bitrate_t *Bitrate;
        Resolution_t *Resolution;
        MinimumBufferSize_t *MinimumBufferSize;
    } Complexity_rec;

    typedef struct {
        kuint ecnt;
        Complexity_rec *arr;
    } Complexity_t;

    typedef struct
    {
        VideoURI_t *VideoURI;
        MIMEType_t *MIMEType;
        AlternativeText_t *AlternativeText;
        AlternativePicture_t *AlternativePicture;
        Complexity_t *Complexity;
    } Video_rec;

    typedef struct
    {
        kuint ecnt;
        Video_rec *arr;
    } Video_t;

    typedef struct
    {
        struct {
            kchar *AudioURI;
        } val;
    } AudioURI_rec;

    typedef struct
    {
        kuint ecnt;
        AudioURI_rec *arr;
    } AudioURI_t;

    typedef struct
    {
        AudioURI_t *AudioURI;
        MIMEType_t *MIMEType;
        AlternativeText_t *AlternativeText;
        AlternativePicture_t *AlternativePicture;
        Complexity_t *Complexity;
    } Audio_rec;

    typedef struct
    {
        kuint ecnt;
        Audio_rec *arr;
    } Audio_t;

    typedef struct
    {
        struct {
            kchar *PictureURI;
        } val;
    } PictureURI_rec;

    typedef struct
    {
        kuint ecnt;
        PictureURI_rec *arr;
    } PictureURI_t;

    typedef struct
    {
        struct {
            kchar *PictureData;
        } val;
    } PictureData_rec;

    typedef struct
    {
        kuint ecnt;
        PictureData_rec *arr;
    } PictureData_t;

    typedef struct
    {
        PictureURI_t *PictureURI;
        PictureData_t *PictureData;
        MIMEType_t *MIMEType;
        AlternativeText_t *AlternativeText;
    } Picture_rec;

    typedef struct
    {
        kuint ecnt;
        Picture_rec *arr;
    } Picture_t;

    typedef struct
    {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            kchar *Text;
        } val;
    } Text_rec;

    typedef struct
    {
        kuint ecnt;
        Text_rec *arr;
    } Text_t;

    typedef struct
    {
        struct {
            kchar *idRef;
            kuchar usage;
        } attr;
        struct {
            kchar *AccessReference;
        } val;
    } AccessReference_rec;

    typedef struct
    {
        kuint ecnt;
        AccessReference_rec *arr;
    } AccessReference_t;

    typedef struct {
        SMIL_t *SMIL;
        Video_t *Video;
        Audio_t *Audio;
        Picture_t *Picture;
        Text_t *Text;
        AccessReference_t *AccessReference;
        PrivateExt_t *PrivateExt;
    } PreviewData_rec;

    typedef struct {
        /* SG_COMMON
           struct {
           kchar *id;
           kuint version;
           } attr;
           */

        NotificationReception_t *NotificationReception;
        BSMList_t *BSMList;
        DescriptorEntry_t *DescriptorEntry;
    } ServiceGuideDeliveryDescriptor_rec;

    typedef struct {
        struct {
            kchar *idRef;
            kchar *audioLanguageIDRef;
            kchar *textLanguageIDRef;
            kushort weight;
        } attr;
    } ServiceReference_rec;

    typedef struct {
        kuint ecnt;
        ServiceReference_rec *arr;
    } ServiceReference_t;

    typedef struct {
        struct {
            kuint AutoStart;
        } val;
    } AutoStart_rec;

    typedef struct {
        kuint ecnt;
        AutoStart_rec *arr;
    } AutoStart_t;

    typedef struct {
        struct {
            kuint startTime;
            kuint endTime;
            kuint duration;
            kuint id;
        } attr;
    } DistributionWindow_rec;

    typedef struct {
        kuint ecnt;
        DistributionWindow_rec *arr;
    } DistributionWindow_t;

    typedef struct {
        struct {
            kchar *idRef;
        } attr;
        AutoStart_t *AutoStart;
        DistributionWindow_t *DistributionWindow;
    } InteractivityDataReference_rec;

    typedef struct {
        kuint ecnt;
        InteractivityDataReference_rec *arr;
    } InteractivityDataReference_t;

    typedef struct {
        struct {
            kuint startTime;
            kuint endTime;
            kuint duration;
            kuint id;
        } attr;
    } PresentationWindow_rec;

    typedef struct {
        kuint ecnt;
        PresentationWindow_rec *arr;
    } PresentationWindow_t;

    typedef struct {
        struct {
            kchar *idRef;
            kchar *contentLocation;
            kchar *audioLanguageIDRef;
            kchar *textLanguageIDRef;
            kbool repeatPlayback;
        } attr;

        AutoStart_t *AutoStart;
        DistributionWindow_t *DistributionWindow;
        PresentationWindow_t *PresentationWindow;
    } ContentReference_rec;

    typedef struct {
        kuint ecnt;
        ContentReference_rec *arr;
    } ContentReference_t;

    typedef struct {
        struct {
            kbool defaultSchedule;
            kbool onDemand;
        } attr;

        ServiceReference_t *ServiceReference;
        InteractivityDataReference_t *InteractivityDataReference;
        ContentReference_t *ContentReference;
        PreviewDataReference_t *PreviewDataReference;
        TermsOfUse_t *TermsOfUse;
        PrivateExt_t *PrivateExt;
    } Schedule_rec;

    typedef struct
    {
        struct {
            kuint DistributionWindowID;
        } val;
    } DistributionWindowID_rec;

    typedef struct
    {
        kuint ecnt;
        DistributionWindowID_rec *arr;
    } DistributionWindowID_t;

    typedef struct
    {
        struct {
            kchar *idRef;
        } attr;
        PresentationWindowIDRef_t *PresentationWindowIDRef;
        DistributionWindowID_t *DistributionWindowID;
    } ScheduleReference_rec;

    typedef struct
    {
        kuint ecnt;
        ScheduleReference_rec *arr;
    } ScheduleReference_t;

    typedef struct
    {
        struct {
            kchar *idRef;
        } attr;
    } Dependencies_rec;

    typedef struct
    {
        kuint ecnt;
        Dependencies_rec *arr;
    } Dependencies_t;

    typedef struct
    {
        struct {
            kchar *idRef;
        } attr;
    } Exclusions_rec;

    typedef struct
    {
        kuint ecnt;
        Exclusions_rec *arr;
    } Exclusions_t;

    typedef struct {
        struct {
            kchar *globalPurchaseItemID;
            kuint binaryPurchaseItemID;
            kushort weight;
            kbool closed;
        } attr;

        ServiceReference_t *ServiceReference;
        ScheduleReference_t *ScheduleReference;
        ContentReference_t *ContentReference;
        PurchaseItemReference_t *PurchaseItemReference;
        ProtectionKeyID_t *ProtectionKeyID;
        Name_t *Name;
        Description_t *Description;
        StartTime_t *StartTime;
        EndTime_t *EndTime;
        Extension_t *Extension;
        ParentalRating_t *ParentalRating;
        Dependencies_t *Dependencies;
        Exclusions_t *Exclusions;
        PrivateExt_t *PrivateExt;
    } PurchaseItem_rec;

    typedef struct
    {
        struct {
            kchar *currency;
        } attr;
        struct {
            kchar *MonetaryPrice;
        } val;
    } MonetaryPrice_rec;

    typedef struct
    {
        kuint ecnt;
        MonetaryPrice_rec *arr;
    } MonetaryPrice_t;

    typedef struct
    {
        struct {
            kuchar tokenType;
            kushort consumptionAmount;
            kuchar consumptionUnit;
            kushort maxReplay;
        } attr;
        struct {
            kushort TotalNumberToken;
        } val;
    } TotalNumberToken_rec;

    typedef struct
    {
        kuint ecnt;
        TotalNumberToken_rec *arr;
    } TotalNumberToken_t;

    typedef struct
    {
        struct {
            kchar *SubscriptionPeriod;
        } val;
    } SubscriptionPeriod_rec;

    typedef struct
    {
        kuint ecnt;
        SubscriptionPeriod_rec *arr;
    } SubscriptionPeriod_t;

    typedef struct
    {
        struct {
            kuchar subscriptionType;
        } attr;
        MonetaryPrice_t *MonetaryPrice;
        TotalNumberToken_t *TotalNumberToken;
        SubscriptionPeriod_t *SubscriptionPeriod;
    } PriceInfo_rec;

    typedef struct
    {
        kuint ecnt;
        PriceInfo_rec *arr;
    } PriceInfo_t;

    typedef struct
    {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            kchar *Title;
        } val;
    } Title_rec;

    typedef struct
    {
        kuint ecnt;
        Title_rec *arr;
    } Title_t;

    typedef struct
    {
        struct {
            kchar *url;
        } attr;
        Description_t *Description;
    } PromotionExtension_rec;

    typedef struct
    {
        kuint ecnt;
        PromotionExtension_rec *arr;
    } PromotionExtension_t;

    typedef struct
    {
        struct {
            kchar *id;
            kuint validFrom;
            kuint validTo;
        } attr;
        Title_t *Title;
        TargetUserProfile_t *TargetUserProfile;
        Description_t *Description;
        PromotionExtension_t *PromotionExtension;
    } PromotionInfo_rec;

    typedef struct
    {
        kuint ecnt;
        PromotionInfo_rec *arr;
    } PromotionInfo_t;

    typedef struct
    {
        struct {
            kchar *idRef;
        } attr;
    } PurchaseChannelReference_rec;

    typedef struct
    {
        kuint ecnt;
        PurchaseChannelReference_rec *arr;
    } PurchaseChannelReference_t;

    typedef struct
    {
        struct {
            kchar *idRef;
            kuchar usage;
        } attr;
        struct {
            kchar *PreviwDataReference;
        } val;
    } PreviwDataReference_rec;

    typedef struct
    {
        kuint ecnt;
        PreviwDataReference_rec *arr;
    } PreviwDataReference_t;

    typedef struct {
        Description_t *Description;
        PriceInfo_t *PriceInfo;
        PromotionInfo_t *PromotionInfo;
        Extension_t *Extension;
        PurchaseItemReference_t *PurchaseItemReference;
        PurchaseChannelReference_t *PurchaseChannelReference;
        PreviwDataReference_t *PreviwDataReference;
        TermsOfUse_t *TermsOfUse;
        PrivateExt_t *PrivateExt;
    } PurchaseData_rec;

    typedef struct
    {
        struct {
            kchar *ContactInfo;
        } val;
    } ContactInfo_rec;

    typedef struct
    {
        kuint ecnt;
        ContactInfo_rec *arr;
    } ContactInfo_t;

    typedef struct
    {
        struct {
            kuchar supportedService;
        } attr;
        struct {
            kchar *PortalURL;
        } val;
    } PortalURL_rec;

    typedef struct
    {
        kuint ecnt;
        PortalURL_rec *arr;
    } PortalURL_t;

    typedef struct
    {
        struct {
            kchar *PurchaseURL;
        } val;
    } PurchaseURL_rec;

    typedef struct
    {
        kuint ecnt;
        PurchaseURL_rec *arr;
    } PurchaseURL_t;

    typedef struct {
        struct {
            kchar *rightsIssuerURI;
        } attr;

        Name_t *Name;
        Description_t *Description;
        ContactInfo_t *ContactInfo;
        PortalURL_t *PortalURL;
        PurchaseURL_t *PurchaseURL;
        Extension_t *Extension;
        PrivateExt_t *PrivateExt;
    } PurchaseChannel_rec;

    typedef struct
    {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            kchar *InteractivityType;
        } val;
    } InteractivityType_rec;

    typedef struct
    {
        kuint ecnt;
        InteractivityType_rec *arr;
    } InteractivityType_t;

    typedef struct
    {
        struct {
            kuint startTime;
            kuint endTime;
        } attr;
        struct {
            kchar *InteractivityWindow;
        } val;
    } InteractivityWindow_rec;

    typedef struct
    {
        kuint ecnt;
        InteractivityWindow_rec *arr;
    } InteractivityWindow_t;

    typedef struct
    {
        struct {
            kchar interactivtyMediaURL;
            kbool pushDelivery;
        } attr;
        struct {
            kchar *InteractiveDelivery;
        } val;
    } InteractiveDelivery_rec;

    typedef struct
    {
        kuint ecnt;
        InteractiveDelivery_rec *arr;
    } InteractiveDelivery_t;

    typedef struct
    {
        struct {
            kint offsetTime;
            kint randomTime;
        } attr;
    } BackOffTiming_rec;

    typedef struct
    {
        kuint ecnt;
        BackOffTiming_rec *arr;
    } BackOffTiming_t;

    typedef struct {
        struct {
            kbool preListenIndicator;
            kchar *interactivityMediaDocumentPointer;
        } attr;

        InteractivityType_t *InteractivityType;
        ServiceReference_t *ServiceReference;
        ContentReference_t *ContentReference;
        ScheduleReference_t *ScheduleReference;
        InteractivityWindow_t *InteractivityWindow;
        InteractiveDelivery_t *InteractiveDelivery;
        Extension_t *Extension;
        BackOffTiming_t *BackOffTiming;
        TermsOfUse_t *TermsOfUse;
        PrivateExt_t *PrivateExt;
    } InteractivityData_rec;

    typedef struct {
        struct {
            // FIXME: Duration of the A/V content declared
            // what is the real meaning of duration
            kuint Length;
        } val;
    } Length_rec;

    typedef struct {
        kuint ecnt;
        Length_rec *arr;
    } Length_t;

    typedef struct {
        struct {
            kchar *globalContentID;
            kbool emergency;
            kbool serviceContentProtection;
            kchar *baseCID;
        } attr;

        ServiceReference_t *ServiceReference;
        ProtectionKeyID_t *ProtectionKeyID;
        Name_t *Name;
        Description_t *Description;
        StartTime_t *StartTime;
        EndTime_t *EndTime;
        AudioLanguage_t *AudioLanguage;
        TextLanguage_t *TextLanguage;
        Length_t *Length;
        ParentalRating_t *ParentalRating;
        TargetUserProfile_t *TargetUserProfile;
        Genre_t *Genre;
        Extension_t *Extension;
        PreviewDataReference_t *PreviewDataReference;
        BroadcastArea_t *BroadcastArea;
        TermsOfUse_t *TermsOfUse;
        PrivateExt_t *PrivateExt;
    } Content_rec;

    typedef struct
    {
        struct {
            kchar *Version;
        } val;
    } Version_rec;

    typedef struct
    {
        kuint ecnt;
        Version_rec *arr;
    } Version_t;

    typedef struct
    {
        Type_t *Type;
        Version_t *Version;
    } BDSType_rec;

    typedef struct
    {
        kuint ecnt;
        BDSType_rec *arr;
    } BDSType_t;

    typedef struct
    {
        struct {
            kchar *SDP;
        } val;
    } SDP_rec;

    typedef struct
    {
        kuint ecnt;
        SDP_rec *arr;
    } SDP_t;

    typedef struct
    {
        struct {
            kchar *uri;
            kchar *idRef;
        } attr;
    } SDPRef_rec;

    typedef struct
    {
        kuint ecnt;
        SDPRef_rec *arr;
    } SDPRef_t;

    typedef struct
    {
        struct {
            kchar *uri;
            kchar *idRef;
        } attr;
    } USBDRef_rec;

    typedef struct
    {
        kuint ecnt;
        USBDRef_rec *arr;
    } USBDRef_t;

    typedef struct
    {
        struct {
            kchar *uri;
            kchar *idRef;
        } attr;
    } ADPRef_rec;

    typedef struct
    {
        kuint ecnt;
        ADPRef_rec *arr;
    } ADPRef_t;

    typedef struct
    {
        SDP_t *SDP;
        SDPRef_t *SDPRef;
        USBDRef_t *USBDRef;
        ADPRef_t *ADPRef;
    } SessionDescription_rec;

    typedef struct
    {
        kuint ecnt;
        SessionDescription_rec *arr;
    } SessionDescription_t;

    typedef struct
    {
        struct {
            kchar *Content_Location;
            // positiveInteger
            kint TOI;
            kulong Content_Length;
            kulong Transfer_Length;
            kchar *Content_Type;
            kchar *Content_Encoding;
            //base64binary
            kchar *Content_MD5;
            kuchar FEC_OTI_FEC_Encoding_ID;
            kulong FEC_OTI_FEC_Instance_ID;
            kulong FEC_OTI_Maximum_Source_Block_Length;
            kulong FEC_OTI_Encoding_Symbol_Length;
            kulong FEC_OTI_Max_Number_of_Encoding_Symbols;
            //base64binary
            kchar *FEC_OTI_Scheme_Specific_Info;
        } attr;
    } File_rec;

    typedef struct
    {
        kuint ecnt;
        File_rec *arr;
    } File_t;

    typedef struct
    {
        struct {
            kchar *Content_Type;
            kchar *Content_Encoding;
            kuchar FEC_OTI_FEC_Encoding_ID;
            kulong FEC_OTI_FEC_Instance_ID;
            kulong FEC_OTI_Maximum_Source_Block_Length;
            kulong FEC_OTI_Encoding_Symbol_Length;
            kulong FEC_OTI_Max_Number_of_Encoding_Symbols;
            // base64binary
            kchar FEC_OTI_Scheme_Specific_Info;
        } attr;
        File_t *File;
    } FileDescription_rec;

    typedef struct
    {
        kuint ecnt;
        FileDescription_rec *arr;
    } FileDescription_t;

    typedef struct
    {
        BDSType_t *BDSType;
        SessionDescription_t *SessionDescription;
        FileDescription_t *FileDescription;
    } BroadcastServiceDelivery_rec;

    typedef struct
    {
        kuint ecnt;
        BroadcastServiceDelivery_rec *arr;
    } BroadcastServiceDelivery_t;

    typedef struct {
        BroadcastServiceDelivery_t *BroadcastServiceDelivery;
    } AccessType_rec;

    typedef struct {
        kuint ecnt;
        AccessType_rec *arr;
    } AccessType_t;

    typedef struct {
        struct {
            kbool type;
        } attr;
    } PermissionsIssuerURI_rec;

    typedef struct {
        kuint ecnt;
        PermissionsIssuerURI_rec *arr;
    } PermissionsIssuerURI_t;

    typedef struct {
        struct {
            kchar *tbkPermissionsIssuerURI;
        } attr;
    } TerminalBindingKeyID_rec;

    typedef struct {
        kuint ecnt;
        TerminalBindingKeyID_rec *arr;
    } TerminalBindingKeyID_t;

    typedef struct {
        struct {
            kuchar kmsType;
            kuchar protectionType;
        } attr;
        ProtectionKeyID_t *ProtectionKeyID;
        PermissionsIssuerURI_t *PermissionsIssuerURI;
        TerminalBindingKeyID_t *TerminalBindingKeyID;
    } KeyManagementSystem_rec;

    typedef struct {
        kuint ecnt;
        KeyManagementSystem_rec *arr;
    } KeyManagementSystem_t;

    typedef struct {
        struct {
            kchar EncryptionType;
        } val;
    } EncryptionType_rec;

    typedef struct {
        kuint ecnt;
        EncryptionType_rec *arr;
    } EncryptionType_t;

    typedef struct
    {
        MIMEType_t *MIMEType;
    } DownloadFile_rec;

    typedef struct
    {
        kuint ecnt;
        DownloadFile_rec *arr;
    } DownloadFile_t;

    typedef struct {
        Video_t *Video;
        Audio_t *Audio;
        DownloadFile_t *DownloadFile;
    } TerminalCapabilityRequirement_rec;

    typedef struct {
        kuint ecnt;
        TerminalCapabilityRequirement_rec *arr;
    } TerminalCapabilityRequirement_t;

    typedef struct {
        struct {
            kuint BandwidthRequirement;
        } val;
    } BandwidthRequirement_rec;

    typedef struct {
        kuint ecnt;
        BandwidthRequirement_rec *arr;
    } BandwidthRequirement_t;

    typedef struct {
        struct {
            kchar *ServiceClass;
        } val;
    } ServiceClass_rec;

    typedef struct {
        kuint ecnt;
        ServiceClass_rec *arr;
    } ServiceClass_t;

    typedef struct {
        AccessType_t *AccessType;
        KeyManagementSystem_t *KeyManagementSystem;
        EncryptionType_t *EncryptionType;
        ServiceReference_t *ServiceReference;
        ScheduleReference_t *ScheduleReference;
        TerminalCapabilityRequirement_t *TerminalCapabilityRequirement;
        BandwidthRequirement_t *BandwidthRequirement;
        ServiceClass_t *ServiceClass;
        PreviewDataReference_t *PreviewDataReference;
        NotificationReception_t *NotificationReception;
        PrivateExt_t *PrivateExt;
    } Access_rec;

    typedef struct {
        struct {
            kushort weight;
            kbool serviceContentProtection;
            kchar *baseCID;
            kbool emergency;
        } attr;

        ProtectionKeyID_t *ProtectionKeyID;
        ServiceType_t *ServiceType;
        GlobalServiceID_t *GlobalServiceID;
        Name_t *Name;
        Description_t *Description;
        AudioLanguage_t *AudioLanguage;
        TextLanguage_t *TextLanguage;
        ParentalRating_t *ParentalRating;
        TargetUserProfile_t *TargetUserProfile;
        Genre_t *Genre;
        Extension_t *Extension;
        PreviewDataReference_t *PreviewDataReference;
        BroadcastArea_t *BroadcastArea;
        TermsOfUse_t *TermsOfUse;
        PrivateExt_t *PrivateExt;
    } Service_rec;

    /////////////////////////////////////////////////////////////////////////

    typedef struct _sg_acc {
        EF_COMMON;
        kuint ecnt;
        Access_rec *arr;
    } Access_t;

    typedef struct _sg_ctt
    {
        EF_COMMON;
        kuint ecnt;
        Content_rec *arr;
    } Content_t;

    typedef struct _sg_iad {
        EF_COMMON;
        kuint ecnt;
        InteractivityData_rec *arr;
    } InteractivityData_t;

    typedef struct _sg_pcc {
        EF_COMMON;
        kuint ecnt;
        PurchaseChannel_rec *arr;
    } PurchaseChannel_t;

    typedef struct _sg_pcd {
        EF_COMMON;
        kuint ecnt;
        PurchaseData_rec *arr;
    } PurchaseData_t;

    typedef struct _sg_pci {
        EF_COMMON;
        kuint ecnt;
        PurchaseItem_rec *arr;
    } PurchaseItem_t;

    typedef struct _sg_pvd {
        EF_COMMON;
        kuint ecnt;
        PreviewData_rec *arr;
    } PreviewData_t;

    typedef struct _sg_sch
    {
        EF_COMMON;
        kuint ecnt;
        Schedule_rec *arr;
    } Schedule_t;

    // sg_sgdd_ctx
    typedef struct _sg_sdd {
        EF_COMMON;

        // le for sg_sgdd_ent::sgdd_ctx_hdr
        K_dlist_entry dir_le;

        // hdr for sub sg_sgdu_ent
        K_dlist_entry sgdu_ctx_hdr;

        struct {
            kchar *pObjectName;
            kuint uiObjectID;
            kint iObjectVersion;
            kint iObjectVersionLength;
            kuint uiContentLength;
        } objinf;

        kuint ecnt;
        ServiceGuideDeliveryDescriptor_rec *arr;
    } ServiceGuideDeliveryDescriptor_t;

    typedef struct _sg_ser
    {
        EF_COMMON;
        kuint ecnt;
        kvoid /*IV_INTERACTIVITY_DIRECTORY*/ *cur_idir;
        Service_rec *arr;
        kbean iad_probe_tmr;
    } Service_t;




















    typedef struct
    {
        struct {
            kuint inputAllowedTime;
            kchar *onTimeOutPointer;
            kbool updateFlag;
            kchar *onActionPointer;
        } attr;
    } ActionDescriptor_rec;

    typedef struct
    {
        kuint ecnt;
        ActionDescriptor_rec *arr;
    } ActionDescriptor_t;

    typedef struct
    {
        struct {
            char *PartType;
        } val;
    } PartType_rec;

    typedef struct
    {
        kuint ecnt;
        PartType_rec *arr;
    } PartType_t;

    typedef struct
    {
        struct {
            kchar *Content_Location;
            kchar *Content_Type;
            kbool start;
        } attr;
        PartType_t *PartType;
    } Object_rec;

    typedef struct
    {
        kuint ecnt;
        Object_rec *arr;
    } Object_t;

    typedef struct
    {
        struct {
            kchar *xml_lang;

            kuint relativePreference;
            kchar *Content_Type;
            kchar *Content_Location;

        } attr;
        Description_t *Description;
        File_t *File;
        Object_t *Object;
    } MediaObjectSet_rec;

    typedef struct
    {
        kuint ecnt;
        MediaObjectSet_rec *arr;
    } MediaObjectSet_t;

    typedef struct
    {
        struct {
            kchar *xml_lang;
        } attr;
        struct {
            char *ChoiceText;
        } val;
    } ChoiceText_rec;

    typedef struct
    {
        kuint ecnt;
        ChoiceText_rec *arr;
    } ChoiceText_t;

    typedef struct
    {
        struct {
            kchar *smsURI;
        } attr;
        ChoiceText_t *ChoiceText;
    } SelectChoice_rec;

    typedef struct
    {
        kuint ecnt;
        SelectChoice_rec *arr;
    } SelectChoice_t;

    typedef struct
    {
        struct {
            kuint relativePreference;
        } attr;
        Description_t *Description;
        SelectChoice_t *SelectChoice;
    } SMSTemplate_rec;

    typedef struct
    {
        kuint ecnt;
        SMSTemplate_rec *arr;
    } SMSTemplate_t;

    typedef struct
    {
        struct {
            char *MessageBody;
        } val;
    } MessageBody_rec;

    typedef struct
    {
        kuint ecnt;
        MessageBody_rec *arr;
    } MessageBody_t;

    typedef struct
    {
        struct {
            kuint relativePreference;
            kchar *toHeader;
            kchar *ccHeader;
            kchar *bccHeader;
            kchar *subjectHeader;
        } attr;
        Description_t *Description;
        MessageBody_t *MessageBody;
    } EmailTemplate_rec;

    typedef struct
    {
        kuint ecnt;
        EmailTemplate_rec *arr;
    } EmailTemplate_t;

    typedef struct
    {
        struct {
            char *PhoneNumber;
        } val;
    } PhoneNumber_rec;

    typedef struct
    {
        kuint ecnt;
        PhoneNumber_rec *arr;
    } PhoneNumber_t;

    typedef struct
    {
        struct {
            kuint relativePreference;
        } attr;
        Description_t *Description;
        PhoneNumber_t *PhoneNumber;
    } VoiceCall_rec;

    typedef struct
    {
        kuint ecnt;
        VoiceCall_rec *arr;
    } VoiceCall_t;

    typedef struct
    {
        struct {
            kuint relativePreference;
            kchar *webURL;
        } attr;
        Description_t *Description;
    } Weblink_rec;

    typedef struct
    {
        kuint ecnt;
        Weblink_rec *arr;
    } Weblink_t;

    typedef struct
    {
        struct {
            kchar *id;
            kbool startMediaFlag;
        } attr;
        ActionDescriptor_t *ActionDescriptor;
        BackOffTiming_t *BackOffTiming;
        MediaObjectSet_t *MediaObjectSet;
        SMSTemplate_t *SMSTemplate;
        EmailTemplate_t *EmailTemplate;
        VoiceCall_t *VoiceCall;
        Weblink_t *Weblink;
        AlternativeText_t *AlternativeText;
    } MediaObjectGroup_rec;

    typedef struct
    {
        kuint ecnt;
        MediaObjectGroup_rec *arr;
    } MediaObjectGroup_t;

    typedef struct _InteractivityMediaDocument_rec {
        struct {
            kchar *groupID;
            kushort groupPosition;
        } attr;
        MediaObjectGroup_t *MediaObjectGroup;
    } InteractivityMediaDocument_rec;

    typedef struct _InteractivityMediaDocument_t {
        EF_COMMON;
        kchar *serid;
        kuint ecnt;
        InteractivityMediaDocument_rec *arr;
    } InteractivityMediaDocument_t;

    /*-----------------------------------------------------------------------
     * Bi-Direction SG Receive
     */
    typedef struct {
        struct {
            kuint TSID;
            kuint Frequency;
            kchar *TSName;
            kchar *TSDes;
            kchar *AURL;
        } attr;
    } TS_rec;
    typedef struct {
        kuint ecnt;
        TS_rec *arr;
    } TS_t;

    typedef struct {
        struct {
            kuint NID;
            kchar *NName;
            kchar *NDes;
            kuint NType;
            kchar *AURL;
        } attr;
        TS_t *TS;
    } Network_rec;
    typedef struct {
        kuint ecnt;
        Network_rec *arr;
    } Network_t;

    typedef struct {
        Network_t *Network;
    } Networks_rec;
    typedef struct {
        kuint ecnt;
        Networks_rec *arr;
    } Networks_t;

    typedef struct {
        Networks_t *Networks;
    } ESG_rec;
    typedef struct {
        kuint ecnt;
        ESG_rec *arr;
    } ESG_t;
    typedef Network_rec BDIR_t;
    /*
     * End Bi-Direction SG Receive
     *----------------------------------------------------------------------*/


#define ITV_DIR_NAME "itv"

#ifdef __cplusplus
}
#endif

#endif /* __SG_COM_H__ */
