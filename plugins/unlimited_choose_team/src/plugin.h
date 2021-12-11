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

#include <mhooks/reapi.h>
#include <vector>

namespace unlimited_choose_team
{
    class Plugin final
    {
        std::vector<std::unique_ptr<mhooks::MHook>> hooks_{};

    public:
        Plugin();
        ~Plugin() = default;

        Plugin(Plugin&&) = delete;
        Plugin(const Plugin&) = delete;

        Plugin& operator=(Plugin&&) = delete;
        Plugin& operator=(const Plugin&) = delete;

    private:
        static void OnShowVguiMenu(const ReGameShowVguiMenuMChain& chain, cssdk::PlayerBase* player,
                                   cssdk::VguiMenu menu, int keys, char* old_menu);

        static cssdk::qboolean OnHandleMenuChooseTeam(const ReGameHandleMenuChooseTeamMChain& chain,
                                                      cssdk::PlayerBase* player, cssdk::ChooseTeamMenuSlot slot);

        static void OnPlayerPostThink(const ReGamePlayerPostThinkMChain& chain, cssdk::PlayerBase* player);
    };
}
