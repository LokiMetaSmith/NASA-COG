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

#include "log_recorder_task.h"


using namespace std;


namespace CogApp
{
  bool Log_Recorder_Task::_init()
  {
    CogCore::Debug<const char *>("LogRecorderTask init\n");
    _nextRecord = 0;
    _numRecords = 0;
    return true;
  }

  bool Log_Recorder_Task::_run()
  {
    if (DEBUG_LOG_RECORDER) {
      CogCore::Debug<const char *>("LogRecorderTask run: ");
      CogCore::Debug<int>(_nextRecord);
      CogCore::Debug<const char *>("\n");
    }
    getConfig()->report->timestamp = millis();

    const int m = getConfig()->MAX_RECORDS;
    if (_numRecords < m) _numRecords++;

    getConfig()->_log_entry[_nextRecord] = *getConfig()->report;
    getConfig()->_log_entry[_nextRecord].timestamp = millis();
    _nextRecord = (_nextRecord + 1) % m;

    return true;
  }
  void Log_Recorder_Task::dumpRecords() {

    if (DEBUG_LOG_RECORDER) {
      CogCore::Debug<const char *>("DUMPING LOG RECORDS: ");
      CogCore::Debug<int>(_numRecords);
      CogCore::Debug<const char *>("\n");
    }
    int firstRecord = _nextRecord - _numRecords;
    // now make sure positive!
    firstRecord = firstRecord % getConfig()->MAX_RECORDS;
    for(int i = 0; i < _numRecords; i++) {
      int j = (firstRecord + i) % getConfig()->MAX_RECORDS;
      MachineStatusReport msr_lre = getConfig()->_log_entry[j];
      getConfig()->outputReport(&msr_lre);
      oedcsNetworkTask->logReport(&msr_lre);
      core->ResetAllWatchdogs();
    }
    _nextRecord = 0;
    _numRecords = 0;
  }

}
