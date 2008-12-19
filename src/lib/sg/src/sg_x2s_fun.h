#ifndef __SG_X2S_FUN_H__
#define __SG_X2S_FUN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include "xmldoc.h"
#include "x2s_macro.h"


X2S_FUN_DECL(Version);
X2S_FUN_DECL(BDSType);
X2S_FUN_DECL(SDP);
X2S_FUN_DECL(SDPRef);
X2S_FUN_DECL(USBDRef);
X2S_FUN_DECL(ADPRef);
X2S_FUN_DECL(SessionDescription);
X2S_FUN_DECL(File);
X2S_FUN_DECL(FileDescription);
X2S_FUN_DECL(BroadcastServiceDelivery);
X2S_FUN_DECL(AccessType);
X2S_FUN_DECL(KeyManagementSystem);
X2S_FUN_DECL(EncryptionType);
X2S_FUN_DECL(TerminalCapabilityRequirement);
X2S_FUN_DECL(BandwidthRequirement);
X2S_FUN_DECL(ServiceClass);
X2S_FUN_DECL(Access);
X2S_FUN_DECL(Type);
X2S_FUN_DECL(Name);
X2S_FUN_DECL(StartTime);
X2S_FUN_DECL(EndTime);
X2S_FUN_DECL(Description);
X2S_FUN_DECL(Extension);
X2S_FUN_DECL(PreviewDataReference);
X2S_FUN_DECL(Genre);
X2S_FUN_DECL(TextLanguage);
X2S_FUN_DECL(ParentalRating);
X2S_FUN_DECL(TargetUserProfile);
X2S_FUN_DECL(ProtectionKeyID);
X2S_FUN_DECL(AudioLanguage);
//X2S_FUN_DECL(IPBroadcastDelivery);
X2S_FUN_DECL(RequestURL);
X2S_FUN_DECL(PollURL);
X2S_FUN_DECL(NotificationReception);
X2S_FUN_DECL(PurchaseItemReference);
X2S_FUN_DECL(PresentationWindowIDRef);
X2S_FUN_DECL(cc);
X2S_FUN_DECL(name_area);
X2S_FUN_DECL(ZipCode);
X2S_FUN_DECL(PP2CellID);
X2S_FUN_DECL(CellArea);
X2S_FUN_DECL(CellTargetArea);
X2S_FUN_DECL(TargetArea);
X2S_FUN_DECL(hor_acc);
X2S_FUN_DECL(BroadcastArea);
X2S_FUN_DECL(Country);
X2S_FUN_DECL(Language);
X2S_FUN_DECL(PreviewDataIDRef);
X2S_FUN_DECL(TermsOfUseText);
X2S_FUN_DECL(TermsOfUse);
X2S_FUN_DECL(PrivateExt);
X2S_FUN_DECL(ServiceReference);
X2S_FUN_DECL(Length);
X2S_FUN_DECL(Content);
X2S_FUN_DECL(sg_fdtFile);
X2S_FUN_DECL(InteractivityType);
X2S_FUN_DECL(InteractivityWindow);
X2S_FUN_DECL(InteractiveDelivery);
X2S_FUN_DECL(BackOffTiming);
X2S_FUN_DECL(InteractivityData);
X2S_FUN_DECL(ContactInfo);
X2S_FUN_DECL(PortalURL);
X2S_FUN_DECL(PurchaseURL);
X2S_FUN_DECL(PurchaseChannel);
X2S_FUN_DECL(MonetaryPrice);
X2S_FUN_DECL(TotalNumberToken);
X2S_FUN_DECL(SubscriptionPeriod);
X2S_FUN_DECL(PriceInfo);
X2S_FUN_DECL(Title);
X2S_FUN_DECL(PromotionExtension);
X2S_FUN_DECL(PromotionInfo);
X2S_FUN_DECL(PurchaseChannelReference);
X2S_FUN_DECL(PreviwDataReference);
X2S_FUN_DECL(PurchaseData);
X2S_FUN_DECL(ScheduleReference);
X2S_FUN_DECL(Dependencies);
X2S_FUN_DECL(Exclusions);
X2S_FUN_DECL(PurchaseItem);
X2S_FUN_DECL(SMIL);
X2S_FUN_DECL(VideoURI);
X2S_FUN_DECL(MIMEType);
X2S_FUN_DECL(AlternativeText);
X2S_FUN_DECL(AlternativePicture);
X2S_FUN_DECL(Video);
X2S_FUN_DECL(AudioURI);
X2S_FUN_DECL(Audio);
X2S_FUN_DECL(PictureURI);
X2S_FUN_DECL(PictureData);
X2S_FUN_DECL(Picture);
X2S_FUN_DECL(Text);
X2S_FUN_DECL(AccessReference);
X2S_FUN_DECL(PreviewData);
X2S_FUN_DECL(AutoStart);
X2S_FUN_DECL(DistributionWindow);
X2S_FUN_DECL(InteractivityDataReference);
X2S_FUN_DECL(PresentationWindow);
X2S_FUN_DECL(ContentReference);
X2S_FUN_DECL(Schedule);
X2S_FUN_DECL(BSMFilterCode);
X2S_FUN_DECL(RoamingRule);
X2S_FUN_DECL(BSMSelector);
X2S_FUN_DECL(BSMList);
X2S_FUN_DECL(TimeGroupingCriteria);
X2S_FUN_DECL(ServiceComponentID);

X2S_FUN_DECL(GenreGroupingCriteria);
X2S_FUN_DECL(ServiceCriteria);
X2S_FUN_DECL(GroupingCriteria);
//X2S_FUN_DECL(Transport);
X2S_FUN_DECL(AlternativeAccessURL);
X2S_FUN_DECL(Fragment);
X2S_FUN_DECL(ServiceGuideDeliveryUnit);
X2S_FUN_DECL(DescriptorEntry);
X2S_FUN_DECL(ServiceGuideDeliveryDescriptor);
X2S_FUN_DECL(ServiceType);
X2S_FUN_DECL(TerminalBindingKeyID);
X2S_FUN_DECL(GlobalServiceID);
X2S_FUN_DECL(PermissionsIssuerURI);
X2S_FUN_DECL(shape);
X2S_FUN_DECL(Service);

X2S_FUN_DECL(MediaObjectGroup);

#ifdef __cplusplus
}
#endif

#endif /* __SG_X2S_FUN_H__ */
