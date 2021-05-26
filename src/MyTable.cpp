/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyTable.cpp
 * Author: etf
 * 
 * Created on June 4, 2019, 11:34 AM
 */

#include <vector>
#include <regex>

#include "MyTable.h"

MyTable::MyTable() {
    
    Symbol * s1=new Symbol("und","und",0,"global",0);
    this->table.push_back(s1); //initial row
}
void MyTable::addSymbol(Symbol* s){
    this->table.push_back(s);
    
}
bool MyTable::symbolExists(Symbol* s){
    bool flag=false;
    string name=s->getName();
    for(int i=0; i<table.size(); i++){
        if(name.compare(table.at(i)->getName())==0){
            flag=true;
            break;
        }
    }
    return flag;
    
}
Symbol*& MyTable::getSymbol(string name){
    //cout<<name;
    for(int i=0; i<table.size(); i++){
        if( table.at(i)->getName().compare(name)==0){
           // std::cout<<table.at(i)->getName();
            return table.at(i);
        }
        
    }
    cout<<"NISAM NASAO";
    return getSymbol("und");
    
 }
    
    

bool MyTable::checkTable(){
    bool flag=true;
    string s="und";
    for(int i=0; i<table.size(); i++){
        if(table.at(i)->getName().compare("und")!=0 && s.compare(table.at(i)->getSection())==0) {
            flag=false;
            break;
        }
    }
    return flag;
 
   
}


MyTable::~MyTable() {
    for(int i=0; i<table.size(); i++) delete table.at(i);
}
void MyTable::printTable(){
    for(int i=0; i<table.size(); i++) {
        table.at(i)->printSymbol();
    }
}
void MyTable::printTableFile(ofstream& f){
    for(int i=0; i<table.size(); i++) table.at(i)->printSymbolFile(f);
}
bool MyTable::symbolExistsName(string& name){
    bool flag=false;
    for(int i=0; i<table.size(); i++){
        if(name.compare(table.at(i)->getName())==0){
            flag=true;
            break;
        }
    }
    return flag;
    
}
void MyTable::updateToGlobal(string& name){
      
    for(int i=0; i<table.size(); i++){
        if(name.compare(table.at(i)->getName())==0){
            table.at(i)->setGlobal();
            break;
        }
    }
  
}
void MyTable::resolveSymbols(){
    string sec;
    int equF;
    for(int i=0; i<table.size(); i++){
        sec=table.at(i)->getSection();
        if(sec.compare("eund")==0) continue;
        equF=table.at(i)->getEquFlag();
        if(sec.compare("gund")==0 && equF==0){
            cout<<"Error!Definition missing for global symbol!";
            std::exit(1);}
        if(equF==1){
            this->resolveSymbol(i);
        }   
    }
    
}
void MyTable::resolveSymbol(int i){
    std::regex num{ "([0-9]+)"};
    int sum=0;
    int n1=0;
    int n2=0;
    int n3=0;
    string sec;
    bool exists=true;
    bool equFlag2=false;
    Symbol *s=this->table.at(i);
    string a1=s->getOperand(1);
    string a2=s->getOperand(2);
    string a3=s->getOperand(3);
    int op1=s->getOperation(1);
    int op2=s->getOperation(2);
    if(std::regex_match(a1,num)){
        //first operand ->num
        n1=std::stoi(a1);    
    }
    else {// first operand->another symbol
        exists=this->symbolExistsName(a1);
        sec=this->getSymbol(a1)->getSection();
        equFlag2=this->getSymbol(a1)->getEquFlag();
        if(!exists || sec.compare("eund")==0 || equFlag2==true ){
            cout<<"ERROR!Couldn't resolve symbol with id:"<<i<<endl;
            std::exit(1);
        }
        n1=this->getSymbol(a1)->getValue();    
    }
    
    if(a2.compare("")==0) { n2=0;}
    else if(std::regex_match(a2,num)){
        //first operand ->num
        n2=std::stoi(a2);    
    }
    else {// first operand->another symbol
        exists=this->symbolExistsName(a2);
        sec=this->getSymbol(a2)->getSection();
        equFlag2=this->getSymbol(a2)->getEquFlag();
        if(!exists || sec.compare("eund")==0 || equFlag2==true ){
            cout<<"ERROR!Couldn't resolve symbol with id:"<<i<<endl;
            std::exit(1);
        }
        n2=this->getSymbol(a2)->getValue();    
    }
    if(a3.compare("")==0){
        n3=0;
    }
    else if(std::regex_match(a3,num)){
        //first operand ->num
        n3=std::stoi(a3);    
    }
    else {// first operand->another symbol
        exists=this->symbolExistsName(a3);
        sec=this->getSymbol(a3)->getSection();
        equFlag2=this->getSymbol(a3)->getEquFlag();
        if(!exists || sec.compare("eund")==0 || equFlag2==true ){
            cout<<"ERROR!Couldn't resolve symbol with id:"<<i<<endl;
            std::exit(1);
        }
        n3=this->getSymbol(a3)->getValue();    
    }
    sum=n1;
    if(op1==0) sum+=n2;
    else sum-=n2;
    if(op2==0) sum+=n3;
    else sum-=n3;
    
    s->setValue(sum);  
}


