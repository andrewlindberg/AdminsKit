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

#include <adminskit/cvars.h>
#include <core/strings.h>
#include <metamod/engine.h>
#include <mhooks/amxx.h>
#include <mhooks/reapi.h>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace metamod;

namespace
{
    GameRules* OnInstallGameRules(const ReGameInstallGameRulesMChain& chain)
    {
        const auto config = str::BuildPathAmxxConfigs("adminskit.cfg");
        auto exec_config = str::Format("exec \"%s\"\n", config);
        str::ReplaceAll(exec_config, '\\', '/');

        engine::ServerCommand(exec_config.c_str());
        engine::ServerExecute();

        return chain.CallNext();
    }
}

namespace adminskit
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain)
    {
        const auto status = cvars::Register() ? chain.CallNext() : amxx::Status::Failed;

        if (status == amxx::Status::Ok) {
            MHookReGameInstallGameRules(DELEGATE_ARG<OnInstallGameRules>, HookChainPriority::High);
        }

        return status;
    }
}
