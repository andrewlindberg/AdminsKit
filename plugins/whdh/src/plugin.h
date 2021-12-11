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

#pragma once

#include "beams.h"
#include "data.h"
#include "marker.h"
#include "resources.h"
#include <cssdk/public/os_defs.h>
#include <mhooks/metamod.h>
#include <mhooks/reapi.h>
#include <memory>
#include <vector>

namespace whdh
{
    class Plugin final
    {
        Beams beams_;
        Marker marker_;
        std::shared_ptr<Data> data_;
        std::shared_ptr<Resources> resources_;
        std::vector<std::unique_ptr<mhooks::MHook>> hooks_{};

    public:
        Plugin(Beams beams, Marker marker, std::shared_ptr<Data> data, std::shared_ptr<Resources> resources);
        ~Plugin();

        Plugin(Plugin&&) = delete;
        Plugin(const Plugin&) = delete;

        Plugin& operator=(Plugin&&) = delete;
        Plugin& operator=(const Plugin&) = delete;

    private:
        void OnResourcesNotice() const;

        ATTR_OPTIMIZE_HOT cssdk::qboolean
        OnAddToFullPack(const GameDllAddToFullPackMChain& chain, cssdk::EntityState* state, int entity_index,
                        cssdk::Edict* entity, cssdk::Edict* host, int host_flags, cssdk::qboolean player,
                        unsigned char* set) const;

        void OnPlayerPreThink(const ReGamePlayerPreThinkMChain& chain, cssdk::PlayerBase* player);
        void OnPlayerSpawn(const ReGamePlayerSpawnMChain& chain, cssdk::PlayerBase* player) const;
        void OnPlayerKilled(const ReGamePlayerKilledMChain& chain, cssdk::PlayerBase* victim, cssdk::EntityVars* attacker, int gib) const;
        void OnDropClient(const ReHldsDropClientMChain& chain, cssdk::IGameClient* client, bool crash, const char* reason) const;
    };
}
