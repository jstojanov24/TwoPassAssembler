

#include "RelData.h"

RelData::RelData(int addr1,int t1, int v1) {
    this->addr=addr1;
    this->type=t1;
    this->val=v1;
}
void RelData::printRelData(){
    std::cout<<this->addr<<'\t'<<this->type<<'\t'<<this->val<<endl;
}
void RelData::printRelData(ofstream& f){
    f<<this->addr<<'\t';
    if(this->type==0) f<<"PC_16";
    else f<<"PCR_16";
    f<<'\t'<<this->val<<endl;
}



RelData::~RelData() {
}

