#pragma once

void XRNLog(const char* format, ...);
void XRNSetLogFunction(void(*logFunction)(const char*));