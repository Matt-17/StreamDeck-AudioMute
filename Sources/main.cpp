#include <StreamDeckSDK/ESDMain.h>
#include <StreamDeckSDK/ESDLogger.h>
#include "MyStreamDeckPlugin.h"

int main(int argc, const char** argv) {
  ESDLogger::Get()->SetWin32DebugPrefix("[SDAudioMute] ");
  return esd_main(argc, argv, new MyStreamDeckPlugin());
}
