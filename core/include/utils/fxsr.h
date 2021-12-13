#pragma once

extern "C" void fxsave_if_supported(char* buffer);
extern "C" void fxrstor_if_supported(char* buffer);
