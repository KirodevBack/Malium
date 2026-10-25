#pragma once
#include "pch.h"
#include "URL.h"

namespace Malium 
{
    namespace Redirection 
    {
        bool ShouldRedirect(URL& uri);
    }
}