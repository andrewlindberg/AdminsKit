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

namespace unlimited_choose_team::cvars
{
    namespace detail
    {
        constexpr auto DEFAULT_ACCESS_FLAGS = "d";
        inline int access{amxx::ReadFlags(DEFAULT_ACCESS_FLAGS)};
    }

    inline const cssdk::CVar* cvar_enable{};
    inline const cssdk::CVar* cvar_access{};

    inline bool Enable()
    {
        return core::cvar::GetValue<bool>(cvar_enable);
    }

    inline int Access()
    {
        return detail::access;
    }

    bool Register();
}
