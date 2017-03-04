#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <fstream>
#include <cstdlib>
#include <stdio.h>

int weight = 0; //global variable that calculates weight of json file

std::string outputStr;

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
    Number(float D) : Num(D){}
    float Num;
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

void skip(char*&, char*&);
Value* Parse(char*&, char*&);
Value* parse_Array(char*&, char*&);
Value* parse_Num(char*&, char*&);
Value* parse_Object(char*&, char*&);
Value* parse_String(char*&, char*&);
Value* parse_True(char*&, char*&);
Value* parse_False(char*&, char*&);
Value* parse_NULL(char*&, char*&);

Value* Parse(char*& f, char*& l)
{
    Value* parseVal;

    //while parseVal is not returned
    while(true)
    {
        if(*f == 't') //if *char = true;
        {
            parseVal = parse_True(f,l);
            weight++;
            return parseVal;
        }
        else if(*f == 'f') //if *char = false;
        {
            parseVal = parse_False(f,l);
            weight++;
            return parseVal;
        }
        else if(*f == 'n')  //if *char = null;
        {
            parseVal = parse_NULL(f,l);
            weight++;
            return parseVal;
        }
        else if(*f == '"')  //if *char = ";
        {
            weight++;
            printf("string called\n");
            parseVal = parse_String(f,l);
            return parseVal;
        }
         else if(*f == '[')  //if *char = [;
        {
            printf("array called\n");
            weight++;
            parseVal = parse_Array(f,l);
            return parseVal;
        }
        else if(isdigit(*f))
        {
           parseVal = parse_Num(f,l);
           weight++;
           //skip(f,l);
           return parseVal;
        }
        else if(*f == '{')
        {
            printf("object called\n");
            weight++;
            parseVal = parse_Object(f,l);
            return parseVal;
        }
        else
        {
            skip(f, l);
        }

    }

}

Value* parse_Array(char*& f, char*& l)
{
    assert(*f == '[');
    f++;

    Array * a = new Array();

    while(*f != ']')
    {
        Value* v = Parse(f,l);

        a->push_back(v);
    }

    if(*f == ']')
    {
    std::cout << "found ]" << std::endl;
    }

    return a;

}

Value* parse_Object(char*& f, char*& l)
{
    assert(*f == '{');
    Object * o = new Object();

    skip(f,l);

    while(*f!='}')
    {

        if(*f == '"')
        {
            printf("found quotation!\n");
            f++;

            std::string str;

            while(*f != '"')
            {
                str += *f;
                f++;
            }

            std::cout << str << std::endl;

            weight++;

            f++;

            o->emplace(str, Parse(f,l));
        }
        else
        {
           skip(f,l);
        }

    }

    return o;
}

Value* parse_Num(char*& f, char*& l)
{
    std::string temp;

    while(isdigit(*f) || *f == '.')
    {
        temp += *f;
        f++;
    }

    Number * n = new Number(atof(temp.c_str()));

    std::cout << n->Num  << std::endl;

    return n;

}

Value* parse_String(char*& f, char*& l)
{
    String * str = new String();

    f++;

    while(*f != '"')
    {
        str+=*f;
        std::cout << *f;
        f++;
    }

    f++;

    return str;

}

Value* parse_True(char*& f, char*& l)
{
    Bool * b = new Bool(true);

    while(*f != 'e')
    {
        f++;
    }
    f++;

    return b;
}

Value* parse_False(char*& f, char*& l)
{
    Bool * b = new Bool(true);

    while(*f != 'e')
    {
        f++;
    }
    f++;

    return b;
}

Value* parse_NULL(char*& f, char*& l)
{
    Value* v = NULL;

    while(*f != 'l')
    {
        f++;
    }
    f++;

    return v;
}

void skip(char*& f, char*& l)
{
    std::cout << "skip called" << std::endl;

    f++;

    while(isspace(*f) || *f == '\n' || *f == '\t' || *f == ',' || *f == ':' || *f == ' ')
    {
        printf("true\n");
        f++;
    }
}

int main(int argc, char *argv[])
{
    //takes a file name as an argument
    //std::ifstream file(argv[1]);
    std::ifstream file("jsonFile.json");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    if(!file)
    {
        std::cout << "File not found\n";
        return 0;
    }

    //std::string str = argv[1];

    char * first = &str[0];
    char * last = &str[str.size()];

    char* f = first;
    char* l = last;

    Value* parsedJson = Parse(f,l);

    std::cout << "weight: " << weight << std::endl << outputStr;

    std::cout << str;

    return 0;
}
