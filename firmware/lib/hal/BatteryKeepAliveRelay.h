// BatteryKeepAliveRelay.h - Copyright (C) 2024
// Copyright (C) 2024 Robert Read.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.



#ifndef BATTERY_KEEP_ALIVE_RELAY_H
#define BATTERY_KEEP_ALIVE_RELAY_H

#ifdef CTL_V_1_1

class BatteryKeepAliveRelay {
public:
  BatteryKeepAliveRelay();
  void turnOff();
  bool init();
};


#endif

#endif
