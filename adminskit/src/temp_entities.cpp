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

#include <adminskit/temp_entities.h>
#include <metamod/engine.h>

using namespace cssdk;
using namespace metamod;

namespace adminskit::temp_entities
{
    void SendBeamPoints(Edict* const client, const MessageType type, const BeamPointsArgs& args)
    {
        engine::MessageBegin(type, static_cast<int>(SvcMessage::TempEntity), nullptr, client);
        engine::WriteByte(TE_BEAM_POINTS);
        engine::WriteCoord(args.origin.x);
        engine::WriteCoord(args.origin.y);
        engine::WriteCoord(args.origin.z);
        engine::WriteCoord(args.destination.x);
        engine::WriteCoord(args.destination.y);
        engine::WriteCoord(args.destination.z);
        engine::WriteShort(args.sprite_index);
        engine::WriteByte(args.starting_frame);
        engine::WriteByte(args.frame_rate);
        engine::WriteByte(args.life_time);
        engine::WriteByte(args.line_width);
        engine::WriteByte(args.noise_amplitude);
        engine::WriteByte(args.color.red);
        engine::WriteByte(args.color.green);
        engine::WriteByte(args.color.blue);
        engine::WriteByte(args.brightness);
        engine::WriteByte(args.scroll_speed);
        engine::MessageEnd();
    }

    void SendBeamEntPoint(Edict* const client, const MessageType type, const BeamEntPointArgs& args)
    {
        engine::MessageBegin(type, static_cast<int>(SvcMessage::TempEntity), nullptr, client);
        engine::WriteByte(TE_BEAM_ENT_POINT);
        engine::WriteShort(args.start_entity);
        engine::WriteCoord(args.destination.x);
        engine::WriteCoord(args.destination.y);
        engine::WriteCoord(args.destination.z);
        engine::WriteShort(args.sprite_index);
        engine::WriteByte(args.starting_frame);
        engine::WriteByte(args.frame_rate);
        engine::WriteByte(args.life_time);
        engine::WriteByte(args.line_width);
        engine::WriteByte(args.noise_amplitude);
        engine::WriteByte(args.color.red);
        engine::WriteByte(args.color.green);
        engine::WriteByte(args.color.blue);
        engine::WriteByte(args.brightness);
        engine::WriteByte(args.scroll_speed);
        engine::MessageEnd();
    }
}
