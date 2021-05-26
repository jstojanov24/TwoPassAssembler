

/* 
 * File:   Symbol.h
 * Author: etf
 *
 * Created on June 4, 2019, 9:33 AM
 */

#ifndef SYMBOL_H
#define SYMBOL_H

using namespace std;
#include <iostream>
#include <string>
#include <fstream>
 /*enum SymbolType{
    GLOBAL, LOCAL
};*/
class Symbol {
private:
    static int id;
    int idS;
    string name;
    string section;
    int value;
    string type; //local global
    int size;
    int isSection; //0-nije sekcija 1-jeste sekcija
    long startAddress; //plus dodaj flegove za sekcije
    int equFlag; // jednak 1 kada je simbol uveden .equ direktivom
    string a1;
    string a2; // da se naznaci od kojih simbola ili vrednosti je dobijan novi simbol
    string a3;
    int op1; // 0->+  1->-
    int op2;
    
    
public:
    Symbol();
    Symbol(const Symbol& orig);
    Symbol(string name1,string section1,int val, string st,int flag);
    virtual ~Symbol();
    void setValue(int val);
    int getValue();
    string getType();
    void setSize(int size);
    string getSection();
    int getSize();
    int getisSection();
    void setStartAddress(long a);
    int getId();
    string getName();
    void printSymbol();
    void setGlobal();
    void setSection(string& sec);
    void setEquFlag();
    int getEquFlag();
    void setOperand(int i,string s);
    string getOperand(int i);
    void setOperation(int i,int op); //
    int getOperation(int i);
    void printSymbolFile(ofstream& f);
    
    


};

#endif /* SYMBOL_H */

