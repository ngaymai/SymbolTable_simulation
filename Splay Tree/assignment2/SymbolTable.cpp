#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    root = NULL;
    for (int i = 0; i < MAX; i++)
        head[i] = NULL;
}
SymbolTable::~SymbolTable()
{
    
    for (int i = 0; i < MAX; i++)
    {        
        head[i] = NULL;
        delete head[i];
    }
    root = empty(root);    
}

node::~node()
{
    delete[] value;
    parent = NULL;
    left = NULL;
    right = NULL;
    delete parent;
    delete left;
    delete right;
}

node* SymbolTable::empty(node* _root)
{
    if(_root)
    {
        empty(_root->left);
        empty(_root->right);
        delete _root;
    }   
    return NULL;
}
int InsertToken(string tmp, string& _name, string& _type, string& _value, bool& _static)
{
    tmp.erase(0, 1);
    int pos = (int)tmp.find(" ");
    _name = tmp.substr(0, pos);
    if (!regex_match(_name, regex("[a-z]\\w*")))
        return 1;
    if (_name == "number" || _name == "string" || _name == "true" || _name == "false")
        throw InvalidInstruction("INSERT " + tmp);
    tmp.erase(0, pos + 1);
    pos = (int)tmp.rfind(" ");
    _type = tmp.substr(0, pos);
    tmp.erase(0, pos + 1);
    regex e1("(number)|(string)");
    regex e2("\\(((|number|string)(\\,(number|string))*)\\)->(number|string)");
    
    if (!(regex_match(_type, e1) || regex_match(_type, e2)))
        return 2;   
    if (tmp == "true")
        _static = 1;
    else if (tmp == "false")
        _static = 0;
    else return 3;   

    if (regex_match(_type, e2))
    {
        int pos = (int)_type.find(">");
        string str = _type.substr(0, pos - 1);
        _type = _type.erase(0, pos + 1);
        _value = str;
        return 11;
    }
    
    return 0;
}

void assignToken(string tmp, string& _name, string& _value)
{
    string ex = "ASSIGN " + tmp;
    tmp.erase(0, 1);
    int pos = (int)tmp.find(" ");
    _name = tmp.substr(0, pos);
    tmp.erase(0, pos + 1);
    _value = tmp;
    regex e1("[a-z]\\w*");    
    regex e2("(^[a-z]\\w*$)|(^\\d+$)|(^'( *[a-zA-Z0-9]*)*'$)|(([a-z]\\w*)\\((|(\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*))(\\,((\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*)))*\\))");
    if (regex_match(_name, e1) && regex_match(_value, e2))
    {
        if (_name == "number" || _name == "string" || _name == "true" || _name == "false")
            throw InvalidInstruction("ASSIGN " + _name + " " + _value);
        return;
    }
        
    else
        throw InvalidInstruction("ASSIGN " + _name + " " + _value);
}

void SymbolTable::run(string filename)
{
    ifstream file;
    file.open(filename);
    while (!file.eof())
    {
        string tmp;
        int n = 0;
        file >> tmp;
        if (tmp == "INSERT")
        {
            string line;
            string _name = "", _type = "", _value = "";
            bool _static = 0;            
            getline(file, line);     
            int check = InsertToken(line, _name, _type, _value, _static);
            if (!check)
            {                
                insert(_name, _type, _value, 0);
            }
            else if (check == 11)
            {                
                insert(_name, _type, _value, 0);                
            }
            else
                throw(InvalidInstruction("INSERT " + line));            
        }
        else if (tmp == "ASSIGN")
        {
            string line;
            string _name = "", _value = "";
            getline(file, line);
            assignToken(line, _name, _value);
            assign(_name, _value, n);
        }
        else if (tmp == "BEGIN")
        {
            begin(file, n + 1);
        }
        else if(tmp == "END")
        {
            throw UnknownBlock();
        }
        else if (tmp == "LOOKUP")
        {
            string _name;
            getline(file, _name);
            _name.erase(0, 1);
            if (regex_match(_name, regex("[a-z]\\w*")))
                lookup(_name, n);
            else throw InvalidInstruction(tmp + " " + _name);
        }
        else if(tmp=="PRINT")
        {
            if (!root) continue;
            else
            {
                print(root);
                cout << endl;
            }           
        }
        else
        {
            throw InvalidInstruction(tmp);
        }
    }
}

