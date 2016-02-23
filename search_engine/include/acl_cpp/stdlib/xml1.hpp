#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"
#include "acl_cpp/stdlib/xml.hpp"

struct ACL_XML;
struct ACL_XML_NODE;
struct ACL_XML_ATTR;

/**
 * �� ACL ���� XML ������ķ�װ������ C++ �û�ʹ�ã������̫ע���������أ�
 * ����ֱ��ʹ�ø��࣬����ڷ����ִ���ҷǳ�ע�����ܣ�����ֱ��ʹ�� ACL ���
 * XML ����������Ϊ����Ҳ�ǵ����� ACL ���е� XML �������̣������ж��ο���
 * ���̣����ܻ���΢Ӱ��һЩ���ܣ�������һ���Ӧ�����Ӱ����΢�������
 */

namespace acl {

class xml1;
class xml1_node;

class ACL_CPP_API xml1_attr : public xml_attr
{
public:
	/**
	 * @override ���������
	 * @return {const char*} ����������Զ���᷵�ؿ�ָ�룬����ֵ
	 *  �п���Ϊ "\0"
	 */
	const char* get_name(void) const;

	/**
	 * @override �������ֵ
	 * @return {const char*} ����ֵ����Զ���᷵�ؿ�ָ�룬����ֵ
	 *  �п���Ϊ "\0"
	 */
	const char* get_value(void) const;

protected:
	friend class xml1_node;

	xml1_attr(xml_node* node, ACL_XML_ATTR* attr);
	~xml1_attr(void) {}

private:
	ACL_XML_ATTR* attr_;
};

class ACL_CPP_API xml1_node : public xml_node
{
public:
	/**
	 * @override ȡ�ñ� XML �ڵ�ı�ǩ��
	 * @return {const char*} ���� XML �ڵ��ǩ����������ؿգ���˵��
	 *  �����ڱ�ǩ xxxx���Է���һ����������Ҫ�жϷ���ֵ
	 */
	const char* tag_name(void) const;

	/**
	 * @override ����� XML �ڵ�� ID �����Բ����ڣ��򷵻ؿ�ָ��
	 * @return {const char*} �� ID ���Դ���ʱ���ض�Ӧ��ֵ�����򷵻ؿ�
	 */
	const char* id(void) const;

	/**
	 * @override ���ظ� XML �ڵ����������
	 * @return {const char*} ���ؿ�˵��û����������
	 */
	const char* text(void) const;

	/**
	 * @override ���ظ� XML �ڵ��ĳ������ֵ
	 * @param name {const char*} ������
	 * @return {const char*} ����ֵ��������ؿ���˵�������Բ�����
	 */
	const char* attr_value(const char* name) const;

	/**
	 * @override �����ڵ����������ʱ����Ҫ���ô˺�������õ�һ�����Զ���
	 * @return {const xml_attr*} ���ص�һ�����Զ�����Ϊ�գ����ʾ
	 *  �ýڵ�û������
	 */
	const xml_attr* first_attr(void) const;

	/**
	 * @override �����ڵ����������ʱ�����ñ����������һ�����Զ���
	 * @return {const xml_attr*} ������һ�����Զ�����Ϊ�գ����ʾ
	 *  �������
	 */
	const xml_attr* next_attr(void) const;

	/**
	 * @override ��� XML �ڵ�����
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {xml_node&}
	 */
	xml_node& add_attr(const char* name, const char* value);

	/**
	 * @override ���� xml �ڵ���ı�����
	 * @param str {const char*} �ַ�������
	 * @return {xml_node&}
	 */
	xml_node& set_text(const char* str);

	/**
	 * @override ���� xml �ڵ���� xml_node �ӽڵ����
	 * @param child {xml_node*} �ӽڵ����
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽڵ������
	 * @return {xml_node&} return_child Ϊ true �����ӽڵ�����ã�
	 *  ���򷵻ر� xml �ڵ�����
	 */
	xml_node& add_child(xml_node* child, bool return_child = false);

	/**
	 * @override �����ڵ㼰���ӽڵ�� xml ���з��룬���ڴ潫�� xml
	 *  ����ͳһ�ͷ�
	 * @return {int} ���ر��ͷŵĽڵ����
	 */
	int detach(void);

	/**
	 * @override ��ñ��ڵ�ĸ����ڵ���������
	 * @return {xml_node&}
	 */
	xml_node& get_parent(void) const;

	/**
	 * @override ��ñ��ڵ��һ���ӽڵ㣬�����ӽڵ�ʱ�����ȵ��ô˺���
	 * @return {xml_node*} ���ؿձ�ʾû���ӽڵ�
	 */
	xml_node* first_child(void);

