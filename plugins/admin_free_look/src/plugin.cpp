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
#include <admin_free_look/cvars.h>
#include <core/amxx_access.h>
#include <core/regamedll_api.h>
#include <cassert>

using namespace core;
using namespace cssdk;
using namespace mhooks;

namespace
{
    const HalfLifeMultiplay* g_game_rules{};
}

namespace admin_free_look
{
    Plugin::Plugin()
    {
        if (const auto* const game_rules = regamedll_api::GameRules(); game_rules) {
            g_game_rules = game_rules->CsGameRules();
        }

        hooks_.emplace_back(MHookReGameRulesInstallGameRules(DELEGATE_ARG<&Plugin::OnInstallGameRules>)->Unique());
        hooks_.emplace_back(MHookReGameGetForceCamera(DELEGATE_ARG<&Plugin::OnGetForceCamera>)->Unique());
    }

    GameRules* Plugin::OnInstallGameRules(const ReGameRulesInstallGameRulesMChain& chain)
    {
        auto* const rules = chain.CallNext();

        assert(rules != nullptr);
        g_game_rules = rules->CsGameRules();

        return rules;
    }

    int Plugin::OnGetForceCamera(const ReGameGetForceCameraMChain& chain, PlayerBase* const observer)
    {
        assert(g_game_rules != nullptr);
        const auto force_camera = chain.CallNext(observer);

        if ((g_game_rules->force_camera_value > 0.F || g_game_rules->force_chase_cam_value > 0.F) &&
            g_game_rules->fade_to_black_value <= 0.F) {
            return amxx_access::Has(observer, cvars::Access()) ? 0 : force_camera;
        }

        return force_camera;
    }
}