int compare(node* _root, node* _key)
{
    if (!_root) return 99;
    if (_root->Level() == _key->Level())
    {
        if (_root->Name() == _key->Name())
            return 0;
        else if (_root->Name() > _key->Name())
            return -1;
        else if (_root->Name() < _key->Name())
            return 1;
    }        
    else if (_root->Level() > _key->Level())
        return -1;
    else if (_root->Level() < _key->Level())
        return 1;  

    return 18;
}

void SymbolTable::zig(node* x)
{
    node* p = x->parent;
    int s = compare(p->left, x);
    if (s == 0)
    {        
        node* B = x->right;        
        x->parent = NULL;
        x->right = p;
        p->left = B;
        p->parent = x;      
        if (B) B->parent = p;
    }
    else
    {        
        node* B = x->left;        
        x->parent = NULL;
        x->left = p;
        p->right = B;
        p->parent = x;
        if (B) B->parent = p;
    }
}
void SymbolTable::zig_zig(node* x)
{
    node* p = x->parent;
    node* g = p->parent;
    int s = compare(p->left, x);
    if (s == 0)
    {
        node* B = x->right;
        node* C = p->right;
        x->parent = g->parent;
        x->right = p;
        p->parent = x;
        p->left = B;
        p->right = g;
        g->parent = p;
        g->left = C;      
        if (B) B->parent = p;
        if (C) C->parent = g;
    }
    else
    {       
        node* B = x->left;
        node* C = p->left;   
        x->parent = g->parent;
        x->left = p;
        p->parent = x;
        p->right = B;
        p->left = g;
        g->parent = p;
        g->right = C;  
        if (B) B->parent = p;
        if (C) C->parent = g;
    } 
    if (x->parent)
    {
        s = compare(x->parent->left, g);
        if (s == 0)
            x->parent->left = x;
        else
            x->parent->right = x;
    }
    
}
void SymbolTable::zig_zag(node* x)
{
    node* p = x->parent;
    node* g = p->parent;
    int s = compare(p->left, x);
    if (s == 0)
    {
        node* A = x->left;
        node* B = x->right;       
        x->parent = g->parent;
        x->left = g;
        x->right = p;
        p->left = B;
        p->parent = x;
        g->right = A;
        g->parent = x;
        if (A) A->parent = g;
        if (B) B->parent = p;
    }
    else
    {
        node* A = x->right;
        node* B = x->left;    
        x->parent = g->parent;
        x->right = g;
        x->left = p;
        p->right = B;
        p->parent = x;
        g->left = A;
        g->parent = x;
        if (A) A->parent = g;
        if (B) B->parent = p;
    }
    if (x->parent)
    {
        s = compare(x->parent->left, g);
        if (s == 0)
            x->parent->left = x;
        else
            x->parent->right = x;
    }

}
int cV(node* a, node* b)
{
    int i = 0;
    while (a->Value(i) != "/" && b->Value(i) != "/")
    {
        if (a->Value(i) > b->Value(i))
            return -1;
        else if (a->Value(i) < b->Value(i))
            return 1;
        else
            i++;
    }
    if (a->Value(i) != "/" && b->Value(i) == "/")
        return -1;
    else if (a->Value(i) == "/" && b->Value(i) != "/")
        return 1;
    else
        return 0;
}
void SymbolTable::splay(node* _key, int& num_splay) 
{
    if (root == _key) return;
    while (_key->parent)
    {        
        if (!_key->parent->parent)
            zig(_key);        
        else if (!(compare(_key->parent->parent->left, _key->parent) || compare(_key->parent->left, _key))) 
            zig_zig(_key);        
        else if (!(compare(_key->parent->parent->right, _key->parent) || compare(_key->parent->right, _key)))
            zig_zig(_key);
        else zig_zag(_key);
        
    }
    this->root = _key;
    num_splay++;
}
node* SymbolTable::search(node* _key, int& num_comp, int& num_splay)
{   
    bool a = 0;
    for (int i = MAX - 1; i >= 0; i--)
    {
        if (head[i])
        {
            if (head[i]->name == _key->name)
            {
                _key->level = head[i]->level;
                a = 1;
                break;
            }
        }
    }
    if (!a)
        _key->level = 0;
    node* prev = NULL;
    node* cur = this->root;
    node* flag = NULL;
    while (cur)
    {
        prev = cur;       
        num_comp++;
        int s = compare(cur, _key);
        if (s == -1) cur = cur->left;
        else if (s == 1) cur = cur->right;
        else
        {
            flag = cur;
            if (cur->value != NULL && _key->value != NULL)
            {
                int s = cV(cur, _key);
                if (s == -1) cur = cur->left;
                else if (s == 1) cur = cur->right;
                else
                    flag = cur;   
            }
            break;
        }       
    }
    if (flag) splay(flag, num_splay);
    else if (prev) splay(prev, num_splay);

    prev = NULL;
    delete prev;
    cur = NULL;
    delete cur;
    return flag;
}

