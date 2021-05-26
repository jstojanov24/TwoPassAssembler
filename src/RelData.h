

#ifndef RELDATA_H
#define RELDATA_H
#include <iostream>
#include <fstream>
using namespace std;


class RelData {
private:
    int addr;
    int type; //0-aps ; 1-pcrel
    int val;
public:
    RelData(int addr1,int t1, int v1);
    virtual ~RelData();
    void printRelData();
    void printRelData(ofstream& f);


};

#endif /* RELDATA_H */

