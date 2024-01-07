/*
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

#include "shutdown_button_task.h"

using namespace std;


bool ShutdownButtonTask::_init()
{
  CogCore::Debug<const char *>("ShutdownButtonTask init\n");
  return true;
}

bool ShutdownButtonTask::_run()
{
  bool shutdownButtonPushed = getHAL()->isShutDownButtonPushed();
  if (shutdownButtonPushed) {
    CogCore::Debug<const char *>("Shut Down Button Is Pushed -- enterring emergency shutdown.\n");
    CogCore::Debug<const char *>("Note: The proper logging of this is not yet implemented.\n");
    getConfig()->ms = EmergencyShutdown;
  }
}

COG_HAL* ShutdownButtonTask::getHAL() {
  return (COG_HAL *) (getConfig()->hal);
}
