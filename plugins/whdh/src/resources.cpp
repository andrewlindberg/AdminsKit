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

#include "resources.h"
#include <cssdk/public/utils.h>
#include <metamod/engine.h>
#include <whdh/cvars.h>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace metamod;

namespace whdh
{
    Resources::Resources()
    {
        install_game_rules_hook_ =
            MHookReGameRulesInstallGameRules(
                {DELEGATE_ARG<&Resources::OnInstallGameRules>, this})
                ->Unique();

        dispatch_spawn_hook_ =
            MHookGameDllDispatchSpawn(
                {DELEGATE_ARG<&Resources::OnDispatchSpawn>, this}, false, HookChainPriority::Normal, false)
                ->Unique();

        server_deactivate_post_hook_ =
            MHookGameDllServerDeactivate(
                {DELEGATE_ARG<&Resources::OnServerDeactivatePost>, this}, true)
                ->Unique();
    }

    GameRules* Resources::OnInstallGameRules(const ReGameRulesInstallGameRulesMChain& chain)
    {
        beam_sprite_index_ = marker_sprite_index_ = INVALID_INDEX;

        install_game_rules_hook_->Disable();
        dispatch_spawn_hook_->Enable();

        return chain.CallNext();
    }

    int Resources::OnDispatchSpawn(const GameDllDispatchSpawnMChain& chain, Edict* const entity)
    {
        if (IsValidEntity(entity)) {
            beam_sprite_path_ = cvars::BeamSprite();
            beam_sprite_index_ = engine::PrecacheModel(beam_sprite_path_.c_str());

            marker_sprite_path_ = cvars::MarkerSprite();
            marker_sprite_index_ = engine::PrecacheModel(marker_sprite_path_.c_str());

            string_whdh_ = engine::AllocString("whdh");
            string_info_target_ = engine::AllocString("info_target");
            string_marker_sprite_ = engine::AllocString(marker_sprite_path_.c_str());

            dispatch_spawn_hook_->Disable();
            Notify();
        }

        return chain.CallNext(entity);
    }

    void Resources::OnServerDeactivatePost(const GameDllServerDeactivateMChain& chain)
    {
        beam_sprite_index_ = marker_sprite_index_ = INVALID_INDEX;

        install_game_rules_hook_->Enable();
        dispatch_spawn_hook_->Disable();
        Notify();

        chain.CallNext();
    }
}
