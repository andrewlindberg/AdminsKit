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

#include <core/observer.h>
#include <cssdk/common/const.h>
#include <cssdk/common/strind.h>
#include <mhooks/metamod.h>
#include <mhooks/reapi.h>
#include <string>

namespace whdh
{
    class Resources final : public core::Observable<>
    {
        cssdk::Strind string_whdh_{};
        cssdk::Strind string_info_target_{};
        cssdk::Strind string_marker_sprite_{};

        std::string beam_sprite_path_{};
        int beam_sprite_index_{cssdk::INVALID_INDEX};

        std::string marker_sprite_path_{};
        int marker_sprite_index_{cssdk::INVALID_INDEX};

        std::unique_ptr<mhooks::MHook> install_game_rules_hook_{};
        std::unique_ptr<mhooks::MHook> dispatch_spawn_hook_{};
        std::unique_ptr<mhooks::MHook> server_deactivate_post_hook_{};

    public:
        Resources();
        ~Resources() override = default;

        Resources(Resources&&) = delete;
        Resources(const Resources&) = delete;

        Resources& operator=(Resources&&) = delete;
        Resources& operator=(const Resources&) = delete;

        [[nodiscard]] bool Precached() const noexcept
        {
            return beam_sprite_index_ != cssdk::INVALID_INDEX && marker_sprite_index_ != cssdk::INVALID_INDEX;
        }

        [[nodiscard]] const cssdk::Strind& StringWhdh() const noexcept
        {
            return string_whdh_;
        }

        [[nodiscard]] const cssdk::Strind& StringInfoTarget() const noexcept
        {
            return string_info_target_;
        }

        [[nodiscard]] const cssdk::Strind& StringMarkerSprite() const noexcept
        {
            return string_marker_sprite_;
        }

        [[nodiscard]] const std::string& BeamSpritePath() const noexcept
        {
            return beam_sprite_path_;
        }

        [[nodiscard]] int BeamSpriteIndex() const noexcept
        {
            return beam_sprite_index_;
        }

        [[nodiscard]] const std::string& MarkerSpritePath() const noexcept
        {
            return marker_sprite_path_;
        }

        [[nodiscard]] int MarkerSpriteIndex() const noexcept
        {
            return marker_sprite_index_;
        }

    private:
        cssdk::GameRules* OnInstallGameRules(const ReGameRulesInstallGameRulesMChain& chain);
        int OnDispatchSpawn(const GameDllDispatchSpawnMChain& chain, cssdk::Edict* entity);
        void OnServerDeactivatePost(const GameDllServerDeactivateMChain& chain);
    };
}
