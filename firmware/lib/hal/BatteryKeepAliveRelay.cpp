// BatteryKeepAliveRelay.cpp - Copyright (C) 2024
// Robert Read.

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


#ifdef CTL_V_1_1

#include "BatteryKeepAliveRelay.h"

#include <machine.h>
#include <debug.h>

BatteryKeepAliveRelay::BatteryKeepAliveRelay()  {
    init();
  };

// This would be clearer in the the .h!! or in the machine hal for the specific device
bool BatteryKeepAliveRelay::init() {
  pinMode(MachineConfig::RELAY_PIN, OUTPUT);
  digitalWrite(MachineConfig::RELAY_PIN,HIGH);
  CogCore::Debug<const char *>("Relay PIN Set HIGH ");
  CogCore::DebugLn<int>(MachineConfig::RELAY_PIN);
  return true;
}

void BatteryKeepAliveRelay::turnOff() {
  digitalWrite(MachineConfig::RELAY_PIN, LOW);
  delay(100);
  CogCore::Debug<const char *>("Aborting Now!");
  delay(100);
  abort();
}

#endif
