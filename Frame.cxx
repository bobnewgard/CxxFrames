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

#include <Frame.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <cstring>
#include <string>

namespace Frames
{
    using namespace std;

    Frame::Frame(void)
    {
        this->iter_idle   = true;
        this->frame.valid = false;
        this->frame.bytes.clear();
    }

    Frame::~Frame(void) { }

    void Frame::give_frame(BVec &&arg_bytes)
    {
        this->frame.bytes = arg_bytes;
        this->frame.valid = true;
        arg_bytes         = BVec();
    }

    void Frame::copy_frame(BVec &arg_bytes)
    {
        arg_bytes.clear();

        for (auto it = this->frame.bytes.begin() ; it != this->frame.bytes.end() ; ++it)
        {
            arg_bytes.push_back(*it);
        }
    }

    void Frame::take_frame(BVec &arg_bytes)
    {
        arg_bytes         = move(this->frame.bytes);
        this->frame.valid = false;

        this->frame.bytes.clear();
    }

    BVec & Frame::to_bvec(BVec & arg_bvec, const uint64_t arg_uint, const unsigned int arg_len)
    {
        for (unsigned int i = 0 ; i < 8 ; i++)
        {
            if ((8-i) > arg_len) continue;

            switch (i)
            {
                case 0 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x38) & 0xff)); break;
                case 1 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x30) & 0xff)); break;
                case 2 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x28) & 0xff)); break;
                case 3 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x20) & 0xff)); break;
                case 4 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x18) & 0xff)); break;
                case 5 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x10) & 0xff)); break;
                case 6 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x08) & 0xff)); break;
                case 7 : arg_bvec.push_back((uint8_t)((arg_uint >> 0x00) & 0xff)); break;
            }
        }

        return arg_bvec;
    }

    BVec & Frame::to_bvec(BVec & arg_bvec, const uint32_t arg_uint, const unsigned int arg_len)
    {
        uint64_t     tmp_uint = (uint64_t) arg_uint;
        unsigned int tmp_len  = (arg_len > 4) ? 4 : arg_len;

        return to_bvec(arg_bvec, tmp_uint, tmp_len);
    }

    BVec & Frame::to_bvec(BVec & arg_bvec, const uint16_t arg_uint, const unsigned int arg_len)
    {
        uint64_t     tmp_uint = (uint64_t) arg_uint;
        unsigned int tmp_len  = (arg_len > 2) ? 2 : arg_len;

        return to_bvec(arg_bvec, tmp_uint, tmp_len);
    }

    BVec & Frame::to_bvec(BVec & arg_bvec, const uint8_t arg_uint, const unsigned int arg_len)
    {
        uint64_t     tmp_uint = (uint64_t) arg_uint;
        unsigned int tmp_len  = (arg_len > 1) ? 1 : arg_len;

        return to_bvec(arg_bvec, tmp_uint, tmp_len);
    }

    bool Frame::nic_open(std::string arg_nic_name)
    {
        #ifndef PCAP_DISABLE
            this->nic_errbuf[0] = '\0';
            this->nic_handle    = pcap_open_live(arg_nic_name.c_str(), 65536, 0, 100, this->nic_errbuf);

            if (this->nic_handle == NULL)
            {
                cerr << "Frame::nic_open(): failure opening device " << this->nic_errbuf << endl << flush;
                return false;
            }

            if (strlen(this->nic_errbuf) != 0)
            {
                cerr << "Frame::nic_open(): warning opening device " << this->nic_errbuf << endl << flush;
                return false;
            }

            if (false)
            {
                cerr << "Frame::nic_open(): okay" << endl << flush;
            }
        #endif

        return true;
    }

    void Frame::nic_close(void)
    {
        #ifndef PCAP_DISABLE
            pcap_close(this->nic_handle);
        #endif
    }

    bool Frame::nic_rx_filter(string arg_expr)
    {
        int      ret;

        #ifndef PCAP_DISABLE
            ret = pcap_compile(this->nic_handle, &this->nic_bpf,arg_expr. c_str(), 0, PCAP_NETMASK_UNKNOWN);

            if (ret < 0)
            {
                pcap_perror(this->nic_handle, "Frame::nic_rx_filter(): pcap_compile failure");
                return false;
            }

            ret = pcap_setfilter(this->nic_handle, &this->nic_bpf);

            if (ret < 0)
            {
                pcap_perror(this->nic_handle, "Frame::nic_rx_filter(): pcap_setfilter failure");
                return false;
            }
        #endif

        return true;
    }

    bool Frame::nic_rx_frame(void)
    {
        struct pcap_pkthdr *hdr;
        const uint8_t      *pkt;
        int                 ret;
        uint32_t            len_min;

        #ifndef PCAP_DISABLE
            ret = pcap_next_ex(this->nic_handle, &hdr, &pkt);

            if (ret == 0)
            {
                cerr << "Frame::nic_rx_frame(): timeout" << endl << flush;
                return false;
            }
            else if (ret == -1)
            {
                pcap_perror(this->nic_handle, "Frame::nic_rx_frame(): failure");
                return false;
            }
            else if (ret == -2)
            {
                cerr << "Frame::nic_rx_frame(): savefile EOF" << endl << flush;
                return false;
            }

            if (hdr->caplen < hdr->len)
            {
                    len_min = hdr->caplen;
            }
            else
            {
                    len_min = hdr->len;
            }

            if (false)
            {
                cerr << "Frame::nic_rx_frame(): caplen is  " << hdr->caplen << endl << flush;
                cerr << "Frame::nic_rx_frame(): len is     " << hdr->len << endl << flush;
                cerr << "Frame::nic_rx_frame(): len_min is " << len_min << endl << flush;
            }

            for (uint32_t i = 0 ; i < len_min ; i++)
            {
                this->frame.bytes.push_back(pkt[i]);
            }

            this->frame.valid = true;
        #endif

        return true;
    }

    bool Frame::nic_tx_frame(void)
    {
        int ret;

        #ifndef PCAP_DISABLE
            ret = pcap_inject(this->nic_handle, frame.bytes.data(), frame.bytes.size());

            if (ret < 0)
            {
                pcap_perror(this->nic_handle, "Frame::nic_tx_frame(): failure");
                return false;
            }

            if (false)
            {
                cerr << "Frame::nic_tx_frame(): okay" << endl << flush;
            }

            this->frame.valid = false;
            this->frame.bytes.clear();
        #endif

        return true;
    }

    bool Frame::get_frame_byte(uint8_t &arg_byte)
    {
        if (not this->frame.valid)
        {
            this->iter_idle = true;
            return false;
        }

        if (this->iter_idle)
        {
            this->iter_idle = false;
            this->iter_pos  = this->frame.bytes.begin();
        }
        else
        {
            ++this->iter_pos;
        }

        if (this->iter_pos == this->frame.bytes.end())
        {
            this->frame.valid = false;
            this->iter_idle   = true;
            return false;
        }

        arg_byte = (*this->iter_pos);
        return true;
    }

    string Frame::gist_bytes(void)
    {
        return this->gist_bytes(frame.bytes);
    }

    string Frame::gist_bytes(BVec &arg_bytes)
    {
        bool   tmp = false;
        stringstream ss;

        ss  << "bytes:{";

        if (arg_bytes.empty())
        {
            ss  << "empty";
        }
        else
        {
            for (auto it = arg_bytes.begin() ; it != arg_bytes.end() ; ++it)
            {
                if (tmp == true) ss << ",";
                ss  << "0x"<< setfill('0') << setw(2) << hex << (int)(*it);
                tmp = true;
            }
        }

        ss  << "}";

        return ss.str();
    }

    string Frame::gist_item(item& arg_item)
    {
        stringstream ss;

        ss  << "{valid:" << boolalpha << arg_item.valid
            << ","+this->gist_bytes(arg_item.bytes)
            << "}";

        return ss.str();
    }

    string Frame::gist(void)
    {
        stringstream ss;

        ss << "{frame:" << this->gist_item(this->frame) << "}";

        return ss.str();
    }
}
