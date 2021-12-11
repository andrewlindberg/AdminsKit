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

#include "data.h"
#include "preferences.h"
#include "resources.h"
#include <cssdk/public/os_defs.h>
#include <cssdk/public/utils.h>
#include <cassert>
#include <memory>

namespace whdh
{
    constexpr auto MARKER_UNIQUE_ID = 1618220438;

    class Marker final
    {
        std::shared_ptr<Data> data_;
        std::shared_ptr<Resources> resources_;
        std::shared_ptr<Preferences> preferences_;

    public:
        Marker(std::shared_ptr<Data> data, std::shared_ptr<Resources> resources, std::shared_ptr<Preferences> preferences);

        [[nodiscard]] static bool IsMarker(const cssdk::Edict* const entity) noexcept
        {
            assert(cssdk::IsValidEntity(entity));
            return entity->vars.impulse == MARKER_UNIQUE_ID;
        }

        ATTR_OPTIMIZE_HOT
        cssdk::qboolean Draw(cssdk::EntityState* state, int entity_index, cssdk::Edict* entity, cssdk::Edict* host,
                             int host_index, int host_flags, cssdk::qboolean player, unsigned char* set) const;

        void CreateEntity(const cssdk::PlayerBase* owner) const;
        void DestroyEntity(const cssdk::PlayerBase* owner) const;
        void DestroyEntity(int owner) const;
    };
}