	/**
	 * @override ��ñ��ڵ����һ���ӽڵ�
	 * @return {xml_node*} ���ؿձ�ʾ�������̽���
	 */
	xml_node* next_child(void);

	/**
	 * @override ���ظ� XML �ڵ������� XML ���е����
	 * @return {int}
	 */
	int depth(void) const;

	/**
	 * @override
	 * @return {bool}
	 */
	bool is_root(void) const;

	/**
	 * @override ���ظ� xml �ڵ����һ���ӽڵ�ĸ���
	 * @return {int} ��Զ >= 0
	 */
	int children_count(void) const;

public:
	/**
	 * ȡ����Ӧ�� ACL ���е� XML �ڵ����
	 * @return {ACL_XML_NODE*} ���ؽڵ����ע���ýڵ��û����ܵ����ͷ�
	 */
	ACL_XML_NODE* get_xml_node(void) const;

	/**
	 * ���ñ��ڵ�ĸ��ڵ�
	 * @param node {xml_node*} ���ڵ�
	 * @return {xml_node&} ���ر��ڵ�����
	 */
	xml_node& set_parent(xml_node* node);

protected:
	friend class xml1;

	/**
	 * xml �ڵ㹹�캯��
	 * @param xml_ptr {xml*} xml �����󣬷ǿ�
	 * @param node {ACL_XML_NODE*} C ���е� xml �ڵ�ָ��
	 */
	xml1_node(xml* xml_ptr, ACL_XML_NODE* node);

	/**
	 * Ҫ��ö�������Ƕ�̬������
	 */
	~xml1_node(void);

private:
	ACL_XML_NODE *node_;
	ACL_ITER* child_iter_;
	ACL_ITER* attr_iter_;

	xml_node* parent_;
	xml1_node* parent_internal_;
};

class string;

class ACL_CPP_API xml1 : public xml
{
public:
	/**
	 * ���캯��
	 * @param data {const char*} xml ��ʽ���ַ�����������������
	 *  xml �ַ�����Ҳ�����ǲ��ֵ� xml �ַ�����Ҳ�����ǿ�ָ�룬
	 *  ������Σ��û���Ȼ�����ò��ֻ������� xml �ַ������� update
	 *  �������ڵ��� update �����н��� xml����ʵ�������캯����
	 *  �� data �����ǿ�ʱ����Ҳ����� update
	 */
	xml1(const char* data = NULL);
	~xml1(void);

	/**
	 * @override ���ڷǱպϵı�ǩ���Ƿ���Ҫ���Ապ��ַ� '/'��ȱʡΪ������
	 * @param on {bool}
	 * @return {xml&}
	 */
	xml& ignore_slash(bool on);

	/**
	 * @override �Ƿ��Զ����� xml ���룬ȱ��Ϊ������
	 * @param on {bool}
	 * @return {xml&}
	 */
	xml& xml_decode(bool on);

	/**
	 * @override ����ʽ��ʽѭ�����ñ�������� XML ���ݣ�Ҳ����һ�������
	 * ������ XML ���ݣ�������ظ�ʹ�ø� XML ������������� XML
	 * ������Ӧ���ڽ�����һ�� XML ����ǰ���� reset() ��������
	 * ����һ�εĽ������
	 * @param data {const char*} xml ����
	 */
	void update(const char* data);

	/**
	 * @override ���� XML ������״̬���� XML ������������Զ�� XML ����
	 * ���н������ڷ���ʹ�ñ� XML ������ǰ����Ҫ���ñ���������
	 * �ڲ� XML ������״̬�������һ�εĽ������
	 */
	void reset(void);

	/**
	 * @override �ӽ����� XML ԭʼ�����н���ȡ�ı�����
	 * @return {const string&} ���ؽ�������������ã�����������
	 *  ���������û�����Ҫ�ͷ�
	 */
	const string& getText(void);

	/**
	 * @override �� XML ������ȡ��ĳ����ǩ�������нڵ㼯��
	 * @param tag {const char*} ��ǩ��(�����ִ�Сд)
	 * @return {const std::vector<xml_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� xml_node �ڵ����ݿ����޸ģ�������ɾ���ýڵ㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<xml_node*>&
		getElementsByTagName(const char* tag) const;

	/**
	 * @override �� xml �����л�ö�Ӧ��ǩ���ĵ�һ�� xml �ڵ����
	 * @param tag {const char*} ��ǩ��(�����ִ�Сд)
	 * @return {xml_node*} ���ؿձ����ñ�ǩ��Ӧ�� xml �ڵ㲻����
	 */
	xml_node* getFirstElementByTag(const char* tag) const;

