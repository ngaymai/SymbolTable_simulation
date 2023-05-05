#include "SymbolTable.h"


SymbolTable::~SymbolTable()
{
	for (int i = 0; i < m; i++)
		delete table[i];
	delete table;
}
SymbolTable::node::~node()
{
	delete[] var;
}
void SymbolTable::addTable(int m)
{
	table = new node*[m];
	for (int i = 0; i < m; i++)
		table[i] = NULL;
}

void SymbolTable::addA(int m, int c)
{
	this->m = m;
	addTable(m);
	this->c1 = c;
}

void SymbolTable::addB(int m, int c1, int c2)
{
	this->m = m;
	addTable(m);
	this->c1 = c1;
	this->c2 = c2;
}

int SymbolTable::linearHash(int key, int i)
{
	int n = key % m;
	
	if (i)	
		n = (n + c1 * i) % m;	
		
	return n;
}

int SymbolTable::quadraticHash(int key, int i)
{
	int n = key % m;

	if (i)
		n = (n % m + c1 * i + c2 * i * i) % m;

	return n;
}

int SymbolTable::doubleHash(int key, int i)
{
	int n1 = key % m;
	int n2 = 1 + key % (m - 2);

	if (i)
		n1 = (n1 + n2 * c1 * i) % m;

	return n1;
}

void SymbolTable::hashCheck(ifstream &file)
{
	string tmp;
	file >> tmp;
	if (tmp == "LINEAR")
	{
		int m, c;
		file >> tmp;
		m = stoi(tmp);
		file >> tmp;
		c = stoi(tmp);
		if (c < 1000000 && m < 1000000)
			addA(m, c);
		this->t = 1;
	}
	else if (tmp == "QUADRATIC")
	{
		int m, c1, c2;
		file >> tmp;
		m = stoi(tmp);
		file >> tmp;
		c1 = stoi(tmp);
		file >> tmp;
		c2 = stoi(tmp);

		if (c1 < 1000000 && m < 1000000 && c2 < 1000000)
			addB(m, c1, c2);
		this->t = 2;
	}
	else if (tmp == "DOUBLE")
	{
		int m, c;
		file >> tmp;
		m = stoi(tmp);
		file >> tmp;
		c = stoi(tmp);
		if (c < 1000000 && m < 1000000)
			addA(m, c);
		this->t = 3;
	}
}

int decode(string key, int level)
{
	string tmp;
	int n;
	if (level)
		tmp = to_string(level);
	for (int i = 0; i < (int)key.size(); i++)
	{
		n = char(key[i]) - 48;
		tmp = tmp + to_string(n);
	}
	n = stoi(tmp);
	return n;
}

int SymbolTable::hashFunction(int key, int i)
{
	if (t == 1)
		key = linearHash(key, i);
	else if (t == 2)
		key = quadraticHash(key, i);
	else if (t == 3)
		key = doubleHash(key, i);
	return key;
}

void assignToken(string tmp, string& _id1, string& _id2)
{
	string ex = "ASSIGN " + tmp;
	tmp.erase(0, 1);
	int pos = (int)tmp.find(" ");
	_id1 = tmp.substr(0, pos);
	tmp.erase(0, pos + 1);
	_id2 = tmp;
	regex e1("[a-z]\\w*");
	regex e2("(^[a-z]\\w*$)|(^\\d+$)|(^'( *[a-zA-Z0-9]*)*'$)|(([a-z]\\w*)\\((|(\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*))(\\,((\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*)))*\\))");
	if (regex_match(_id1, e1) && regex_match(_id2, e2))
	{
		if (_id1 == "number" || _id1 == "string" || _id1 == "true" || _id1 == "false")
			throw InvalidInstruction("ASSIGN " + _id1 + " " + _id2);
		return;
	}

	else
		throw InvalidInstruction("ASSIGN " + _id1 + " " + _id2);
}

