

#include "RelTable.h"

RelTable::RelTable(string sec) {
    this->section=sec;
}
void RelTable::addRelData(RelData* d){
    this->relTab.push_back(d);
}

void RelTable::printRelTable(){
    if(this->section.compare("data")==0) cout<<"#.rel.data" << std::endl;		
    else cout<<"#.rel.text"<<std::endl;
    cout<< "#Offset" << '\t'<< "Type" << '\t'<< "Value" << std::endl;
    for(int i=0; i<this->relTab.size(); i++)
        this->relTab.at(i)->printRelData();
	
}
void RelTable::printRelTable(ofstream& f){
     if(this->section.compare("data")==0) f<<"#.rel.data" << std::endl;		
     else f<<"#.rel.text"<<std::endl;
     
     f<< "#Offset" << '\t'<< "Type" << '\t'<< "Value" << std::endl;
     for(int i=0; i<this->relTab.size(); i++){
        this->relTab.at(i)->printRelData(f);
     }
    
}

RelTable::RelTable(const RelTable& orig) {
}

RelTable::~RelTable() {
    for(int i=0; i<this->relTab.size(); i++){
        delete this->relTab.at(i);
    }
}