	/**
	 * @override �� xml �����л�����е�������༶��ǩ����ͬ�� xml �ڵ�ļ���
	 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ��������� xml ���ݣ�
	 *  <root> <first> <second> <third name="test1"> text1 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test2"> text2 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test3"> text3 </third> </second> </first> ...
	 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з��������Ľڵ�
	 * @return {const std::vector<xml_node*>&} ���������� xml �ڵ㼯��, 
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� xml_node �ڵ����ݿ����޸ģ�������ɾ���ýڵ㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<xml_node*>& getElementsByTags(const char* tags) const;

	/**
	 * @override �� xml �����л��ָ���༶��ǩ���ĵ�һ�� xml �ڵ�
	 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ��������� xml ���ݣ�
	 *  <root> <first> <second> <third name="test1"> text1 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test2"> text2 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test3"> text3 </third> </second> </first> ...
	 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з��������Ľڵ�
	 * @return {xml_node*} ���ؿձ�ʾ������
	 */
	xml_node* getFirstElementByTags(const char* tags) const;

	/**
	 * @override �� xml �����л����������������� name ������ֵ��ͬ�� xml �ڵ㼯��
	 * @param value {const char*} ������Ϊ name ������ֵ
	 * @return {const std::vector<xml_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� xml_node �ڵ����ݿ����޸ģ�������ɾ���ýڵ㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<xml_node*>&
		getElementsByName(const char* value) const;

	/**
	 * @override �� xml �����л�����и���������������ֵ�� xml �ڵ�Ԫ�ؼ���
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {const std::vector<xml_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 */
	const std::vector<xml_node*>& getElementsByAttr(
		const char* name, const char* value) const;

	/**
	 * @override �� xml �����л��ָ�� id ֵ�� xml �ڵ�Ԫ��
	 * @param id {const char*} id ֵ
	 * @return {xml_node*} xml �ڵ�Ԫ��, ������ NULL ���ʾû�з���������
	 *  xml �ڵ�, ����ֵ����Ҫ�ͷ�
	 */
	xml_node* getElementById(const char* id) const;

	/**
	 * @override ����һ�� xml_node �ڵ����
	 * @param tag {const char*} ��ǩ��
	 * @param text {const char*} �ı��ַ���
	 * @return {xml_node*} �²����� xml_node ������Ҫ�û��ֹ��ͷţ���Ϊ��
	 *  xml �����ͷ�ʱ��Щ�ڵ���Զ����ͷţ���Ȼ�û�Ҳ�����ڲ���ʱ����
	 *  reset ���ͷ���Щ xml_node �ڵ����
	 */
	xml_node& create_node(const char* tag, const char* text = NULL);

	/**
	 * @override ��ø��ڵ���󣬵���Ҫע�⣬�ýڵ�Ϊ��ڵ㣬���治���
	 *  �κ����ݣ��������� xml �ڵ�������㸸����
	 * @return {xml_node&}
	 */
	xml_node& get_root(void);

	/**
	 * @override ��ʼ������ xml ���󲢻�õ�һ���ڵ�
	 * @return {xml_node*} ���ؿձ�ʾ�� xml ����Ϊ�սڵ�
	 *  ע�����صĽڵ�����û������ֹ��ͷţ���Ϊ�ö���
	 *  �ڲ����Զ��ͷ�
	 */
	xml_node* first_node(void);

	/**
	 * @override ������ xml �������һ�� xml �ڵ�
	 * @return {xml_node*} ���ؿձ�ʾ�������
	 *  ע�����صĽڵ�����û������ֹ��ͷţ���Ϊ�ö���
	 *  �ڲ����Զ��ͷ�
	 */
	xml_node* next_node(void);

	/**
	 * @override �� xml ������ת���ַ���
	 * @param out {string&} �洢ת������Ļ�����
	 */
	void build_xml(string& out) const;

	/**
	 * @override
	 */
	const char* to_string(size_t* len = NULL) const;

public:
	/**
	 * ȡ�� acl ���е� ACL_XML ����
	 * @return {ACL_XML*} ��ֵ������Ϊ�գ�ע���û������޸ĸö����ֵ��
	 *  ���������ͷŸö���
	 */
	ACL_XML* get_xml(void) const
	{
		return xml_;
	}

private:
	ACL_XML *xml_;
	ACL_ITER* iter_;
	xml1_node* root_;
};

} // namespace acl
