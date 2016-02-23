#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_pool.hpp"

namespace acl
{

/**
 * memcache �ͻ������ӳ��࣬���ุ��Ϊ connect_pool������ֻ��ʵ�ָ����е��麯��
 * create_connect ��ӵ�������ӳظ��� connect_pool �Ĺ��ܣ����⣬���ഴ��
 * �����Ӷ����� memcache ���������ڵ��� connect_pool::peek ʱ����
 * �ı��� http_request �࣬��������Ҫ�� peek ���ص������ǿ��תΪ memcache
 * ����󣬱����ʹ�� memcache �������й��ܣ����� memcache ��Ϊ
 * connect_client ������
 */
class ACL_CPP_API memcache_pool : public connect_pool
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} ����˵�ַ����ʽ��ip:port
	 * @param count {size_t} ���ӳص�������������ƣ�����ֵΪ 0 ʱ��û������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 */
	memcache_pool(const char* addr, size_t count, size_t idx = 0);
	~memcache_pool();

protected:
	// ���ി�麯��
	virtual connect_client* create_connect();
};

} // namespace acl