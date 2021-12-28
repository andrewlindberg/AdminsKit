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

#include "preferences_bin.h"
#include "resources.h"
#include <adminskit/localization.h>
#include <core/menu.h>
#include <core/type_conversion.h>
#include <mhooks/amxx.h>
#include <mhooks/metamod.h>
#include <mhooks/reapi.h>
#include <array>
#include <memory>
#include <vector>

namespace whdh
{
    class Preferences final
    {
        PreferencesBin bin_;
        const adminskit::Localization localization_;
        const std::shared_ptr<Resources> resources_;
        std::vector<std::unique_ptr<mhooks::MHook>> hooks_{};
        std::array<UserPreferences, cssdk::MAX_CLIENTS + 1> preferences_{};
        core::Menu menu_{{core::DELEGATE_ARG<&Preferences::MenuHandler>, this}};

    public:
        Preferences(PreferencesBin&& bin, adminskit::Localization&& localization, std::shared_ptr<Resources> resources);
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
        void OnClientAuthorized(const AmxxClientAuthorizedMChain& chain, int index, const char* auth_id);
        void OnServerDeactivatePost(const GameDllServerDeactivateMChain& chain);
    };
}
