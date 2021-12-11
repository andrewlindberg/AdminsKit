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

#include <unlimited_choose_team/cvars.h>
#include <core/strings.h>
#include <mhooks/reapi.h>

using namespace core;
using namespace cssdk;
using namespace mhooks;

namespace
{
    void OnAccessCvarChange(const CVar* const, const std::string_view, std::string& new_value)
    {
        new_value = str::Trim(new_value);
        unlimited_choose_team::cvars::detail::access = amxx::ReadFlags(new_value.c_str());
    }
}

namespace unlimited_choose_team::cvars
{
    bool Register()
    {
        cvar_enable = cvar::Register("uct_enable", "0");
        cvar_access = cvar::Register("uct_access", detail::DEFAULT_ACCESS_FLAGS);

        const auto registered =
            cvar_enable && cvar_access;

        if (static auto cvar_change_hooked = false; registered && !cvar_change_hooked) {
            MHookCVarChange(cvar_access, DELEGATE_ARG<OnAccessCvarChange>);
            cvar_change_hooked = true;
        }

        return registered;
    }
}
