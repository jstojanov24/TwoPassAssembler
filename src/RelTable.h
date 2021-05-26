
#ifndef RELTABLE_H
#define RELTABLE_H

#include "RelData.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class RelTable {
private:
    std::string section;
    std::vector<RelData*> relTab;
public:
    RelTable(string sec);
    RelTable(const RelTable& orig);
    virtual ~RelTable();
    void addRelData(RelData* d);
    void printRelTable();
    void printRelTable(ofstream& f);


};

#endif /* RELTABLE_H */

