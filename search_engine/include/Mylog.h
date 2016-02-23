#ifndef _MYLOG_H
#define _MYLOG_H

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/StringQueueAppender.hh>
#include <log4cpp/RollingFileAppender.hh>
//#include <log4cpp/NTEventLogAppender.hh>
//#include <log4cpp/SyslogAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/NDC.hh>

#include <iostream>
#include <string>
#include <sstream>
#include "log4cppEnum.h"

namespace wd{
class Mylog
{
private:
	Mylog(void);
	~Mylog(void);

	static Mylog *m_pMylog;
	PriorityLevel m_prioritylevel;
	AppenderStyle m_appenderstyle;
	LayoutStyle m_layoutstyle;
	log4cpp::Appender* m_pAppender1;
	log4cpp::Appender* m_pAppender2;
	log4cpp::PatternLayout* m_pLayout1;
	log4cpp::PatternLayout* m_pLayout2;
	log4cpp::Category* m_pCategory;
public:
	static Mylog* getInstance(void);
	static void destroyMylog();

	void Debug(const char *msg);
	void Info(const char *msg);
	void Notice(const char *msg);
	void Warn(const char *msg);
	void Error(const char *msg);
	void Crit(const char *msg);
	void Alert(const char *msg);
	void Fatal(const char *msg);
	void Emerg(const char *msg);
};

}//namespace wd




#define logDestroy() \
	wd::Mylog::destroyMylog();

#define logError(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Error(oss.str().c_str());\
}

#define logCrit(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Crit(oss.str().c_str());\
}

#define logAlert(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Alert(oss.str().c_str());\
}

#define logFatal(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Fatal(oss.str().c_str());\
}

#define logEmerg(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Emerg(oss.str().c_str());\
}

#define logWarn(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Warn(oss.str().c_str());\
}

#define logNotice(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Notice(oss.str().c_str());\
}

#define logInfo(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Info(oss.str().c_str());\
}

#define logDebug(msg) \
{	std::ostringstream oss;\
	oss << __FILE__ <<":" << __LINE__ << ":" << msg;\
	wd::Mylog::getInstance()->Debug(oss.str().c_str());\
}

#endif// _MYLOG_H
