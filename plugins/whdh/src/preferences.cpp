/*
 *  Copyright (C) 2020 the_hunter
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "preferences.h"
#include <core/amxx_access.h>
#include <core/messages.h>
#include <core/strings.h>
#include <cssdk/public/utils.h>
#include <whdh/cvars.h>
#include <cassert>
#include <utility>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace whdh;

namespace
{
    namespace ak = adminskit;
    constexpr auto ON = "ON"; //-V707
    constexpr auto OFF = "OFF"; //-V707

    bool HasAccess(const PlayerBase* const player)
    {
        return amxx_access::Has(player, cvars::Access());
    }

    auto FmtMenuTitle(Edict* const client, const ak::Localization& localization)
    {
        return localization.GetText("MENU_TITLE", client);
    }

    auto FmtMenuItemVisibility(Edict* const client, const UserPreferences& preferences, const ak::Localization& localization)
    {
        const auto* const label = [preferences] {
            switch (preferences.visibility) {
            case Visibility::Off:
                return "TURN_OFF";
            case Visibility::Opponents:
                return "OPPONENTS";
            case Visibility::Teammates:
                return "TEAMMATES";
            case Visibility::OpponentsAndTeammates:
                return "OPP_TEAM";
            }

            return str::EMPTY;
        }();

        return localization.GetText(label, client);
    }

    auto FmtMenuItemMarker(Edict* const client, const UserPreferences& preferences, const ak::Localization& localization)
    {
        return str::Format("%s %s", localization.GetText("MARKER", client),
                           localization.GetText(preferences.marker ? ON : OFF, client));
    }

    auto FmtMenuItemLines(Edict* const client, const UserPreferences& preferences, const ak::Localization& localization)
    {
        return str::Format("%s %s", localization.GetText("LINES", client),
                           localization.GetText(preferences.lines ? ON : OFF, client));
    }

    auto FmtMenuItemBoxes(Edict* const client, const UserPreferences& preferences, const ak::Localization& localization)
    {
        return str::Format("%s %s", localization.GetText("BOXES", client),
                           localization.GetText(preferences.boxes ? ON : OFF, client));
    }

    auto FmtMenuItemExit(Edict* const client, const ak::Localization& localization)
    {
        return localization.GetText("EXIT", client);
    }
}

namespace whdh
{
    Preferences::Preferences(ak::Localization&& localization, std::shared_ptr<Resources> resources)
        : localization_(std::move(localization)), resources_(std::move(resources))
    {
        hooks_.emplace_back(MHookReGamePlayerPreThink({DELEGATE_ARG<&Preferences::OnPlayerPreThink>, this})->Unique());
        hooks_.emplace_back(MHookReHldsClientConnected({DELEGATE_ARG<&Preferences::OnClientConnected>, this})->Unique());
    }

    void Preferences::ShowMenu(Edict* const client)
    {
        assert(IsValidEntity(client));

        const auto& preferences = operator[](client);
        const auto& text = str::Format("%s\n\n1. %s\n2. %s\n3. %s\n4. %s\n\n0. %s\n",
                                       FmtMenuTitle(client, localization_),
                                       FmtMenuItemVisibility(client, preferences, localization_),
                                       FmtMenuItemMarker(client, preferences, localization_),
                                       FmtMenuItemLines(client, preferences, localization_),
                                       FmtMenuItemBoxes(client, preferences, localization_),
                                       FmtMenuItemExit(client, localization_));

        menu_.Show(client, MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_0, text);
    }

    void Preferences::MenuHandler(Edict* const client, const int selected_item)
    {
        if (IsPlayerAlive(client)) {
            return;
        }

        switch (selected_item) {
        case 1:
            ++operator[](client).visibility;
            break;

        case 2:
            operator[](client).marker ^= true;
            break;

        case 3:
            operator[](client).lines ^= true;
            break;

        case 4:
            operator[](client).boxes ^= true;
            break;

        default:
            return;
        }

        ShowMenu(client);
    }

    void Preferences::OnPlayerPreThink(const ReGamePlayerPreThinkMChain& chain, PlayerBase* const player)
    {
        chain.CallNext(player);
        assert(IsValidEntity(player));

        if (player->button_pressed & IN_RELOAD && !IsPlayerAlive(player) && HasAccess(player)) {
            if (!resources_->Precached()) {
                messages::SendTextMessage(player->GetEdict(), HudPrint::Center, localization_.GetText("NOT_READY"));
            }
            else if (auto* const edict = player->GetEdict(); menu_.IsOpen(edict)) {
                menu_.Close(edict);
            }
            else {
                ShowMenu(edict);
            }
        }
    }

    void Preferences::OnClientConnected(const ReHldsClientConnectedMChain& chain, IGameClient* const client)
    {
        chain.CallNext(client);

        if (client) {
            if (const auto client_index = client->GetId() + 1; IsClient(client_index)) {
                preferences_[client_index].Reset();
            }
        }
    }
}
