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

namespace whdh::cvars
{
    namespace detail
    {
        constexpr auto DEFAULT_ACCESS_FLAGS = "d";
        inline int access{amxx::ReadFlags(DEFAULT_ACCESS_FLAGS)};

        inline cssdk::Color24 box_color{0, 255, 0};
        inline cssdk::Color24 box_color_t{105, 60, 60};
        inline cssdk::Color24 box_color_ct{60, 60, 105};
        inline cssdk::Color24 box_color_hit{0, 0, 0};
        inline cssdk::Color24 line_color_t{150, 0, 0};
        inline cssdk::Color24 line_color_ct{0, 0, 150};
        inline cssdk::Color24 line_color_hit{255, 0, 255};
    }

    inline const cssdk::CVar* cvar_enable{};
    inline const cssdk::CVar* cvar_access{};
    inline const cssdk::CVar* cvar_spec_only{};
    inline const cssdk::CVar* cvar_beam_sprite{};
    inline const cssdk::CVar* cvar_marker_sprite{};
    inline const cssdk::CVar* cvar_beam_update_rate{};
    inline const cssdk::CVar* cvar_box_color{};
    inline const cssdk::CVar* cvar_box_color_t{};
    inline const cssdk::CVar* cvar_box_color_ct{};
    inline const cssdk::CVar* cvar_box_color_hit{};
    inline const cssdk::CVar* cvar_line_color_t{};
    inline const cssdk::CVar* cvar_line_color_ct{};
    inline const cssdk::CVar* cvar_line_color_hit{};

    [[nodiscard]] inline bool Enable()
    {
        return core::cvar::GetValue<bool>(cvar_enable);
    }

    [[nodiscard]] inline int Access()
    {
        return detail::access;
    }

    [[nodiscard]] inline bool SpecOnly()
    {
        return core::cvar::GetValue<bool>(cvar_spec_only);
    }

    [[nodiscard]] inline const char* BeamSprite()
    {
        return core::cvar::GetValue<const char*>(cvar_beam_sprite);
    }

    [[nodiscard]] inline const char* MarkerSprite()
    {
        return core::cvar::GetValue<const char*>(cvar_marker_sprite);
    }

    [[nodiscard]] inline float BeamUpdateRate()
    {
        return core::cvar::GetValue<float>(cvar_beam_update_rate);
    }

    [[nodiscard]] inline const cssdk::Color24& BoxColor()
    {
        return detail::box_color;
    }

    [[nodiscard]] inline const cssdk::Color24& BoxColorT()
    {
        return detail::box_color_t;
    }

    [[nodiscard]] inline const cssdk::Color24& BoxColorCt()
    {
        return detail::box_color_ct;
    }

    [[nodiscard]] inline const cssdk::Color24& BoxColorHit()
    {
        return detail::box_color_hit;
    }

    [[nodiscard]] inline const cssdk::Color24& LineColorT()
    {
        return detail::line_color_t;
    }

    [[nodiscard]] inline const cssdk::Color24& LineColorCt()
    {
        return detail::line_color_ct;
    }

    [[nodiscard]] inline const cssdk::Color24& LineColorHit()
    {
        return detail::line_color_hit;
    }

    bool Register();
}
