/*
  panel.h -- the front panel lights and switch

  Copyright 2023, Robert L. Read

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

#ifndef PANEL_H
#define PANEL_H


enum class LED_STATUS {
  OFF,
  BLINKING,
  STEADY_ON
};

class FrontPanel {
 public:
  void init();
  void setFaultLED(bool onOrOff);
  void setStatusLED(bool onOrOff);
  void setStatusLEDfromState(LED_STATUS ms);
};

#endif