void SymbolTable::run(string filename)
{
	ifstream file;
	file.open(filename);
	hashCheck(file);
	string tmp;
	int n = 0;	
	while (!file.eof())
	{
		file >> tmp;
		if (tmp == "INSERT")
		{
			string _id, _var;
			int _num = -1;
			regex e1("[a-z]\\w*");
			file >> _id;
			getline(file, _var);
			if (_var != "")
				_num = stoi(_var);
			if (regex_match(_id, e1))
				insert(_id, _num, n);
			else
				throw InvalidDeclaration(_id);
		}
		else if (tmp == "ASSIGN")
		{
			string line;
			string _id1, _id2;
			getline(file,line);
			assignToken(line, _id1, _id2);
			assign(_id1, _id2, n);
		}
		else if (tmp == "CALL")
		{
			string line;
			file >> line;
			call(line, n);
		}
		else if (tmp == "BEGIN")
		{
			begin(file, n + 1);
		}
		else if (tmp == "LOOKUP")
		{
			string _id;
			file >> _id;
			lookup(_id, n);
		}
		else if (tmp == "END")
		{
			Delete(n);
			return;
		}
		else
		{
			throw InvalidInstruction("tmp");
		}
	}
}

bool SymbolTable::compare(node* a, node* b)
{
	if (a->id == b->id)
	{
		if ((a->var != NULL) ^ (b->var != NULL))
			return 1;
		else if (a->num == -1 && a->level == b->level)
			return 1;
		else if (a->value == b->value && a->value != "")
		{
			if (a->num == b->num)
			{
				for (int i = 0; i < a->num; i++)
					if (a->var!=NULL && b->var!=NULL && (a->var[i] == "" || b->var[i] == "" || a->var[i] != b->var[i]))
						return 0;
				return 1;
			}
		}
	}

	return 0;	
}

void SymbolTable::insert(string _id, int _num, int _level)
{
	int key = decode(_id, _level);
	int idx = hashFunction(key, 0);
	node* ptr;
	if (_num != -1)
		ptr = new node(_id, 0, _num);
	else
		ptr = new node(_id, _level);
	int i = 0;
	int flag = idx;

	if (table[idx] != NULL)
	{
		do
		{
			if (compare(table[idx], ptr))
				throw Redeclared(_id);
			i++;
			idx = hashFunction(key, i);
		} while (table[idx] != NULL && idx != flag);
	}
	if (table[idx] != NULL && idx == flag)
	{
		if (_num == -1)
			throw Overflow("INSERT " + _id);
		else
			throw Overflow("INSERT " + _id + " " + to_string(_num));
	}
	 table[idx] = ptr;
	 cout << i << endl;
	 ptr = NULL;
	 delete ptr;
}

