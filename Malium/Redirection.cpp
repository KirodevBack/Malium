#include "pch.h"
#include "Redirection.h"
#include "Options.h"
#include "URL.h"

namespace Malium
{
    namespace Redirection
    {
        bool ShouldRedirect(URL& uri) 
        {
            constexpr static const wchar_t* redirectedPaths[] =
            {
                L"/fortnite/api/v2/versioncheck/",
                L"/fortnite/api/game/v2/profile/",
                L"/content/api/pages/",
                L"/affiliate/api/public/affiliates/slug",
                L"/socialban/api/public/v1",
                L"/fortnite/api/cloudstorage/system"
            };

            constexpr static const wchar_t* redirectedPathsDev[] =
            {
                L"/fortnite/api/game/v2/profile/",
                L"/affiliate/api/public/affiliates/slug",
                L"/content/api/pages/"
            };

            static constexpr const wchar_t* redirectedUrls[] =
            {
                L"ol.epicgames.com",
                L"ol.epicgames.net",
                //L".akamaized.net", // if you have your own blurl
                //L"cdn2.unrealengine.com", // only needed if you have fixed friend images in friends list
                L"on.epicgames.com",
                L"game-social.epicgames.com",
                L"ak.epicgames.com",
                L"epicgames.dev",
                L"superawesome.com"
            };


            switch (URLSet) 
            {
            case Hybrid:
                for (const auto* path : redirectedPaths)
                    if (uri.Path.starts_with(path)) 
                        return true;

                break;
            case Dev:
                for (const auto* path : redirectedPathsDev)
                    if (uri.Path.starts_with(path))
                        return true;

                break;
            case Default:
                for (const auto* url : redirectedUrls)
                    if (uri.Domain.ends_with(url)) 
                        return true;

                break;
            case All:
                return true;
            }

            return false;
        }
    }
}
