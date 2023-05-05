#include "SymbolTable.h"

void SymbolTable::run(string filename)
{
    ifstream file;
    file.open(filename);
    regex check_name("^[a-z]\\w*$");
    regex check_string("^\\'( *[a-zA-Z0-9]*)*\\'$");
    regex check_number("^\\d+$");

    while (!file.eof())
    {
        string tmp;
        int n = 0;
        file >> tmp;
        if (tmp == "INSERT")
        {
            string name, type;
            file >> name;
            getline(file, type);
            type.erase(0, 1);
            string ins = "INSERT " + name + " " + type;
            InvalidInstruction exp(ins);
            if (regex_match(name, regex("^[a-z]\\w*$")) && (type == "string" || type == "number"))
            {
                for (int i = 0; i < MAX; i++)
                {
                    if (store[i] == "")
                    {
                        store[i] = name;
                        break;
                    }
                }

                insert(name, type, n);
            }
            else
                throw exp;
        }
        else if (tmp == "ASSIGN")
        {
            string name, value;
            file >> name;
            getline(file, value);
            value.erase(0, 1);
            string ins = "ASSIGN " + name + " " + value;
            InvalidInstruction exp(ins);
            if (regex_match(name, check_name))
                assign(name, value, n);
            else
                throw exp;
        }
        else if (tmp == "BEGIN")
        {
            Begin(file, n + 1);
        }
        else if (tmp == "END")
        {
            UnknownBlock exp;
            throw exp;
        }
        else if (tmp == "LOOKUP")
        {
            string name;
            file >> name;
            string ins = "LOOKUP " + name;
            InvalidInstruction exp(ins);
            if (regex_match(name, check_name))
                lookup(name);
            else
                throw exp;
        }
        else if (tmp == "PRINT")
        {
            print();
        }
        else if (tmp == "RPRINT")
        {
            Rprint();
        }
        else
        {
            string extmp;
            getline(file, extmp);
            tmp = tmp + extmp;
            string ins = tmp;
            InvalidInstruction exp(ins);
            throw exp;
        }
    }
}

int SymbolTable::Hash_Function(string identifiers)
{
    int index = 0;
    for (unsigned int i = 0; i < identifiers.length(); i++)
        index += identifiers[i] * (i + 1);
    index = index % 100;
    return index;
}

void SymbolTable::insert(string id_name, string id_type, int pos)
{

    int index = Hash_Function(id_name);
    Node *ptr = new Node(id_name, id_type, pos);
    string ins = "INSERT " + id_name + " " + id_type;
    Redeclared exp(ins);

    if (Table[index] == NULL)
    {
        Table[index] = ptr;
        cout << "success" << endl;
        ptr = NULL;
        delete ptr;
        return;
    }
    else if (Table[index]->_name != id_name)
    {
        while (Table[index] != NULL && Table[index]->_name != id_name)
            index = (index + 1) % 100;
        if (Table[index] == NULL)
        {
            Table[index] = ptr;
            cout << "success" << endl;
            ptr = NULL;
            delete ptr;
            return;
        }
    }
    if (Table[index]->_name == id_name && Table[index]->_level == pos)
    {
        ptr = NULL;
        delete ptr;
        throw exp;
    }
    ptr->next = Table[index];
    Table[index] = ptr;
    cout << "success" << endl;
    ptr = NULL;
    delete ptr;
    return;
}

void SymbolTable::assign(string id_name, string id_value, int pos)
{
    int index = Hash_Function(id_name);

    string ins = "ASSIGN " + id_name + " " + id_value;
    Undeclared exp1(ins);
    TypeMismatch exp2(ins);
    string type;
    regex CheckNumber("^\\d+$");
    regex CheckString("^\\'( *[a-zA-Z0-9]*)*\\'$");
    if (regex_match(id_value, CheckNumber))
        type = "number";
    else if (regex_match(id_value, CheckString))
        type = "string";

    if (Table[index] == NULL)
        throw exp1;
    else if (Table[index]->_name != id_name)
    {
        while (Table[index] != NULL && Table[index]->_name != id_name)
            index = (index + 1) % 100;
        if (Table[index] == NULL)
            throw exp1;
    }
    Node *cur = Table[index];
    while (cur != NULL)
    {
        if (type == cur->_type)
        {
            cout << "success" << endl;
            return;
        }

        cur = cur->next;
    }
    throw exp2;
}