void SymbolTable::assign(string _id1, string _id2, int _level)
{
	regex e1("^[a-z]\\w*$");
	regex e2("^'( *[a-zA-Z0-9]*)*'$");
	regex e3("^\\d+$");
	regex e4("([a-z]\\w*)\\((|(\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*))(\\,((\\d+)|('( *[a-zA-Z0-9]*)*')|([a-z]\\w*)))*\\)");
	int cnt = 0;
	if (regex_match(_id2, e1))
	{
		node* tmp = search(_id2, _level, cnt);
		if (tmp == NULL)
			throw Undeclared(_id2);
		else
		{
			node* tmp2 = search(_id1, _level, cnt);
			if (tmp2 == NULL)
				throw Undeclared(_id1);
			else if (tmp2->value == "" && tmp->value == "")
				throw TypeCannotBeInferred("ASSIGN " + _id1 + " " + _id2);
			else if (tmp->value == "")
				tmp->value = tmp2->value;
			else if (tmp2->value == "")
				tmp2->value = tmp->value;
			else if (tmp->value != tmp2->value)
				throw TypeMismatch("ASSIGN " + _id1 + " " + _id2);
		}
	}
	else if (regex_match(_id2, e2))
	{
		node* tmp = search(_id1, _level, cnt);
		if (tmp == NULL)
			throw Undeclared(_id1);
		else
			tmp->value = "string";
	}
	else if (regex_match(_id2, e3))
	{
		node* tmp = search(_id1, _level, cnt);
		if (tmp == NULL)
			throw Undeclared(_id1);
		else
			tmp->value = "number";
	}
	else if (regex_match(_id2, e4))
	{		
		creNode(_id2, _id1, _level, cnt);
	}
	cout << cnt << endl;
}
void SymbolTable::creNode(string str, string _id,int _level, int& cnt)
{	
	regex e1("[a-z]\\w*");
	regex e2("'( *[a-zA-Z0-9]*)*'");
	regex e3("\\d+");
	int n = 0;		
	int j = 0;
	int start = (int)str.find("(");
	string _name = str.substr(0, start);	
	start++;
	int pos = (int)str.find(",", start);
	while (pos != -1)
	{
		n++;
		pos = (int)str.find(",", pos + 1);
	}
	pos = (int)str.find(")");
	n = (start == pos) ? 0 : n + 1;	
	node* ptr = find(_name, n, j);
	cnt += j;
	if (ptr == NULL)
		throw Undeclared(_name);
	else if (n>0)
	{
		string _value;
		start = (int)str.find("(");
		pos = (int)str.find(",");
		int i = 0;
		while (pos != -1)
		{
			_value = str.substr(start + 1, pos - start - 1);
			if (regex_match(_value, e3))
				_value = "number";
			else if (regex_match(_value, e2))
				_value = "string";
			else if (regex_match(_value, e1))
			{
				node* tmp = search(_value, _level, j);
				cnt += j;
				if (tmp == NULL)
					throw Undeclared(_value);
				else if (tmp->value == "")
					throw TypeCannotBeInferred("ASSIGN " + _id + " " + str);
				else
					_value = tmp->value;
			}
			ptr->var[i] = _value;
			i++;
			start = pos;
			pos = (int)str.find(",", start + 1);
		}
		pos = (int)str.find(")");
		_value = str.substr(start + 1, pos - start - 1);
		if (regex_match(_value, e3))
			_value = "number";
		else if (regex_match(_value, e2))
			_value = "string";
		else if (regex_match(_value, e1))
		{
			node* tmp = search(_value, _level, j);
			cnt += j;
			if (tmp == NULL)
				throw Undeclared(_value);
			else if (tmp->value == "")
				throw TypeCannotBeInferred("ASSIGN " + _id + " " + str);
			else
				_value = tmp->value;
		}
		ptr->var[i] = _value;
	}
	node* tmp = search(_id, _level, j);
	cnt += j;
	if (tmp != NULL)
		if (tmp->value != "")
			ptr->value = tmp->value;
		else
			throw TypeCannotBeInferred("ASSIGN " + _id + " " + str);
	else
		throw Undeclared(_id);	
}
SymbolTable::node* SymbolTable::search(string _id, int _level, int& i)
{
	if (_level < 0) return NULL;
	int key = decode(_id, _level);	
	int idx = hashFunction(key, 0);
	i = 0;
	int flag = idx;

	if (table[idx] != NULL && (table[idx]->id != _id || table[idx]->num != -1))
	{
		do
		{
			idx = hashFunction(key, ++i);
		} while (table[idx] != NULL && (table[idx]->id != _id || table[idx]->num != -1) && idx != flag);
	}
	else
		if (table[idx] != NULL && table[idx]->id == _id && table[idx]->num == -1)
			return table[idx];
		
	return search(_id, _level - 1, i);
	
}
SymbolTable::node* SymbolTable::find(string _id, int _num, int& i)
{
	int key = decode(_id, 0);
	int idx = hashFunction(key, 0);
	i = 0;
	int flag = idx;

	if (table[idx] != NULL && (table[idx]->num != _num || table[idx]->id != _id))
	{
		do
		{
			idx = hashFunction(key, ++i);
		} while (table[idx] != NULL && (table[idx]->num != _num || table[idx]->id != _id) && idx != flag);
	}
	else
		if (table[idx] != NULL && table[idx]->num == _num && table[idx]->id == _id)
			return table[idx];
		

	return NULL;
}
void SymbolTable::Delete(int _level)
{
	for (int i = 0; i < this->m; i++)
	{
		if (table[i] != NULL && table[i]->level == _level)
		{
			delete table[i];
			table[i] = NULL;
		}
	}
}
void SymbolTable::call(string str, int _level)
{
	
	regex e1("[a-z]\\w*");
	regex e2("'( *[a-zA-Z0-9]*)*'");
	regex e3("\\d+");
	int n = 0;	
	int j = 0;
	int cnt = 0;
	int start = (int)str.find("(");
	string _name = str.substr(0, start);
	start++;
	int pos = (int)str.find(",", start);
	while (pos != -1)
	{
		n++;
		pos = (int)str.find(",", pos + 1);
	}
	pos = (int)str.find(")");
	n = (start == pos) ? 0 : n + 1;
	node* ptr = find(_name, n, j);
	cnt += j;
	if (ptr == NULL)
		throw Undeclared(_name);
	else if (n > 0)
	{
		string _value;
		start = (int)str.find("(");
		pos = (int)str.find(",");
		int i = 0;
		while (pos != -1)
		{
			_value = str.substr(start + 1, pos - start - 1);
			if (regex_match(_value, e3))
				_value = "number";
			else if (regex_match(_value, e2))
				_value = "string";
			else if (regex_match(_value, e1))
			{
				node* tmp = search(_value, _level, j);
				cnt += j;
				if (tmp == NULL)
					throw Undeclared(_value);
				else if (tmp->value == "")
					throw TypeCannotBeInferred("ASSIGN " + str);
				else
					_value = tmp->value;
			}
			ptr->var[i] = _value;
			i++;
			start = pos;
			pos = (int)str.find(",", start + 1);
		}
		pos = (int)str.find(")");
		_value = str.substr(start + 1, pos - start - 1);
		
		if (regex_match(_value, e3))
			_value = "number";
		else if (regex_match(_value, e2))
			_value = "string";
		else if (regex_match(_value, e1))
		{
			node* tmp = search(_value, _level, j);
			cnt += j;
			if (tmp == NULL)
				throw Undeclared(_value);
			else if (tmp->value == "")
				throw TypeCannotBeInferred("ASSIGN " + str);
			else
				_value = tmp->value;
		}
		ptr->var[i] = _value;
	}
	
	ptr->value = "void";
	cout << cnt << endl;
		
}
void SymbolTable::begin(ifstream& file, int n)
{
	while (!file.eof())
	{
		string tmp;
		file >> tmp;
		if (tmp == "INSERT")
		{
			string _id, _var;
			int _num = -1;
			regex e1("[a-z]\\w*");
			file >> _id;
			getline(file, _var);
			if (_var != "")
				throw InvalidDeclaration(_id);
			if (regex_match(_id, e1))
				insert(_id, _num, n);
			else
				throw InvalidDeclaration(_id);
		}
		else if (tmp == "ASSIGN")
		{
			string line;
			string _id1, _id2;
			getline(file, line);
			assignToken(line, _id1, _id2);
			assign(_id1, _id2, n);

		}
		else if (tmp == "CALL")
		{
			string line;
			file >> line;
			call(line, n);
		}
		else if (tmp == "BEGIN")
		{
			begin(file, n + 1);
		}
		else if (tmp == "PRINT")
		{
			print(0, true);
		}
		else if (tmp == "LOOKUP")
		{
			string _id;
			file >> _id;
			lookup(_id, n);
		}
		else if (tmp == "END")
		{
			Delete(n);
			return;
		}
		
	}
	throw UnclosedBlock(n);
}
void SymbolTable::lookup(string _id, int _level)
{
	if (_level < 0)
		throw Undeclared(_id);
	int key = decode(_id, _level);
	int idx = hashFunction(key, 0);
	int i = 0;
	int flag = idx;
	if (table[idx] != NULL)
	{
		if (table[idx]->id == _id)
		{
			cout << idx << endl;
			return;
		}
		else
		{
			do
				idx = hashFunction(key, ++i);
			while (table[i] != NULL && table[idx]->id != _id && flag != idx);
		}
	}
	if (table[idx]!=NULL&& table[idx]->id == _id)
	{
		cout << idx << endl;
		return;
	}
	return lookup(_id, _level - 1);		
}
void SymbolTable::print(int idx, bool p)
{
	if (idx == this->m)
		return;
	else if (table[idx] != NULL)
	{
		if (p)
		{
			cout << idx << " " << table[idx]->id << "//" << table[idx]->level;
			print(idx + 1, false);
		}
		else
		{
			cout << ";" << idx << " " << table[idx]->id << "//" << table[idx]->level;
			print(idx + 1, false);
		}
	}
	if (p)
		print(idx + 1, false);
	else
		print(idx + 1, true);
}

