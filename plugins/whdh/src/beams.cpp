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

#include "beams.h"
#include <adminskit/utils.h>
#include <core/type_conversion.h>
#include <metamod/engine.h>
#include <whdh/cvars.h>
#include <algorithm>
#include <cmath>
#include <utility>

using namespace whdh;
using namespace core;
using namespace cssdk;
using namespace mhooks;
using namespace metamod;

namespace
{
    [[nodiscard]] bool CheckUpdateTime(float& update_time)
    {
        if (g_global_vars->time - update_time >= cvars::BeamUpdateRate()) {
            update_time = g_global_vars->time;
            return true;
        }

        return false;
    }

    [[nodiscard]] int GetLifeTime() noexcept
    {
        return static_cast<int>(cvars::BeamUpdateRate() * 10.F);
    }

    [[nodiscard]] Vector GetBoxOrigin(const PlayerBase* const observable, const PlayerBase* const target,
                                      const float distance_to_hitpoint) noexcept
    {
        auto origin = target->vars->origin - observable->vars->origin;

        origin /= origin.Length();
        origin *= distance_to_hitpoint - 12.F;
        origin += observable->vars->origin + target->vars->view_ofs;

        if ((target->vars->button & IN_DUCK) || (target->vars->flags & FL_DUCKING) ||
            (target->physics_flags & PFLAG_DUCKING)) {
            origin.z += 7.F;
        }

        return origin;
    }

    [[nodiscard]] Vector GetBoxDestination(const Vector& start_position, float scaled_bone_length) noexcept
    {
        if (scaled_bone_length < 4.F) {
            scaled_bone_length = 4.F;
        }

        return Vector{start_position.x, start_position.y, start_position.z - scaled_bone_length};
    }

    [[nodiscard]] int GetBoxWidth(const float scaled_bone_length) noexcept
    {
        return std::clamp(static_cast<int>(std::round(scaled_bone_length * 3.F)), 12, 255);
    }

    [[nodiscard]] int GetBoxBrightness(const float distance_to_target) noexcept
    {
        if (distance_to_target >= 2040.F) {
            return 30;
        }

        constexpr auto x1 = 1200.F;
        constexpr auto x2 = 255.F;
        constexpr auto y1 = 2040.F;
        constexpr auto y2 = 30.F;

        return std::clamp(static_cast<int>(std::round(x2 + (distance_to_target - x1) * (y2 - x2) / (y1 - x1))), 30, 255);
    }

    [[nodiscard]] const Color24& GetBoxColor(const PlayerBase* const target, const float distance_to_target,
                                             const float distance_to_hitpoint, const float last_hit_time,
                                             const TraceResult& trace_result) noexcept
    {
        if (g_global_vars->time - last_hit_time < 1.F && !cvars::BoxColorHit().IsNull()) {
            return cvars::BoxColorHit();
        }

        if (!IsClient(type_conversion::IndexOfEntity(trace_result.entity_hit)) &&
            std::abs(distance_to_hitpoint - distance_to_target) >= FLOAT_EPSILON) {
            return cvars::BoxColor();
        }

        return target->team == TeamName::Ct ? cvars::BoxColorCt() : cvars::BoxColorT();
    }

    [[nodiscard]] int GetLineWidth(const float distance_to_target) noexcept
    {
        if (distance_to_target >= 2040.F) {
            return 1;
        }

        constexpr auto x1 = 30.F;
        constexpr auto x2 = 90.F;
        constexpr auto y1 = 2040.F;
        constexpr auto y2 = 1.F;

        return std::clamp(
            static_cast<int>(std::round(x2 + (distance_to_target - x1) * (y2 - x2) / (y1 - x1))), 1, 255);
    }

    [[nodiscard]] const Color24& GetLineColor(const PlayerBase* const target, const float last_hit_time) noexcept
    {
        if (g_global_vars->time - last_hit_time < 1.F && !cvars::LineColorHit().IsNull()) {
            return cvars::LineColorHit();
        }

        return target->team == TeamName::Ct ? cvars::LineColorCt() : cvars::LineColorT();
    }
}

