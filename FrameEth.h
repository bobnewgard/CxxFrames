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

#ifndef _FRAME_ETH_H_
    #define _FRAME_ETH_H_
    
    #include <Frame.h>
    #include <unordered_map>

    namespace Frames
    {
        enum EthField  { ETH_DMAC, ETH_SMAC, ETH_TYPE_INSERT, ETH_TYPE_ENCAP, ETH_PAYLOAD };

        enum class EtherType : uint16_t
        {
            ETYP_IPV4 = 0x0800,
            ETYP_ARP  = 0x0806,
            ETYP_VLAN = 0x8100,
            ETYP_IPV6 = 0x86DD,
            ETYP_FLOW = 0x8808,
            ETYP_QINQ = 0x88A8,
            ETYP_MPSB = 0xBEE5
        };

        const unsigned PayloadMinBytes = 46;
        const unsigned PayloadMaxBytes = 1500;
        const unsigned FrameMinBytes   = PayloadMinBytes + 14;
        const uint8_t  MAC_BCST[]      = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

        class FrameEth : public Frame
        {
            private:
                std::unordered_map<EthField, item> spec;

            public:
                FrameEth(void);
                virtual ~FrameEth(void);

                void get_vec_eth_type(BVec &arg_vec, EtherType arg_et);
                void set_eth_dmac(const BVec &arg_mac);
                void set_eth_smac(const BVec &arg_mac);
                void set_eth_type(uint16_t arg_et);
                void set_eth_type(EtherType arg_et);
                void set_eth_payload(const BVec &arg_bytes);
                void insert(const BVec &arg_type, const BVec &arg_bytes);
                virtual void encapsulate(void);
                virtual std::string gist(void);
        };
    }
#endif
