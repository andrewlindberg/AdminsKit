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

#include <core/amxx_access.h>
#include <core/regamedll_api.h>
#include <core/rehlds_api.h>
#include <core/type_conversion.h>
#include <mhooks/amxx.h>
#include <array>

using namespace core;
using namespace cssdk;
using namespace mhooks;

namespace adminskit
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain);
}

namespace admin_free_look
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain);
}

namespace invisible_spectator
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain);
}

namespace server_stats
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain);
}

namespace unlimited_choose_team
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain);
}

namespace whdh
{
    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain);
}

namespace
{
    constexpr auto NUM_ATTACH_HOOKS = 7;
    const std::array<const MHook*, NUM_ATTACH_HOOKS>* g_amxx_attach_hooks{};

    amxx::Status OnAmxxAttach(const AmxxAttachMChain& chain)
    {
        auto status = amxx::Status::Failed;

        if (rehlds_api::Init() && regamedll_api::Init()) {
            amxx_access::Init();
            type_conversion::Init();
            status = chain.CallNext();
        }

        if (g_amxx_attach_hooks != nullptr) {
            for (const auto* const hook : *g_amxx_attach_hooks) {
                delete hook;
            }

            delete g_amxx_attach_hooks; // cppcheck-suppress autovarInvalidDeallocation
            g_amxx_attach_hooks = nullptr;
        }

        return status;
    }
}

void Main()
{
    g_amxx_attach_hooks = new const std::array<const MHook*, NUM_ATTACH_HOOKS>{
        MHookAmxxAttach(DELEGATE_ARG<OnAmxxAttach>, HookChainPriority::Uninterruptable),
        MHookAmxxAttach(DELEGATE_ARG<adminskit::OnAmxxAttach>, HookChainPriority::High),
        MHookAmxxAttach(DELEGATE_ARG<admin_free_look::OnAmxxAttach>),
        MHookAmxxAttach(DELEGATE_ARG<invisible_spectator::OnAmxxAttach>),
        MHookAmxxAttach(DELEGATE_ARG<server_stats::OnAmxxAttach>),
        MHookAmxxAttach(DELEGATE_ARG<unlimited_choose_team::OnAmxxAttach>),
        MHookAmxxAttach(DELEGATE_ARG<whdh::OnAmxxAttach>)};
}
