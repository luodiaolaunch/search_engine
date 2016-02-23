#ifndef WD_CONFIG_H
#define WD_CONFIG_H

#include <string>
#include <set>
#include <fstream>
#include <sys/socket.h>
#include <signal.h>
#include "Mylog.h"
#include "Singleton.h"
#include "SafeFile.h"
#include "MyConf.h"
#include "TcpConnection.h"


namespace wd 
{
class ThreadPool;
using std::string;
using std::set;

#define CONTROLLEN CMSG_LEN(sizeof(int))

const string CONFIG_NAME("../conf/searchEngine.conf");
const string LOG_NAME = "mylog";
const string SOURCE_NAME("mysource");
const string RIPEPAGE_NAME("myripepage");
const string OFFSET_NAME("myoffset");
const string INDEX_NAME("myindex");
const string STOPLIST_NAME("mystoplist");
const string DATA_DIR("mydata");
const string IP_NAME("myip");
const string PORT_NAME("myport");
const string TITLE_TAG1("【 标  题 】");
const string TITLE_TAG2("【 标 题 】");
const string TITLE_TAG3("标 题");
const string TITLE_TAG4("标  题");
const string TEXT_TAG("【 正  文 】");
const string DOC_START("<doc>\n");
const string DOC_END("\n</doc>\n");
const string DOCID_START("<docid>\n");
const string DOCID_END("\n</docid>\n");
const string DOCURL_START("<docurl>\n");
const string DOCURL_END("\n</docurl>\n");
const string DOCTITLE_START("<doctitle>\n");
const string DOCTITLE_END("\n</doctitle>\n");
const string DOCCONTENT_START("<doccontent>\n");
const string DOCCONTENT_END("\n</doccontent>\n");

const string REDIS_ADD("127.0.0.1:6379");
const string REDIS_HASH_KEY("wangdao searchEngine");
const string REDIS_SEP_TAG(":");

const string JIEBA_DICT("../include/jieba/dict/jieba.dict.utf8");
const string JIEBA_HMM("../include/jieba/dict/hmm_model.utf8");
const string JIEBA_USR_DICT("../include/jieba/dict/user.dict.utf8");
const string JIEBA_STOP_WORDS("../include/jieba/dict/stop_words.utf8");

const int PAGE_LINE_MAX = 20;
const int RIPE_RESERVE_VAL = 1000;
const int TOPK_VAL = 10;
const int PAGEDUP_VAL = 7;
const int BUF_SIZE = 512;
const int EPOLL_WAIT = 10 * 60 * 1000;
const int PAGE_NUM = 3;
const int THREADS_NUM = 4;
const int BUF_NUM = 20;	
const int THREADS_MAX = 20;
const int THREADS_LOW = 2;
const int THREADS_HIGH = 6;

const int REDIS_CONN_TIMEOUT = 10;
const int REDIS_RW_TIMEOUT = 10;


void TraverseDirNonrecursive(const string &dirname, set<string> &set_filename);
void onConnection(const wd::TcpConnectionPtr &conn);
void onMessage(const wd::TcpConnectionPtr &conn, ThreadPool &processPool);
void onClose(const wd::TcpConnectionPtr &conn);
void sig_chld(int signo);
void sig_usr1(int signo, siginfo_t *siginfo, void *context);
int send_fd(int fd, int fd_to_send);
int recv_fd(int fd);
void RegisterSignal();
}//namespace wd

#endif
