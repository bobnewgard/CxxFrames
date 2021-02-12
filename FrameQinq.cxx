/*
 *  Copyright 2020-2021 Robert Newgard
 *
 *  This file is part of CxxFrames.
 *
 *  CxxFrames is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CxxFrames is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with CxxFrames.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <FrameQinq.h>

namespace Frames
{
    using namespace std;

    void insert_qinq(FrameEth &arg_eth, unsigned arg_tci)
    {
        BVec    type;
        BVec    tci;
        uint8_t tci1 = (uint8_t)((arg_tci & 0xFF00) >> 8);
        uint8_t tci0 = (uint8_t)(arg_tci & 0x00FF);

        arg_eth.get_vec_eth_type(type, EtherType::ETYP_QINQ);

        tci.push_back(tci1);
        tci.push_back(tci0);

        arg_eth.insert(type, tci);
    }

}
