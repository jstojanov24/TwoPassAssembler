/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Symbol.cpp
 * Author: etf
 * 
 * Created on June 4, 2019, 9:33 AM
 */

#include "Symbol.h"

int Symbol::id=0;
Symbol::Symbol() {
    this->idS=id++;
 
}

Symbol::Symbol(const Symbol& orig) {
}

Symbol::~Symbol() {
}
Symbol::Symbol(string name1,string section1,int val, string st,int flag){
    this->idS=id++;
    this->name=name1;
    this->section=section1;
    this->value=val;
    this->type=st;
    this->size=0;
    this->startAddress=0;
    this->isSection=flag;
    this->a1="";
    this->a2="";
    this->a3="";
    this->equFlag=0;
    this->op1=0;
    this->op2=0;
    
}

 void Symbol:: setValue(int val){
     this->value=val;
 }
    int Symbol::getValue(){
        return this->value;
    }
    string Symbol::getName(){
        return this->name;
    }
    string Symbol::getType(){
    return this->type;}
    void Symbol::setSize(int size){
        this->size=size;
    }
    string Symbol::getSection(){
        return this->section;
        
    }
    int Symbol::getSize(){
        return this->size;
    }
    int Symbol::getisSection(){
        return this->isSection;
    }
    void Symbol::setStartAddress(long a){
        this->startAddress=a;
        
    }
    int Symbol::getId(){
        return this->idS;
    }
    void Symbol::printSymbol(){
        cout<<this->name<<"-"<<this->idS<<","<<this->value<<","<<this->type<<","<<this->section;
       cout<<this->size;
       if(this->equFlag==1) cout<<a1<<","<<a2<<","<<a3;
        cout<<endl;
    }
    void Symbol::printSymbolFile(ofstream& f){
        f<<this->idS<<'\t'<<this->name<<'\t';
        if(this->section.compare("eund")==0) f<<"und";
        else f<<this->section;
        
        f<<'\t'<<this->value<<'\t';
        f<<this->type<<'\t';
        if(this->isSection==1) f<<this->size;
        f<<std::endl;
    }
    void Symbol::setGlobal(){
        this->type="global";
    }
    void Symbol::setSection(string& sec){
        this->section=sec;
    }
    void Symbol::setEquFlag(){
        this->equFlag=1;
    }
    int Symbol::getEquFlag(){
        return this->equFlag;
    }
    void Symbol::setOperand(int i, string s){
        if(i==1) this->a1=s;
        else if(i==2) this->a2=s;
        else this->a3=s;
    }
    string Symbol::getOperand(int i){
        if(i==1) return this->a1;
        else if(i==2) return this->a2;
        else return this->a3;
    }
    int Symbol::getOperation(int i){
        if(i==1) return op1;
        else return op2;
        
    }
    void Symbol::setOperation(int i, int op){
        if(i==1) this->op1=op;
        else this->op2=op;
    }
   
