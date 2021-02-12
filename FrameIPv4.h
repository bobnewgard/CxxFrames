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

#ifndef _FRAME_IPV4_H_
    #define _FRAME_IPV4_H_
    
    #include <FrameEth.h>

    namespace Frames
    {
        enum IPv4Field { IPV4_PROTO, IPV4_SIP, IPV4_DIP, IPV4_PAYLOAD };
        enum UdpField  { UDP_SPORT, UDP_DPORT, UDP_PAYLOAD };

        enum class IPv4Proto : uint8_t
        {
            PROTO_ICMP = 0x01,
            PROTO_TCP  = 0x06,
            PROTO_UDP  = 0x11
        };

        const uint8_t IPV4_VERS   = 0x04;
        const uint8_t IPV4_HLEN   = 0x05;
        const uint8_t IPV4_TOS    = 0x00;
        const uint8_t IPV4_FRAG[] = {0x00, 0x00, 0x40, 0x00};
        const uint8_t IPV4_TTL    = 0x20;

        class FrameIPv4 : public FrameEth
        {
            private:
                std::unordered_map<IPv4Field, item> spec;
                uint16_t                            checksum;

            public:
                FrameIPv4(void);
                virtual ~FrameIPv4(void);

                void set_ipv4_proto(const IPv4Proto arg_proto);
                void set_ipv4_sip(const BVec &arg_sip);
                void set_ipv4_dip(const BVec &arg_dip);
                void set_ipv4_payload(const BVec &arg_payload);

                uint32_t cksum_calc(const BVec &arg_hdr);
                void cksum_gen(BVec &arg_hdr);
                bool cksum_chk(const BVec &arg_hdr);

                virtual void encapsulate(void);
                virtual std::string gist(void);
        };
    }
#endif

// Checksum
//                       checksum
//                           V
// [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]
// 4500 0073 0000 4000 4011 b861 c0a8 0001
// c0a8 00c7 .... .... .... .... .... ....
//
// Calculate
//     perl6 -e 'printf("0x%x\n", 0x4500+0x0073+0x0000+0x4000+0x4011+0x0000+0xc0a8+0x0001+0xc0a8+0x00c7)'
//     0x2479c
//     perl6 -e 'printf("0x%x\n", 0x2 + 0x479c)'
//     0x479e
//     perl6 -e 'printf("0x%x\n", 0x479e +^ 0xffff )'
//     0xb861
//
// Check
//     perl6 -e 'printf("0x%x\n", 0x4500+0x0073+0x0000+0x4000+0x4011+0xb861+0xc0a8+0x0001+0xc0a8+0x00c7)'
//     0x2fffd
//     perl6 -e 'printf("0x%x\n", 0x2 + 0xfffd)'
//     0xffff
//
// Bash script
// (
//     function sum()
//     {
//         IDX=$1
//         WRD=$2
// 
//         CKSUM=$(/opt/Rakudo/2018.10/bin/perl6 -e 'printf("0x%x\n", '$CKSUM' + '$WRD')')
//         echo "CKSUM[$IDX] 1: $CKSUM"
//         OVRFL=$(/opt/Rakudo/2018.10/bin/perl6 -e 'printf("0x%x\n", '$CKSUM' +> 16)')
//         echo "OVRFL[$IDX]: $OVRFL"
//         CKSUM=$(/opt/Rakudo/2018.10/bin/perl6 -e 'printf("0x%x\n", ('$CKSUM' +& 0xffff) + '$OVRFL')')
//         echo "CKSUM[$IDX] 2: $CKSUM"
//         echo
//         
//         IDX=$((IDX + 1))
//     }
// 
//     CKSUM=0x0000;
//     echo "CKSUM[0]: $CKSUM"
//     echo
// 
//     sum 1  0x4500
//     sum 2  0x0073
//     sum 3  0x0000
//     sum 4  0x4000
//     sum 5  0x4011
//     sum 6  0x0000
//     sum 7  0xc0a8
//     sum 8  0x0001
//     sum 9  0xc0a8
//     sum 10 0x00c7
//     
//     CKSUM=$(/opt/Rakudo/2018.10/bin/perl6 -e 'printf("0x%x\n", '$CKSUM' +^ 0xffff )')
//     echo "CKSUM[11]: $CKSUM"
// )
//
// Bash script result
//   CKSUM[0]: 0x0000
//
//   CKSUM[1] 1: 0x4500
//   OVRFL[1]: 0x0
//   CKSUM[1] 2: 0x4500
//
//   CKSUM[2] 1: 0x4573
//   OVRFL[2]: 0x0
//   CKSUM[2] 2: 0x4573
//
//   CKSUM[3] 1: 0x4573
//   OVRFL[3]: 0x0
//   CKSUM[3] 2: 0x4573
//
//   CKSUM[4] 1: 0x8573
//   OVRFL[4]: 0x0
//   CKSUM[4] 2: 0x8573
//
//   CKSUM[5] 1: 0xc584
//   OVRFL[5]: 0x0
//   CKSUM[5] 2: 0xc584
//
//   CKSUM[6] 1: 0xc584
//   OVRFL[6]: 0x0
//   CKSUM[6] 2: 0xc584
//
//   CKSUM[7] 1: 0x1862c
//   OVRFL[7]: 0x1
//   CKSUM[7] 2: 0x862d
//
//   CKSUM[8] 1: 0x862e
//   OVRFL[8]: 0x0
//   CKSUM[8] 2: 0x862e
//
//   CKSUM[9] 1: 0x146d6
//   OVRFL[9]: 0x1
//   CKSUM[9] 2: 0x46d7
//
//   CKSUM[10] 1: 0x479e
//   OVRFL[10]: 0x0
//   CKSUM[10] 2: 0x479e
//
//   CKSUM[11]: 0xb861
//
