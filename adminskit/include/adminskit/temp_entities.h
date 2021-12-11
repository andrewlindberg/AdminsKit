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

#include <cssdk/common/const.h>
#include <cssdk/common/vector.h>
#include <cssdk/engine/edict.h>

namespace adminskit::temp_entities
{
    struct BeamPointsArgs
    {
        cssdk::Vector origin{};
        cssdk::Vector destination{};
        int sprite_index{};
        int starting_frame{};
        int frame_rate{};
        int life_time{1};
        int line_width{1};
        int noise_amplitude{};
        cssdk::Color24 color{};
        int brightness{255};
        int scroll_speed{};

        BeamPointsArgs& SetOrigin(const cssdk::Vector& value)
        {
            origin = value;
            return *this;
        }

        BeamPointsArgs& SetDestination(const cssdk::Vector& value)
        {
            destination = value;
            return *this;
        }

        BeamPointsArgs& SetSpriteIndex(const int value)
        {
            sprite_index = value;
            return *this;
        }

        BeamPointsArgs& SetStartingFrame(const int value)
        {
            starting_frame = value;
            return *this;
        }

        BeamPointsArgs& SetFrameRate(const int value)
        {
            frame_rate = value;
            return *this;
        }

        BeamPointsArgs& SetLifeTime(const int value)
        {
            life_time = value;
            return *this;
        }

        BeamPointsArgs& SetLineWidth(const int value)
        {
            line_width = value;
            return *this;
        }

        BeamPointsArgs& SetNoiseAmplitude(const int value)
        {
            noise_amplitude = value;
            return *this;
        }

        BeamPointsArgs& SetColor(const cssdk::Color24& value)
        {
            color = value;
            return *this;
        }

        BeamPointsArgs& SetBrightness(const int value)
        {
            brightness = value;
            return *this;
        }

        BeamPointsArgs& SetScrollSpeed(const int value)
        {
            scroll_speed = value;
            return *this;
        }
    };

    struct BeamEntPointArgs
    {
        int start_entity{};
        cssdk::Vector destination{};
        int sprite_index{};
        int starting_frame{};
        int frame_rate{};
        int life_time{1};
        int line_width{1};
        int noise_amplitude{};
        cssdk::Color24 color{};
        int brightness{255};
        int scroll_speed{};

        BeamEntPointArgs& SetStartEntity(const int value)
        {
            start_entity = value;
            return *this;
        }

        BeamEntPointArgs& SetDestination(const cssdk::Vector& value)
        {
            destination = value;
            return *this;
        }

        BeamEntPointArgs& SetSpriteIndex(const int value)
        {
            sprite_index = value;
            return *this;
        }

        BeamEntPointArgs& SetStartingFrame(const int value)
        {
            starting_frame = value;
            return *this;
        }

        BeamEntPointArgs& SetFrameRate(const int value)
        {
            frame_rate = value;
            return *this;
        }

        BeamEntPointArgs& SetLifeTime(const int value)
        {
            life_time = value;
            return *this;
        }

        BeamEntPointArgs& SetLineWidth(const int value)
        {
            line_width = value;
            return *this;
        }

        BeamEntPointArgs& SetNoiseAmplitude(const int value)
        {
            noise_amplitude = value;
            return *this;
        }

        BeamEntPointArgs& SetColor(const cssdk::Color24& value)
        {
            color = value;
            return *this;
        }

        BeamEntPointArgs& SetBrightness(const int value)
        {
            brightness = value;
            return *this;
        }

        BeamEntPointArgs& SetScrollSpeed(const int value)
        {
            scroll_speed = value;
            return *this;
        }
    };

    /**
     * @brief Beam effect between two points.
    */
    void SendBeamPoints(cssdk::Edict* client, cssdk::MessageType type, const BeamPointsArgs& args);

    /**
     * @brief Beam effect between a point and an entity.
    */
    void SendBeamEntPoint(cssdk::Edict* client, cssdk::MessageType type, const BeamEntPointArgs& args);
}
