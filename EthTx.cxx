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

const string    SP       = "\x20";
const string    nic_name = "enx406c8f197c7d";
const BVec      tx_dmac  = {0x30,0x9c,0x23,0x1c,0x68,0x47}; // enx309c231c6847
const BVec      tx_smac  = {0x40,0x6c,0x8f,0x19,0x7c,0x7d}; // enx406c8f197c7d
const uint16_t  tx_etyp  = 0x1005;
const BVec      tx_pyld  = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x01, 0xF9, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x08
  };

int main(int argc, char **argv)
{
    FrameEth *tx_frame = new FrameEth();
    bool      nic_ret;

    cerr << "-- Start ------------------------" << endl << flush;

    tx_frame->set_eth_dmac(tx_dmac);
    tx_frame->set_eth_smac(tx_smac);
    tx_frame->set_eth_type(tx_etyp);
    tx_frame->set_eth_payload(tx_pyld);

    cerr << "FrameEth:"+tx_frame->gist() << endl << flush;

    tx_frame->encapsulate();

    cerr << "FrameEth:"+tx_frame->gist() << endl << flush;
    cerr << "-- End --" << endl << flush;

    nic_ret = tx_frame->nic_open(nic_name);

    if (nic_ret == false)
    {
        cerr << "EthTx: nic_open failure" << endl << flush;
        exit(1);
    }

    nic_ret = tx_frame->nic_tx_frame();

    if (nic_ret == false)
    {
        cerr << "EthTx: nic_tx_frame failure" << endl << flush;
        exit(1);
    }

    tx_frame->nic_close();

    exit(0);
}
