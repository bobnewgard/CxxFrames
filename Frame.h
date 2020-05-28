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

#ifndef _FRAME_RAW_H_
    #define _FRAME_RAW_H_

    #include <cstdint>
    #include <string>
    #include <vector>
    #include <array>
    #include <pcap.h>

    namespace Frames
    {
        typedef std::vector<uint8_t> BVec;
        typedef std::vector<uint8_t>::iterator BVecIter;
        typedef struct bpf_program pcap_bpf;

        struct item
        {
            bool    valid;
            BVec    bytes;
        };

        class Frame
        {
            private:
                item         frame;
                bool         iter_idle;
                BVecIter     iter_pos;
                char         nic_errbuf[PCAP_ERRBUF_SIZE];
                pcap_t      *nic_handle;
                pcap_bpf     nic_bpf;

            public:
                Frame(void);
                virtual ~Frame(void);

                void give_frame(BVec &&arg_bytes);
                void copy_frame(BVec &arg_bytes);
                void take_frame(BVec &arg_bytes);
                static BVec & to_bvec(BVec & arg_bvec, const uint64_t arg_uint, const unsigned int arg_len = 8);
                static BVec & to_bvec(BVec & arg_bvec, const uint32_t arg_uint, const unsigned int arg_len = 4);
                static BVec & to_bvec(BVec & arg_bvec, const uint16_t arg_uint, const unsigned int arg_len = 2);
                static BVec & to_bvec(BVec & arg_bvec, const uint8_t  arg_uint, const unsigned int arg_len = 1);
                bool nic_open(std::string arg_nic_name);
                void nic_close(void);
                bool nic_rx_filter(std::string arg_expr);
                bool nic_rx_frame(void);
                bool nic_tx_frame(void);
                bool get_frame_byte(uint8_t &arg_byte);
                std::string gist_bytes();
                std::string gist_bytes(BVec &arg_bytes);
                std::string gist_item(item &arg_item);
                virtual std::string gist(void);
        };
    }
#endif
