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

#include <core/localization.h>
#include <adminskit/cvars.h>
#include <core/strings.h>
#include <utility>

namespace adminskit
{
    class Localization final : public core::Localization
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param filename The name of the localization file.
        */
        explicit Localization(const std::string& filename)
            : core::Localization{core::str::BuildPathAmxxLang(filename)}
        {
        }

        /**
         * @brief Gets the text by the text label.
         *
         * @remark Set lang value to <c>"server"</c> to get the language from the amx_language CVar.
         * @remark Set lang value to <c>"player"</c> to get the language from the player config (setinfo "lang").
        */
        [[nodiscard]] const auto& GetText(std::string_view&& label, cssdk::Edict* client = nullptr) const;

        /**
         * @brief Gets the text by the text label.
         *
         * @remark Set lang value to <c>"server"</c> to get the language from the amx_language CVar.
         * @remark Set lang value to <c>"player"</c> to get the language from the player config (setinfo "lang").
        */
        template <typename... Args>
        [[nodiscard]] auto GetTextFormat(std::string_view&& label, Args&&... args) const;

        /**
         * @brief Gets the text by the text label.
         *
         * @remark Set lang value to <c>"server"</c> to get the language from the amx_language CVar.
         * @remark Set lang value to <c>"player"</c> to get the language from the player config (setinfo "lang").
        */
        template <typename... Args>
        [[nodiscard]] auto GetTextFormat(std::string_view&& label, cssdk::Edict* client, Args&&... args);
    };

    inline const auto& Localization::GetText(std::string_view&& label, cssdk::Edict* const client) const
    {
        return core::Localization::GetText(cvars::Lang(), std::forward<std::string_view>(label), client);
    }

    template <typename... Args>
    auto Localization::GetTextFormat(std::string_view&& label, Args&&... args) const
    {
        return core::Localization::GetTextFormat(cvars::Lang(), std::forward<std::string_view>(label),
                                                 std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto Localization::GetTextFormat(std::string_view&& label, cssdk::Edict* const client, Args&&... args)
    {
        return core::Localization::GetTextFormat(cvars::Lang(), std::forward<std::string_view>(label), client,
                                                 std::forward<Args>(args)...);
    }
}
