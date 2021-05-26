/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyTable.h
 * Author: etf
 *
 * Created on June 4, 2019, 11:34 AM
 */

#ifndef MYTABLE_H
#define MYTABLE_H
#include "Symbol.h"
#include <vector>
#include <fstream>
using namespace std;


class MyTable {
private:
    std::vector<Symbol*> table;
    void resolveSymbol(int i);
    
public:
    MyTable();
    //MyTable(const MyTable& orig);
    virtual ~MyTable();
    void addSymbol(Symbol* s);
    bool symbolExists(Symbol* s);
    bool checkTable();
    Symbol*& getSymbol(string name);
    void printTable();
    void printTableFile(ofstream& f);
    bool symbolExistsName(string& name);  //ne mora referenca
    void updateToGlobal(string& name);
    void resolveSymbols();
    
};

#endif /* MYTABLE_H */

