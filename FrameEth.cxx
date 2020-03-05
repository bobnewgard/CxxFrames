/*
 *  Copyright 2020 Robert Newgard
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
#include <iostream>
#include <FrameEth.h>

namespace Frames
{
    using namespace std;

    FrameEth::FrameEth(void) : Frame()
    {
        spec[ ETH_DMAC        ] = {false, 0xff, BVec()};
        spec[ ETH_SMAC        ] = {false, 0xff, BVec()};
        spec[ ETH_TYPE_INSERT ] = {false, 0xff, BVec()};
        spec[ ETH_TYPE_ENCAP  ] = {false, 0xff, BVec()};
        spec[ ETH_PAYLOAD     ] = {false, 0xff, BVec()};
    }

    FrameEth::~FrameEth(void) { }

    void FrameEth::get_vec_eth_type(BVec &arg_vec, EtherType arg_et)
    {
        uint16_t etyp = (uint16_t)arg_et;

        arg_vec.push_back((uint8_t)((etyp & 0xFF00) >> 8));
        arg_vec.push_back((uint8_t)(etyp & 0x00FF));
    }

    void FrameEth::set_eth_dmac(const BVec &arg_mac)
    {
        if (arg_mac.size() != 6)
        {
            cerr << "[ERR] set_eth_dmac(): mac parameter size is not 6 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ETH_DMAC].bytes.insert(this->spec[ETH_DMAC].bytes.end(), arg_mac.begin(), arg_mac.end());
        this->spec[ETH_DMAC].valid = true;
    }

    void FrameEth::set_eth_smac(const BVec &arg_mac)
    {
        if (arg_mac.size() != 6)
        {
            cerr << "[ERR] set_eth_smac(): mac parameter size is not 6 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ETH_SMAC].bytes.insert(this->spec[ETH_SMAC].bytes.end(), arg_mac.begin(), arg_mac.end());
        this->spec[ETH_SMAC].valid = true;
    }

    void FrameEth::set_eth_type(uint16_t arg_et)
    {
        this->spec[ETH_TYPE_ENCAP].bytes.push_back((uint8_t)(arg_et >> 8));
        this->spec[ETH_TYPE_ENCAP].bytes.push_back((uint8_t)(arg_et & 0x00FF));
        this->spec[ETH_TYPE_ENCAP].valid = true;
    }

    void FrameEth::set_eth_type(EtherType arg_et)
    {
        this->set_eth_type((uint16_t)(arg_et));
    }

    void FrameEth::set_eth_payload(const BVec &arg_bytes)
    {
        this->spec[ETH_PAYLOAD].bytes.insert(this->spec[ETH_PAYLOAD].bytes.end(), arg_bytes.begin(), arg_bytes.end());
        this->spec[ETH_PAYLOAD].valid = true;
    }

    void FrameEth::insert(const BVec &arg_type, const BVec &arg_bytes)
    {
        if (arg_type.size() != 2)
        {
            cerr << "[ERR] insert(): type parameter size is not 2 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ETH_TYPE_INSERT].bytes.insert(this->spec[ETH_TYPE_INSERT].bytes.end(), arg_type.begin(), arg_type.end());
        this->spec[ETH_TYPE_INSERT].bytes.insert(this->spec[ETH_TYPE_INSERT].bytes.end(), arg_bytes.begin(), arg_bytes.end());
        this->spec[ETH_TYPE_INSERT].valid = true;
    }

    void FrameEth::encapsulate(void)
    {
        bool   okay   = true;
        string errmsg = "[ERR] encapsulate(): cannot encapsulate with an invalid";
        BVec   bytes;

        if (not this->spec[ETH_DMAC].valid)       okay = false;
        if (not this->spec[ETH_SMAC].valid)       okay = false;
        if (not this->spec[ETH_TYPE_ENCAP].valid) okay = false;
        if (not this->spec[ETH_PAYLOAD].valid)    okay = false;

        if (not this->spec[ETH_DMAC].valid)       cerr << errmsg << " dmac"         << endl << flush;
        if (not this->spec[ETH_SMAC].valid)       cerr << errmsg << " smac"         << endl << flush;
        if (not this->spec[ETH_TYPE_ENCAP].valid) cerr << errmsg << " payload type" << endl << flush;
        if (not this->spec[ETH_PAYLOAD].valid)    cerr << errmsg << " payload"      << endl << flush;

        if (not okay) exit(1);

        bytes.insert(bytes.end(), this->spec[ETH_DMAC].bytes.begin(), this->spec[ETH_DMAC].bytes.end());
        bytes.insert(bytes.end(), this->spec[ETH_SMAC].bytes.begin(), this->spec[ETH_SMAC].bytes.end());

        if (this->spec[ETH_TYPE_INSERT].valid)
        {
            bytes.insert(bytes.end(), this->spec[ETH_TYPE_INSERT].bytes.begin(), this->spec[ETH_TYPE_INSERT].bytes.end());
        }

        bytes.insert(bytes.end(), this->spec[ETH_TYPE_ENCAP].bytes.begin(), this->spec[ETH_TYPE_ENCAP].bytes.end());
        bytes.insert(bytes.end(), this->spec[ETH_PAYLOAD].bytes.begin(), this->spec[ETH_PAYLOAD].bytes.end());

        give_frame(move(bytes));
    }

    string FrameEth::gist(void)
    {
        stringstream ss;

        ss  << "{spec:"
            << "ETH_DMAC:" << gist_item(this->spec[ETH_DMAC])
            << ",ETH_SMAC:" << gist_item(this->spec[ETH_SMAC]);

        if (this->spec[ETH_TYPE_INSERT].valid)
        {
            ss << ",ETH_TYPE_INSERT:" << gist_item(this->spec[ETH_TYPE_INSERT]);
        }


        ss  << ",ETH_TYPE_ENCAP:" << gist_item(this->spec[ETH_TYPE_ENCAP])
            << ",ETH_PAYLOAD:" << gist_item(this->spec[ETH_PAYLOAD])
            << ",frame:" << Frame::gist()
            << "}";

        return ss.str();
    }
}
