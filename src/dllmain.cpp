#include "dllmain.h"

DeclareHook(QueryPerfCounter, int64_t) {
  return TickLimiter::OffsetTime + (int64_t)((original() - TickLimiter::BaseTime) * TickLimiter::speed);
};

namespace TickLimiter
{
  double speed = 1.0;
  bool initialised = false;
  int64_t BaseTime = 0, OffsetTime = 0;

  void Setup(){
    BaseTime = OffsetTime = _Query_perf_counter();
    initialised = TryIntrinHookNoWarning(QueryPerfCounter, &_Query_perf_counter);
  }

  void SetTickRate(double tickRate) {
    if (initialised){
      OffsetTime = QueryPerfCounter_Hook_::_hook();
      BaseTime = QueryPerfCounter_Hook_::_original();
    }
    speed = tickRate/20.0;
  }
};

DeclareHook(CommandErrorHandler, void, string *a1) {
  string cmd = *a1;

  bool is_original = true;

  if (cmd.length() >= 4) {
    is_original = false;
    char cmd_tick[4] = {'t', 'i', 'c', 'k'};

    for (size_t i = 0; i < 4; i++)
      if (cmd_tick[i] != cmd[i]) {
        is_original = true;
        break;
      }

    if (!is_original) {
      string tickrate_s = "";
      bool is_arg = false;
      for (size_t i = 4; i < cmd.length(); i++){
        if(cmd[i] == 0x20){
          if(is_arg){
            custom_error("Syntax error: Unexpected \"{}\": at \"{}\"", cmd[i], cmd);
            return;
          }
          continue;
        }else{
          is_arg = true;
          if(!isdigit(cmd[i])){
            custom_error("Syntax error: Unexpected \"{}\": at \"{}\"", cmd[i], cmd);
            return;
          }
          tickrate_s+=cmd[i];
        }
      }

      if(tickrate_s.empty()){
        custom_info("Tick rate is {}", TickLimiter::speed*20);
        return;
      }

      double tickrate = stod(tickrate_s);

      if(tickrate == 0.0){
        custom_error("Tick rate cannot be less than 1.");
        return;
      }

      if(tickrate >= 50000)
        custom_info("At this tick rate the game may lag", tickrate);

      TickLimiter::SetTickRate(tickrate);

      custom_info("Tick rate set to {}", tickrate);
    }
  }

  if (is_original){
    original(a1);
    cout << clr_color;
  }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {

      //Enable Ansi color codes
      HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      SetConsoleMode(hOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

      custom_info("Injected!");

      TickLimiter::Setup();
      TrySigHook(CommandErrorHandler,
                  // 1.21.70.04
                  "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D 6C 24 C9 48 81 EC B0 00 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 2F 48 8B F1 45 33 FF 4C 89 7D F7 4C 8B 71 28 4D 85 F6");

      DisableThreadLibraryCalls(hModule);
      break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      Unhook(QueryPerfCounter);
      Unhook(CommandErrorHandler);
      break;
  }

  return TRUE;
}

//needed for direct import
__declspec(dllexport) void init() {}