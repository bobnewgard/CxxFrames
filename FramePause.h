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

/*
 * "IEEE Std 802.3-2008, Section 2", Annex 31B, MAC Control PAUSE operation
 */

#ifndef _FRAME_PAUSE_H_
    #define _FRAME_PAUSE_H_
    
    #include <FrameEth.h>

    namespace Frames
    {
        const unsigned PAUSE_MAC_CTRL = 0x0001;
        const unsigned PAUSE_PAD_SIZE = 42;
        const uint8_t  PAUSE_MCST[]   = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x01};

        class FramePause : public FrameEth
        {
            private:
                unsigned quanta;

            public:
                FramePause(void);
                virtual ~FramePause(void);

                void set_pause_param(unsigned arg_quanta);
                virtual void encapsulate(void);
                virtual std::string gist(void);
        };
    }
#endif
