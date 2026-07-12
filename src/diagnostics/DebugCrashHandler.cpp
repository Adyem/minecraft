#include "../../src/diagnostics/DebugCrashHandler.hpp"

DebugCrashHandler::DebugCrashHandler()
{
}

DebugCrashHandler::DebugCrashHandler(const DebugCrashHandler &other)
{
    *this = other;
}

DebugCrashHandler::~DebugCrashHandler()
{
}

DebugCrashHandler &DebugCrashHandler::operator=(const DebugCrashHandler &other)
{
    (void)other;
    return (*this);
}

int DebugCrashHandler::enable()
{
#ifdef DEBUG
    int32_t error_code;

    error_code = dbg_enable_crash_stack_traces();
    if (error_code != FT_ERR_SUCCESS)
        return (ApplicationError::fail("debug crash stack trace setup", error_code));
#endif
    return (0);
}
