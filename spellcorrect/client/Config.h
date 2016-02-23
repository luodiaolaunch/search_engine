#ifndef WD_CONFIG_H
#define WD_CONFIG_H

#include "Mylog.h"
#include "Singleton.h"
#include "SafeFile.h"
#include "MyConf.h"
#include "Cache.h"

#include <string>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <iostream>

extern wd::Cache myCache;

namespace wd
{

const std::string LOG_NAME = "mylog";
const std::string CONFIG_FILE("../conf/server.conf");
const std::string DICT_NAME_CN("mydict_cn");
const std::string DICT_NAME_EN("mydict_en");
const std::string IP_NAME("myip");
const std::string PORT_NAME("myport");
const std::string INDEX_NAME_CN("myindex_cn");
const std::string INDEX_NAME_EN("myindex_en");
const std::string CACHE_NAME("mycache");
const std::string ORIGIN_NAME("myori");
const std::string DATA_DIR("mydata");
const std::string SRC_FILE_CN("cn");
const std::string SRC_FILE_EN("en");
const std::string DICT_FILE_CN("cn_");
const std::string DICT_FILE_EN("en_");
const std::string ALPHA_SEQ("abcdefghijklmnopqrstuvwxyz");

const std::string JIEBA_DICT("../inc/jieba/dict/jieba.dict.utf8");
const std::string JIEBA_HMM("../inc/jieba/dict/hmm_model.utf8");
const std::string JIEBA_USR_DICT("../inc/jieba/dict/user.dict.utf8");

const int EPOLL_WAIT = 10 * 60;
const int POLL_WAIT = 10 * 60 * 1000;
const int THREAD_NUM = 10;
const int TASK_NUM = 20;
const int RESERVE_VALUE = 1000;
const int RESULT_SIZE = 5;
const int CACHE_INTERVAL = 5;//10*60;
const int CACHE_VAL = 3;//10*60;


}//namespace

#endif
