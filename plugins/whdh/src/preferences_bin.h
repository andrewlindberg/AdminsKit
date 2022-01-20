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

#include <cssdk/public/os_defs.h>
#include <cista/containers/hash_map.h>
#include <cista/containers/string.h>
#include <core/strings.h>
#include <cstdio>
#include <memory>

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
        Visibility visibility{Visibility::Opponents};
        bool marker{true};
        bool lines{true};
        bool boxes{false};
        bool transp_entities{false};
        int timestamp{0};

        [[nodiscard]] bool IsDefault() const
        {
            return visibility == Visibility::Opponents && marker && lines && !boxes && !transp_entities;
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

    class PreferencesBin final
    {
        static inline std::string except_msg_{}; // NOLINT(bugprone-dynamic-static-initializers, readability-identifier-naming)
        cista::raw::hash_map<cista::raw::string, UserPreferences> preferences_map_{};

    public:
        static std::string Path()
        {
            return core::str::BuildPathAmxxData("whdh_prefs.bin");
        }

        static std::unique_ptr<std::FILE, int (*)(std::FILE*)> Open(const char* mode);

        void Serialize();
        void Deserialize();
        void EraseOutdated();

        UserPreferences Load(const char* auth_id);
        void Save(const char* auth_id, UserPreferences& preferences);
    };
}
