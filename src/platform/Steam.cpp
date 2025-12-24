#include "Steam.hpp"
#include <steam/steam_api.h>
#include <iostream>

namespace Steam
{
    bool Init()
    {
        if (!SteamAPI_Init())
        {
            std::cerr << "SteamAPI_Init failed\n";
            return false;
        }

        std::cout << "Steam initialized\n";
        return true;
    }

    void Shutdown()
    {
        SteamAPI_Shutdown();
    }
}
