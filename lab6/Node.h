#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;

class Node
{
public:
    int type;         // 用来区分InternalNode和LeafNode
    int parent_index; // 记录当前结点是父结点的第几个孩子
    Node *parent = NULL;
    virtual void rebalance(){};
    virtual long long insert(int pos, string cipher) { return 0; };	// 插入新的密文
    virtual long long search(int pos) { return 0; };	//查找pos对应的code
};

class InternalNode : public Node
{
public:
    std::vector<int> child_num; // 子节点具有的加密值个数
    std::vector<Node *> child;  // 子节点指针

    InternalNode();
    void rebalance() override;
    long long insert(int pos, string cipher) override; 
    long long search(int pos) override; 
    void insert_node(int index, Node *new_node); // 插入新的Node
};

class LeafNode : public Node
{
public:
    std::vector<std::string> cipher; // 密文
    std::vector<long long> encoding; // 编码
    LeafNode *left_bro = NULL;       // 左兄弟节点
    LeafNode *right_bro = NULL;      // 右兄弟节点
    long long lower = -1;
    long long upper = -1;

    LeafNode();
    long long Encode(int pos);
    void rebalance() override;
    long long insert(int pos, string cipher) override;
    long long search(int pos) override;
};

const int M = 128;
extern Node *root;
extern long long start_update;  //  更新区间的左端点
extern long long end_update;    //  更新区间的右端点
extern std::map<string, long long> update;
void root_initial();                 // 初始化
long long get_update(string cipher); // 根据密文获取对应的更新后的code