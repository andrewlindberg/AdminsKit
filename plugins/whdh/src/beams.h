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

#include "data.h"
#include "preferences.h"
#include "resources.h"
#include <adminskit/temp_entities.h>
#include <cssdk/dll/player.h>
#include <cssdk/public/utils.h>
#include <cassert>
#include <memory>

namespace whdh
{
    class Beams final
    {
        struct DrawArgs
        {
            const cssdk::PlayerBase* observer{};
            const cssdk::PlayerBase* target{};
            float distance_to_target{};
            float last_hit_time{};

            DrawArgs& SetObserver(const cssdk::PlayerBase* const value)
            {
                assert(IsValidEntity(value));
                observer = value;

                return *this;
            }

            DrawArgs& SetTarget(const cssdk::PlayerBase* const value)
            {
                assert(IsValidEntity(value));
                target = value;

                return *this;
            }

            DrawArgs& SetDistanceToTarget(const float value)
            {
                assert(value >= 0.F);
                distance_to_target = value;

                return *this;
            }

            DrawArgs& SetLastHitTime(const float value)
            {
                assert(value >= 0.F);
                last_hit_time = value;

                return *this;
            }
        };

        std::shared_ptr<Data> data_;
        std::shared_ptr<Resources> resources_;
        std::shared_ptr<Preferences> preferences_;

        DrawArgs draw_args_{};
        adminskit::temp_entities::BeamPointsArgs points_args_{};
        adminskit::temp_entities::BeamEntPointArgs ent_point_args_{};

    public:
        Beams(std::shared_ptr<Data> data, std::shared_ptr<Resources> resources, std::shared_ptr<Preferences> preferences);
        void Draw(const cssdk::PlayerBase* observer, int observer_index);

    private:
        void DrawBox(const cssdk::PlayerBase* observable, const DrawArgs& draw_args);
        void DrawLine(const DrawArgs& draw_args);
    };
}
