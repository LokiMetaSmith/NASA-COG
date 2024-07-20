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
    // Removing this guard
    if (DEBUG_LOG_RECORDER) {
      CogCore::Debug<const char *>("LogRecorderTask run: ");
      CogCore::Debug<int>(_nextRecord);
      CogCore::Debug<const char *>("\n");
    }
    getConfig()->report->timestamp = millis();

    const int m = getConfig()->MAX_RECORDS;
    if (_numRecords < m) _numRecords++;

    getConfig()->_log_entry[_nextRecord] = *getConfig()->report;
    // Copy errors here...
    for (int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++) {
      getConfig()->_log_entry[_nextRecord].errors[i]
        = getConfig()->errors[i].fault_present;
    }
    getConfig()->_log_entry[_nextRecord].timestamp = millis();
    _nextRecord = (_nextRecord + 1) % m;

    if (currently_dumping) {
      dumpRecords();
    }

    return true;
  }
  void Log_Recorder_Task::dumpRecords() {
    currently_dumping = true;
    int recordCount = getConfig()->MAX_RECORDS;
    unsigned long myStartTime = millis();
    int minr = min(_numRecords,MAX_RECORDS_TO_DUMP_AT_ONCE);
    if (DEBUG_LOG_RECORDER) {
      CogCore::Debug<unsigned long>(myStartTime);
      CogCore::Debug<const char *>(" STARTING DUMPING LOG RECORDS: ");
      CogCore::Debug<int>(minr);
      CogCore::Debug<const char *>("\n");
    }
    int firstRecord = _nextRecord - _numRecords;
    // now make sure positive!
    firstRecord = firstRecord % recordCount;

    // in order to limit the time of dumping to only 20 seconds,
    // I am limiting the amount that we do within a single task.
    // We have measured each record as taking about 93 ms.
    for(int i = 0; i < minr; i++) {
      int j = (firstRecord + i) % recordCount;
      MachineStatusReport msr_lre = getConfig()->_log_entry[j];
      unsigned long spot_time0 = millis();
      getConfig()->outputReport(&msr_lre);
      unsigned long spot_time1 = millis();
      oedcsNetworkTask->logReport(&msr_lre);
      unsigned long spot_time2 = millis();
      core->ResetAllWatchdogs();
      unsigned long spot_time3 = millis();
      if (DEBUG_LOG_RECORDER) {
        CogCore::Debug<const char *>("PER RECORD TIMES");
        CogCore::Debug<const char *>("outputReport: ");
        CogCore::DebugLn<long>(spot_time1 - spot_time0);
        CogCore::Debug<const char *>("logReport: ");
        CogCore::DebugLn<long>(spot_time2 - spot_time1);
        CogCore::Debug<const char *>("watchDogs: ");
        CogCore::DebugLn<long>(spot_time3 - spot_time2);
      }
    }
    _numRecords -= minr;
    if (0 == _numRecords) {
      currently_dumping = false;
    }

    unsigned long myFinishTime = millis();

    if (DEBUG_LOG_RECORDER) {
      CogCore::Debug<unsigned long>(myFinishTime);
      CogCore::Debug<const char *>(" FINISHED DUMPING LOG RECORDS.\n");
      CogCore::Debug<const char *>("Time to dump: ");
      CogCore::Debug<unsigned long>(myFinishTime - myStartTime);
      CogCore::DebugLn<const char *>(" mSeconds.\n ");
      CogCore::Debug<const char *>(" Records: ");
      CogCore::DebugLn<int>(minr);
      CogCore::Debug<const char *>(" Remaining: ");
      CogCore::DebugLn<int>(_numRecords);
    }
  }

}