void SymbolTable::insert(string _name, string _type, string _value, int _level)
{
    int num_comp = 0, num_splay = 0;
    node* key = new node(_name, _type, _level);
    if (_value != "") key->inValue(_value);
    if (!root)
    {
        cout << num_comp << " " << num_splay << endl;        
        root = key;
        if (key->level)
            for (int i = 0; i < MAX; i++)
                if (!head[i])
                {
                    head[i] = key;
                    break;
                }
        key = NULL;
        delete key;
        return ;
    }
    node* cur = root;
    node* p = NULL;
    while (cur)
    {
        p = cur;
        int s = compare(cur, key);
        if (s == -1)
            cur = cur->left;
        else if (s == 1)
            cur = cur->right;
        else
        {
            if (cur->value != NULL && key->value != NULL)
            {
                int s = cV(cur, key);
                if (s == -1)
                    cur = cur->left;
                else if (s == 1)
                    cur = cur->right;
                else
                {                    
                    delete key;
                    cur = NULL;
                    p = NULL;
                    delete cur;
                    delete p;
                    string _static = (_level) ? "true" : "false";
                    throw Redeclared("INSERT " + _name + " " + _value + "->" + _type + " " + _static);
                }
            }           
            else if (key->value != NULL)
            {                
                delete key;
                cur = NULL;
                p = NULL;
                delete cur;
                delete p;
                string _static = (_level) ? "true" : "false";
                throw Redeclared("INSERT " + _name + " " + _value + "->" + _type + " " + _static);
            }
            else
            {                
                delete key;
                cur = NULL;
                p = NULL;
                delete cur;
                delete p;
                string _static = (_level) ? "true" : "false";
                throw Redeclared("INSERT " + _name + " " + _type + " " + _static);
            }            
        }  
        num_comp++;
    }
    key->parent = p;
    int s = compare(p, key);
    if (s == -1)
        p->left = key;
    else if (s == 1)
        p->right = key;
    else
    {
        if (p->value != NULL && key->value != NULL)
        {
            int s = cV(p, key);
            if (s == -1)
                p->left = key;
            else if (s == 1)
                p->right = key;
        }
    }  
    
    splay(key, num_splay);
    cout << num_comp << " " << num_splay << endl;    
    if (key->level)
        for (int i = 0; i < MAX; i++)
            if (!head[i])
            {
                head[i] = key;
                break;
            }
                
    key = NULL;
    delete key;
    cur = NULL;
    delete cur;
    p = NULL;
    delete p;
    return;
}

void node::inValue(string str)
{
    int n = 1;
    if ((int)str.find("()") != -1)
    {
        value = new string[1];
        value[0] = "/";
        return;
    }        
    int pos = (int)str.find(",");
    while (pos != -1)
    {
        pos = (int)str.find(",", pos + 1);
        n++;
    }
    value = new string[n+1];
    str.erase(0, 1);
    pos = (int)str.find(",");
    int i = 0;
    while (pos != -1)
    {
        string _value = str.substr(0, pos);
        value[i] = _value;
        str.erase(0, pos + 1);
        pos = (int)str.find(",");
        i++;
    }
    pos = (int)str.find(")");
    string _value = str.substr(0, pos);
    value[i] = _value;
    value[n] = "/";
}
node* creNode(string str)
{
    node* newNode = new node("", "", 0);
    regex e2("' *[\\w^_]*'");
    regex e3("\\d+");
    regex e1("([a-z]\\w*)\\((|(\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*))(\\,((\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*)))*\\)");
    if (regex_match(str, e1))
    {
        int pos = (int)str.find("(");
        string _name = str.substr(0, pos);
        newNode->inName(_name);
        str.erase(0, pos);
        str = regex_replace(str, e2, "string");        
        string _value;
        int s = 0;
        pos = (int)str.find(",");
        while (pos != -1)
        {
            _value = str.substr(s + 1, pos - s - 1);
            if (regex_match(_value, e3))
                str.replace(s + 1, pos - s - 1, "number");
            s = (int)str.find(",", s + 1);
            pos = (int)str.find(",", s + 1);
        }
        pos = (int)str.find(")");
        _value = str.substr(s + 1, pos - s - 1);
        if (regex_match(_value, e3))
            str.replace(s + 1, pos - s - 1, "number");
        newNode->inValue(str);
        return newNode;
    }
    
    delete newNode;
    return NULL;
}

