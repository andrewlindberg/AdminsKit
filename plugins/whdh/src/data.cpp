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

#include "data.h"
#include <core/type_conversion.h>
#include <cssdk/engine/global_vars.h>

using namespace core;
using namespace cssdk;
using namespace mhooks;

namespace whdh
{
    Data::Data()
    {
        player_take_damage_hook_ =
            MHookReGamePlayerTakeDamage({DELEGATE_ARG<&Data::OnPlayerTakeDamage>, this})->Unique();
    }

    qboolean Data::OnPlayerTakeDamage(const ReGamePlayerTakeDamageMChain& chain, PlayerBase* const victim,
                                      EntityVars* const inflictor, EntityVars* const attacker, float& damage,
                                      const int damage_type)
    {
        const auto take_damage = chain.CallNext(victim, inflictor, attacker, damage, damage_type);

        if (take_damage && damage_type & DMG_BULLET && victim->vars != attacker && IsValidEntity(attacker)) {
            const auto victim_index = type_conversion::IndexOfEntity(victim);
            const auto attacker_index = type_conversion::IndexOfEntity(attacker->containing_entity);

            if (IsClient(attacker_index) && IsClient(victim_index)) {
                last_hit_time_[attacker_index][victim_index] = g_global_vars->time;
            }
        }

        return take_damage;
    }
}
