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

#include "plugin.h"
#include <unlimited_choose_team/cvars.h>
#include <core/amxx_access.h>
#include <core/regamedll_api.h>
#include <cssdk/public/utils.h>
#include <cassert>

using namespace core;
using namespace cssdk;
using namespace mhooks;

namespace
{
    [[nodiscard]] bool HasAccess(const PlayerBase* const player)
    {
        assert(IsValidEntity(player));
        return amxx_access::Has(player, unlimited_choose_team::cvars::Access());
    }
}

namespace unlimited_choose_team
{
    Plugin::Plugin()
    {
        hooks_.emplace_back(MHookReGameShowVguiMenu(DELEGATE_ARG<&Plugin::OnShowVguiMenu>)->Unique());
        hooks_.emplace_back(MHookReGameHandleMenuChooseTeam(DELEGATE_ARG<&Plugin::OnHandleMenuChooseTeam>)->Unique());
        hooks_.emplace_back(MHookReGamePlayerPostThink(DELEGATE_ARG<&Plugin::OnPlayerPostThink>)->Unique());
    }

    void Plugin::OnShowVguiMenu(const ReGameShowVguiMenuMChain& chain, PlayerBase* const player,
                                const VguiMenu menu, const int keys, char* const old_menu)
    {
        if (menu != VguiMenu::Team || !HasAccess(player)) {
            chain.CallNext(player, menu, keys, old_menu);
            return;
        }

        auto* const cs_player = player->GetCsPlayer();
        cs_player->force_show_menu = true;

        static char team_select[] = "#IG_Team_Select_Spect";
        chain.CallNext(player, menu, MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_0, team_select);
    }

    qboolean Plugin::OnHandleMenuChooseTeam(const ReGameHandleMenuChooseTeamMChain& chain, PlayerBase* const player,
                                            const ChooseTeamMenuSlot slot)
    {
        if (slot == ChooseTeamMenuSlot::Spectator && HasAccess(player) && IsPlayerAlive(player)) {
            regamedll_api::GameRules()->ChangePlayerTeam(player, "SPECTATOR", true, false);
            return 1;
        }

        return chain.CallNext(player, slot);
    }

    void Plugin::OnPlayerPostThink(const ReGamePlayerPostThinkMChain& chain, PlayerBase* const player)
    {
        chain.CallNext(player);

        if (HasAccess(player)) {
            player->team_changed = false;
        }
    }
}
