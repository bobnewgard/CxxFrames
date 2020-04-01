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

        struct item
        {
            bool    valid;
            uint8_t mask;
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

            public:
                Frame(void);
                virtual ~Frame(void);

                void give_frame(BVec &&arg_bytes, const uint8_t arg_mask = 0xff);
                void copy_frame(BVec &arg_bytes, uint8_t &arg_mask);
                void take_frame(BVec &arg_bytes, uint8_t &arg_mask);
                bool nic_open(std::string arg_nic_name);
                void nic_close(void);
                bool nic_frame_tx(void);
                bool get_frame_byte(uint8_t &arg_byte);
                std::string gist_bytes(BVec &arg_bytes);
                std::string gist_item(item &arg_item);
                virtual std::string gist(void);
        };
    }
#endif

// Usage:
//     #include <utility>
//     #include <vector>
//     #include <Frame.h>
//     tr_frame->give_frame(move(tr_vec));
//     tr_frame->copy_frame(tr_vec, tr_mask);
//     tr_frame->take_frame(tr_vec, tr_mask);
