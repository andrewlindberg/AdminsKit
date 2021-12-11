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

#include "resources.h"
#include <adminskit/localization.h>
#include <core/menu.h>
#include <core/type_conversion.h>
#include <cssdk/public/os_defs.h>
#include <mhooks/reapi.h>
#include <array>
#include <memory>
#include <vector>

namespace whdh
{
    enum class Visibility
    {
        Off,
        Opponents,
        Teammates,
        OpponentsAndTeammates
    };

    struct UserPreferences
    {
        Visibility visibility{};
        bool marker{};
        bool lines{};
        bool boxes{};

        void Reset()
        {
            visibility = Visibility::Opponents;
            marker = lines = true;
            boxes = false;
        }
    };

#ifdef INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable : 2047)
#endif

    inline Visibility& operator++(Visibility& visibility)
    {
        if (visibility < Visibility::Off) {
            return visibility = static_cast<Visibility>(static_cast<int>(Visibility::Off) + 1);
        }

        if (visibility >= Visibility::OpponentsAndTeammates) {
            return visibility = Visibility::Off;
        }

        return visibility = static_cast<Visibility>(static_cast<int>(visibility) + 1);
    }

#ifdef INTEL_COMPILER
#pragma warning(pop)
#endif

    class Preferences final
    {
        const adminskit::Localization localization_;
        const std::shared_ptr<Resources> resources_;
        std::vector<std::unique_ptr<mhooks::MHook>> hooks_{};
        std::array<UserPreferences, cssdk::MAX_CLIENTS + 1> preferences_{};
        core::Menu menu_{{core::DELEGATE_ARG<&Preferences::MenuHandler>, this}};

    public:
        Preferences(adminskit::Localization&& localization, std::shared_ptr<Resources> resources);
        ~Preferences() = default;

        Preferences(Preferences&&) = delete;
        Preferences(const Preferences&) = delete;

        Preferences& operator=(Preferences&&) = delete;
        Preferences& operator=(const Preferences&) = delete;

        [[nodiscard]] UserPreferences& operator[](const int player) noexcept
        {
            return preferences_[player];
        }

        [[nodiscard]] UserPreferences& operator[](const cssdk::Edict* const player) noexcept
        {
            return preferences_[core::type_conversion::IndexOfEntity(player)];
        }

        [[nodiscard]] UserPreferences& operator[](const cssdk::PlayerBase* const player) noexcept
        {
            return preferences_[core::type_conversion::IndexOfEntity(player)];
        }

        [[nodiscard]] const UserPreferences& operator[](const int player) const noexcept
        {
            return preferences_[player];
        }

        [[nodiscard]] const UserPreferences& operator[](const cssdk::Edict* const player) const noexcept
        {
            return preferences_[core::type_conversion::IndexOfEntity(player)];
        }

        [[nodiscard]] const UserPreferences& operator[](const cssdk::PlayerBase* const player) const noexcept
        {
            return preferences_[core::type_conversion::IndexOfEntity(player)];
        }

    private:
        void ShowMenu(cssdk::Edict* client);
        void MenuHandler(cssdk::Edict* client, int selected_item);
        void OnPlayerPreThink(const ReGamePlayerPreThinkMChain& chain, cssdk::PlayerBase* player);
        void OnClientConnected(const ReHldsClientConnectedMChain& chain, cssdk::IGameClient* client);
    };
}
