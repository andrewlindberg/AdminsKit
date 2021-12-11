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

namespace invisible_spectator::cvars
{
    inline const cssdk::CVar* cvar_enable{};
    inline const cssdk::CVar* cvar_access{};
    inline const cssdk::CVar* cvar_command{};
    inline const cssdk::CVar* cvar_auto_invis{};
    inline const cssdk::CVar* cvar_lock_say{};
    inline const cssdk::CVar* cvar_hide_from_admins{};

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

    inline bool Auto()
    {
        return core::cvar::GetValue<bool>(cvar_auto_invis);
    }

    inline bool LockSay()
    {
        return core::cvar::GetValue<bool>(cvar_lock_say);
    }

    inline bool HideFromAdmins()
    {
        return core::cvar::GetValue<bool>(cvar_hide_from_admins);
    }

    inline bool Register()
    {
        cvar_enable = core::cvar::Register("ispec_enable", "0");
        cvar_access = core::cvar::Register("ispec_access", "d");
        cvar_command = core::cvar::Register("ispec_command", "/is");
        cvar_auto_invis = core::cvar::Register("ispec_auto", "1");
        cvar_lock_say = core::cvar::Register("ispec_lock_say", "1");
        cvar_hide_from_admins = core::cvar::Register("ispec_hide_from_admins", "1");

        return cvar_enable && cvar_access && cvar_command && cvar_auto_invis && cvar_lock_say && cvar_hide_from_admins;
    }
}
