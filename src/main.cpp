

#include <vector>
#include <string>
//#include <cstdlib>
#include <iostream>
#include <fstream>

#include "Symbol.h"
#include "MyTable.h"
#include "MyCompiler.h"
#include "RelData.h"
#include "RelTable.h"

using namespace std;

int main(int argc, char** argv) {
    
    std::string inputFile=argv[1];
    std::string outputFile=argv[2];
    MyCompiler* mc=new MyCompiler();
    std::ifstream in;
    in.open(inputFile);
    mc->firstRun(in);
    mc->printSymbolTable();
    int ret=mc->returnLocationCounter();
    cout<<endl;
    cout<<ret;
    cout<<endl;
    in.open(inputFile);
    cout<<"SECOND RUN!"<<endl;
    mc->secondRun(in);
    mc->printBinaryCode();
    mc->printRelocationTable();
    ofstream myfile;
    myfile.open (outputFile);
    mc->printElfFormat(myfile);
    return 0;
}



