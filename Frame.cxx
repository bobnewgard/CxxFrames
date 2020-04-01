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

#include <Frame.h>
#include <sstream>
#include <iomanip>
#include <utility>

namespace Frames
{
    using namespace std;

    Frame::Frame(void)
    {
        this->iter_idle   = true;
        this->frame.valid = false;
        this->frame.mask  = 0xff;
        this->frame.bytes.clear();
    }

    Frame::~Frame(void) { }

    void Frame::give_frame(BVec &&arg_bytes, const uint8_t arg_mask)
    {
        this->frame.bytes = arg_bytes;
        this->frame.mask  = arg_mask;
        this->frame.valid = true;
        arg_bytes = BVec();
    }

    void Frame::copy_frame(BVec &arg_bytes, uint8_t &arg_mask)
    {
        arg_mask = this->frame.mask;

        arg_bytes.clear();
        for (auto it = this->frame.bytes.begin() ; it != this->frame.bytes.end() ; ++it)
        {
            arg_bytes.push_back(*it);
        }
    }

    void Frame::take_frame(BVec &arg_bytes, uint8_t &arg_mask)
    {
        arg_mask          = this->frame.mask;
        arg_bytes         = move(this->frame.bytes);
        this->frame.valid = false;
        this->frame.mask  = 0xff;

        this->frame.bytes.clear();
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
            << ",mask:0x" << setfill('0') << setw(2) << hex << (int)arg_item.mask
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
