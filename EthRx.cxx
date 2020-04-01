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

const string SP = "\x20";

void usage(string arg_prog)
{
    stringstream ss;

    ss  << "usage:"+SP+arg_prog+SP;

    cerr << ss.str() << endl << flush;
    exit(1);
}

int main(int argc, char **argv)
{
    string prog = argv[0];

    if (argc < 2)
    {
        usage(prog);
    }

    exit(0);
}
