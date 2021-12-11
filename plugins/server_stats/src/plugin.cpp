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
#include <server_stats/cvars.h>
#include <adminskit/utils.h>
#include <core/amxx_access.h>
#include <core/messages.h>
#include <core/strings.h>
#include <metamod/engine.h>
#include <algorithm>

using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace metamod;
using namespace adminskit;

namespace
{
    constexpr auto FL_FPS = 1 << 0;
    constexpr auto FL_CPU = 1 << 1;
    constexpr auto FL_VIRT_MEM = 1 << 2;
    constexpr auto FL_PHYS_MEM = 1 << 3;
}

namespace server_stats
{
    Plugin::Plugin()
    {
        hooks_.emplace_back(
            MHookGameDllStartFrame(
                {DELEGATE_ARG<&Plugin::OnStartFrame>, this},
                false, HookChainPriority::Normal, false)
                ->Unique());

        hooks_.emplace_back(
            MHookReHldsDropClient(
                {DELEGATE_ARG<&Plugin::OnDropClient>, this},
                HookChainPriority::Normal, false)
                ->Unique());

        hooks_.emplace_back(
            MHookGameDllServerDeactivate(
                {DELEGATE_ARG<&Plugin::OnServerDeactivate>, this},
                false, HookChainPriority::Normal, false)
                ->Unique());

        client_command_hook_ =
            MHookGameDllClientCommand(
                {DELEGATE_ARG<&Plugin::OnClientCommand>, this}, false)
                ->Unique();

        hud_params_.channel = 5;
    }

    void Plugin::Subscribe(Edict* const player)
    {
        assert(cssdk::IsValidEntity(player));
        subscribers_.push_back(player);

        for (const auto& hook : hooks_) {
            hook->Enable();
        }
    }

    void Plugin::Unsubscribe(const Edict* const player)
    {
        assert(player != nullptr);

        subscribers_.erase(
            std::remove_if(subscribers_.begin(), subscribers_.end(), [player](const auto* const element) {
                return element == player;
            }),
            subscribers_.end());

        if (subscribers_.empty()) {
            for (const auto& hook : hooks_) {
                hook->Disable();
            }

            fps_ = frame_count_ = 1;
            show_hud_last_time_ = frame_count_start_time_ = 0.F;
        }
    }

    bool Plugin::IsSubscriber(const Edict* player) const
    {
        return std::any_of(
            subscribers_.cbegin(), subscribers_.cend(), [player](const auto* const element) {
                return element == player;
            });
    }

    void Plugin::OnStartFrame(const GameDllStartFrameMChain& chain)
    {
        chain.CallNext();

        ++frame_count_;
        auto time_passed = g_global_vars->time - frame_count_start_time_;

        if (time_passed >= 1.F) {
            fps_ = static_cast<std::size_t>(std::round(static_cast<float>(frame_count_) / time_passed));
            frame_count_start_time_ = g_global_vars->time;
            frame_count_ = 0;
        }

        if (time_passed = g_global_vars->time - show_hud_last_time_; time_passed < cvars::UpdateRate()) {
            return;
        }

        hud_params_.x = cvars::HudPosX();
        hud_params_.y = cvars::HudPosY();
        hud_params_.red1 = cvars::HudColor().red;
        hud_params_.green1 = cvars::HudColor().green;
        hud_params_.blue1 = cvars::HudColor().blue;
        hud_params_.hold_time = cvars::UpdateRate();

        std::string message{};
        const auto flags = cvars::Display();
        show_hud_last_time_ = g_global_vars->time;

        if (flags & FL_FPS) {
            message += str::Format(" | FPS: %u", fps_);
        }

        if (flags & FL_CPU) {
            message += str::Format(" | CPU: %.2f", GetCpuUsed());
        }

        if (flags & FL_VIRT_MEM) {
            const auto& value = BytesToHumanReadableString(GetVirtualMemoryUsed());
            message += str::Format(" | Virt. Memory: %s", value.c_str());
        }

        if (flags & FL_PHYS_MEM) {
            const auto& value = BytesToHumanReadableString(GetPhysicalMemoryUsed());
            message += str::Format(" | Phys. Memory: %s", value.c_str());
        }

        if (message = str::Trim(str::Trim(message), '|'); message.empty()) {
            return;
        }

        for (auto* const subscriber : subscribers_) {
            if (IsValidEntity(subscriber)) {
                messages::SendHudMessage(subscriber, hud_params_, message);
            }
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

        if (const auto* const say_text = engine::CmdArgv(1); !str::IsNullOrEmpty(say_text) &&
            str::IEquals(say_text, cvars::Command()) && EntityPrivateData<EntityBase>(client)->IsNetClient()) {
            MetaSetResult(MetaResult::Supercede);

            if (IsSubscriber(client)) {
                Unsubscribe(client);
            }
            else {
                Subscribe(client);
            }
        }
    }

    void Plugin::OnDropClient(const ReHldsDropClientMChain& chain, IGameClient* const client, const bool crash,
                              const char* const reason)
    {
        if (client) {
            if (const auto* const player = client->GetEdict(); player) {
                Unsubscribe(player);
            }
        }

        chain.CallNext(client, crash, reason);
    }

    void Plugin::OnServerDeactivate(const GameDllServerDeactivateMChain& chain)
    {
        for (const auto* const subscriber : subscribers_) {
            Unsubscribe(subscriber);
        }

        chain.CallNext();
    }
}
