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

#include <adminskit/localization.h>
#include <mhooks/messages.h>
#include <mhooks/metamod.h>
#include <mhooks/reapi.h>
#include <array>
#include <vector>

namespace invisible_spectator
{
    class Plugin final
    {
        adminskit::Localization localization_;
        std::vector<std::unique_ptr<mhooks::MHook>> hooks_{};
        std::array<bool, cssdk::MAX_CLIENTS + 1> player_invisible_{};
        std::array<bool, cssdk::MAX_CLIENTS + 1> player_invis_enabled_{};

    public:
        explicit Plugin(adminskit::Localization&& localization);
        ~Plugin() = default;

        Plugin(Plugin&&) = delete;
        Plugin(const Plugin&) = delete;

        Plugin& operator=(Plugin&&) = delete;
        Plugin& operator=(const Plugin&) = delete;

    private:
        [[nodiscard]] bool ShouldBeInvisible(int player_index) const;
        void UpdateUserInfo(int player_index, cssdk::Edict* player_edict = nullptr) const;
        void OnClientConnected(const ReHldsClientConnectedMChain& chain, cssdk::IGameClient* client);
        void OnEventTeamInfo(cssdk::MessageType type, int id, const float* origin, const cssdk::Edict* client, const mhooks::MessageArgs& args);
        void OnClientCommand(const GameDllClientCommandMChain& chain, cssdk::Edict* client);
        void OnWriteFullClientUpdate(const ReHldsWriteFullClientUpdateMChain& chain, cssdk::IGameClient* client, char* info,
                                     std::size_t max_len, cssdk::SizeBuf* info_buffer, cssdk::IGameClient* receiver);
    };
}
