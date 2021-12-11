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

#include <amxx/api.h>
#include <core/cvar.h>
#include <cssdk/common/const.h>

namespace server_stats::cvars
{
    namespace detail
    {
        inline int display{amxx::ReadFlags("ab")};
        inline cssdk::Color24 hud_color{48, 213, 200};
        inline float hud_pos_x{0.011F};
        inline float hud_pos_y{0.91F};
    }

    inline const cssdk::CVar* cvar_enable{};
    inline const cssdk::CVar* cvar_access{};
    inline const cssdk::CVar* cvar_command{};
    inline const cssdk::CVar* cvar_display{};
    inline const cssdk::CVar* cvar_update_rate{};
    inline const cssdk::CVar* cvar_hud_pos{};
    inline const cssdk::CVar* cvar_hud_color{};

    inline bool Enable()
    {
        return core::cvar::GetValue<bool>(cvar_enable);
    }

    inline int Access()
    {
        return amxx::ReadFlags(core::cvar::GetValue<const char*>(cvar_access));
    }

    inline const char* Command()
    {
        return core::cvar::GetValue<const char*>(cvar_command);
    }

    inline int Display()
    {
        return detail::display;
    }

    inline float UpdateRate()
    {
        return core::cvar::GetValue<float>(cvar_update_rate);
    }

    inline float HudPosX()
    {
        return detail::hud_pos_x;
    }

    inline float HudPosY()
    {
        return detail::hud_pos_y;
    }

    inline const cssdk::Color24& HudColor()
    {
        return detail::hud_color;
    }

    bool Register();
}
