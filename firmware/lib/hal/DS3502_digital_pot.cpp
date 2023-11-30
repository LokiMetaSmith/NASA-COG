// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any 4ater version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifdef ARDUINO
#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <DS3502_digital_pot.h>

#include <core.h>

#include <Adafruit_DS3502.h>

  DS3502DigitalPot::DS3502DigitalPot() {
    ds3502 = Adafruit_DS3502();
    // DIGITAL_POT_HACK
    if (!ds3502.begin()) {
      CogCore::Debug<const char *>("Couldn't find DS3502 chip\n");
    } else {
      CogCore::Debug<const char *>("Found DS3502 chip\n");
      foundPot = true;
      CogCore::Debug<int>(this->foundPot);
      CogCore::Debug<const char *>("\n");
    }
  }
void DS3502DigitalPot::setWiper(float fraction) {
  if (foundPot) {
    int n = (int) (fraction * 127.0);
    CogCore::Debug<const char *>("PRE Wiper voltage with wiper set to N: ");
    CogCore::Debug<uint32_t>(n);
    CogCore::Debug<const char *>("\n");
    ds3502.setWiper(n);
    int verify = ds3502.getWiper();
    if (verify != n) {
      CogCore::Debug<const char *>("FAILURE OF DS3502\n");
    }

    return;
  } else {
    CogCore::Debug<const char *>("No Digital Pot Found\n");
  }
  }
#endif