void SymbolTable::assign(string _name, string _value, int _level)
{
    if (!root) throw(Undeclared("ASSIGN " + _name + " " + _value));
    regex e1("^[a-z]\\w*$");
    regex e2("^' *[\\w^_]*'$");
    regex e3("^\\d+$");       
    regex e4("([a-z]\\w*)\\((|(\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*))(\\,((\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*)))*\\)");
    int num_comp = 0, num_splay = 0;    
    if (regex_match(_value, e1))
    {
        node* key = new node(_value, "", _level);       
        node* rsc = search(key, num_comp, num_splay);
        if (!rsc || rsc->value != NULL)
        {
            delete key;
            rsc = NULL;
            delete rsc;
            throw(Undeclared("ASSIGN " + _name + " " + _value));
        }
        
        key->inName(_name);
        node* res = search(key, num_comp, num_splay);
        if (!res)
        {
            delete key;
            rsc = NULL;
            res = NULL;
            delete rsc;
            delete res;
            throw(Undeclared("ASSIGN " + _name + " " + _value));
        }
         
        if (rsc->type != res->type || res->value != NULL) 
        {
            delete key;
            rsc = NULL;
            res = NULL;
            delete rsc;
            delete res;
            throw(TypeMismatch("ASSIGN " + _name + " " + _value));
        }    
        delete key;
        rsc = NULL;
        res = NULL;
        delete rsc;
        delete res;
    }
    if (regex_match(_value, e2))
    {
        node* key = new node(_name, "string", _level);
        node* res = search(key, num_comp, num_splay);
        if (!res)
        {
            delete key;
            res = NULL;
            delete res;
            throw(Undeclared("ASSIGN " + _name + " " + _value));
        }
        if (key->type != root->type || root->value != NULL) 
        {
            delete key;
            res = NULL;
            delete res;
            throw(TypeMismatch("ASSIGN " + _name + " " + _value));
        }
        delete key;
        res = NULL;
        delete res;
    }
    if (regex_match(_value, e3))
    {
        node* key = new node(_name, "number", _level);
        node* res = search(key, num_comp, num_splay);
        if (!root)
        {
            delete key;
            res = NULL;
            delete res;
            throw(Undeclared("ASSIGN " + _name + " " + _value));
        }
        if (key->type != root->type || root->value != NULL) 
        {
            delete key;
            res = NULL;
            delete res;
            throw(TypeMismatch("ASSIGN " + _name + " " + _value));
        }
        delete key;
        res = NULL;
        delete res;
    }
    if (regex_match(_value, e4))
    {        
        node* key = creNode(_value);       
        int t = find(key);
        if (t == 1) 
        {            
            delete key;
            throw Undeclared("ASSIGN " + _name + " " + _value);
        }
        else if (t == 2) 
        {            
            delete key;
            throw TypeMismatch("ASSIGN " + _name + " " + _value);
        }
        node* rsc = search(key, num_comp, num_splay);
        if(!rsc) 
        {            
            delete key;
            rsc = NULL;
            delete rsc;
            throw Undeclared("ASSIGN " + _name + " " + _value);
        }
        if (rsc->value == NULL || cV(rsc, key) != 0)
        {            
            delete key;
            rsc = NULL;
            delete rsc;
            throw(TypeMismatch("ASSIGN " + _name + " " + _value));
        }               
        delete key;
        key = new node(_name, "", _level);
        node* res = search(key, num_comp, num_splay);
        if (!res)
        {
            delete key;            
            rsc = NULL;            
            res = NULL;
            delete rsc;
            delete res;
            throw(Undeclared("ASSIGN " + _name + " " + _value));
        }          
        if (res->type != rsc->type || res->value != NULL) 
        {
            delete key;            
            rsc = NULL;
            res = NULL;
            delete rsc;
            delete res;
            throw(TypeMismatch("ASSIGN " + _name + " " + _value));
        }
        delete key;
        rsc = NULL;
        res = NULL;
        delete rsc;
        delete res;
    }
    
    cout << num_comp << " " << num_splay << endl;
}

