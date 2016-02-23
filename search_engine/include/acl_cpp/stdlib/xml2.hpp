#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/xml.hpp"

struct ACL_XML2;
struct ACL_XML2_NODE;
struct ACL_XML2_ATTR;

namespace acl {

class xml2;
class xml2_node;

class ACL_CPP_API xml2_attr : public xml_attr
{
public:
	/**
	 * @override
	 * @return {const char*}
	 */
	const char* get_name() const;

	/**
	 * @override
	 * @return {const char*}
	 */
	const char* get_value() const;

protected:
	friend class xml2_node;

	xml2_attr(xml_node* node, ACL_XML2_ATTR* attr);
	~xml2_attr(void) {}

private:
	ACL_XML2_ATTR* attr_;
};

class ACL_CPP_API xml2_node : public xml_node
{
public:
	/**
	 * @override
	 */
	const char* tag_name(void) const;

	/**
	 * @override
	 */
	const char* id(void) const;

	/**
	 * @override
	 */
	const char* text(void) const;

	/**
	 * @override
	 */
	const char* attr_value(const char* name) const;

	/**
	 * @override
	 */
	const xml_attr* first_attr(void) const;

	/**
	 * @override
	 */
	const xml_attr* next_attr(void) const;

	/**
	 * @override
	 */
	xml_node& add_attr(const char* name, const char* value);

	/**
	 * @override
	 */
	xml_node& set_text(const char* str);

	/**
	 * @override
	 */
	xml_node& add_child(xml_node* child, bool return_child = false);

	/**
	 * @override
	 */
	int detach(void);

	/**
	 * @override
	 */
	xml_node& get_parent(void) const;

	/**
	 * @override
	 */
	xml_node* first_child(void);

	/**
	 * @override
	 */
	xml_node* next_child(void);

	/**
	 * @override
	 */
	int depth(void) const;

	/**
	 * @override
	 * @return {bool}
	 */
	bool is_root(void) const;

	/**
	 * @override
	 */
	int children_count(void) const;

public:
	void set_xml2_node(ACL_XML2_NODE* node);

	ACL_XML2_NODE* get_xml_node(void) const;
	xml_node& set_parent(xml_node* node);

protected:
	friend class xml2;

	xml2_node(xml* xml_ptr, ACL_XML2_NODE* node);
	~xml2_node(void);

private:
	ACL_XML2_NODE *node_;
	ACL_ITER* child_iter_;
	ACL_ITER* attr_iter_;

	xml_node* parent_;
	xml2_node* parent_internal_;
};

class fstream;

class ACL_CPP_API xml2 : public xml
{
public:
	/**
	 * ���캯����ʹ xml ������������ָ�����ڴ����
	 * @param addr {char*} �ڴ�ӳ��ĵ�ַ���ڴ��ַ�����������в���������
	 *  ���洢�ڸ��ڴ�������
	 * @param size {size_t} addr �ڴ�ռ��С�����������������ݳ�����ֵ
	 *  ʱ���������ֹͣ�����Ա��뱣֤�ÿռ��С�㹻��
	 * @param data {const char*} �ǿ�ʱ�Զ����ý�������
	 */
	xml2(char* addr, size_t size, const char* data = NULL);

	/**
	 * ���캯����ʹ xml ������������ָ���ڴ�ӳ���ļ���
	 * @param filepath {const char*} �ڴ�ӳ���ļ���
	 * @param size {size_t} �ڴ�ӳ���ļ�������С�����ڴ�ӳ���ļ����Զ�
	 *  ����ʱ��Ӧ�����˴�С
	 * @param data {const char*} �ǿ�ʱ�Զ����ý�������
	 * @param block {size_t} �ڴ�ӳ���ļ�����ʱ�ĳ�ʼ��С
	 * @param keep_open {bool} �򿪵��ڴ�ӳ���ļ��ľ���Ƿ�һֱ���ִ�
	 *  ״̬��������������ļ�����ʱ��Ч��
	 */
	xml2(const char* filepath, size_t size, const char* data = NULL,
		size_t block = 8192, bool keep_open = true);

	/**
	 * ���캯����ʹ xml ������������ָ���ڴ�ӳ���ļ���
	 * @param fp {fstream&} �ڴ�ӳ���ļ���������ļ����Ӧ�ڱ� xml ����
	 *  �ͷ�ǰһֱ���ִ�״̬���� fp ������ xml �����ͷź�ſ��Թر�
	 * @param size {size_t} �ڴ�ӳ���ļ�������С�����ڴ�ӳ���ļ����Զ�
	 *  ����ʱ��Ӧ�����˴�С
	 * @param data {const char*} �ǿ�ʱ�Զ����ý�������
	 * @param block {size_t} �ڴ�ӳ���ļ�����ʱ�ĳ�ʼ��С
	 */
	xml2(fstream& fp, size_t size, const char* data = NULL,
		size_t block = 8192);

	/**
	 * ���캯����ʹ xml ������������ָ���ڴ�ӳ���ļ���
	 * @param fd {ACL_FILE_HANDLE} �ڴ�ӳ���ļ���������ļ����Ӧ�ڱ� xml
	 *  �����ͷ�ǰһֱ���ִ�״̬���� fp ������ xml �����ͷź�ſ��Թر�
	 * @param size {size_t} �ڴ�ӳ���ļ�������С�����ڴ�ӳ���ļ����Զ�
	 *  ����ʱ��Ӧ�����˴�С
	 * @param data {const char*} �ǿ�ʱ�Զ����ý�������
	 * @param block {size_t} �ڴ�ӳ���ļ�����ʱ�ĳ�ʼ��С
	 */
#if defined(_WIN32) || defined(_WIN64)
	xml2(void* fd,  size_t size, const char* data = NULL,
		size_t block = 8192);
#else
	xml2(int fd, size_t size, const char* data = NULL,
		size_t block = 8192);
#endif

	~xml2(void);

	/**
	 * @override
	 */
	xml& ignore_slash(bool on);

	/**
	 * @override
	 */
	xml& xml_decode(bool on);

	/**
	 * @override
	 */
	void update(const char* data);

	/**
	 * @override
	 */
	void reset(void);

	/**
	 * @override
	 */
	const string& getText(void);

	/**
	 * @override
	 */
	const std::vector<xml_node*>&
		getElementsByTagName(const char* tag) const;

	/**
	 * @override
	 */
	xml_node* getFirstElementByTag(const char* tag) const;

	/**
	 * @override
	 */
	const std::vector<xml_node*>&
		getElementsByTags(const char* tags) const;

	/**
	 * @override
	 */
	xml_node* getFirstElementByTags(const char* tags) const;

	/**
	 * @override
	 */
	const std::vector<xml_node*>&
		getElementsByName(const char* value) const;

	/**
	 * @override
	 */
	const std::vector<xml_node*>& getElementsByAttr(
		const char* name, const char* value) const;
			
	/**
	 * @override
	 */
	xml_node* getElementById(const char* id) const;

	/**
	 * @override
	 */
	xml_node& create_node(const char* tag, const char* text = NULL);

	/**
	 * @override
	 */
	xml_node& get_root(void);

	/**
	 * @override
	 */
	xml_node* first_node(void);

	/**
	 * @override
	 */
	xml_node* next_node(void);

	/**
	 * @override
	 */
	void build_xml(string& out) const;

	/**
	 * @override
	 */
	const char* to_string(size_t* len = NULL) const;

public:
	ACL_XML2* get_xml2(void) const
	{
		return xml_;
	}

private:
	ACL_XML2* xml_;
	ACL_ITER* iter_;
	xml2_node* root_;
};

} // namespace acl
