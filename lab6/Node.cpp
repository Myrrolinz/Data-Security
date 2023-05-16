#include "Node.h"
#include <array>
#include <math.h>
#include <assert.h>
#include <vector>
#include <map>
#include <fstream>

Node* root = nullptr;
long long start_update = -1;
long long end_update = -1;
std::map<string, long long> update;

InternalNode::InternalNode()
{
    this->type = 2;
    this->parent_index = -1;
    this->parent = NULL;
}

void InternalNode::insert_node(int index, Node *new_node)
{
    this->child.insert(this->child.begin() + index, new_node);

    if (new_node->type == 1)
    {
        // 如果新结点是LeafNode
        this->child_num.insert(this->child_num.begin() + index, ((LeafNode *)new_node)->cipher.size());
        ((LeafNode *)new_node)->parent = this;
    }
    else
    {
        // 如果是InternalNode
        int res = 0;
        for (size_t i = 0; i < ((InternalNode *)new_node)->child_num.size(); i++)
        {
            res += ((InternalNode *)new_node)->child_num.at(i);
        }
        this->child_num.insert(this->child_num.begin() + index, res);
        ((InternalNode *)new_node)->parent = this;
    }

    // 插入的新node改变了原有node位置，因此需要重新记录parent_index和child_num
    for (int i = 0; i < this->child.size(); i++)
    {
        this->child.at(i)->parent_index = i;
        if (this->child.at(i)->type == 1)
        {
            LeafNode *tmp = (LeafNode *)this->child.at(i);
            this->child_num.at(i) = tmp->cipher.size();
        }
    }
    if (this->child.size() >= M)
    {
        this->rebalance();
    }
}

void InternalNode::rebalance()
{
    InternalNode *new_node = new InternalNode();
    // 将当前结点的后半部分数据存入new_node
    int middle = floor(this->child.size() * 0.5);
    while (middle > 0)
    {
        new_node->child.insert(new_node->child.begin(), this->child.at(this->child.size() - 1));
        new_node->child_num.insert(new_node->child_num.begin(), this->child_num.at(this->child_num.size() - 1));
        this->child.pop_back();
        this->child_num.pop_back();
        middle--;
    }
    for (int i = 0; i < new_node->child.size(); i++)
    {
        new_node->child.at(i)->parent_index = i;
        new_node->child.at(i)->parent = new_node;
    }

    if (!this->parent)
    {
        // 如果当前结点是root结点
        InternalNode *new_root = new InternalNode();
        new_root->insert_node(0, this);
        new_root->insert_node(1, new_node);
        root = new_root;
    }
    else
    {
        int res = 0;
        for (size_t i = 0; i < this->child_num.size(); i++)
        {
            res += this->child_num.at(i);
        }
        ((InternalNode *)this->parent)->child_num.at(this->parent_index) = res;
        ((InternalNode *)this->parent)->insert_node(this->parent_index + 1, new_node);
    }
}

long long InternalNode::insert(int pos, string cipher)
{
    for (int i = 0; i < this->child.size(); i++)
    {
        if (pos > this->child_num.at(i))
        {
            pos = pos - this->child_num.at(i);
        }
        else
        {
            this->child_num.at(i)++;
            return this->child.at(i)->insert(pos, cipher);
        }
    }
    // 如果没有符合的，则放到最后一个
    this->child_num.back() = this->child_num.back()++;
    return this->child.back()->insert(pos, cipher);
}

long long InternalNode::search(int pos)
{
    int i = 0;
    for (; i < this->child.size(); i++)
    {
        if (pos < this->child_num.at(i))
        {
            return this->child.at(i)->search(pos);
        }
        else
        {
            pos = pos - this->child_num.at(i);
        }
    }
    return 0;
}

LeafNode::LeafNode()
{
    this->type = 1;
    this->parent_index = -1;
    this->parent = NULL;
}

