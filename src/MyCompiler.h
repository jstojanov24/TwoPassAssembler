

#ifndef MYCOMPILER_H
#define MYCOMPILER_H
#include "MyTable.h"
#include "RelTable.h"
#include "RelData.h"
#include <fstream>
#include <iostream>
#include <regex>

class MyCompiler {
private:
    MyTable* myTable;
    int lc;
    std::string inputType;
    bool wasLabel; //da oznaci da je labela il u preth redu il u novom
    bool wasTextSection;
    bool wasDataSection;
    bool wasBssSection;
    bool oneWord; 
    string currentSection;
    RelTable* relTable1; //text rtable
    RelTable* relTable2; //data rtable
    std::vector<string> binCodeText;
    std::vector<string> binCodeData;
 
    void addNewSymbol(Symbol* s);
    void dealWithDirectives(string& s,string& s2);
    int parseInput(string& s);
    void defInput(string& word, string& line);
    void dirInput(string& word, string& line);
    void sectionInput(string& word);
    void labelInput(string& word);
    void instructionInput(string& word,string& line);
    void equExpression(string& line);
    void secondRunDir(string& word,string& line);
    void secondRunInstructions(string& word,string& line);
    void addZero(string& sec,int num);
    void insertData(string& type,string& val);
    string toBinaryString(int a);
    string getOpCode(string s);
    string fillZero(string s,int f);
    string toHexStringHelper(string s);
    string toHexStringNegative(int a);
   
public:
    MyCompiler();
    virtual ~MyCompiler();
    void firstRun(ifstream& in);
    void printSymbolTable();
    int returnLocationCounter();
    void secondRun(ifstream& in);
    string toHexString(string s);
    void printBinaryCode();
    void printRelocationTable();
    void printElfFormat(ofstream& f);
    
private:

};

#endif /* MYCOMPILER_H */

