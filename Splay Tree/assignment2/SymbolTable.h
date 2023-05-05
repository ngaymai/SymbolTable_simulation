#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

const int MAX = 28;
class node
{
private:
    string name;
    int level;
    string type;
    string* value;
    node* left;
    node* right;
    node* parent;
    friend class SymbolTable;
public:
    node() :name(""), level(-1),type(""), value(NULL), left(NULL), right(NULL), parent(NULL) {}
    node(string _name, string _type, int _level)
        :name(_name), level(_level), type(_type), value(NULL), left(NULL), right(NULL), parent(NULL) {}    
    ~node();
    
    string Name() { return this->name; }    
    int Level() { return this->level; }
    string Type() { return this->type; }
    string Value(int i) { return this->value[i]; }
    node* Left() { return left; }
    node* Right() { return right; }
    void inType(string _type) { this->type = _type; }
    void inName(string _name) { this->name = _name; }
    void inValue(string str);     
    void inLeft(node* _left) { this->left = _left; }
    void inRight(node* _right) { this->right = _right; }
    void inParent(node* _parent) { this->parent = _parent; }
};
class SymbolTable
{    
    node* root;
    node* head[MAX];
public:
    SymbolTable();
    ~SymbolTable();
    void run(string filename);
    node* search(node* _key, int& num_comp, int& num_splay);
    void insert(string _name, string _type,string _value, int _level);
    void assign(string _name, string _value, int _level); 
    void zig(node* x);
    void zig_zig(node* x);
    void zig_zag(node* x);  
    void splay(node* _key, int& num_splay);
    void Delete(node* _key);
    void begin(ifstream &file, int _level);
    void end(int _level);
    void lookup(string _name, int _level);
    void print(node* _root);
    node* rightMax(node* _root);    
    node* empty(node* _root);
    int find(node* k);
    
    
};


#endif