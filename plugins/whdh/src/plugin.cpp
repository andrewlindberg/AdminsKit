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
#include <core/amxx_access.h>
#include <core/type_conversion.h>
#include <cssdk/common/const.h>
#include <cssdk/engine/global_vars.h>
#include <cssdk/public/utils.h>
#include <whdh/cvars.h>
#include <cassert>
#include <utility>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace metamod;

namespace
{
    [[nodiscard]] bool HasAccess(const int player)
    {
        return amxx_access::Has(player, whdh::cvars::Access());
    }
}

namespace whdh
{
    Plugin::Plugin(Beams beams, Marker marker, std::shared_ptr<Data> data, std::shared_ptr<Resources> resources)
        : beams_(std::move(beams)), marker_(std::move(marker)), data_(std::move(data)), resources_(std::move(resources))
    {
        assert(data_ != nullptr);
        assert(resources_ != nullptr);

        hooks_.emplace_back(
            MHookGameDllAddToFullPack(
                {DELEGATE_ARG<&Plugin::OnAddToFullPack>, this},
                false, HookChainPriority::Normal, false)
                ->Unique());

        hooks_.emplace_back(
            MHookReGamePlayerPreThink(
                {DELEGATE_ARG<&Plugin::OnPlayerPreThink>, this},
                HookChainPriority::Normal, false)
                ->Unique());

        hooks_.emplace_back(
            MHookReGamePlayerSpawn(
                {DELEGATE_ARG<&Plugin::OnPlayerSpawn>, this},
                HookChainPriority::Normal, false)
                ->Unique());

        hooks_.emplace_back(
            MHookReGamePlayerKilled(
                {DELEGATE_ARG<&Plugin::OnPlayerKilled>, this},
                HookChainPriority::Normal, false)
                ->Unique());

        hooks_.emplace_back(
            MHookReHldsDropClient(
                {DELEGATE_ARG<&Plugin::OnDropClient>, this},
                HookChainPriority::Normal, false)
                ->Unique());

        resources_->Subscribe(
            {DELEGATE_ARG<&Plugin::OnResourcesNotice>, this});
    }

    Plugin::~Plugin()
    {
        assert(g_global_vars != nullptr);
        assert(g_global_vars->max_clients > 0 && g_global_vars->max_clients <= MAX_CLIENTS);

        for (auto i = 1; i <= g_global_vars->max_clients; ++i) {
            marker_.DestroyEntity(i);
        }
    }

    void Plugin::OnResourcesNotice() const
    {
        if (resources_->Precached()) {
            for (const auto& hook : hooks_) {
                hook->Enable();
            }
        }
        else {
            for (const auto& hook : hooks_) {
                hook->Disable();
            }
        }

        data_->Reset();
    }

    qboolean Plugin::OnAddToFullPack(const GameDllAddToFullPackMChain& chain, EntityState* const state,
                                     const int entity_index, Edict* const entity, Edict* const host,
                                     const int host_flags, const qboolean player, unsigned char* const set) const
    {
        const auto result = chain.CallNext(state, entity_index, entity, host, host_flags, player, set);

        if (player || !IsValidEntity(entity) || !Marker::IsMarker(entity)) {
            return result;
        }

        SetResult(Result::Supercede);

        if (!IsValidEntity(entity->vars.owner) || !IsPlayerAlive(entity->vars.owner)) {
            return 0;
        }

        if (entity->vars.owner == host || !IsValidEntity(host) || IsPlayerAlive(host)) {
            return 0;
        }

        const auto host_index = type_conversion::IndexOfEntity(host);

        if (!IsClient(host_index) || !HasAccess(host_index)) {
            return 0;
        }

        return marker_.Draw(state, entity_index, entity, host, host_index, host_flags, player, set);
    }

    void Plugin::OnPlayerPreThink(const ReGamePlayerPreThinkMChain& chain, PlayerBase* const player)
    {
        chain.CallNext(player);

        if (!IsPlayerAlive(player) && player->team != TeamName::Unassigned) {
            if (const auto player_index = type_conversion::IndexOfEntity(player); HasAccess(player_index)) {
                beams_.Draw(player, player_index);
            }
        }
    }

    void Plugin::OnPlayerSpawn(const ReGamePlayerSpawnMChain& chain, PlayerBase* const player) const
    {
        chain.CallNext(player);

        if (!player->just_connected && player->team != TeamName::Unassigned && player->team != TeamName::Spectator &&
            IsPlayerAlive(player)) {
            marker_.CreateEntity(player);
        }
    }

    void Plugin::OnPlayerKilled(const ReGamePlayerKilledMChain& chain, PlayerBase* const victim,
                                EntityVars* const attacker, const int gib) const
    {
        chain.CallNext(victim, attacker, gib);
        marker_.DestroyEntity(victim);
    }

    void Plugin::OnDropClient(const ReHldsDropClientMChain& chain, IGameClient* const client, const bool crash,
                              const char* const reason) const
    {
        chain.CallNext(client, crash, reason);

        if (client) {
            if (const auto index = client->GetId() + 1; IsClient(index)) {
                marker_.DestroyEntity(index);
            }
        }
    }
}
