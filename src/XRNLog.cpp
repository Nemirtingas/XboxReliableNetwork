#include "XRNPrivate.h"

static void XRNLogDefaultFunction(const char* message)
{
}

static void(*s_logFunction)(const char*) = XRNLogDefaultFunction;

void XRNLog(const char* format, ...)
{
	char logBuffer[2048]{};
	va_list args;
	va_start(args, format);
	vsprintf_s(logBuffer, sizeof(logBuffer) - 1, format, args);
	va_end(args);

	s_logFunction(logBuffer);
}

void XRNSetLogFunction(void(*logFunction)(const char*))
{
	if (logFunction == nullptr)
		s_logFunction = XRNLogDefaultFunction;
	else
		s_logFunction = logFunction;
}