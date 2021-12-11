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

#include <core/strings.h>
#include <cssdk/common/const.h>
#include <cssdk/dll/player.h>
#include <cssdk/public/os_defs.h>
#include <cssdk/public/rehlds/rehlds_interfaces.h>
#include <cssdk/public/utils.h>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <string_view>
#include <type_traits>

namespace adminskit
{
    /**
     * @brief N/D
    */
    [[nodiscard]] cssdk::Color24 StringToColor24(const std::string& str);

    /**
     * @brief N/D
    */
    [[nodiscard]] cssdk::PlayerBase* GetObservablePlayer(const cssdk::PlayerBase* observer) noexcept;

    /**
     * @brief N/D
    */
    bool ValidateCvarValue(const cssdk::CVar* cvar, std::string_view old_value,
                           std::string& new_value, std::add_pointer_t<bool(std::string&)> policy);
    /**
     * @brief N/D
    */
    bool ColorCvarPolicy(std::string& new_value);

    /**
     * @brief N/D
    */
    bool FlagsCvarPolicy(std::string& new_value);

    /**
     * @brief N/D
    */
    bool HudPosCvarPolicy(std::string& new_value);

    /**
     * @brief N/D
    */
    [[nodiscard]] std::size_t GetPhysicalMemoryUsed();

    /**
     * @brief N/D
    */
    [[nodiscard]] std::size_t GetVirtualMemoryUsed();

    /**
     * @brief N/D
    */
    [[nodiscard]] double GetCpuUsed();

    /**
     * @brief N/D
    */
    template <typename Bytes>
    std::string BytesToHumanReadableString(Bytes bytes)
    {
        constexpr const char* suffix[] = {"B", "KiB", "MiB", "GiB", "TiB"};
        constexpr auto length = std::size(suffix);

        std::size_t i = 0;
        auto b = static_cast<double>(bytes);

        if (bytes > 1024) {
            for (; (bytes / 1024) > 0 && i < length - 1; ++i, bytes /= 1024) {
                b = bytes / 1024.0;
            }
        }

        return core::str::Format("%.02lf %s", b, suffix[i]);
    }

    /**
     * @brief N/D
    */
    [[nodiscard]] inline double StringToDouble(const std::string& str)
    {
        return std::strtod(str.c_str(), nullptr);
    }

    /**
     * @brief N/D
    */
    [[nodiscard]] inline bool IsValidClient(cssdk::IGameClient* const client) noexcept
    {
        return client && client->IsConnected() && IsValidEntity(client->GetEdict());
    }

    /**
     * @brief N/D
    */
    [[nodiscard]] inline bool IsSpectatorOrUnassigned(const cssdk::PlayerBase* const player) noexcept
    {
        assert(cssdk::IsValidEntity(player));
        return player->team == cssdk::TeamName::Spectator || player->team == cssdk::TeamName::Unassigned;
    }

    /**
     * @brief N/D
    */
    [[nodiscard]] inline bool IsSpectatorOrUnassigned(const cssdk::Edict* const player) noexcept
    {
        assert(cssdk::IsValidEntity(player));
        return IsSpectatorOrUnassigned(cssdk::EntityPrivateData<const cssdk::PlayerBase>(player));
    }
}