void SymbolTable::Begin(ifstream &file, int level)
{
    regex check_name("^[a-z]\\w*$");
    regex check_string("^\\'( *[a-zA-Z0-9]*)*\\'$");
    regex check_number("^\\d+$");
    if (file.is_open())
    {
        string tmp;
        int n = level;
        while (!file.eof())
        {
            file >> tmp;

            string name, type, value;
            if (tmp == "INSERT")
            {
                file >> name;
                getline(file, type);
                type.erase(0, 1);
                string ins = "INSERT " + name + " " + type;
                InvalidInstruction exp(ins);
                if (regex_match(name, regex("^[a-z]\\w*$")) && (type == "string" || type == "number"))
                {
                    for (int i = 0; i < MAX; i++)
                    {
                        if (store[i] == "")
                        {
                            store[i] = name;
                            break;
                        }
                    }

                    insert(name, type, n);
                }
                else
                    throw exp;
            }
            else if (tmp == "ASSIGN")
            {
                file >> name;
                getline(file, value);
                value.erase(0, 1);
                string ins = "ASSIGN " + name + " " + value;
                InvalidInstruction exp(ins);
                if (regex_match(name, check_name))
                    assign(name, value, n);
                else
                    throw exp;
            }
            else if (tmp == "LOOKUP")
            {
                file >> name;
                string ins = "LOOKUP " + name;
                InvalidInstruction exp(ins);
                if (regex_match(name, check_name))
                    lookup(name);
                else
                    throw exp;
            }
            else if (tmp == "BEGIN")
            {
                Begin(file, n + 1);
            }
            else if (tmp == "END")
            {
                End(level);
                return;
            }
            else if (tmp == "PRINT")
            {
                print();
            }
            else if (tmp == "RPRINT")
            {
                Rprint();
            }
            else
            {
                string extmp;
                getline(file, extmp);
                tmp = tmp + extmp;
                string ins = tmp;
                InvalidInstruction exp(ins);
                throw exp;
            }
        }
        UnclosedBlock exp(n);
        throw exp;
    }
}

void SymbolTable::End(int pos)
{
    for (int i = 0; i < MAX; i++)
    {
        if (Table[i] != NULL)
        {
            Node *cur = Table[i];
            if (cur->_level == pos)
            {
                Table[i] = cur->next;
                cur->next = NULL;
                delete cur;
            }
        }
    }
}

void SymbolTable::lookup(string id_name)
{
    int index = Hash_Function(id_name);
    string ins = "LOOKUP " + id_name;
    Undeclared exp(ins);
    if (Table[index] == NULL)
        throw exp;
    else if (Table[index]->_name != id_name)
    {
        Node *start = Table[index];
        index = (index + 1) % 100;
        while (Table[index] != NULL && Table[index]->_name != id_name && Table[index] != start)
            index = (index + 1) % 100;
        if (Table[index] == NULL || Table[index] == start)
            throw exp;
    }

    cout << Table[index]->_level << endl;
    return;
}
void SymbolTable::print()
{
    string a[MAX];
    for (int i = 0; i < MAX; i++)
    {
        if (store[i] != "")
        {
            for (int j = 0; j < i; j++)
            {
                if (a[j] == store[i])
                {
                    a[j] = "";
                }
            }
            a[i] = store[i];
        }
    }

    for (int i = 0; i < MAX; i++)
    {
        if (a[i] != "")
            goto GOTO;
    }
    return;

GOTO:
    string prt;

    for (int i = 0; i < MAX; i++)
    {
        if (a[i] != "")
        {
            int index = Hash_Function(a[i]);
            if (Table[index]->_name == a[i])
            {
                if (prt != "")
                    prt = prt + " " + Table[index]->_name + "//" + to_string(Table[index]->_level);
                else
                    prt = Table[index]->_name + "//" + to_string(Table[index]->_level);
                continue;
            }
            while (Table[index]->_name != a[i])
            {
                index = (index + 1) % 100;
            }
            if (prt != "")
                prt = prt + " " + Table[index]->_name + "//" + to_string(Table[index]->_level);
            else
                prt = Table[index]->_name + "//" + to_string(Table[index]->_level);
        }
    }
    cout << prt << endl;
    return;
}
void SymbolTable::Rprint()
{
    string a[MAX];
    for (int i = 0; i < MAX; i++)
    {
        if (store[i] != "")
        {
            for (int j = 0; j < i; j++)
            {
                if (a[j] == store[i])
                {
                    a[j] = "";
                }
            }
            a[i] = store[i];
        }
    }

    for (int i = 0; i < MAX; i++)
    {
        if (a[i] != "")
            goto GOTO;
    }
    return;

GOTO:
    string prt;
    for (int i = MAX - 1; i >= 0; i--)
    {
        if (a[i] != "")
        {
            int index = Hash_Function(a[i]);
            if (Table[index]->_name == a[i])
            {
                if (prt != "")
                    prt = prt + " " + Table[index]->_name + "//" + to_string(Table[index]->_level);
                else
                    prt = Table[index]->_name + "//" + to_string(Table[index]->_level);
                continue;
            }
            while (Table[index]->_name != a[i])
            {
                index = (index + 1) % 100;
            }
            if (prt != "")
                prt = prt + " " + Table[index]->_name + "//" + to_string(Table[index]->_level);
            else
                prt = Table[index]->_name + "//" + to_string(Table[index]->_level);
        }
    }
    cout << prt << endl;
    return;
}