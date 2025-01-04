/*

  serialReport.cpp -- read temperatures sensors

  Copyright (C) 2023 Robert Read.

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

#include <serialReportTask.h>

bool SerialReportTask::_run()
{
  if (DEBUG_SERIAL_REPORT > 0) {
    CogCore::Debug<const char *>("Running SerialReport\n");
  }
  // we need to copy the errors from the Config here...
  for (int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++) {
    CogCore::Debug<const char *>("SerialReportTask Done with List\n");
    CogCore::DebugLn<int>(i);
    CogCore::DebugLn<long>((long) getConfig());
    CogCore::DebugLn<long>((long) &getConfig()->errors[i]);
    CogCore::DebugLn<int>(getConfig()->errors[i].fault_present);

    //getConfig()->report->errors[i] = getConfig()->errors[i].fault_present;
  }
  CogCore::Debug<const char *>("SerialReportTask Done with List\n");
  getConfig()->outputReport(getConfig()->report);
  return true;
}

bool SerialReportTask::_init()
{
   CogCore::Debug<const char *>("XXXX SerialReportTask _init()\n");
  return true;
}
SerialReportTask::SerialReportTask() {
}
