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

#include "plugin.h"
#include <adminskit/localization.h>
#include <whdh/cvars.h>
#include <core/strings.h>
#include <mhooks/amxx.h>
#include <mhooks/reapi.h>
#include <cassert>

using namespace whdh;
using namespace core;
using namespace cssdk;
using namespace mhooks;

namespace
{
    const Plugin* g_plugin{};

    void OnEnableCvarChange(const CVar* const, const std::string_view, std::string& new_value)
    {
        if (new_value = str::Trim(new_value); new_value.empty() || new_value.front() == '0') {
            delete g_plugin;
            g_plugin = nullptr;
        }
        else if (!g_plugin) {
            const auto data = std::make_shared<Data>();
            const auto resources = std::make_shared<Resources>();
            const auto preferences = std::make_shared<Preferences>(PreferencesBin{}, adminskit::Localization{"whdh.txt"}, resources);
            g_plugin = new Plugin{{data, resources, preferences}, {data, resources, preferences}, data, resources};
        }
    }
}

namespace whdh
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain)
    {
        if (cvars::Register()) {
            const auto status = chain.CallNext();

            if (status == amxx::Status::Ok) {
                assert(cvars::cvar_enable != nullptr);
                MHookCVarChange(cvars::cvar_enable, DELEGATE_ARG<OnEnableCvarChange>);
            }

            return status;
        }

        return amxx::Status::Failed;
    }
}