int SymbolTable::find(node* k)
{
    int n;
    int i = 0;    
        while (k->value[i] != "/")
        {
            if (k->value[i] != "string" && k->value[i] != "number")
            {
                node* tmp = new node(k->value[i], "", 0);
                root = search(tmp,n,n);
                if (!root)
                {
                    delete tmp;
                    return 1;
                }
                else if (root->value != NULL || root->name != k->value[i])
                {
                    delete tmp;
                    return 2;
                }
                else
                    k->value[i] = root->type;
                delete tmp;
            }
            i++;
        }        
    return 0;
}

node* SymbolTable::rightMax(node* _root)
{
    if (!_root->right) return _root;
    return rightMax(_root->right);
}

void SymbolTable::Delete(node* _key)
{
    int a = 0;
    node* temp;
    if (!root) return;    
    root = search(_key, a, a);
    if (root->name != _key->name || root->level != _key->level) return;    
    if (!root->left)
    {
        temp = root;
        root = root->right; 
        if (root)
            root->parent = NULL;
    }    
    else
    {
        temp = root;        
        node* R = root->right;
        node* L = root->left;   
            L->parent = NULL;
        node* key = rightMax(L);
        root = L;
        splay(key, a);
        if (R)
        {
            root->right = R;
            R->parent = root;
        }
        key = NULL;
        R = NULL;
        L = NULL;
        delete key;
        delete R;
        delete L;
    }    
    delete temp;  

}

void SymbolTable::end(int level)
{
    for (int i = 0; i < MAX; i++)
    {
        
        if (head[i] && head[i]->level == level)
        {
            Delete(head[i]);            
            head[i] = NULL;            
        }
    }
}

void SymbolTable::lookup(string _name, int level)
{
    node* key = new node(_name, "", level);
    int a = 0;    
    if (!search(key, a, a))
    {
        delete key;
        throw Undeclared("LOOKUP " + _name);
    }
    else if (key->name == root->name) cout << root->level << endl;
    
    delete key;
}
void SymbolTable::print(node* _root)
{
    if (!_root) return;
    else
    {
        cout << _root->name << "//" << _root->level;
        if (_root->left)
        {
            cout << " ";
            print(_root->left);
        }
        if (_root->right)
        {
            cout << " ";
            print(_root->right);
        }
    }
    return;        
}
void SymbolTable::begin(ifstream &file, int n)
{   
    if (file.is_open())
    {
        while (!file.eof())
        {
            string tmp;            
            file >> tmp;
            if (tmp == "INSERT")
            {
                string line;
                string _name = "", _type = "", _value = "";
                bool _static = 0;
                getline(file, line);
                int check = InsertToken(line, _name, _type, _value, _static);
                if (!check)
                {
                    int _level = (_static) ? 0 : n;
                    insert(_name, _type, _value, _level);
                }
                else if (check == 11)
                {
                    int _level = (_static) ? 0 : n;
                    if (!_level) insert(_name, _type, _value, _level);
                    else
                        throw(InvalidDeclaration("INSERT" + line));
                }
                else
                    throw(InvalidInstruction("INSERT " + line));
            }
            else if (tmp == "ASSIGN")
            {
                string line;
                string _name = "", _value = "";
                getline(file, line);
                assignToken(line, _name, _value);
                assign(_name, _value, n);
            }
            else if (tmp == "BEGIN")
            {
                begin(file, n + 1);
            }
            else if (tmp == "END")
            {
                end(n);
                return;
            }
            else if (tmp == "LOOKUP")
            {
                string _name;
                getline(file, _name);
                _name.erase(0, 1);
                if (regex_match(_name, regex("[a-z]\\w*")))
                    lookup(_name, n + 1);
                else throw InvalidInstruction(tmp + " " + _name);
            }
            else if (tmp == "PRINT")
            {
                if (!root) continue;
                else
                {
                    print(root);
                    cout << endl;
                }
            }
            else
            {
                throw(InvalidInstruction(tmp));
            }
        }
        throw UnclosedBlock(n);
    }    
}
