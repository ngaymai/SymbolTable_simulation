#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

const int MAX = 100;

class Node
{
    string _name, _type;
    int _level;
    Node *next;

public:
    Node() : _name(""), _type(""), _level(-1), next(NULL){};
    Node(string name, string type, int level) : _name(name), _type(type), _level(level), next(NULL){};

    friend class SymbolTable;
};

class SymbolTable
{
    Node *Table[MAX];
    string store[MAX];

public:
    SymbolTable()
    {
        for (int i = 0; i < MAX; i++)
        {
            Table[i] = NULL;
            store[i] = "";
        }
    };
    ~SymbolTable()
    {
        for (int i = 0; i < MAX; i++)
            delete Table[i];
    };
    void run(string filename);
    int Hash_Function(string identifiers);
    void insert(string id_name, string id_type, int pos);
    void assign(string id_name, string id_value, int pos);
    void Begin(ifstream &file, int level);
    void End(int pos);
    void lookup(string id_name);
    void print();
    void Rprint();
};
#endif