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

#include <core/cvar.h>
#include <cssdk/common/cvar.h>

namespace adminskit::cvars
{
    inline const cssdk::CVar* cvar_lang{};

    inline const char* Lang()
    {
        return core::cvar::GetValue<const char*>(cvar_lang);
    }

    inline bool Register()
    {
        cvar_lang = core::cvar::Register("adminskit_lang", "server");
        return cvar_lang;
    }
}
