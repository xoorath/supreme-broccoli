#include "../Include/Log.h"

INITIALIZE_EASYLOGGINGPP

namespace XO {

class LogDispatchCallback : public el::LogDispatchCallback
{
    el::base::DefaultLogBuilder m_Builder;

    void Output(const char* szFormat, ...) {
        char szBuff[2048];
        va_list arg;
        va_start(arg, szFormat);
        _vsnprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
        va_end(arg);
        OutputDebugStringA(szBuff);
    }

    virtual void handle(const el::LogDispatchData* handlePtr) override {
        // we print the file and number first so we can double click in the output window to navigate to the source. The new lines are just for readability.
        Output("%s(%d)\n%s", handlePtr->logMessage()->file().c_str(), handlePtr->logMessage()->line(), m_Builder.build(handlePtr->logMessage(), true).c_str());
    }
};


LogManager::LogManager(const char* configPath) {
    el::Loggers::reconfigureAllLoggers(el::Configurations(configPath));
    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Helpers::installLogDispatchCallback<LogDispatchCallback>("LogDispatchCallback");
}

LogManager::~LogManager() {
    el::Helpers::uninstallLogDispatchCallback<LogDispatchCallback>("LogDispatchCallback");
    el::base::elStorage = nullptr;
}

}