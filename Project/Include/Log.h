#pragma once

#include <ThirdParty/easylogging++/easylogging++.h>

#if defined(GAME_FINAL)
#define xoFatal(msg)        { LOG(FATAL) << msg; exit(-1); }
#define xoFatalIf(cnd, msg) if(cnd) { LOG(FATAL) << msg; exit(-1); }

#define xoLog(msg)
#define xoWarn(msg)
#define xoErr(msg)

#define xoLogIf(cnd, msg)
#define xoWarnIf(cnd, msg)
#define xoErrIf(cnd, msg)
#else
#define xoFatal(msg)        { LOG(FATAL) << msg; __debugbreak(); exit(-1); }
#define xoFatalIf(cnd, msg) if(cnd) { LOG(FATAL) << msg; __debugbreak(); exit(-1); }

#define xoLog(msg)      LOG(INFO) << msg;
#define xoWarn(msg)     LOG(WARN) << msg;
#define xoErr(msg)      LOG(ERROR) << msg;

#define xoLogIf(cnd, msg)    if(cnd) { LOG(INFO) << msg; }
#define xoWarnIf(cnd, msg)   if(cnd) { LOG(WARN) << msg; }
#define xoErrIf(cnd, msg)    if(cnd) { LOG(ERROR) << msg;}
#endif

namespace XO {

class LogManager {
public:
    LogManager(const char* configPath);
    ~LogManager();
};

}