namespace whdh
{
    Beams::Beams(std::shared_ptr<Data> data, std::shared_ptr<Resources> resources, std::shared_ptr<Preferences> preferences)
        : data_(std::move(data)), resources_(std::move(resources)), preferences_(std::move(preferences))
    {
        assert(data_ != nullptr);
        assert(resources_ != nullptr);
        assert(preferences_ != nullptr);
    }

    void Beams::Draw(const PlayerBase* const observer, int observer_index)
    {
        assert(IsValidEntity(observer));
        assert(IsClient(observer_index));

        if (!CheckUpdateTime(data_->BeamUpdateTime(observer_index)) ||
            static_cast<ObserverMode>(observer->vars->i_user1) != ObserverMode::InEye ||
            (observer->team != TeamName::Spectator && cvars::SpecOnly())) {
            return;
        }

        const auto& user_preferences = (*preferences_)[observer_index];
        const auto visibility = user_preferences.visibility;

        if (visibility <= Visibility::Off || visibility > Visibility::OpponentsAndTeammates) {
            return;
        }

        if (!user_preferences.boxes && !user_preferences.lines) {
            return;
        }

        if (const auto* const observable = adminskit::GetObservablePlayer(observer); IsValidEntity(observable)) {
            draw_args_.observer = observer;
            const auto observable_index = observer->vars->i_user2;

            for (auto i = 1; i <= g_global_vars->max_clients; ++i) {
                if (i == observer_index || i == observable_index) {
                    continue;
                }

                const auto* target = type_conversion::PlayerByIndex(i);

                if (!target || !IsPlayerAlive(target) || adminskit::IsSpectatorOrUnassigned(target)) {
                    continue;
                }

                if (visibility == Visibility::Opponents && target->team == observable->team) {
                    continue;
                }

                if (visibility == Visibility::Teammates && target->team != observable->team) {
                    continue;
                }

                draw_args_.target = target;
                draw_args_.last_hit_time = data_->LastHitTime(observable_index, i);
                draw_args_.distance_to_target = observable->vars->origin.Distance(target->vars->origin);

                if (user_preferences.boxes) {
                    DrawBox(observable, draw_args_);
                }

                if (user_preferences.lines) {
                    DrawLine(draw_args_);
                }
            }
        }
    }

    void Beams::DrawBox(const PlayerBase* const observable, const DrawArgs& draw_args)
    {
        assert(resources_->Precached());

        static TraceResult trace_result{};
        engine::TraceLine(observable->vars->origin, draw_args.target->vars->origin,
                          TR_IGNORE_GLASS, observable->GetEdict(), &trace_result);

        const auto distance_to_hitpoint = observable->vars->origin.Distance(trace_result.end_position);
        const auto scaled_bone_length = trace_result.fraction * 50.F;

        points_args_.sprite_index = resources_->BeamSpriteIndex();
        points_args_.origin = GetBoxOrigin(observable, draw_args.target, distance_to_hitpoint);
        points_args_.destination = GetBoxDestination(points_args_.origin, scaled_bone_length);
        points_args_.line_width = GetBoxWidth(scaled_bone_length);
        points_args_.brightness = GetBoxBrightness(draw_args.distance_to_target);
        points_args_.color = GetBoxColor(draw_args.target, draw_args.distance_to_target, distance_to_hitpoint, draw_args.last_hit_time, trace_result);
        points_args_.life_time = GetLifeTime();

        SendBeamPoints(
            draw_args.observer->GetEdict(), MessageType::OneUnreliable, points_args_);
    }

    void Beams::DrawLine(const DrawArgs& draw_args)
    {
        assert(resources_->Precached());

        ent_point_args_.sprite_index = resources_->BeamSpriteIndex();
        ent_point_args_.start_entity = draw_args.observer->vars->i_user2;
        ent_point_args_.destination = draw_args.target->vars->origin;
        ent_point_args_.line_width = GetLineWidth(draw_args.distance_to_target);
        ent_point_args_.color = GetLineColor(draw_args.target, draw_args.last_hit_time);
        ent_point_args_.life_time = GetLifeTime();

        SendBeamEntPoint(
            draw_args.observer->GetEdict(), MessageType::OneUnreliable, ent_point_args_);
    }
}
