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

#include <server_stats/cvars.h>
#include <adminskit/utils.h>
#include <core/strings.h>
#include <cssdk/public/os_defs.h>
#include <mhooks/reapi.h>
#include <cstdio>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace adminskit;
using namespace server_stats;

namespace
{
    bool UpdateRateCvarPolicy(std::string& new_value)
    {
        if (!str::IsEmptyOrWhiteSpace(new_value)) {
            const auto rate = StringToDouble(new_value);
            return rate >= 0.1 && rate <= 10.;
        }

        return false;
    }

    void OnDisplayCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        ValidateCvarValue(cvar, old_value, new_value, FlagsCvarPolicy);
        cvars::detail::display = amxx::ReadFlags(new_value.c_str());
    }

    void OnUpdateRateCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        ValidateCvarValue(cvar, old_value, new_value, UpdateRateCvarPolicy);
    }

    void OnHudPosCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        if (ValidateCvarValue(cvar, old_value, new_value, HudPosCvarPolicy)) {
#ifdef MSVC_COMPILER
            sscanf_s(new_value.c_str(), "%f %f", &cvars::detail::hud_pos_x, &cvars::detail::hud_pos_y);
#else
            std::sscanf(new_value.c_str(), "%f %f", &cvars::detail::hud_pos_x, &cvars::detail::hud_pos_y);
#endif
        }
    }

    void OnHudColorCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        cvars::detail::hud_color = StringToColor24(new_value);
    }
}

namespace server_stats::cvars
{
    bool Register()
    {
        cvar_enable = cvar::Register("sstats_enable", "0");
        cvar_access = cvar::Register("sstats_access", "l");
        cvar_command = cvar::Register("sstats_command", "/sstats");
        cvar_display = cvar::Register("sstats_display", "ab");
        cvar_update_rate = cvar::Register("sstats_update_rate", "1.0");
        cvar_hud_pos = cvar::Register("sstats_hud_pos", "0.011 0.91");
        cvar_hud_color = cvar::Register("sstats_hud_color", "48 213 200");

        const auto registered = cvar_enable && cvar_access && cvar_command &&
            cvar_display && cvar_update_rate && cvar_hud_pos && cvar_hud_color;

        if (static auto cvar_change_hooked = false; registered && !cvar_change_hooked) {
            MHookCVarChange(cvar_display, DELEGATE_ARG<OnDisplayCvarChange>);
            MHookCVarChange(cvar_update_rate, DELEGATE_ARG<OnUpdateRateCvarChange>);
            MHookCVarChange(cvar_hud_pos, DELEGATE_ARG<OnHudPosCvarChange>);
            MHookCVarChange(cvar_hud_color, DELEGATE_ARG<OnHudColorCvarChange>);
            cvar_change_hooked = true;
        }

        return registered;
    }
}
