#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <cstdlib>

int weight = 0;

struct Value
{
    virtual ~Value() = default;
    Value() = default;
};

struct Null : Value
{
};

struct Bool : Value
{
    Bool(bool B) : Val(B){}
    bool Val;
};

struct Number : Value
{
    Number(double D) : Num(D){}
    double Num;
};

struct String : Value, std::string
{
    using std::string::string;
};

struct Array : Value, std::vector<Value*>
{
    using std::vector<Value*>::vector;

    ~Array()
    {
        for(Value* v: *this)
        {
            delete v;
        }
    }
};

struct Object : Value, std::unordered_map<std::string, Value*>
{
    using std::unordered_map<std::string, Value*>::unordered_map;
};

struct jsonText
{
    jsonText()
    {
        f = nullptr;
        l = nullptr;
    }

    jsonText(char* first, char* last)
    {
        f = first;
        l = last;
    }
    char* f;
    char* l;
};

char* skip(char*, char*);
Value* Parse(char*, char*);
Value* parse_Array(char*, char*);
Value* parse_Num(char*, char*);
Value* parse_Object(char*, char*);
Value* parse_String(char*, char*);

Value* Parse(char* f, char* l)
{
    Value* parseVal;

    if(*f == 't')
    {
        Bool * b = new Bool(true);
        parseVal = b;
        weight++;
    }
    else if(*f == 'f')
    {
        Bool * b = new Bool(false);
        parseVal = b;
        weight++;
    }
    else if(*f == 'n')
    {
        parseVal = NULL;
        weight++;
    }
    else if(*f == '"')
    {
        weight++;
        parseVal = parse_String(f,l);
    }
     else if(*f == '[')
    {
        parseVal = parse_Array(f,l);
        weight++;
    }
    else if(isdigit(*f))
    {
       parseVal = parse_Num(f,l);
       //weight++;
    }
    else if(*f == '{')
    {
        parseVal = parse_Object(f,l);
        weight++;
    }
    else
    {
        f = skip(f,l);
    }

    return parseVal;

}

Value* parse_Array(char* f, char* l)
{
    assert(*f == '[');
    f++;

    Array * a = new Array();

    while(*f != ']')
    {
        Value* v = Parse(f,l);

        a->push_back(v);

        if(*f == ',')
        {
            f++;
        }
        else
        {
            f = skip(f,l);
        }
    }

    return a;

}

Value* parse_Object(char* f, char* l)
{
    assert(*f == '{');
    f++;

    Object * o = new Object();

    while(*f != '}')
    {
       if(*f == '"')
       {
            f++;

            std::string str;

            while(*f != '"')
            {
                str += *f;
                f++;
            }
            o->emplace(str, Parse(f,l));
            f++;
       }
       else
       {
            f = skip(f,l);
       }
    }

    return o;

}

Value* parse_Num(char* f, char* l)
{
    char * temp;

    while(isdigit(*f) || *f == '.')
    {
        temp += *f;
        f++;
    }

    Number * n = new Number(std::atof(temp));

    weight++;

    return n;

}

Value* parse_String(char* f, char* l)
{
    String * str = new String();

    f++;

    while(*f != '"')
    {
        if(*f == '\\')
        {
            f+=2;
        }
        else
        {
            str+=*f;
            f++;
        }
    }
    f = skip(f,l);
    return str;

}

char* skip(char* f, char* l)
{
    f++;

    while(f != l && std::isspace(*f))
    {
        f++;
    }

    return f;
}

int main(int argc, char *argv[])
{
    std::ifstream file("jsonFile.json");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    char * first = &str[0];
    char * last = &str[str.size()];

    jsonText jsonFileText;
    char* f = first;
    char* l = last;

    Value* parsedJson = Parse(f,l);

    std::cout << "weight: "<< weight;

    return 0;
}
