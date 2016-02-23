#include "Mylog.h"
#include "Config.h"
#include <string>

namespace wd
{

Mylog* Mylog::m_pMylog = NULL;
Mylog* Mylog::getInstance(void)
{
	if(m_pMylog == NULL)
	{
		m_pMylog = new Mylog();
	}
	return m_pMylog;
}

void Mylog::destroyMylog(void)
{
	if(m_pMylog != NULL)
		delete m_pMylog;
}

Mylog::Mylog(void)
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(LOG_NAME);

	m_prioritylevel = NOTSET;
	m_appenderstyle = OSTREAM_APPENDER;
	m_layoutstyle = BASIC_LAYOUT;

	m_pAppender1 = new log4cpp::OstreamAppender("OStreamAppender", &std::cout);
	m_pAppender2 = new log4cpp::FileAppender("spellcheck", filename.c_str());

	m_pLayout1 = new log4cpp::PatternLayout();
	m_pLayout2 = new log4cpp::PatternLayout();
	m_pLayout1->setConversionPattern("%d: %p %c%x %m%n");
	m_pLayout2->setConversionPattern("%d: %p %c%x %m%n");
	m_pAppender1->setLayout(m_pLayout1);
	m_pAppender2->setLayout(m_pLayout2);
	
	m_pCategory = &log4cpp::Category::getRoot();
	m_pCategory->addAppender(m_pAppender1);
	m_pCategory->addAppender(m_pAppender2);
	m_pCategory->setPriority(log4cpp::Priority::DEBUG);
}


Mylog::~Mylog(void)
{
	log4cpp::Category::shutdown();
//	std::cout << "~Mylog() end" << std::endl;
}

void Mylog::Debug(const char *msg)
{
	m_pCategory->debug(msg);
}

void Mylog::Info(const char *msg)
{
	m_pCategory->info(msg);
}

void Mylog::Notice(const char *msg)
{	
	m_pCategory->notice(msg);
}

void Mylog::Warn(const char *msg)
{	
	m_pCategory->warn(msg);
}

void Mylog::Error(const char *msg)
{
	m_pCategory->error(msg);
}

void Mylog::Crit(const char *msg)
{
	m_pCategory->crit(msg);
}

void Mylog::Alert(const char *msg)
{
	m_pCategory->alert(msg);
}

void Mylog::Fatal(const char *msg)
{
	m_pCategory->fatal(msg);
}

void Mylog::Emerg(const char *msg)
{
	m_pCategory->emerg(msg);
}

}//namespace wd


