#import "Rate.h"

#import <GoogleMobileAds/GADBannerView.h>
#import <GoogleMobileAds/GADInterstitial.h>

#ifndef LITEVERSION
//#import "SA_OAuthTwitterEngine.h"
//#import "SA_OAuthTwitterController.h"
#import "FacebookSDK/FacebookSDK.h"
#endif

@interface SocialPlugins : NSObject
#ifndef LITEVERSION
//< SA_OAuthTwitterControllerDelegate >
#endif
{
    @public
        GADBannerView*          bannerView_;
#ifndef LITEVERSION
//        SA_OAuthTwitterEngine*  twitter;
#endif
}

#ifndef LITEVERSION
    //@property ( strong, nonatomic ) FBSession* session;
    //@property ( strong, nonatomic ) Facebook*  facebook;
#endif

@end

