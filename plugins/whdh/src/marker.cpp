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

#include "marker.h"
#include <core/regamedll_api.h>
#include <core/type_conversion.h>
#include <metamod/engine.h>
#include <metamod/gamedll.h>
#include <whdh/cvars.h>
#include <utility>

using namespace core;
using namespace cssdk;
using namespace metamod;

namespace
{
    enum FramePoint
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        FramePointEnd
    };

    ATTR_OPTIMIZE_HOT void CalculateFramePoints(const Vector& origin, const Vector& perpendicular_angles,
                                                Vector frame_points[FramePointEnd])
    {
        constexpr auto top_offset = 25.F;
        constexpr auto side_offset = 13.F;
        constexpr auto bottom_offset = 36.F;

        static Vector forward{}, right{}, up{};
        engine::AngleVectors(perpendicular_angles, forward, right, up);

        static Vector side_dislocation{};
        side_dislocation = right * side_offset;

        static Vector top_dislocation{};
        top_dislocation = up * top_offset;

        static Vector bottom_dislocation{};
        bottom_dislocation = (up * bottom_offset).Negate();

        frame_points[TopLeft] = top_dislocation - side_dislocation + origin;
        frame_points[TopRight] = top_dislocation + side_dislocation + origin;
        frame_points[BottomLeft] = bottom_dislocation - side_dislocation + origin;
        frame_points[BottomRight] = bottom_dislocation + side_dislocation + origin;
    }

    ATTR_OPTIMIZE_HOT bool TraceEyesFrame(Edict* const entity_to_ignore, const Vector& eyes,
                                          const Vector frame_points[FramePointEnd],
                                          Vector frame_points_traced[FramePointEnd],
                                          int& closer_frame_point)
    {
        static TraceResult trace_result{};
        auto small_fraction = 1.F;

        for (auto i = 0; i < FramePointEnd; ++i) {
            engine::TraceLine(eyes, frame_points[i], TR_IGNORE_GLASS, entity_to_ignore, &trace_result);

            if (trace_result.fraction >= 1.F || trace_result.fraction <= 0.F) {
                return false;
            }

            if (trace_result.fraction < small_fraction) {
                small_fraction = trace_result.fraction;
                closer_frame_point = i;
            }

            frame_points_traced[i] = trace_result.end_position;
        }

        return true;
    }
}

namespace whdh
{
    Marker::Marker(std::shared_ptr<Data> data, std::shared_ptr<Resources> resources, std::shared_ptr<Preferences> preferences)
        : data_(std::move(data)), resources_(std::move(resources)), preferences_(std::move(preferences))
    {
        assert(data_ != nullptr);
        assert(resources_ != nullptr);
        assert(preferences_ != nullptr);
    }

    qboolean Marker::Draw(EntityState* const state, const int entity_index, Edict* const entity, Edict* const host,
                          const int host_index, const int host_flags, const qboolean player, unsigned char* const set) const
    {
        const auto& preferences = (*preferences_)[host_index];
        const auto visibility = preferences.visibility;

        if (!preferences.marker || visibility <= Visibility::Off || visibility > Visibility::OpponentsAndTeammates) {
            return 0;
        }

        if (const auto host_team = EntityPrivateData<PlayerBase>(host)->team;
            host_team == TeamName::Unassigned || (host_team != TeamName::Spectator && cvars::SpecOnly())) {
            return 0;
        }

        Edict* observable{};

        if (IsClient(host->vars.i_user2)) {
            observable = type_conversion::EdictByIndex(host->vars.i_user2);

            if (!IsValidEntity(observable)) {
                return 0;
            }
        }
        else {
            observable = host;
        }

        assert(IsValidEntity(observable));
        const auto observable_team = static_cast<int>(EntityPrivateData<PlayerBase>(observable)->team);

        if ((visibility == Visibility::Opponents && entity->vars.team == observable_team) ||
            (visibility == Visibility::Teammates && entity->vars.team != observable_team)) {
            return 0;
        }

        static Vector direction_angles{};
        engine::VecToAngles((entity->vars.owner->vars.origin - observable->vars.origin).Normalize(), direction_angles);
        direction_angles.x = -direction_angles.x;

        static Vector observer_eyes{};
        observer_eyes = GetGunPosition(observable->vars);

        static Vector frame_points[FramePointEnd]{};
        CalculateFramePoints(entity->vars.owner->vars.origin, direction_angles, frame_points);

        static Vector frame_points_traced[FramePointEnd]{};
        auto closer_frame_point = 0;

        if (!TraceEyesFrame(observable, observer_eyes, frame_points, frame_points_traced, closer_frame_point)) {
            return 0;
        }

        if (!gamedll::AddToFullPack(state, entity_index, entity, host, host_flags, player, set)) {
            return 0;
        }

        static Vector forward{}, right{}, up{};
        engine::AngleVectors(direction_angles, forward, right, up);

        static Vector point_up{};
        point_up = frame_points_traced[closer_frame_point] + up;

        static Vector point_right{};
        point_right = frame_points_traced[closer_frame_point] + right;

        static Vector4D plane{};
        plane = Vector4D::Plane3Points(frame_points_traced[closer_frame_point], point_right, point_up);

        // Move to plane
        for (auto i = 0; i < FramePointEnd; ++i) {
            if (i != closer_frame_point) {
                plane.PlaneRayIntersect(frame_points_traced[i], observer_eyes - frame_points_traced[i], frame_points_traced[i]);
            }
        }

        state->angles = direction_angles;
        state->origin = (frame_points_traced[TopLeft] + frame_points_traced[BottomRight]) / 2.F;
        state->scale = std::clamp(frame_points_traced[TopLeft].Distance(frame_points_traced[TopRight]) * 0.013F, 0.005F, 1.F);

        return 1;
    }

    void Marker::CreateEntity(const PlayerBase* const owner) const
    {
        assert(IsValidEntity(owner));

        DestroyEntity(owner);
        auto* const entity = regamedll_api::Funcs()->create_named_entity2(resources_->StringInfoTarget());

        if (!IsValidEntity(entity)) {
            return;
        }

        entity->vars.solid = SolidType::NotSolid;
        entity->vars.class_name = resources_->StringWhdh();
        entity->vars.model = resources_->StringMarkerSprite();
        entity->vars.model_index = resources_->MarkerSpriteIndex();

        if (gamedll::Spawn(entity)) {
            entity->vars.flags |= FL_KILL_ME;
            return;
        }

        entity->vars.impulse = MARKER_UNIQUE_ID;
        entity->vars.owner = owner->GetEdict();
        entity->vars.team = static_cast<int>(owner->team);
        entity->vars.frame = static_cast<float>(entity->vars.team - 1);
        entity->vars.effects |= EF_FORCE_VISIBILITY | EF_OWNER_NO_VISIBILITY;

        data_->MarkerEntity(entity->vars.owner) = entity;
    }

    void Marker::DestroyEntity(const PlayerBase* const owner) const
    {
        assert(IsValidEntity(owner));
        DestroyEntity(type_conversion::IndexOfEntity(owner));
    }

    void Marker::DestroyEntity(const int owner) const
    {
        assert(IsClient(owner));

        auto* const entity = data_->MarkerEntity(owner);
        data_->MarkerEntity(owner) = nullptr;

        if (IsValidEntity(entity)) {
            entity->vars.flags |= FL_KILL_ME;
            entity->vars.effects |= EF_NO_DRAW;
        }
    }
}
