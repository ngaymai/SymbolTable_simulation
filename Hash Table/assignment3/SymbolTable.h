#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"


class SymbolTable{
    class node;
  
    node** table;
    int m, c1, c2, t;
public:
    SymbolTable() :table(NULL), m(-1), c1(-1), c2(-1), t(-1) {};
    ~SymbolTable();
    void addTable(int);
    void addA(int, int);
    void addB(int, int, int);
    void run(string filename);
    void hashCheck(ifstream&);
    int hashFunction(int, int);
    int linearHash(int, int);
    int quadraticHash(int, int);
    int doubleHash(int, int);
    void insert(string, int, int);
    bool compare(node*, node*);
    void assign(string, string, int);
    void creNode(string, string, int, int&);
    node* search(string, int, int&);
    node* find(string, int, int&);
    void Delete(int);
    void call(string,int);
    void begin(ifstream&,int);
    void print(int,bool);
    void lookup(string,int);
private:
    class node {
    
        string id;
        int level; 
        string value = "";
        string* var = NULL;
        int num = -1;

        friend class SymbolTable;
    public:
        node() :id(""), level(-1) {};
        ~node();
        node(string _id, int _level, int _num) :id(_id), level(_level), num(_num)
        {
            var = new string[num];
            for (int i = 0; i < num; i++)
                var[i] = "";
        }
        node(string _id, int _level) : id(_id), level(_level) {};
        
    };
    
    
    
};
#endif