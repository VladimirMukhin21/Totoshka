#pragma once

class Command {
public:
  static const byte NO_COMMAND = 0;
  static const byte TELEMETRY_START = 1;
  static const byte TELEMETRY_STOP = 2;
  static const byte DRIVE = 3;  
  static const byte COUNT_ARGS = 2;

  byte command = NO_COMMAND;
  byte args[COUNT_ARGS] = { 0, 0 };
  byte countReadedArgs = 0;

  void readPart();
  bool isCompleted();
  void clear();
};

void Command::readPart() {
  int readed = Serial.read();
  if (readed == -1) {
    return;
  }

  if (command == NO_COMMAND) {
    if ((readed == TELEMETRY_START)
        || (readed == TELEMETRY_STOP)
        || (readed == DRIVE)) {
      clear();
      command = readed;
    }
  }
  else {
    args[countReadedArgs] = readed;
    countReadedArgs++;
  }
}

bool Command::isCompleted() {
  return countReadedArgs == COUNT_ARGS;
}

void Command::clear() {
  command = NO_COMMAND;
  args[0] = 0;
  args[1] = 0;
  countReadedArgs = 0;
}