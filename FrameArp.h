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

#ifndef _FRAME_ARP_H_
    #define _FRAME_ARP_H_
    
    #include <FrameEth.h>

    namespace Frames
    {
        enum class ArpOp : uint8_t
        {
            OP_NIL = 0x00,
            OP_REQ = 0x01,
            OP_ACK = 0x02
        };

        enum ArpField { ARP_OP, ARP_QMAC, ARP_QIP, ARP_TMAC, ARP_TIP };

        const uint8_t  ARP_HW_TYP[] = {0x00, 0x01};
        const uint8_t  ARP_HW_SIZ   = 0x06;
        const uint8_t  ARP_PR_SIZ   = 0x04;
        const unsigned ARP_PAD_SIZE = 18;

        class FrameArp : public FrameEth
        {
            private:
                std::unordered_map<ArpField, item> spec;
                ArpOp                              spec_op;

            public:
                FrameArp(void);
                virtual ~FrameArp(void);

                void set_arp_op(ArpOp arg_op);
                void set_arp_qmac(const BVec &arg_qmac);
                void set_arp_qip(const BVec &arg_qip);
                void set_arp_tmac(const BVec &arg_tmac);
                void set_arp_tip(const BVec &arg_tip);
                virtual void encapsulate(void);
                virtual std::string gist(void);
        };
    }
#endif
