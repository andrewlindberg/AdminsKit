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

#include <core/type_conversion.h>
#include <cssdk/common/const.h>
#include <cssdk/public/utils.h>
#include <mhooks/reapi.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <memory>

namespace whdh
{
    class Data final
    {
        std::unique_ptr<mhooks::MHook> player_take_damage_hook_{};
        std::array<float, cssdk::MAX_CLIENTS + 1> beam_update_time_{};
        std::array<cssdk::Edict*, cssdk::MAX_CLIENTS + 1> marker_entities_{};
        std::array<std::array<float, cssdk::MAX_CLIENTS + 1>, cssdk::MAX_CLIENTS + 1> last_hit_time_{};

    public:
        Data();
        ~Data() = default;

        Data(Data&&) = delete;
        Data(const Data&) = delete;

        Data& operator=(Data&&) = delete;
        Data& operator=(const Data&) = delete;

        [[nodiscard]] float& BeamUpdateTime(const int player) noexcept
        {
            assert(cssdk::IsClient(player));
            return beam_update_time_[player];
        }

        [[nodiscard]] cssdk::Edict*& MarkerEntity(const int owner) noexcept
        {
            assert(cssdk::IsClient(owner));
            return marker_entities_[owner];
        }

        [[nodiscard]] cssdk::Edict*& MarkerEntity(const cssdk::Edict* const owner) noexcept
        {
            assert(cssdk::IsValidEntity(owner));
            return MarkerEntity(core::type_conversion::IndexOfEntity(owner));
        }

        [[nodiscard]] float LastHitTime(const int attacker, const int victim) const noexcept
        {
            assert(cssdk::IsClient(attacker));
            assert(cssdk::IsClient(victim));
            return last_hit_time_[attacker][victim];
        }

        void Reset() noexcept
        {
            beam_update_time_.fill(0.F);
            marker_entities_.fill(nullptr);
            std::for_each(last_hit_time_.begin(), last_hit_time_.end(), [](auto& element) { element.fill(0.F); });
        }

    private:
        cssdk::qboolean OnPlayerTakeDamage(const ReGamePlayerTakeDamageMChain& chain, cssdk::PlayerBase* victim,
                                           cssdk::EntityVars* inflictor, cssdk::EntityVars* attacker,
                                           float& damage, int damage_type);
    };
}
