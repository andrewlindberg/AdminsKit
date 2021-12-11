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
#include <invisible_spectator/cvars.h>
#include <adminskit/utils.h>
#include <core/amxx_access.h>
#include <core/messages.h>
#include <core/rehlds_api.h>
#include <core/strings.h>
#include <core/type_conversion.h>
#include <cssdk/public/utils.h>
#include <metamod/engine.h>
#include <utility>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace metamod;
using namespace messages;
using namespace adminskit;

namespace
{
    [[nodiscard]] bool HasAccess(const int player)
    {
        return amxx_access::Has(player, invisible_spectator::cvars::Access());
    }
}

namespace invisible_spectator
{
    Plugin::Plugin(adminskit::Localization&& localization)
        : localization_(std::move(localization))
    {
        player_invisible_.fill(false);
        player_invis_enabled_.fill(cvars::Auto());

        hooks_.emplace_back(MHookReHldsClientConnected({DELEGATE_ARG<&Plugin::OnClientConnected>, this})->Unique());
        hooks_.emplace_back(MHookGameEvent("TeamInfo", {DELEGATE_ARG<&Plugin::OnEventTeamInfo>, this})->Unique());
        hooks_.emplace_back(MHookGameDllClientCommand({DELEGATE_ARG<&Plugin::OnClientCommand>, this}, false)->Unique());
        hooks_.emplace_back(MHookReHldsWriteFullClientUpdate({DELEGATE_ARG<&Plugin::OnWriteFullClientUpdate>, this})->Unique());
    }

    bool Plugin::ShouldBeInvisible(const int player_index) const
    {
        if (!player_invis_enabled_[player_index] || !HasAccess(player_index)) {
            return false;
        }

        const auto* const player_edict = type_conversion::EdictByIndex(player_index);
        return IsValidEntity(player_edict) && IsSpectatorOrUnassigned(player_edict);
    }

    void Plugin::UpdateUserInfo(const int player_index, Edict* player_edict) const
    {
        auto* const client = rehlds_api::ServerStatic()->GetGameClient(player_index - 1);

        if (!IsValidClient(client)) {
            return;
        }

        if (!player_edict) {
            player_edict = type_conversion::EdictByIndex(player_index);
        }

        if (!IsValidEntity(player_edict)) {
            return;
        }

        rehlds_api::Funcs()->sv_update_user_info(client);
        SendChatColorMessage(player_edict, player_index,
                             localization_.GetText(player_invisible_[player_index] ? "INVIS_ENABLED" : "INVIS_DISABLED",
                                                   player_edict));
    }

    void Plugin::OnClientConnected(const ReHldsClientConnectedMChain& chain, IGameClient* const client)
    {
        chain.CallNext(client);

        if (client) {
            if (const auto client_index = client->GetId() + 1; IsClient(client_index)) {
                player_invisible_[client_index] = false;
                player_invis_enabled_[client_index] = cvars::Auto();
            }
        }
    }

    void Plugin::OnEventTeamInfo(const MessageType, const int, const float* const, const Edict* const,
                                 const MessageArgs& args)
    {
        if (args.Count() != 2) {
            return;
        }

        const auto player_index = args.GetInteger(0);
        const auto& team = args.GetString(1);

        if (!IsClient(player_index)) {
            return;
        }

        if (cvars::Auto()) {
            player_invis_enabled_[player_index] = team.front() == 'S' || team.front() == 'U';
        }

        if (ShouldBeInvisible(player_index) != player_invisible_[player_index]) {
            UpdateUserInfo(player_index);
        }
    }

    void Plugin::OnClientCommand(const GameDllClientCommandMChain& chain, Edict* const client)
    {
        chain.CallNext(client);

        if (!IsValidEntity(client)) {
            return;
        }

        if (const auto* const command = engine::CmdArgv(0); str::IsNullOrEmpty(command) ||
            (!str::Equals(command, "say") && !str::Equals(command, "say_team"))) {
            return;
        }

        const auto* const say_text = engine::CmdArgv(1);
        const auto client_index = type_conversion::IndexOfEntity(client);

        if (str::IsNullOrEmpty(say_text) || !IsClient(client_index) || !HasAccess(client_index)) {
            return;
        }

        if (str::IEquals(say_text, cvars::Command())) {
            MetaSetResult(MetaResult::Supercede);

            if (!IsSpectatorOrUnassigned(client)) {
                SendChatColorMessage(client, client_index, localization_.GetText("NOT_SPECTATOR", client));
                return;
            }

            player_invis_enabled_[client_index] ^= true;
            UpdateUserInfo(client_index, client);
        }
        else if (player_invisible_[client_index] && cvars::LockSay()) {
            MetaSetResult(MetaResult::Supercede);
            SendChatColorMessage(client, client_index, localization_.GetText("BE_QUIET", client));
        }
    }

    void Plugin::OnWriteFullClientUpdate(const ReHldsWriteFullClientUpdateMChain& chain, IGameClient* const client,
                                         char* const info, const std::size_t max_len, SizeBuf* const info_buffer,
                                         IGameClient* const receiver)
    {
        if (IsValidClient(client)) {
            const auto client_index = client->GetId() + 1;
            player_invisible_[client_index] = ShouldBeInvisible(client_index);

            if (!player_invisible_[client_index] ||
                (!cvars::HideFromAdmins() && IsValidClient(receiver) && HasAccess(receiver->GetId() + 1))) {
                chain.CallNext(client, info, max_len, info_buffer, receiver);
            }
            else {
                static char null_info[MAX_INFO_STRING]{};
                chain.CallNext(client, null_info, 0, info_buffer, receiver);
            }
        }
        else {
            chain.CallNext(client, info, max_len, info_buffer, receiver);
        }
    }
}
