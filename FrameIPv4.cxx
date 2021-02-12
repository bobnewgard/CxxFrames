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
#include <iostream>
#include <sstream>
#include <FrameIPv4.h>

namespace Frames
{
    using namespace std;

    FrameIPv4::FrameIPv4(void) : FrameEth()
    {
        this->spec[ IPV4_PROTO   ] = {false, BVec()};
        this->spec[ IPV4_SIP     ] = {false, BVec()};
        this->spec[ IPV4_DIP     ] = {false, BVec()};
        this->spec[ IPV4_PAYLOAD ] = {false, BVec()};
        this->checksum             = 0;
    }

    FrameIPv4::~FrameIPv4(void) { }

    void FrameIPv4::set_ipv4_proto(const IPv4Proto arg_proto)
    {
        this->spec[IPV4_PROTO].bytes.push_back((uint8_t)(arg_proto));
        this->spec[IPV4_PROTO].valid = true;
    }

    void FrameIPv4::set_ipv4_sip(const BVec &arg_sip)
    {
        if (arg_sip.size() != 4)
        {
            cerr << "[ERR] set_ipv4_sip(): IP parameter size is not 4 bytes" << endl << flush;
            exit(1);
        }

        this->spec[IPV4_SIP].bytes.insert(this->spec[IPV4_SIP].bytes.end(), arg_sip.begin(), arg_sip.end());
        this->spec[IPV4_SIP].valid = true;
    }

    void FrameIPv4::set_ipv4_dip(const BVec &arg_dip)
    {
        if (arg_dip.size() != 4)
        {
            cerr << "[ERR] set_ipv4_dip(): IP parameter size is not 4 bytes" << endl << flush;
            exit(1);
        }

        this->spec[IPV4_DIP].bytes.insert(this->spec[IPV4_DIP].bytes.end(), arg_dip.begin(), arg_dip.end());
        this->spec[IPV4_DIP].valid = true;
    }

    void FrameIPv4::set_ipv4_payload(const BVec &arg_pyld)
    {
        this->spec[IPV4_PAYLOAD].bytes.insert(this->spec[IPV4_PAYLOAD].bytes.end(), arg_pyld.begin(), arg_pyld.end());
        this->spec[IPV4_PAYLOAD].valid = true;
    }

    uint32_t FrameIPv4::cksum_calc(const BVec &arg_hdr)
    {
        uint32_t word  = 0;
        uint32_t accum = 0;
        bool     mod0  = true;

        for (auto it = arg_hdr.begin() ; it != arg_hdr.end() ; ++it)
        {
            if (mod0)
            {
                word = (*it) << 8;
            }
            else
            {
                word  = word + (*it);
                accum = accum + word;
            }

            mod0 = not mod0;
        }

        return (accum & 0xFFFF) + (accum >> 16);
    }

    void FrameIPv4::cksum_gen(BVec &arg_hdr)
    {
        uint32_t accum;
            
        accum          = 0;
        arg_hdr[10]    = 0;
        arg_hdr[11]    = 0;
        accum          = this->cksum_calc(arg_hdr);
        accum          = ~accum;
        arg_hdr[10]    = (accum & 0x0000FF00) >> 8;
        arg_hdr[11]    = accum & 0x000000FF;
        this->checksum = accum;
    }

    bool FrameIPv4::cksum_chk(const BVec &arg_hdr)
    {
        uint32_t accum = this->cksum_calc(arg_hdr);

        if (accum == 0xFFFF) return true;
        return false;
    }


    void FrameIPv4::encapsulate(void)
    {
        bool     okay    = true;
        string   errmsg  = "[ERR] encapsulate(): cannot encapsulate with an invalid";
        uint32_t tlength = this->spec[IPV4_PAYLOAD].bytes.size() + (IPV4_HLEN * 4);
        BVec     bytes;

        if (not this->spec[ IPV4_PROTO ].valid) okay = false;
        if (not this->spec[ IPV4_SIP   ].valid) okay = false;
        if (not this->spec[ IPV4_DIP   ].valid) okay = false;
        if (tlength > 65535)                    okay = false;


        if (not this->spec[ IPV4_PROTO ].valid) cerr << errmsg << " proto"  << endl << flush;
        if (not this->spec[ IPV4_SIP   ].valid) cerr << errmsg << " sip"    << endl << flush;
        if (not this->spec[ IPV4_DIP   ].valid) cerr << errmsg << " dip"    << endl << flush;
        if (tlength > 65535)                    cerr << errmsg << " length" << endl << flush;

        if (not okay) exit(1);

        bytes.push_back((IPV4_VERS << 4) + IPV4_HLEN);
        bytes.push_back(IPV4_TOS);
        bytes.push_back(tlength >> 8);
        bytes.push_back(tlength & 0x00FF);
        bytes.push_back(IPV4_FRAG[0]);
        bytes.push_back(IPV4_FRAG[1]);
        bytes.push_back(IPV4_FRAG[2]);
        bytes.push_back(IPV4_FRAG[3]);
        bytes.push_back(IPV4_TTL);
        bytes.push_back(this->spec[IPV4_PROTO].bytes[0]);
        bytes.push_back(0x00);
        bytes.push_back(0x00);
        bytes.push_back(this->spec[IPV4_SIP].bytes[0]);
        bytes.push_back(this->spec[IPV4_SIP].bytes[1]);
        bytes.push_back(this->spec[IPV4_SIP].bytes[2]);
        bytes.push_back(this->spec[IPV4_SIP].bytes[3]);
        bytes.push_back(this->spec[IPV4_DIP].bytes[0]);
        bytes.push_back(this->spec[IPV4_DIP].bytes[1]);
        bytes.push_back(this->spec[IPV4_DIP].bytes[2]);
        bytes.push_back(this->spec[IPV4_DIP].bytes[3]);

        this->cksum_gen(bytes);

        if (not this->cksum_chk(bytes))
        {
            cerr << errmsg << " header checksum 0x" << setfill('0') << setw(4) << hex << this->checksum << endl << flush;
            exit(1);
        }

        bytes.insert(bytes.end(), this->spec[IPV4_PAYLOAD].bytes.begin(), this->spec[IPV4_PAYLOAD].bytes.end());

        this->set_eth_type(EtherType::ETYP_IPV4);
        this->set_eth_payload(bytes);
        FrameEth::encapsulate();
    }

    string FrameIPv4::gist(void)
    {
        stringstream ss;

        ss  << "{spec:"
            << ",IPV4_PROTO:"   << gist_item(this->spec[IPV4_PROTO])
            << ",IPV4_SIP:"     << gist_item(this->spec[IPV4_SIP])
            << ",IPV4_DIP:"     << gist_item(this->spec[IPV4_DIP])
            << ",IPV4_PAYLOAD:" << gist_item(this->spec[IPV4_PAYLOAD])
            << ",frame:"        << FrameEth::gist()
            << "}";

        return ss.str();
    }
}
