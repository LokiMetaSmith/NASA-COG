/*
Public Invention's COG Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef TIMER_H
#define TIMER_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <chrono>
#endif

namespace CogCore {

// TODO: This should be for Linux only
//using namespace std::chrono;

class Timer {
    private:
        uint32_t _msElapsed;
        uint32_t _msStart;
    public:
        static uint64_t TimeSinceEpochMs();
        void Init(uint32_t msStart);
        void Init();
        uint32_t Update();
        uint32_t GetElapsed();
        void Reset();
};

}

#endif
