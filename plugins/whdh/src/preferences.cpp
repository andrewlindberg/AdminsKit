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
#include <core/console.h>
#include <core/messages.h>
#include <core/strings.h>
#include <cssdk/public/utils.h>
#include <metamod/engine.h>
#include <whdh/cvars.h>
#include <cassert>
#include <exception>
#include <utility>

using namespace core;
using namespace cssdk;
using namespace metamod;
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

    auto FmtMenuItemTranspEnt(Edict* const client, const UserPreferences& preferences, const ak::Localization& localization)
    {
        return str::Format("%s %s", localization.GetText("TRANSP_ENTITIES", client),
                           localization.GetText(preferences.transp_entities ? ON : OFF, client));
    }

    auto FmtMenuItemExit(Edict* const client, const ak::Localization& localization)
    {
        return localization.GetText("EXIT", client);
    }
}

namespace whdh
{
    Preferences::Preferences(PreferencesBin&& bin, ak::Localization&& localization, std::shared_ptr<Resources> resources)
        : bin_(std::move(bin)), localization_(std::move(localization)), resources_(std::move(resources))
    {
        hooks_.emplace_back(MHookReGamePlayerPreThink({DELEGATE_ARG<&Preferences::OnPlayerPreThink>, this})->Unique());
        hooks_.emplace_back(MHookAmxxClientAuthorized({DELEGATE_ARG<&Preferences::OnClientAuthorized>, this})->Unique());
        hooks_.emplace_back(MHookGameDllServerDeactivate({DELEGATE_ARG<&Preferences::OnServerDeactivatePost>, this}, true)->Unique());

        try {
            bin_.Deserialize();
        }
        catch (const std::exception& ex) {
            console::Error("Failed to deserialize WHDH user settings: %s", ex.what());
        }
        catch (...) {
            console::Error("Failed to deserialize WHDH user settings. Unknown error.");
        }
    }

    void Preferences::ShowMenu(Edict* const client)
    {
        assert(IsValidEntity(client));

        const auto& preferences = operator[](client);
        const auto& text = str::Format("%s\n\n1. %s\n2. %s\n3. %s\n4. %s\n5. %s\n\n0. %s\n",
                                       FmtMenuTitle(client, localization_),
                                       FmtMenuItemVisibility(client, preferences, localization_),
                                       FmtMenuItemMarker(client, preferences, localization_),
                                       FmtMenuItemLines(client, preferences, localization_),
                                       FmtMenuItemBoxes(client, preferences, localization_),
                                       FmtMenuItemTranspEnt(client, preferences, localization_),
                                       FmtMenuItemExit(client, localization_));

        menu_.Show(client, MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_0, text);
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

        case 5:
            operator[](client).transp_entities ^= true;
            break;

        default:
            return;
        }

        bin_.Save(engine::GetPlayerAuthId(client), operator[](client));
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

    void Preferences::OnClientAuthorized(const AmxxClientAuthorizedMChain& chain, const int index, const char* const auth_id)
    {
        preferences_[index] = bin_.Load(auth_id);
        chain.CallNext(index, auth_id);
    }

    void Preferences::OnServerDeactivatePost(const GameDllServerDeactivateMChain& chain)
    {
        chain.CallNext();

        try {
            bin_.Serialize();
        }
        catch (const std::exception& ex) {
            console::Error("Failed to serialize WHDH user settings: %s", ex.what());
        }
        catch (...) {
            console::Error("Failed to serialize WHDH user settings. Unknown error.");
        }
    }
}
