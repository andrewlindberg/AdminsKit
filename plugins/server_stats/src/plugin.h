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

#include <cssdk/common/const.h>
#include <cssdk/public/utils.h>
#include <mhooks/metamod.h>
#include <mhooks/reapi.h>
#include <cstddef>
#include <vector>

namespace server_stats
{
    class Plugin final
    {
        std::size_t fps_{};
        std::size_t frame_count_{};
        float show_hud_last_time_{};
        float frame_count_start_time_{};
        cssdk::HudTextParams hud_params_{};
        std::vector<cssdk::Edict*> subscribers_{};
        std::vector<std::unique_ptr<mhooks::MHook>> hooks_{};
        std::unique_ptr<mhooks::MHook> client_command_hook_{};

    public:
        Plugin();
        ~Plugin() = default;

        Plugin(Plugin&&) = delete;
        Plugin(const Plugin&) = delete;

        Plugin& operator=(Plugin&&) = delete;
        Plugin& operator=(const Plugin&) = delete;

    private:
        void Subscribe(cssdk::Edict* player);
        void Unsubscribe(const cssdk::Edict* player);
        [[nodiscard]] bool IsSubscriber(const cssdk::Edict* player) const;
        void OnStartFrame(const GameDllStartFrameMChain& chain);
        void OnClientCommand(const GameDllClientCommandMChain& chain, cssdk::Edict* client);
        void OnDropClient(const ReHldsDropClientMChain& chain, cssdk::IGameClient* client, bool crash, const char* reason);
        void OnServerDeactivate(const GameDllServerDeactivateMChain& chain);
    };
}
