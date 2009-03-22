/* vim:set et sw=4 sts=4: */
#ifndef __SG_S2N_FUN_H__
#define __SG_S2N_FUN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ktypes.h>
#include "xmldoc.h"
#include "s2n_macro.h"
#include "ivisa_sg_config.h"

S2N_FUN_DECL(Version);
S2N_FUN_DECL(BDSType);
S2N_FUN_DECL(SDP);
S2N_FUN_DECL(SDPRef);
S2N_FUN_DECL(USBDRef);
S2N_FUN_DECL(ADPRef);
S2N_FUN_DECL(SessionDescription);
S2N_FUN_DECL(File);
S2N_FUN_DECL(FileDescription);
S2N_FUN_DECL(BroadcastServiceDelivery);
S2N_FUN_DECL(AccessType);
S2N_FUN_DECL(KeyManagementSystem);
S2N_FUN_DECL(EncryptionType);
S2N_FUN_DECL(TerminalCapabilityRequirement);
S2N_FUN_DECL(BandwidthRequirement);
S2N_FUN_DECL(ServiceClass);
//S2N_FUN_DECL(Access);
S2N_FUN_DECL(Type);
S2N_FUN_DECL(Name);
S2N_FUN_DECL(StartTime);
S2N_FUN_DECL(EndTime);
S2N_FUN_DECL(Description);
S2N_FUN_DECL(Extension);
S2N_FUN_DECL(PreviewDataReference);
S2N_FUN_DECL(Genre);
S2N_FUN_DECL(TextLanguage);
S2N_FUN_DECL(ParentalRating);
S2N_FUN_DECL(TargetUserProfile);
S2N_FUN_DECL(ProtectionKeyID);
S2N_FUN_DECL(AudioLanguage);

S2N_FUN_DECL(IPBroadcastDelivery);

S2N_FUN_DECL(IPBroadcastDelivery);

S2N_FUN_DECL(BroadcastDelivery);

S2N_FUN_DECL(RequestURL);
S2N_FUN_DECL(PollURL);
S2N_FUN_DECL(NotificationReception);
S2N_FUN_DECL(PurchaseItemReference);
S2N_FUN_DECL(PresentationWindowIDRef);
S2N_FUN_DECL(cc);
S2N_FUN_DECL(name_area);
S2N_FUN_DECL(ZipCode);
S2N_FUN_DECL(PP2CellID);
S2N_FUN_DECL(CellArea);
S2N_FUN_DECL(CellTargetArea);
S2N_FUN_DECL(TargetArea);
S2N_FUN_DECL(hor_acc);
S2N_FUN_DECL(BroadcastArea);
S2N_FUN_DECL(Country);
S2N_FUN_DECL(Language);
S2N_FUN_DECL(PreviewDataIDRef);
S2N_FUN_DECL(TermsOfUseText);
S2N_FUN_DECL(TermsOfUse);
S2N_FUN_DECL(PrivateExt);
S2N_FUN_DECL(ServiceReference);
S2N_FUN_DECL(Length);
//S2N_FUN_DECL(Content);
S2N_FUN_DECL(InteractivityType);
S2N_FUN_DECL(InteractivityWindow);
S2N_FUN_DECL(InteractiveDelivery);
S2N_FUN_DECL(BackOffTiming);
//S2N_FUN_DECL(InteractivityData);
S2N_FUN_DECL(ContactInfo);
S2N_FUN_DECL(PortalURL);
S2N_FUN_DECL(PurchaseURL);
S2N_FUN_DECL(PurchaseChannel);
S2N_FUN_DECL(MonetaryPrice);
S2N_FUN_DECL(TotalNumberToken);
S2N_FUN_DECL(SubscriptionPeriod);
S2N_FUN_DECL(PriceInfo);
S2N_FUN_DECL(Title);
S2N_FUN_DECL(PromotionExtension);
S2N_FUN_DECL(PromotionInfo);
S2N_FUN_DECL(PurchaseChannelReference);
S2N_FUN_DECL(PreviwDataReference);
S2N_FUN_DECL(PurchaseData);
S2N_FUN_DECL(ScheduleReference);
S2N_FUN_DECL(Dependencies);
S2N_FUN_DECL(Exclusions);
S2N_FUN_DECL(PurchaseItem);
S2N_FUN_DECL(SMIL);
S2N_FUN_DECL(VideoURI);
S2N_FUN_DECL(MIMEType);
S2N_FUN_DECL(AlternativeText);
S2N_FUN_DECL(AlternativePicture);
S2N_FUN_DECL(Video);
S2N_FUN_DECL(AudioURI);
S2N_FUN_DECL(Audio);
S2N_FUN_DECL(PictureURI);
S2N_FUN_DECL(PictureData);
S2N_FUN_DECL(Picture);
S2N_FUN_DECL(Text);
S2N_FUN_DECL(AccessReference);
S2N_FUN_DECL(PreviewData);
S2N_FUN_DECL(AutoStart);
S2N_FUN_DECL(DistributionWindow);
S2N_FUN_DECL(InteractivityDataReference);
S2N_FUN_DECL(PresentationWindow);
S2N_FUN_DECL(ContentReference);
S2N_FUN_DECL(Schedule);
S2N_FUN_DECL(BSMFilterCode);
S2N_FUN_DECL(RoamingRule);
S2N_FUN_DECL(BSMSelector);
S2N_FUN_DECL(BSMList);
S2N_FUN_DECL(TimeGroupingCriteria);
S2N_FUN_DECL(GenreGroupingCriteria);
S2N_FUN_DECL(ServiceCriteria);
S2N_FUN_DECL(GroupingCriteria);
S2N_FUN_DECL(Transport);
S2N_FUN_DECL(AlternativeAccessURL);
S2N_FUN_DECL(Fragment);
S2N_FUN_DECL(ServiceGuideDeliveryUnit);
S2N_FUN_DECL(DescriptorEntry);
S2N_FUN_DECL(ServiceGuideDeliveryDescriptor);
S2N_FUN_DECL(ServiceType);
S2N_FUN_DECL(TerminalBindingKeyID);
S2N_FUN_DECL(GlobalServiceID);
S2N_FUN_DECL(PermissionsIssuerURI);
S2N_FUN_DECL(shape);
//S2N_FUN_DECL(Service);

S2N_FUN_DECL(InteractivityMediaDocument);

#ifdef __cplusplus
}
#endif

#endif /* __SG_S2N_FUN_H__ */