void Recode(vector<LeafNode *> node_list)
{
    long long left_bound = node_list.at(0)->lower;
    long long right_bound = node_list.back()->upper;
    int total_cipher_num = 0;

    for (size_t i = 0; i < node_list.size(); i++)
    {
        total_cipher_num += node_list.at(i)->cipher.size();
    }

    if ((right_bound - left_bound) > total_cipher_num)
    {
        // 如果当前的更新区间，足以包含待放的pos
        start_update = left_bound;
        end_update = right_bound;
        // 计算间隔量，使code均匀分布
        long long frag = floor((right_bound - left_bound) / total_cipher_num);
        assert(frag >= 1);
        long long cd = left_bound;
        for (size_t i = 0; i < node_list.size(); i++)
        {
            node_list.at(i)->lower = cd;
            for (int j = 0; j < node_list.at(i)->encoding.size(); j++)
            {
                node_list.at(i)->encoding.at(j) = cd;
                update.insert(make_pair(node_list.at(i)->cipher.at(j), cd));
                cd = cd + frag;
            }
            node_list.at(i)->upper = cd;
        }
        node_list.back()->upper = right_bound;
    }

    else
    {
        // 若不足以包含，则继续向左兄弟结点和右兄弟结点扩展更新区间
        if (node_list.at(0)->left_bro)
        {
            // 如果左兄弟存在，则加入更新列表
            node_list.insert(node_list.begin(), node_list.at(0)->left_bro);
        }

        if (node_list.back()->right_bro)
        {
            // 如果右兄弟存在，则加入更新列表
            node_list.push_back(node_list.back()->right_bro);
        }
        else
        {
            // 扩展最后一个结点的大小
            node_list.back()->upper = node_list.back()->upper * 2;
            if (node_list.back()->upper >= pow(2, 60))
                node_list.back()->upper = pow(2, 60);
        }
        Recode(node_list);
    }
}

long long LeafNode::Encode(int pos)
{
    long long left = this->lower;
    long long right = this->upper;

    if (pos > 0)
    {
        left = this->encoding.at(pos - 1);
    }
    if (pos < this->encoding.size() - 1)
    {
        right = this->encoding.at(pos + 1);
    }

    if (floor(right - left) < 2)
    {
        // 如果该区间已经没有位置，则需要recode
        std::vector<LeafNode *> node_list;
        node_list.push_back(this);
        Recode(node_list);
        // 返回特殊值0，标志已经调整，需要更新数据库
        return 0;
    }
    else
    {
        // 否则直接以left和right的平均值向上取整作为新的code
        unsigned long long re = right;
        long long frag = (right - left) / 2;
        re = re - frag;
        this->encoding.at(pos) = re;
        return this->encoding.at(pos);
    }
}

void LeafNode::rebalance()
{
    LeafNode *new_node = new LeafNode();
    // 将本结点后半部分数据放到new_node
    int middle = floor(this->cipher.size() * 0.5);
    while (middle > 0)
    {
        new_node->cipher.insert(new_node->cipher.begin(), this->cipher.back());
        new_node->encoding.insert(new_node->encoding.begin(), this->encoding.back());
        this->encoding.pop_back();
        this->cipher.pop_back();
        middle--;
    }
    // 将新结点连接在this结点右边
    new_node->lower = new_node->encoding.at(0);
    new_node->upper = this->upper;
    this->upper = new_node->encoding.at(0);
    if (this->right_bro)
    {
        this->right_bro->left_bro = new_node;
    }
    new_node->right_bro = this->right_bro;
    this->right_bro = new_node;
    new_node->left_bro = this;

    if (!this->parent)
    {
        // 如果this是root结点，则创建一个新的root
        InternalNode *new_root = new InternalNode();
        new_root->insert_node(0, this);
        new_root->insert_node(1, new_node);
        root = new_root;
    }
    else
    {
        ((InternalNode *)this->parent)->child_num.at(this->parent_index) = this->cipher.size();
        ((InternalNode *)this->parent)->insert_node(this->parent_index + 1, new_node);
    }
}

long long LeafNode::insert(int pos, string cipher)
{
    // 将密文插入到对应的pos
    this->cipher.insert(this->cipher.begin() + pos, cipher);
    this->encoding.insert(this->encoding.begin() + pos, -1);
    long long cd = this->Encode(pos);
    if (this->cipher.size() >= M)
    {
        this->rebalance();
    }
    return cd;
}

long long LeafNode::search(int pos)
{
    return this->encoding.at(pos);
}

void root_initial()
{
    root = new LeafNode();
    ((LeafNode *)root)->lower = 0;
    ((LeafNode *)root)->upper = pow(2, 62);
    update.clear();
    start_update = -1;
    end_update = -1;
};

long long get_update(string cipher)
{
    if (update.count(cipher) > 0)
    {
        return update[cipher];
    }
    return 0;
}