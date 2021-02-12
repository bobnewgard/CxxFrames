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

#include <iomanip>
#include <sstream>
#include <FramePause.h>

namespace Frames
{
    using namespace std;

    FramePause::FramePause(void) : FrameEth()
    {
        this->quanta = 1;
    }

    FramePause::~FramePause(void) { }

    void FramePause::set_pause_param(unsigned arg_quanta)
    {
        this->quanta = arg_quanta & 0xFFFF;
    }

    void FramePause::encapsulate(void)
    {
        BVec mcst;
        BVec bytes;

        mcst.push_back(PAUSE_MCST[0]);
        mcst.push_back(PAUSE_MCST[1]);
        mcst.push_back(PAUSE_MCST[2]);
        mcst.push_back(PAUSE_MCST[3]);
        mcst.push_back(PAUSE_MCST[4]);
        mcst.push_back(PAUSE_MCST[5]);

        bytes.push_back((uint8_t)(PAUSE_MAC_CTRL >> 8));
        bytes.push_back((uint8_t)(PAUSE_MAC_CTRL & 0x00FF));
        bytes.push_back((uint8_t)(this->quanta >> 8));
        bytes.push_back((uint8_t)(this->quanta & 0x00FF));

        for (unsigned int i = 0 ; i < PAUSE_PAD_SIZE ; ++i)
        {
            bytes.push_back(0x00);
        }

        this->set_eth_dmac(mcst);
        this->set_eth_type(EtherType::ETYP_FLOW);
        this->set_eth_payload(bytes);
        FrameEth::encapsulate();
    }

    string FramePause::gist(void)
    {
        stringstream ss;

        ss  << "{"
            << "quanta:0x" << setfill('0') << setw(4) << hex << this->quanta
            << ",frame:"   << FrameEth::gist()
            << "}";

        return ss.str();
    }
}
