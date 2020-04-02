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
#include <sstream>
#include <string>
#include <Frame.h>
#include <FrameEth.h>

using namespace std;
using namespace Frames;

const string SP       = "\x20";
const string nic_name = "enx309c231c6847";

int main(int argc, char **argv)
{
    FrameEth *rx_frame = new FrameEth();
    bool      nic_ret;

    nic_ret = rx_frame->nic_open(nic_name);

    if (nic_ret == false)
    {
        cerr << "EthRx: nic_open() failure" << endl << flush;
        exit(1);
    }

    nic_ret = rx_frame->nic_rx_filter("not ip6");

    if (nic_ret == false)
    {
        cerr << "EthRx: nic_rx_filter() failure" << endl << flush;
        exit(1);
    }

    nic_ret = rx_frame->nic_rx_frame();

    if (nic_ret == false)
    {
        cerr << "EthRx: nic_rx_frame() failure" << endl << flush;
        exit(1);
    }

    rx_frame->nic_close();

    cerr << "FrameEth:"+rx_frame->gist() << endl << flush;

    exit(0);
}
