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
#include <FrameArp.h>

namespace Frames
{
    using namespace std;

    FrameArp::FrameArp(void) : FrameEth()
    {
        this->spec[ ARP_OP   ] = {false, 0xff, BVec()};
        this->spec[ ARP_QMAC ] = {false, 0xff, BVec()};
        this->spec[ ARP_QIP  ] = {false, 0xff, BVec()};
        this->spec[ ARP_TMAC ] = {false, 0xff, BVec()};
        this->spec[ ARP_TIP  ] = {false, 0xff, BVec()};

        this->spec_op = ArpOp::OP_NIL;
    }

    FrameArp::~FrameArp(void) { }

    void FrameArp::set_arp_op(ArpOp arg_op)
    {
        this->spec[ARP_OP].bytes.push_back((uint8_t)arg_op);
        this->spec[ARP_OP].valid = true;

        this->spec_op = arg_op;
    }

    void FrameArp::set_arp_qmac(const BVec &arg_qmac)
    {
        if (arg_qmac.size() != 6)
        {
            cerr << "[ERR] set_arp_qmac(): mac parameter size is not 6 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ARP_QMAC].bytes.insert(this->spec[ARP_QMAC].bytes.end(), arg_qmac.begin(), arg_qmac.end());
        this->spec[ARP_QMAC].valid = true;
    }

    void FrameArp::set_arp_qip(const BVec &arg_qip)
    {
        if (arg_qip.size() != 4)
        {
            cerr << "[ERR] set_arp_qip(): IP parameter size is not 4 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ARP_QIP].bytes.insert(this->spec[ARP_QIP].bytes.end(), arg_qip.begin(), arg_qip.end());
        this->spec[ARP_QIP].valid = true;
    }

    void FrameArp::set_arp_tmac(const BVec &arg_tmac)
    {
        if (arg_tmac.size() != 6)
        {
            cerr << "[ERR] set_arp_tmac(): mac parameter size is not 6 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ARP_TMAC].bytes.insert(this->spec[ARP_TMAC].bytes.end(), arg_tmac.begin(), arg_tmac.end());
        this->spec[ARP_TMAC].valid = true;
    }

    void FrameArp::set_arp_tip(const BVec &arg_tip)
    {
        if (arg_tip.size() != 4)
        {
            cerr << "[ERR] set_arp_tip(): IP parameter size is not 4 bytes" << endl << flush;
            exit(1);
        }

        this->spec[ARP_TIP].bytes.insert(this->spec[ARP_TIP].bytes.end(), arg_tip.begin(), arg_tip.end());
        this->spec[ARP_TIP].valid = true;
    }

    void FrameArp::encapsulate(void)
    {
        bool   okay    = true;
        string errmsg  = "[ERR] encapsulate(): cannot encapsulate with an invalid";
        BVec   pr_type;
        BVec   ip_bcst;
        BVec   bytes;

        if (not this->spec[ ARP_OP   ].valid) okay = false;
        if (not this->spec[ ARP_QMAC ].valid) okay = false;
        if (not this->spec[ ARP_QIP  ].valid) okay = false;
        if (not this->spec[ ARP_TMAC ].valid) okay = false;
        if (not this->spec[ ARP_TIP  ].valid) okay = false;

        if (not this->spec[ ARP_OP   ].valid) cerr << errmsg << " op"   << endl << flush;
        if (not this->spec[ ARP_QMAC ].valid) cerr << errmsg << " qmac" << endl << flush;
        if (not this->spec[ ARP_QIP  ].valid) cerr << errmsg << " qip"  << endl << flush;
        if (not this->spec[ ARP_TMAC ].valid) cerr << errmsg << " tmac" << endl << flush;
        if (not this->spec[ ARP_TIP  ].valid) cerr << errmsg << " tip"  << endl << flush;

        if (not okay) exit(1);

        this->get_vec_eth_type(pr_type, EtherType::ETYP_IPV4);

        ip_bcst.push_back(MAC_BCST[0]);
        ip_bcst.push_back(MAC_BCST[1]);
        ip_bcst.push_back(MAC_BCST[2]);
        ip_bcst.push_back(MAC_BCST[3]);
        ip_bcst.push_back(MAC_BCST[4]);
        ip_bcst.push_back(MAC_BCST[5]);

        bytes.push_back(ARP_HW_TYP[0]);
        bytes.push_back(ARP_HW_TYP[1]);
        bytes.insert(bytes.end(), pr_type.begin(), pr_type.end());
        bytes.push_back(ARP_HW_SIZ);
        bytes.push_back(ARP_PR_SIZ);
        bytes.push_back(0x00);
        bytes.insert(bytes.end(), this->spec[ARP_OP].bytes.begin(), this->spec[ARP_OP].bytes.end());

        if (this->spec_op == ArpOp::OP_REQ)
        {
            bytes.insert(bytes.end(), this->spec[ ARP_QMAC ].bytes.begin(), this->spec[ ARP_QMAC ].bytes.end());
            bytes.insert(bytes.end(), this->spec[ ARP_QIP  ].bytes.begin(), this->spec[ ARP_QIP  ].bytes.end());
            bytes.push_back(0x00);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
            bytes.push_back(0x00);
            bytes.insert(bytes.end(), this->spec[ ARP_TIP  ].bytes.begin(), this->spec[ ARP_TIP  ].bytes.end());

            this->set_eth_dmac(ip_bcst);
            this->set_eth_smac(this->spec[ARP_QMAC].bytes);
        }
        else
        {
            bytes.insert(bytes.end(), this->spec[ ARP_TMAC ].bytes.begin(), this->spec[ ARP_TMAC ].bytes.end());
            bytes.insert(bytes.end(), this->spec[ ARP_TIP  ].bytes.begin(), this->spec[ ARP_TIP  ].bytes.end());
            bytes.insert(bytes.end(), this->spec[ ARP_QMAC ].bytes.begin(), this->spec[ ARP_QMAC ].bytes.end());
            bytes.insert(bytes.end(), this->spec[ ARP_QIP  ].bytes.begin(), this->spec[ ARP_QIP  ].bytes.end());

            this->set_eth_dmac(this->spec[ARP_QMAC].bytes);
            this->set_eth_smac(this->spec[ARP_TMAC].bytes);
        }

        for (unsigned int i = 0 ; i < ARP_PAD_SIZE ; ++i)
        {
            bytes.push_back(0x00);
        }

        this->set_eth_type(EtherType::ETYP_IPV4);
        this->set_eth_payload(bytes);
        FrameEth::encapsulate();
    }
    string FrameArp::gist(void)
    {
        stringstream ss;

        ss  << "{spec:"
            << "ARP_OP:"    << gist_item(this->spec[ARP_OP])
            << ",ARP_QMAC:" << gist_item(this->spec[ARP_QMAC])
            << ",ARP_QIP:"  << gist_item(this->spec[ARP_QIP])
            << ",ARP_TMAC:" << gist_item(this->spec[ARP_TMAC])
            << ",ARP_TIP:"  << gist_item(this->spec[ARP_TIP])
            << ",frame:"    << FrameEth::gist()
            << "}";

        return ss.str();
    }

}
