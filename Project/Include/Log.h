#pragma once

#include <ThirdParty/easylogging++/easylogging++.h>

#define xoFatal(msg)        LOG(FATAL) << msg;
#define xoFatalIf(cnd, msg) if(cnd) { LOG(FATAL) << msg;}


#if defined(GAME_FINAL)
#define xoLog(msg)
#define xoWarn(msg)
#define xoErr(msg)

#define xoLogIf(cnd, msg)
#define xoWarnIf(cnd, msg)
#define xoErrIf(cnd, msg)
#else
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