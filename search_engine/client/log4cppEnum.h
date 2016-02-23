#pragma once

namespace wd
{

enum PriorityLevel
{
	EMERG = 0, 
	FATAL = 0, 
	ALERT = 100,
	CRIT = 200,
	ERROR = 300,
	WARN = 400,
	NOTICE = 500,
	INFO = 600,
	DEBUG = 700,
	NOTSET = 800,
	PriorityLevel_Num = 10
};

enum AppenderStyle
{
	CONSOLE_APPENDER,
	OSTREAM_APPENDER,
	FILE_APPENDER,
	ROLLING_FILE_APPENDER,
	STRING_QUEUE_APPENDER,
	NTEVENTLOG_APPENDER,
	SYSLOG_APPENDER,
	LogStyle_Num
};

enum LayoutStyle
{
	BASIC_LAYOUT,
	SIMPLE_LAYOUT,
	PATTERN_LAYOUT,
	LayoutStyle_Num
};


}//namespace wd
