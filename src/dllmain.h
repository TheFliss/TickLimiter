#pragma once

#include "detours_hook_api.h"
#include "pch.h"

#define clr_color "\033[97m"
#define red_color "\033[91m"

template<typename... Args>
void custom_error(std::string_view fmt, Args&&... args){
  std::string fm = std::vformat(fmt, std::make_format_args(args...));
  printf(xorstr_(red_color"[Tick Limiter ERROR] %s\n"clr_color),fm.c_str());
}

template<typename... Args>
void custom_info(std::string_view fmt, Args&&... args){
  std::string fm = std::vformat(fmt, std::make_format_args(args...));
  printf(xorstr_(clr_color"[Tick Limiter INFO] %s\n"clr_color),fm.c_str());
}

namespace TickLimiter
{
	extern double speed;
	extern bool initialised;
	extern int64_t BaseTime, OffsetTime;
  void Setup();
  void SetTickRate(double relSpeed);
};