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

#include <whdh/cvars.h>
#include <adminskit/utils.h>
#include <core/console.h>
#include <core/strings.h>
#include <mhooks/reapi.h>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace adminskit;
using namespace whdh::cvars;

namespace
{
    bool BeamUpdateRateCvarPolicy(std::string& new_value)
    {
        if (!str::IsEmptyOrWhiteSpace(new_value)) {
            if (const auto rate = StringToDouble(new_value); rate >= 0.1 && rate <= 1.0) {
                new_value = str::Format("%.1f", std::clamp(std::floor(rate * 10. + 0.5) / 10., 0.1, 1.));
                return true;
            }
        }

        return false;
    }

    void OnAccessCvarChange(const CVar* const, const std::string_view, std::string& new_value)
    {
        new_value = str::Trim(new_value);
        whdh::cvars::detail::access = amxx::ReadFlags(new_value.c_str());
    }

    void OnBeamUpdateRateCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, BeamUpdateRateCvarPolicy);
    }

    void OnBoxColorCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::box_color = StringToColor24(new_value);
    }

    void OnBoxColorTCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::box_color_t = StringToColor24(new_value);
    }

    void OnBoxColorCtCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::box_color_ct = StringToColor24(new_value);
    }

    void OnBoxColorHitCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::box_color_hit = StringToColor24(new_value);
    }

    void OnLineColorTCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::line_color_t = StringToColor24(new_value);
    }

    void OnLineColorCtCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::line_color_ct = StringToColor24(new_value);
    }

    void OnLineColorHitCvarChange(const CVar* const cvar, const std::string_view old_value, std::string& new_value)
    {
        assert(cvar != nullptr);
        ValidateCvarValue(cvar, old_value, new_value, ColorCvarPolicy);
        whdh::cvars::detail::line_color_hit = StringToColor24(new_value);
    }
}

namespace whdh::cvars
{
    bool Register()
    {
        cvar_enable = cvar::Register("whdh_enable", "0");
        cvar_access = cvar::Register("whdh_access", detail::DEFAULT_ACCESS_FLAGS);
        cvar_spec_only = cvar::Register("whdh_spec_only", "0");
        cvar_save_prefs = cvar::Register("whdh_save_prefs", "7");

        cvar_beam_sprite = cvar::Register("whdh_beam_sprite", "sprites/laserbeam.spr");
        cvar_marker_sprite = cvar::Register("whdh_marker_sprite", "sprites/whdh.spr");
        cvar_beam_update_rate = cvar::Register("whdh_beam_update_rate", "0.2");

        cvar_box_color = cvar::Register("whdh_box_color", "0 255 0");
        cvar_box_color_t = cvar::Register("whdh_box_color_t", "105 60 60");
        cvar_box_color_ct = cvar::Register("whdh_box_color_ct", "60 60 105");
        cvar_box_color_hit = cvar::Register("whdh_box_color_hit", "0 0 0");

        cvar_line_color_t = cvar::Register("whdh_line_color_t", "150 0 0");
        cvar_line_color_ct = cvar::Register("whdh_line_color_ct", "0 0 150");
        cvar_line_color_hit = cvar::Register("whdh_line_color_hit", "255 0 255");

        const auto registered = cvar_enable && cvar_access && cvar_spec_only && cvar_save_prefs && cvar_beam_sprite &&
            cvar_marker_sprite && cvar_beam_update_rate && cvar_box_color && cvar_box_color_t && cvar_box_color_ct &&
            cvar_box_color_hit && cvar_line_color_t && cvar_line_color_ct && cvar_line_color_hit;

        if (static auto cvar_change_hooked = false; registered && !cvar_change_hooked) {
            MHookCVarChange(cvar_access, DELEGATE_ARG<OnAccessCvarChange>);
            MHookCVarChange(cvar_beam_update_rate, DELEGATE_ARG<OnBeamUpdateRateCvarChange>);
            MHookCVarChange(cvar_box_color, DELEGATE_ARG<OnBoxColorCvarChange>);
            MHookCVarChange(cvar_box_color_t, DELEGATE_ARG<OnBoxColorTCvarChange>);
            MHookCVarChange(cvar_box_color_ct, DELEGATE_ARG<OnBoxColorCtCvarChange>);
            MHookCVarChange(cvar_box_color_hit, DELEGATE_ARG<OnBoxColorHitCvarChange>);
            MHookCVarChange(cvar_line_color_t, DELEGATE_ARG<OnLineColorTCvarChange>);
            MHookCVarChange(cvar_line_color_ct, DELEGATE_ARG<OnLineColorCtCvarChange>);
            MHookCVarChange(cvar_line_color_hit, DELEGATE_ARG<OnLineColorHitCvarChange>);
            cvar_change_hooked = true;
        }

        return registered;
    }
}
