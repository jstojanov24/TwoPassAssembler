

#include "MyCompiler.h"

#include <regex> 

MyCompiler::MyCompiler() {
    this->myTable=new MyTable();
    this->lc=0;
    this->wasLabel=false;
    this->currentSection="begin";
    this->oneWord=false;
    this->inputType="start";
    this->wasBssSection=false;
    this->wasTextSection=false;
    this->wasDataSection=false;
    this->relTable1=new RelTable("text");
    this->relTable2=new RelTable("data");  
}


MyCompiler::~MyCompiler() {
    delete this->myTable;
}

void MyCompiler::firstRun(ifstream& in){
    //in.open("ulaz.txt");
    int endFlag=0;
    string line;
    string word;
    string line2;
    string delimiter1=" ";
    while(getline(in,line)){
        //cout<<line<<endl;
        line2=line;
        if(line2.compare("\n")==0) continue;
        while(line2.front()==' ') line2.erase(0,1);
        if(line2.compare("")==0) continue;
        int pos=line.find(delimiter1);
        if(pos!=std::string::npos){
            word=line.substr(0,pos);
            cout<<word<<endl;
            line2.erase(0,pos+1);
            while(line2.front()==' ') line2.erase(0,1);
            cout<<line2<<endl;
        }
        else {
            word=line;
            cout<<word<<endl;
            line2="";
            oneWord=true;
        }
        int res=this->parseInput(word);
        if(res==1) {
            cout<<"LABELA";
            if(oneWord==true || line2.compare("")==0) {
                cout<<"SAMA LABELA";
                oneWord=false;
                this->labelInput(word);
                
              }
            else { //nije sama labela
                this->labelInput(word);
                string word2;
                int pos2=line2.find(delimiter1);
                if(pos2!=std::string::npos){
                word2=line2.substr(0,pos2);
                 cout<<word2<<endl;
                 line2.erase(0,pos2+1);
                 while(line2.front()==' ') line2.erase(0,1);
                 cout<<line2<<endl;    
               }//znaci da je nakon labele samo ejdna rec
                else {//jedna rec i word2 je samo line2
                    word2=line2;
                    line2="";
                 }
                 int res2=this->parseInput(word2);
                 if(res2==1) {
                     cout<<"ERROR!Label after Label!!";
                     std::exit(1);
                 }
                 else if(res2==2){
                     cout<<"INSTRUKCIJA";
                     if(this->currentSection.compare("text")==0){
                               this->instructionInput(word2,line2);}
                     else {
                         cout<<"ERROR!Instruction outside  text section !"<<endl;
                         std::exit(1);
                     }
                 }
                 else if(res2==3){
                     cout<<"SEKCIJA";
                     if(line2.compare("")==0) this->sectionInput(line2);
                     else {
                         cout<<"Error!Section directive must be alone!";
                         std::exit(1);
                       }}
                 else if(res2==4){
                     if(line2.compare("")==0){
                         cout<<"ERROR!Directive without value!";
                         std::exit(1);
                     }
                     else this->dirInput(word2,line2);
                     
                 }
                 else if(res2==5){
                     if(line2.compare("")==0){
                         cout<<"ERROR!Symbol name missing!!";
                         std::exit(1);
                     }
                     this->defInput(word2,line2);
                 }
                 else if(res2==6){
                     cout<<"END!";
                     endFlag=1;
                     
                 }
                 else {
                     cout<<"SYNTAX ERROR!";
                     std::exit(1);
                 }   
                
            } 
        }
        else if (res==2){ 
            cout<< "INSTRUKCIJA";
            if(line2.compare("")==0) cout<<"JEDAN BAJT";
            if(this->currentSection.compare("text")==0)
            {this->instructionInput(word,line2);}
            else {
                cout<<"ERROR!Instruction outside text section!"<<endl;
                std::exit(1);
            }
        }
        else if(res==3) {
            cout<<"SEKCIJA";
            if(oneWord==false){
                if(line2.compare("")!=0){
                    std::cout<<"ERROR! Section directive must be alone in a line!";
                    std::exit(1);
                }
            }else oneWord=false;
          this->sectionInput(word);  
        }
         else if (res==4){ 
            cout<<"DIREKTIVA";
            if(oneWord|| line2.compare("")==0) {
                if(line2=="") cout<<"PRAZAN STRING ";
                std::cout<<"ERROR!Directive without value!! ";
                std::exit(1);  
            }
            this->dirInput(word,line2);
        }
        else if(res==5) {
            cout<<"DEFINICIJA"<<"\t";
            if(oneWord || line2.compare("")==0){
                oneWord=false;
                if(line2=="") cout<<"PRAZAN STRING ";
                std::cout<<"ERROR!Symbol name missing!";
                std::exit(1);  
            }
        this->inputType="def"; 
        this->defInput(word,line2); 
      }
        else if(res==6){
            cout<<"END!";
            endFlag=1;
            
        }
        else if (res==0) {  
                cout<<"ERROR!Couldn't detect any directives,instructions or definitions!!";
                std::exit(1);         
        }
        
        cout<<endl; 
        if(endFlag==1) break;
        
    }
    if(endFlag==0){
        cout<<"ERROR! File missing .end!";
        std::exit(1);
    }
    else {
        if(this->currentSection.compare("text")==0){
            this->myTable->getSymbol("text")->setSize(lc);
        }
        else if(this->currentSection.compare("data")==0){
            this->myTable->getSymbol("data")->setSize(lc);
        }
        else this->myTable->getSymbol("bss")->setSize(lc);
    }
    in.close();
    this->myTable->resolveSymbols();
}
void MyCompiler::addNewSymbol(Symbol* s){
    this->myTable->addSymbol(s);
}
void MyCompiler::dealWithDirectives(string& s,string& s2){
    
    
}
int MyCompiler::parseInput(string& s){
    std::regex instruction{ "(int|add|sub|mul|div|cmp|and|or|xor|not|test|push|pop|call|iret|ret|mov|shl|shr|jmp|jeq|jne|jgt|xchg|halt)" };
    std::regex label{ "([a-zA-Z][a-zA-Z0-9_]*):" };
    std::regex sections{ "(\\.)(text|data|bss)" };
    std::regex directives{ "\\.(align|word|byte|skip|equ)" };
    std::regex definitions{ "\\.(extern|global)"};
    if(std::regex_match(s,label)) return 1;
    if(std::regex_match(s,instruction)) return 2;
    if(std::regex_match(s,sections)) return 3;
    if(std::regex_match(s,directives)) return 4;
    if(std::regex_match(s,definitions)) return 5;
    if(s.compare(".end")==0) return 6;
    else return 0;
    
    return 1;
}
void MyCompiler::defInput(string& word, string& line){
    string word2;
    string delimiter=",";
     bool symExists=false;
     bool loop=true;
     int pos=0;
     int len=0;
     
    if(word.compare(".extern")==0){ //extern simbol
         pos=line.find(delimiter);
        if(pos==std::string::npos){ //samo jedan simbol
            while(line.front()==' ') line.erase(0,1);
            len=line.size();
             while(line.rfind(" ")==--len) line.erase(len,len+1);
            symExists=this->myTable->symbolExistsName(line);
            if(symExists){
                cout<<"ERROR!!!Multiple definitions of symbol!";
                std::exit(2);
            }
            else {//pravimo novi simbol
                Symbol * sym=new Symbol(line,"eund",0,"global",0); 
                this->myTable->addSymbol(sym);   
            }   
         }
        
        else {//vise simbola
            
        while(loop){
        pos=line.find(delimiter);
        if(pos!=std::string::npos){
            word2=line.substr(0,pos); 
            line.erase(0,pos+1);
            while(line.front()==' ') line.erase(0,1);
            len=word2.size();
            while(word2.rfind(" ")==--len) word2.erase(len,len+1);
            
            symExists=this->myTable->symbolExistsName(word2);
            if(symExists){
                 cout<<"ERROR!!!Multiple definitions of symbol!";
                std::exit(2);
            }
            else {//pravimo novi simbol
                Symbol * sym=new Symbol(word2,"eund",0,"global",0); //prazan string znaci da ne znamo sekciju
                this->myTable->addSymbol(sym);   
            }   
        }
        else {
            while(line.front()==' ') line.erase(0,1);
            if(line=="" ){
                loop=false;
                cout<<"SYNTAX ERROR!";
                std::exit(1);
            }
            else {
                loop=false;
                len=line.size();
                while(line.rfind(" ")==--len) line.erase(len,len+1);
                symExists=this->myTable->symbolExistsName(line);
               if(symExists){
                cout<<"ERROR!!!Multiple definitions of symbol!";
                std::exit(2);
                 }
                 else {
                Symbol * sym=new Symbol(line,"eund",0,"global",0);
                this->myTable->addSymbol(sym);    }   
             }
           }
        }
      } 
        
        
        
        
        
    }
    else {//za global granu
        
         symExists=false;
         loop=true;
        
        pos=line.find(delimiter);
        if(pos==std::string::npos){ //samo jedan simbol
            while(line.front()==' ') line.erase(0,1);
            len=line.size();
            while(line.rfind(" ")==--len) line.erase(len,len+1);
            symExists=this->myTable->symbolExistsName(line);
            if(symExists){
                this->myTable->updateToGlobal(line); //updejt na global
            }
            else {
                Symbol * sym=new Symbol(line,"gund",0,"global",0); 
                this->myTable->addSymbol(sym);   
            }   
         }
        
        else {//vise simbola
            
        while(loop){
         
          pos=line.find(delimiter);
          if(pos!=std::string::npos){
            word2=line.substr(0,pos); 
            line.erase(0,pos+1);
            while(line.front()==' ') line.erase(0,1);
            len=word2.size();
             while(word2.rfind(" ")==--len) word2.erase(len,len+1);
            symExists=this->myTable->symbolExistsName(word2);
            if(symExists){
                this->myTable->updateToGlobal(word2); //updejt na global
            }
            else {
                Symbol * sym53=new Symbol(word2,"gund",0,"global",0); 
                this->myTable->addSymbol(sym53);  
                
              }   
        }
           else {
            while(line.front()==' ') line.erase(0,1);
            if(line=="" ){
                loop=false;
                cout<<"SYNTAX ERROR!";
                std::exit(1);
            }
            else {
                loop=false;
                len=line.size();
                while(line.rfind(" ")==--len) line.erase(len,len+1);
                symExists=this->myTable->symbolExistsName(line);
               if(symExists){
                this->myTable->updateToGlobal(line); //updejt na global
                 }
                 else {//pravimo novi simbol
                Symbol * sym=new Symbol(line,"gund",0,"global",0); //prazan string znaci da ne znamo sekciju
                this->myTable->addSymbol(sym);    }   
             }
           }
        }
      } 
        
    }
    
}
void MyCompiler::printSymbolTable(){
    this->myTable->printTable();
}
void MyCompiler::dirInput(string& word, string& line){
    int cnt=0;
    int offset=0;
    int pos=0;
    string del1=",";
    bool loop=true;
    string word2="";
    if(word.compare(".byte")==0 || word.compare(".word")==0){
             
         while(loop){
           pos=line.find(del1);
           if(pos!=std::string::npos){
            word2=line.substr(0,pos); 
            line.erase(0,pos+1);
            while(line.front()==' ') line.erase(0,1);  
            cnt++;
           }
           else {
               if(line.compare("")==0){
                   std::cout<<"ERROR!SYNTAX ERROR!";
                   std::exit(1);
               }
               loop=false;
               cnt++; 
           }
         }
           if(word.compare(".byte")==0) offset=cnt;
           else offset=cnt*2;
           this->lc+=offset;
    
    }
    else if( word.compare(".skip")==0){
        while(line.front()==' ') line.erase(0,1);
        int len=line.size();
        while(line.rfind(" ")==--len) line.erase(len,len+1);
        
        std::regex num{ ("([0-9]+)")};
        if(std::regex_match(line,num)) lc+=std::stoi(line);
        else {
            cout<"ERROR! Directive .skip needs decimal value!!!";
            std::exit(1);
        }  
    }  
    else if(word.compare(".align")==0){
        while(line.front()==' ') line.erase(0,1);
        int len2=line.size();
        while(line.rfind(" ")==--len2) line.erase(len2,len2+1);
         std::regex num2{ ("([0-9]+)")};
         int alignNum=0;
        if(std::regex_match(line,num2))  alignNum=std::stoi(line);
        else {
            cout<<"ERROR! Directive .align needs decimal value!";
            std::exit(1);
        }
         if(this->lc==0) lc=alignNum;
         else {
         int diff=this->lc%alignNum;
         if(diff!=0){
             lc+=(alignNum-diff);
                        }
           }
    }
    else if(word.compare(".equ")==0) {
        this->equExpression(line);
    }
    
    
    
}
void MyCompiler::equExpression(string& line){
    string word;
    string delimiter1=",";
    string delimiter2="+";
    string delimiter3="-";
    int pos=line.find(delimiter1);
    if(pos!=std::string::npos){
         word=line.substr(0,pos); 
         line.erase(0,pos+1);
         while(line.front()==' ') line.erase(0,1);  
    } //word ukazuje na ime simbola , a line na izraz za dodelu vrednosti 
    else {
        cout<<"ERROR!Syntax error for .equ directive!";
        std::exit(1);
    }
    if(line.compare("")==0) {
         cout<<"ERROR!Syntax error for .equ directive!";
         std::exit(1);
    }
    bool flag=this->myTable->symbolExistsName(word);
    string sec;
    bool globalFlag=false; 
    if(flag){ //simbol postoji
        sec=this->myTable->getSymbol(word)->getSection();
        if(sec.compare("gund")==0){ //glob simbol, to je ok
            globalFlag=true;
            this->myTable->getSymbol(word)->setSection(this->currentSection);
            this->myTable->getSymbol(word)->setEquFlag();
        }
        else {
            cout<<"ERROR!Multiple definitions of symbol!";
            std::exit(2);
        }
    }
    else {
        Symbol * newS=new Symbol(word,this->currentSection,0,"local",0);
        this->myTable->addSymbol(newS); 
        this->myTable->getSymbol(word)->setEquFlag();
    }
    int cnt=0;
    string word2;
    int pos2=line.find(delimiter2);
    int pos3=line.find(delimiter3);
    int pos4;
    int len;
    while(pos2!=std::string::npos || pos3!=std::string::npos){
         cnt++;
        if(pos2==std::string::npos)
        {this->myTable->getSymbol(word)->setOperation(cnt,1); pos4=pos3;}
        else if(pos3==std::string::npos) {this->myTable->getSymbol(word)->setOperation(cnt,0); pos4=pos2;}
        else {
            if(pos2>pos3){ this->myTable->getSymbol(word)->setOperation(cnt,1); pos4=pos3;}
            else {this->myTable->getSymbol(word)->setOperation(cnt,0);  pos4=pos2;}
        }
         word2=line.substr(0,pos4); //word2 je moj novi operand
         line.erase(0,pos4+1);
         while(line.front()==' ') line.erase(0,1);
         len=line.size();
         while(line.rfind(" ")==--len) line.erase(len,len+1);
         this->myTable->getSymbol(word)->setOperand(cnt,word2);    
         if(cnt==2) break;
         pos2=line.find(delimiter2);
         pos3=line.find(delimiter3);   
    }
    cnt++;
    while(line.front()==' ') line.erase(0,1);
    len=line.size();
    while(line.rfind(" ")==--len) line.erase(len,len+1);
    this->myTable->getSymbol(word)->setOperand(cnt,line);
   
}
int MyCompiler::returnLocationCounter(){
    return lc;
}
void MyCompiler::sectionInput(string& word){
    if(word.compare(".text")==0){
        if(this->wasTextSection){
            std:cout<<"ERROR!Text section previously declared!";
            std::exit(1);
        }
        else this->wasTextSection=true;
        Symbol *s1=new Symbol("text","text",0,"local",1);
        this->addNewSymbol(s1);
        if(this->currentSection.compare("data")==0){
            this->myTable->getSymbol("data")->setSize(lc);
        }
        else if(currentSection.compare("bss")==0){
            this->myTable->getSymbol("bss")->setSize(lc);
        }
        
        lc=0;
        currentSection="text";
        
        
    }else if(word.compare(".data")==0){
        
         if(this->wasDataSection){
          cout<<"ERROR!Data section previously declared!";
            std::exit(1);
        }
        else this->wasDataSection=true;
        Symbol *s2=new Symbol("data","data",0,"local",1);
        this->addNewSymbol(s2);
        if(this->currentSection.compare("text")==0){
            this->myTable->getSymbol("text")->setSize(lc);
            
        }
        else if(currentSection.compare("bss")==0){
            this->myTable->getSymbol("bss")->setSize(lc);
        }
        lc=0;
        currentSection="data";
        
    }else if(word.compare(".bss")==0){
         if(this->wasBssSection){
         cout<<"ERROR!Bss section previously declared!";
            std::exit(1);
        }
        else this->wasBssSection=true;
        Symbol *s3=new Symbol("bss","bss",0,"local",1);
        this->addNewSymbol(s3);
        if(this->currentSection.compare("data")==0){
            this->myTable->getSymbol("data")->setSize(lc);
        }
        else if(currentSection.compare("text")==0){
            this->myTable->getSymbol("text")->setSize(lc);
        }
        lc=0;
        currentSection="bss";
        
        
    }
    
}

void MyCompiler::labelInput(string& word){
    //pop back funkcija uklanja posl karakter
    word.pop_back(); //skine :
    bool flag=this->myTable->symbolExistsName(word);
    string sec;
    if(flag){ //simbol postoji
        sec=this->myTable->getSymbol(word)->getSection();
        if(sec.compare("gund")==0){ //glob simbol, to je ok
            this->myTable->getSymbol(word)->setValue(lc);
            this->myTable->getSymbol(word)->setSection(this->currentSection);
        }
        else {
            //ili extern ili simbol vec postoji u ovoj tabeli 
            cout<<"ERROR!Multiple definitions of symbol!";
            std::exit(2);
        }
    }
    else {
        // novi simbol
        Symbol * newS=new Symbol(word,this->currentSection,lc,"local",0);
        this->myTable->addSymbol(newS);
        
    }
    
    
}
void MyCompiler::instructionInput(string& word, string& line){
        int instrSize=1; //svaka instr ima bar 1B
        int regCount=0;
        string word2;
        string word1;
        string delimiter1=",";
        string delimiter2="]";
        if(line.compare("")!=0){
        while(line.front()==' ') line.erase(0,1);
        int len=line.size();
        while(line.rfind(" ")==--len) line.erase(len,len+1);}
        std::regex symb {"([a-zA-Z][a-zA-Z0-9_]*)"};
        std::regex arithm{ "(add|sub|mul|div|and|or|xor|test|cmp|mov)" };
        std::regex oneByte { "(halt|ret|iret)"};
        std::regex branch{"(jmp|jeq|jne|jgt)"};
        std::regex oneOper { "(push|pop|not)"};
        std::regex shiftInstr {"(shl|shr)"};
        std::regex num{ "([0-9]+)"};
        //std::regex instr { "(test|cmp|mov)"};
        
        if(std::regex_match(word,arithm)){
           
            instrSize=3; 
            regCount=0;
             int pos=0;
            //moram da isparsiram prvu rec
             pos=line.find(delimiter1);
             if(pos!=std::string::npos){
               word1=line.substr(0,pos); 
               line.erase(0,pos+1); //line ukazuje na 2. operand
               while(line.front()==' ') line.erase(0,1);
               
            }
             else {
                //no second operand,error
                cout<<"ERROR! WRONG FORMAT FOR  INSTRUCTION!";
                std::exit(1);
            }
            if(word1.at(0)=='r' && word1.at(1)>='0' && word1.at(1)<='7'){
                if(word1.size()<3) regCount++;
                else instrSize+=2;
            }
            else if(word1.compare("psw")==0) regCount++;
            else if(word1.at(0)=='r' && word1.at(1)>='8' && word1.size()<3){
                cout<<"Error! Only registers r0-r7 can be used!";
                std::exit(1);
            }
            else if(std::regex_match(word1,symb)){
                instrSize+=2;
            }
           else if(word1.at(0)!='[' || word1.at(1)!='r' || word1.at(2)>='8' || word1.at(3)!=']'){
                cout<<"ERROR! WRONG FORMAT FOR  INSTRUCTION!";
                std::exit(1);
            }//done with first operand
             
            if(line.compare("")==0){
                cout<<"ERROR! WRONG FORMAT FOR  INSTRUCTION!";
                std::exit(1);
             }
            if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7'){
                if(line.size()<3)
                regCount++;
            }
            else if(line.compare("psw")==0) regCount++;
            else if(line.at(0)=='r' && line.at(1)>='8' && line.size()<3){
                cout<<"Error! Only registers r0-r7 can be used!";
                std::exit(1);
                
            }
            else if(line.at(0)!='[' || line.at(1)!='r' || line.at(2)>='8' || line.at(3)!=']' ) instrSize+=2;
   
            if(regCount==0) {
                cout<<"ERROR!A/L instructions must have at least one register as operand!";
                std::exit(1);
            }
            else this->lc+=instrSize;
              
        }
        else  if(std::regex_match(word,oneByte)){
            if(line.compare("")==0){
                this->lc++;
            }
            else {
                cout<<"ERROR! WRONG INSTRUCTION FORMAT!!";
                std::exit(1);
            }
             
        }
        else if(std::regex_match(word,branch)){
            regCount=0;
            instrSize=2; 
            if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7' && line.length()==2){
                regCount++;
            }
            else if(line.at(0)=='r' && line.at(1)>='8' && line.size()<3){
                cout<<"Error! Only registers r0-r7 can be used!";
                std::exit(1);   
            }
            else if(std::regex_match(line,symb)){
                instrSize+=2;
            }
            else if(line.at(0)=='$') {
                line.erase(0,1);
                if(std::regex_match(line,symb)) instrSize+=2;
                else {
                    cout<<"ERROR! Wrong format fro branch instruction!";
                    std::exit(1);
                }
                
            }
           /* else if( line.at(0)=='['){
                if(line.at(1)!='r' ||  line.at(2)>'8' || line.at(3)!=']'){
                    cout<<"ERROR!Wrong format for branch instruction!";
                 std::exit(1);
                }
            }*/
            else {
                cout<<"ERROR!Wrong format for branch instruction!";
                std::exit(1);
            }
            this->lc+=instrSize;
        }
        else if(std::regex_match(word,oneOper)){
            
            instrSize=2;
            int pos3;
            
            if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7'){
                if(line.length()!=2){  
                  if( line.at(2)=='[' ){
                      pos3=line.find(delimiter2);
                      if(pos3!=std::string::npos){
                      instrSize+=2;   
                     }
                     else {
                        cout<<"ERROR!Brackets missing!";
                         std::exit(1);
                     }
                   }
                 }
            }
            else if(line.at(0)=='r' && line.at(1)>='8' && line.length()<3){
                cout<<"Error! Only registers r0-r7 can be used!";
                std::exit(1);   
            }
            else if( line.at(0)=='['){
                if(line.at(1)!='r' ||  line.at(2)>'8' || line.at(3)!=']'){
                    cout<<"ERROR!Wrong format  for one operand instruction!";
                 std::exit(1);
                }
            }
            else if(line.compare("psw")!=0) {
                 cout<<"ERROR!Wrong format  for one operand instruction!";
                 std::exit(1);
            }
            this->lc+=instrSize;
            
        }
        else if(word.compare("int")==0){
            instrSize=2;
            if(line.at(0)=='&'){
                line.erase(0,1);
                if(regex_match(line,symb)) instrSize+=2;
                else {
                    cout<<"Error!Wrong format for int instruction!";
                    std::exit(1);
                }
            }
            else if(regex_match(line,num)) instrSize+=2;
            else {
                cout<<"Error!Wrong format for int instruction!";
                std::exit(1); 
            }
            this->lc+=instrSize;
        }
        else if(std::regex_match(word,shiftInstr)){
             int pos4=line.find(delimiter1);
             instrSize=3;
             if(pos4!=std::string::npos){
               word1=line.substr(0,pos4); 
               line.erase(0,pos4+1); //line ukazuje na 2. operand
               while(line.front()==' ') line.erase(0,1);  
            }
             else {
                //no second operand,error
                cout<<"ERROR! WRONG FORMAT FOR SHIFT INSTRUCTION!";
                std::exit(1);
            }
            if(line.compare("")==0)  {
                cout<<"ERROR! WRONG FORMAT FOR SHIFT INSTRUCTION!";
                std::exit(1);
            }
             if(word1.at(0)!='r' || word1.at(1)>='8'|| word1.length()>=3) {
                 if(word1.compare("psw")!=0){
                 cout<<"ERROR!First operand must be register";
                 std::exit(1);}
             }
             if(line.at(0)=='&'){
                 line.erase(0,1);
                 if(!std::regex_match(line,symb)){
                     std::cout<<" SYNTAX ERROR!";
                     std::exit(1);
                 }
                 else instrSize+=2;
             }
             else if(std::regex_match(line,num)){
                 instrSize+=2;  
             }
             else {
                 std::cout<<"ERROR!Wrong format for second operand!";
                 std::exit(1);
             }
             
             this->lc+=instrSize;     
        }
        else if(word.compare("xchg")==0){
            instrSize=3;
            regCount=0;
            int pos5=line.find(delimiter1);
             if(pos5!=std::string::npos){
               word1=line.substr(0,pos5); 
               line.erase(0,pos5+1); //line ukazuje na 2. operand
               while(line.front()==' ') line.erase(0,1);  
            }
             else {
                //no second operand,error
                cout<<"ERROR! Instruction needs two operands!";
                std::exit(1);
            }
            if(line.compare("")==0)  {
                 cout<<"ERROR! Instruction needs two operands!";
                std::exit(1);
            }
            if(word1.at(0)=='r' && word1.at(1)<='7' && word1.at(1)>='0' && word1.length()==2)
                regCount++;
            else if(word1.at(0)=='r' && word1.at(1)>='8' && word1.length()==2){
                cout<<"Error! Only registers r0-r7 can be used!";
                std::exit(1);   
            }
            else if(word1.compare("psw")==0) regCount++;
            else if(word1.at(0)=='r' && word1.at(1)<='7' && word1.at(1)>='0' && word1.at(2)=='['){
                pos5=word1.find(delimiter2);
                if(pos5!=std::string::npos) instrSize+=2;
                else  {
                    cout<<"ERROR!Syntax error!";
                    std::exit(1);
                }
            }
            else if(word1.at(0)!='[' ||  word1.at(1)!='r' || word1.at(2)>='8'  || word1.at(3)!=']' || word1.length()!=4){
                cout<<"ERROR! Wrong format for first operand!";
                std::exit(1);
            }
            //done with first operand
            if(line.at(0)=='r' && line.at(1)<='7' && line.at(1)>='0' && line.length()==2)
                regCount++;
            else if(line.at(0)=='r' && line.at(1)>='8' && line.length()==2){
                cout<<"Error! Only registers r0-r7 can be used!";
                std::exit(1);   
            }
            else if(line.compare("psw")==0) regCount++;
            else if(line.at(0)=='r' && line.at(1)<='7' && line.at(1)>='0' && line.at(2)=='['){
                pos5=line.find(delimiter2);
                if(pos5!=std::string::npos) instrSize+=2;
                else  {
                    
                    cout<<"ERROR!Syntax error!";
                    std::exit(1);
                }
            }
            else if(line.at(0)!='[' ||  line.at(1)!='r' || line.at(2)>='8'  || line.at(3)!=']' || line.length()!=4){
                cout<<"ERROR! Wrong format for second operand!";
                std::exit(1);
            }
            if(regCount==0) {
                cout<<"ERROR! At least one operand must be register!";
                std::exit(1);
            }
            this->lc+=instrSize;
    
        }
        else if(word.compare("call")==0){
            instrSize=4;
            if(std::regex_match(line,symb)){
                this->lc+=instrSize; 
            }
            else {
                cout<<"Error! Wrong format for call instruction!";
                std::exit(1);
            }
        }
        
   }
void MyCompiler::secondRun(ifstream& in){
    int endFlag=0;
    this->lc=0;
    string line;
    string word;
    string line2;
    string delimiter1=" ";
    while(getline(in,line)){
        line2=line;
        if(line2.compare("\n")==0) continue;
        while(line2.front()==' ') line2.erase(0,1);
        if(line2.compare("")==0) continue;
        int pos=line.find(delimiter1);
        if(pos!=std::string::npos){
            word=line.substr(0,pos);
            cout<<word<<endl;
            line2.erase(0,pos+1);
            while(line2.front()==' ') line2.erase(0,1);
            cout<<line2<<endl;
        }
        else {
            word=line;
            cout<<word<<endl;
            line2="";
            oneWord=true;
        }
        int res=this->parseInput(word);
        if(res==1) {
            if(oneWord==true || line2.compare("")==0) {
                oneWord=false;
                cout<<"sama labela"<<endl;
              }
            else { 
                string word2;
                int pos2=line2.find(delimiter1);
                if(pos2!=std::string::npos){
                word2=line2.substr(0,pos2);
                line2.erase(0,pos2+1);
                 while(line2.front()==' ') line2.erase(0,1);
                 }
                else {
                    word2=line2;
                    line2="";
                 }
                 int res2=this->parseInput(word2);
                 
                  if(res2==2){
                      this->secondRunInstructions(word2,line2);
                 }
                 else if(res2==3){
                    
                     oneWord=false;
                     if(word2.compare(".text")==0) this->currentSection="text";
                     else if(word2.compare(".data")==0) this->currentSection="data";
                     else if(word2.compare(".bss")==0) this->currentSection="bss";
                     this->lc=0;
                    }
                 else if(res2==4){
                     if(word.compare(".equ")!=0) this->secondRunDir(word2,line2);  
                 }
                 else if(res2==6){
                    
                     endFlag=1;   
                 }  
                
            } 
        }
        else if (res==2){ 
            cout<< "INSTRUKCIJA";
            oneWord=false;
            this->secondRunInstructions(word,line2);
        }
        else if(res==3) {
            oneWord=false;
            if(word.compare(".text")==0) this->currentSection="text";
            else if(word.compare(".data")==0) this->currentSection="data";
            else if(word.compare(".bss")==0) this->currentSection="bss";
            this->lc=0;
        }
         else if (res==4){ 
            if(word.compare(".equ")!=0) this->secondRunDir(word,line2);
        }
        else if(res==5) {
             continue; //ili nista
      }
        else if(res==6){
            endFlag=1;   
        }
       
    if(endFlag==1) break;    
    }
    
    in.close();
    
}

void MyCompiler::secondRunDir(string& word, string& line){
   
    if(word.compare(".align")==0){
        while(line.front()==' ') line.erase(0,1);
        int len=line.size();
        while(line.rfind(" ")==--len) line.erase(len,len+1);
        int alignNum=std::stoi(line);
        int num=0;
        if(this->lc==0) {lc=alignNum; num=alignNum;}
        else {
         int diff=this->lc%alignNum;
         if(diff!=0){
             lc+=(alignNum-diff);
             num=alignNum-diff; } 
        }
        if(num==0) return;
        this->addZero(currentSection,num);        
    }
    else if(word.compare(".skip")==0){
        cout<<"dodajem u skip deo";
        while(line.front()==' ') line.erase(0,1);
        int len2=line.size();
        while(line.rfind(" ")==--len2) line.erase(len2,len2+1);
        int num2=std::stoi(line);
        this->lc+=num2;
        this->addZero(currentSection,num2);
    }
    else if(word.compare(".byte")==0 || word.compare(".word")==0){
        cout<<"usao sam u bajt ili word deo";
        while(line.front()==' ') line.erase(0,1);
        int len3=line.size();
        while(line.rfind(" ")==--len3) line.erase(len3,len3+1);
        bool loop=true;
        int pos;
        string del1=",";
        string word2;
        int cnt=0;
        while(loop){
           pos=line.find(del1);
           if(pos!=std::string::npos){
            word2=line.substr(0,pos); 
            line.erase(0,pos+1);
            while(line.front()==' ') line.erase(0,1);
            cnt++;
            this->insertData(word,word2);
           }
           else {
               loop=false;
               cnt++; 
               this->insertData(word,line);
           }
       }
       if(word.compare(".word")==0) lc+=2*cnt;
       else lc+=cnt;   
    }
    
}
void MyCompiler::addZero(string& sec, int num){
    int i;
    if(sec.compare("text")==0){
        for( i=0; i<num; i++) this->binCodeText.push_back("00 ");    
    }
    else if(sec.compare("data")==0){
        for( i=0; i<num; i++) this->binCodeData.push_back("00 "); 
    }
    else cout<<"NE ZNAM KOJA JE SEKCIJA!";
  
}
void MyCompiler::insertData(string& type, string& val){
    if(this->currentSection.compare("bss")==0) return;
    int num;
    string data;
    if(val.front()>=48 && val.front()<=57){//number,not symbol
        num=std::stoi(val);
        data=this->toBinaryString(num);
        if((type.compare(".byte")==0 && data.length()>8) || (type.compare(".word")==0 && data.length()>16)){
            cout<<"Error!Value is too big!";
            std::exit(1);
        }
        if(type.compare(".byte")==0) data=this->fillZero(data,8);
        else data=this->fillZero(data,16);
        data=this->toHexString(data);
        cout<<data<<"  ";
        if(currentSection.compare("text")==0){
                 this->binCodeText.push_back(data);
                  
        }
        else if(currentSection.compare("data")==0){
               this->binCodeData.push_back(data); 
              
        }
    }
    else {//symbol
       
        bool exists=this->myTable->symbolExistsName(val);
        if(exists==false){
            cout<<"ERROR!Symbol doesn't exist in symbol table!";
            std::exit(1);
        }
        num=this->myTable->getSymbol(val)->getValue();
        string sec=this->myTable->getSymbol(val)->getType();
        int ids;
        if(sec.compare("global")==0){
          
            if(type.compare(".byte")==0)
               this->addZero(currentSection,1);
            else this->addZero(currentSection,2);
            
            ids=this->myTable->getSymbol(val)->getId();
            RelData* rd=new RelData(lc,0,ids);
            if(currentSection.compare("data")==0){
               this->relTable2->addRelData(rd); 
               
            }
            else {
                this->relTable1->addRelData(rd);
                
            }
        }
        else {
            string sec4=this->myTable->getSymbol(val)->getSection();
            ids=this->myTable->getSymbol(sec4)->getId();
            data=this->toBinaryString(num);
            
            if((type.compare(".byte")==0 && data.length()>8) || (type.compare(".word")==0 && data.length()>16)){
                   cout<<"Error!Value is too big!";
                   std::exit(1);
            }
            if(type.compare(".byte")==0) data=this->fillZero(data,8);
            else data=this->fillZero(data,16);
            data=this->toHexString(data);
            
            if(currentSection.compare("text")==0){
                    this->binCodeText.push_back(data);}
            
            else if(currentSection.compare("data")==0){
                  this->binCodeData.push_back(data); 
            }
               
             RelData* rd2=new RelData(lc,0,ids);
             if(currentSection.compare("data")==0) {
                 this->relTable2->addRelData(rd2);
                 cout<<"rel zapis u data sekciji";
             }
             else {
                 this->relTable1->addRelData(rd2);  
             cout<<"rel zapis u text sekciji";}  
            }    
    }    
    
}

string MyCompiler::toBinaryString(int n){
    std::string r;
    while(n!=0) {
       r=(n%2==0 ?"0":"1")+r;
       n/=2;
    }
    return r;
}
string MyCompiler::toHexString(string s){
    int len=s.length();
    string res="";
    if(len==8){
        string s1=s.substr(0,4);
        s.erase(0,4);
        res=this->toHexStringHelper(s1);
        res=res+this->toHexStringHelper(s)+" ";
        return res;   
    }
    else {
        string s2=s.substr(0,4);
        s.erase(0,4);
        string s3=s.substr(0,4);
        s.erase(0,4);
        string s4=s.substr(0,4);
        s.erase(0,4);  //little endian,prvo saljemo s4 i s pa s2 i s3
        res=this->toHexStringHelper(s4);
        res=res+this->toHexStringHelper(s)+" ";
        res=res+this->toHexStringHelper(s2);
        res=res+this->toHexStringHelper(s3)+" ";
        return res;  
    }
    
    
}
string MyCompiler::fillZero(string s,int f){//dodaje nule za lakse odr hex string-a
    string s1=s;
    while(s1.length()<f) s1="0"+s1;
    return s1;   
}
string MyCompiler::toHexStringHelper(string s){
     if (s.compare("0000")==0) return "0";
     if (s.compare("0001")==0) return "1";
     if (s.compare("0010")==0) return "2";
     if (s.compare("0011")==0) return "3";
     if (s.compare("0100")==0) return "4";
     if (s.compare("0101")==0) return "5";
     if (s.compare("0110")==0) return "6";
     if (s.compare("0111")==0) return "7";
     if (s.compare("1000")==0) return "8";
     if (s.compare("1001")==0) return "9";
     if (s.compare("1010")==0) return "A";
     if (s.compare("1011")==0) return "B";
     if (s.compare("1100")==0) return "C";
     if (s.compare("1101")==0) return "D";
     if (s.compare("1110")==0) return "E";
     if (s.compare("1111")==0) return "F";
}

void MyCompiler::printElfFormat(ofstream& f){
    myTable->printTableFile(f);
    f<<endl;
    f<<"TEXT"<<endl;
    for(int i=0; i<this->binCodeText.size(); i++)
        f<<this->binCodeText.at(i);
    f<<endl;
    this->relTable1->printRelTable(f);
    f<<endl;

    f<<"DATA"<<endl;
    for(int i=0; i<this->binCodeData.size(); i++)
        f<<this->binCodeData.at(i);
    f<<endl;
    this->relTable2->printRelTable(f);
    f<<endl;
    
    
}
void MyCompiler::printBinaryCode(){
    cout<<"DATA"<<endl;
    for(int i=0; i<this->binCodeData.size(); i++)
        cout<<this->binCodeData.at(i);
    cout<<endl;
    cout<<"TEXT"<<endl;
    for(int j=0; j<this->binCodeText.size(); j++)
        cout<<this->binCodeText.at(j);
    cout<<endl;   
}
void MyCompiler::printRelocationTable(){
    this->relTable1->printRelTable();
    cout<<endl;
    this->relTable2->printRelTable();
    cout<<endl;
}
string MyCompiler::getOpCode(string s){
    if(s.compare("halt")==0) return "0C ";
    if(s.compare("xchg")==0) return "14 ";
    if(s.compare("int")==0)  return "1C ";
    if(s.compare("mov")==0)  return "24 ";
    if(s.compare("add")==0)  return "2C ";
    if(s.compare("sub")==0)  return "34 ";
    if(s.compare("mul")==0)  return "37 ";
    if(s.compare("div")==0)  return "44 ";
    if(s.compare("cmp")==0)  return "4C ";
    if(s.compare("not")==0)  return "54 ";
    if(s.compare("and")==0)  return "5C ";
    if(s.compare("or")==0)   return "64 ";
    if(s.compare("xor")==0)  return "6C ";
    if(s.compare("test")==0) return "74 ";
    if(s.compare("shl")==0)  return "7C ";
    if(s.compare("shr")==0)  return "84 ";
    if(s.compare("push")==0) return "8C ";
    if(s.compare("pop")==0)  return "94 ";
    if(s.compare("jmp")==0)  return "9C ";
    if(s.compare("jeq")==0)  return "A4 ";
    if(s.compare("jne")==0)  return "AC ";
    if(s.compare("jgt")==0)  return "B4 ";
    if(s.compare("call")==0) return "BC ";
    if(s.compare("ret")==0)  return "C4 ";
    if(s.compare("ret")==0)  return "CC ";
    else return "00";
 }

void MyCompiler::secondRunInstructions(string& word, string& line){
    std::regex oneByte { "(halt|ret|iret)"};
    std::regex  num{ "([0-9]+)"};
    std::regex oneOper { "(push|pop|not)"};
    std::regex symb { "([a-zA-Z][a-zA-Z0-9_]*)" };
    std::regex twoOper { "(add|sub|mul|div|and|or|xor|test|cmp|mov)" };
    std::regex branch{"(jmp|jeq|jne|jgt)"};
    
    if(line.compare("")!=0){
    while(line.front()==' ') line.erase(0,1);
    int len=line.size();
    while(line.rfind(" ")==--len) line.erase(len,len+1);
    }
    string s1;
    string s2;
    string s3;
    string s4;
    int val;
    int pos;
    int id;
    string del1=",";
    string op1;
    string op2;
    string type;
    bool exists;
    bool hasBytes=false;
    bool negVal=false;
    
    if(std::regex_match(word,oneByte)){
        s1=this->getOpCode(word);
        this->binCodeText.push_back(s1);
        lc+=1; 
    }
    else if(word.compare("int")==0){
        s1=this->getOpCode("int");
        if(std::regex_match(line,num)){//broj
            val=std::stoi(line);// na 16b
        }
        else {//simbol
            line.erase(0,1);
            exists=this->myTable->symbolExistsName(line);
            if(exists==false){
                cout<<"ERROR! Symbol doesn't exist in symbol table!";
                std::exit(1);
            }
            val=this->myTable->getSymbol(line)->getValue();  
        }
         s3=this->toBinaryString(val);
         s3=this->fillZero(s3,16);
         s3=this->toHexString(s3);
         s2="00 "; //neposredno adresiranje
         s4=s1+s2+s3;
         this->binCodeText.push_back(s4);
         this->lc+=4;
    }
    else if(word.compare("shl")==0 || word.compare("shr")==0){
        string opcode=this->getOpCode(word);
        pos=line.find(del1);
        op1=line.substr(0,pos); 
        line.erase(0,pos+1);
        while(line.front()==' ') line.erase(0,1); 
        s1="001";
        if(op1.at(0)=='p') s1=s1+"11110";
        else {
            op1.erase(0,1);
            int spom=std::stoi(op1);
            string spom2=this->toBinaryString(spom);
            spom2=this->fillZero(spom2,4);
            s1=s1+spom2+"0";
        }
        s1=this->toHexString(s1);
        s1=opcode+s1+"00 "; //drugi operand je neposredno adresiranje
        //sada nam preostaje samo da nadjemo vrednost
        
        if(std::regex_match(line,num)){
            val=std::stoi(line);
        }
        else {//simbol
            line.erase(0,1);
            exists=this->myTable->symbolExistsName(line);
            if(exists==false){
                cout<<"ERROR! Symbol doesn't exists in symbol table!";
                std::exit(1);
            }
            val=this->myTable->getSymbol(line)->getValue();  
        }
         s3=this->toBinaryString(val);
         s3=this->fillZero(s3,16);
         s3=this->toHexString(s3);
         s1=s1+s3;
         this->binCodeText.push_back(s1);
         this->lc+=5; 
    }
    else if(std::regex_match(word,oneOper)){
        int reg;
        hasBytes=false;
        int insSize=2;
        string regs;
        s1=this->getOpCode(word);
        if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7' && line.length()==2){
            s2="001";
            line.erase(0,1);
            reg=std::stoi(line);
            regs=this->toBinaryString(reg);
            regs=this->fillZero(regs,4);
            s2=s2+regs+"0";   
        }
        else if(line.at(0)=='p'){
            s2="00111110";
        }
        else if(line.at(0)=='['){
            s2="010";
            line.erase(0,2);
            line.pop_back();
            reg=std::stoi(line);
            regs=this->toBinaryString(reg);
            regs=this->fillZero(regs,4);
            s2=s2+regs+"0";
        }
        else{//reg indirektno sa pomerajem
            insSize+=2;
            hasBytes=true;
            s2="100";
            line.erase(0,1);
            string reg2=line.substr(0,1);
            reg=std::stoi(reg2);
            line.erase(0,2);
            line.pop_back(); //line mi ukazuje na operand
            regs=this->toBinaryString(reg);
            regs=this->fillZero(regs,4);
            s2=s2+regs+"0";
        }
        s2=this->toHexString(s2);
        s1=s1+s2; //prvi i drugi bajt
        if(hasBytes==true){
            if(line.at(0)=='-') {
                line.erase(0,1);
                negVal=true;
            }
            if(regex_match(line,num)){//broj
                val=std::stoi(line);
                if(negVal){
                    s3=this->toHexStringNegative(val);
                }
                else {
                s3=this->toBinaryString(val);
                s3=this->fillZero(s3,16);
                s3=this->toHexString(s3);    
                }
             }
            else {
                exists=this->myTable->symbolExistsName(line);
                if(exists==false){
                  cout<<"ERROR! Symbol doesn't exists in symbol table!";
                   std::exit(1);
                 }
                if(this->myTable->getSymbol(line)->getSection().compare("eund")==0){
                    id=this->myTable->getSymbol(line)->getId();
                    RelData* rd=new RelData(lc+2,0,id);
                    s3=" 00 00 ";
                    this->relTable1->addRelData(rd);
                }
                else{
                    val=this->myTable->getSymbol(line)->getValue();
                    s3=this->toBinaryString(val);
                    s3=this->fillZero(s3,16);
                    s3=this->toHexString(s3); 
                }
            }
            s1=s1+s3;
        }
        this->binCodeText.push_back(s1);
        this->lc+=insSize;
      }
    else if (word.compare("call")==0){
        s1=this->getOpCode(word);
        s2="00 "; 
        exists=this->myTable->symbolExistsName(line);
        if(exists==false) {
             cout<<"ERROR! Symbol doesn't exists in symbol table!";
             std::exit(1);   
        }       
        type=this->myTable->getSymbol(line)->getType();
        int ef=this->myTable->getSymbol(line)->getEquFlag();
        if(type.compare("global")==0 || ef==1){ 
            s3="00 00 ";
            id=this->myTable->getSymbol(line)->getId();    
        }
        else {
            val=this->myTable->getSymbol(line)->getValue();
            s3=this->toBinaryString(val);
            if(s3.length()>16) {
                cout<<"Error! Value is too big!";
                std::exit(1);
            }
            s3=this->fillZero(s3,16);
            s3=this->toHexString(s3);
            s4=this->myTable->getSymbol(line)->getSection();
            id=this->myTable->getSymbol(s4)->getId();    
        }
        this->relTable1->addRelData(new RelData(lc+2,0,id));
        s1=s1+s2+s3;
        this->binCodeText.push_back(s1);
        this->lc+=4;
    }
    else if(std::regex_match(word,twoOper)){
       
        s1=this->getOpCode(word);
        pos=line.find(del1);
        op1=line.substr(0,pos); 
        line.erase(0,pos+1);//line je drugi operand
        while(line.front()==' ') line.erase(0,1); 
        int isize=3;
        int cntPom=2;
        hasBytes=false;
        if(op1.compare("psw")==0){
            s1=s1+"3E ";
        }
        else if(op1.at(0)=='r' && op1.at(1)>='0' && op1.at(1)<='7' && op1.length()==2){
            op1.erase(0,1);
            val=std::stoi(op1);
            s2=this->toBinaryString(val);
            s2=this->fillZero(s2,4);
            s2="001"+s2+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
         
        }
        else if(op1.at(0)=='['){
            op1.erase(0,2);
            op1.pop_back();
            val=std::stoi(op1);
            s4=this->toBinaryString(val);
            s4=this->fillZero(s4,4);
            s2="010"+s4+"0"; 
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(std::regex_match(op1,symb)){
            isize+=2;
            s1=s1+"00 ";
            exists=this->myTable->symbolExistsName(op1);
            if(exists==false){
                cout<<"Error!Symbol doesn't exist in symbol table!";
                std::exit(1);
            }
            s3=this->myTable->getSymbol(op1)->getType();
            int eflag2=this->myTable->getSymbol(op1)->getEquFlag();
            if(s3.compare("global")==0 || eflag2==1){
                id=this->myTable->getSymbol(op1)->getId();
                s4="00 00 ";
                this->relTable1->addRelData(new RelData(lc+cntPom,0,id));
            }
            else {
                val=this->myTable->getSymbol(op1)->getValue();
                s3=this->myTable->getSymbol(op1)->getSection();
                id=this->myTable->getSymbol(s3)->getId();
                this->relTable1->addRelData(new RelData(lc+cntPom,0,id));
                s4=this->toBinaryString(val);
                s4=this->fillZero(s4,16);
                s4=this->toHexString(s4);     
            }
            s1=s1+s4;
        }
        else {//reg ind sa pomerajem
            isize+=2;
            op1.erase(0,1);
            s4=op1.substr(0,1);
            val=std::stoi(s4);
            op1.erase(0,2);
            op1.pop_back(); //op1 mi ukazuje na pomeraj 
            s4=this->toBinaryString(val);
            s4=this->fillZero(s4,4);
            s2="100"+s4+"0"; 
            s2=this->toHexString(s2);
            s1=s1+s2;
            if(op1.at(0)=='-') { op1.erase(0,1); negVal=true;}
            if(std::regex_match(op1,num)){
                val=std::stoi(op1);
                if(negVal){
                    s2=this->toHexStringNegative(val);
                    negVal=false;
                }
                else {
                s2=this->toBinaryString(val);
                if(s2.length()>16){
                    cout<<"Error!Value is too big!";
                    std:exit(1);
                }
                s2=this->fillZero(s2,16); 
                s2=this->toHexString(s2); }
            }
            else {//simbol
                
                s2="00 00 ";
                exists=this->myTable->symbolExistsName(op1);
                 if(exists==false){
                  cout<<"ERROR! Symbol doesn't exists in symbol table!";
                   std::exit(1);
                 }
                 if(this->myTable->getSymbol(op1)->getSection().compare("eund")==0){
                    id=this->myTable->getSymbol(op1)->getId();
                    RelData* rd=new RelData(lc+2,0,id);
                    s2=" 00 00 ";
                    this->relTable1->addRelData(rd);
                 }
                 else{
                    val=this->myTable->getSymbol(op1)->getValue();
                    s2=this->toBinaryString(val);
                    s2=this->fillZero(s2,16);
                    s2=this->toHexString(s2); 
                }    
            }
            s1=s1+s2;
            cntPom+=2;
        }
        cntPom++;
        if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7' && line.length()==2){
            line.erase(0,1);
            val=std::stoi(line);
            s2=this->toBinaryString(val);
            s2=this->fillZero(s2,4);
            s2="001"+s2+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(line.compare("psw")==0){
             s2="3E ";
             s1=s1+s2;
        }
        else if(line.at(0)=='['){
            line.erase(0,2);
            line.pop_back();
            val=std::stoi(line);
            s4=this->toBinaryString(val);
            s4=this->fillZero(s4,4);
            s2="010"+s4+"0";  
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7' && line.at(2)=='['){
            isize+=2;
            line.erase(0,1);
            s4=line.substr(0,1);
            val=std::stoi(s4);
            line.erase(0,2);
            line.pop_back(); //line mi ukazuje na pomeraj 
            s4=this->toBinaryString(val);
            s4=this->fillZero(s4,4);
            s2="100"+s4+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
            if(line.at(0)=='-'){
                line.erase(0,1);
                negVal=true;
            }
            if(std::regex_match(line,num)){
                val=std::stoi(line);
                if(negVal){
                    negVal=false;
                    s2=this->toHexStringNegative(val);
                }
                else {
                s2=this->toBinaryString(val);
                if(s2.length()>16){
                    cout<<"Error!Value is to big!";
                    std::exit(1);
                }
                s2=this->fillZero(s2,16);
                s2=this->toHexString(s2);  } 
            }
            else {
                 exists=this->myTable->symbolExistsName(line);
                 if(exists==false){
                   cout<<"ERROR! Symbol doesn't exists in symbol table!";
                   std::exit(1);
                 }
                 if(this->myTable->getSymbol(line)->getSection().compare("eund")==0){
                    id=this->myTable->getSymbol(line)->getId();
                    RelData* rd1=new RelData(lc+cntPom,0,id);
                    s2=" 00 00 ";
                    this->relTable1->addRelData(rd1);
                }
                else{
                    val=this->myTable->getSymbol(line)->getValue();
                    s2=this->toBinaryString(val);
                    s2=this->fillZero(s2,16);
                    s2=this->toHexString(s2); 
                }
            }
            s1=s1+s2;
            cout<<"caos";
         }

        else if(std::regex_match(line,num)){
            isize+=2;
            hasBytes=true;
            s2="00 ";
            s1=s1+s2;
           
            val=std::stoi(line);
            s2=this->toBinaryString(val);
            if(s2.length()>16){
                cout<<"Error!Value is too big!";
                std::exit(1);
            }
            s2=this->fillZero(s2,16);
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(line.at(0)=='&'){
            s2="00 ";
            s1=s1+s2;
            isize+=2;
            line.erase(0,1);
            exists=this->myTable->symbolExistsName(line);
            if(exists==false){
                cout<<"Error!Symbol doesn't exist in symbol table!";
                std::exit(1);
            }
            val=this->myTable->getSymbol(line)->getValue();
            s2=this->toBinaryString(val);
            s2=this->fillZero(s2,16);
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(line.at(0)=='*'){
            isize+=2;
            line.erase(0,1);
            if(std::regex_match(line,num)){
                s2="A0 ";
                s1=s1+s2;
                val=std::stoi(line);
                s2=this->toBinaryString(val);
                if(s2.length()>16) {
                    cout<<"Error!Value is too big!";
                    std::exit(1);
                }
                s2=this->fillZero(s2,16);
                s2=this->toHexString(s2);
                s1=s1+s2;            
            }
            else {
                cout<<"Error! Wrong number format!";
                std::exit(1);
            }
        }
        else {
            isize+=2;
            s1=s1+"00 ";
            exists=this->myTable->symbolExistsName(line);
            if(exists==false){
                cout<<"Error!Symbol doesn't exist in symbol table!";
                std::exit(1);
            }
            s3=this->myTable->getSymbol(line)->getType();
            int eflag=this->myTable->getSymbol(line)->getEquFlag();
            if(s3.compare("global")==0 || eflag==1){
                id=this->myTable->getSymbol(line)->getId();
                s4="00 00 ";
                this->relTable1->addRelData(new RelData(lc+cntPom,0,id));
            }
            else {
                val=this->myTable->getSymbol(line)->getValue();
                s3=this->myTable->getSymbol(line)->getSection();
                id=this->myTable->getSymbol(s3)->getId();
                this->relTable1->addRelData(new RelData(lc+cntPom,0,id));
                s4=this->toBinaryString(val);
                s4=this->fillZero(s4,16);
                s4=this->toHexString(s4);     
            }
            s1=s1+s4;
        }
       
        this->lc+=isize;
        this->binCodeText.push_back(s1);
    }
    else if(std::regex_match(word,branch)){
        s1=this->getOpCode(word);
        bool pcRel=false;
        int inSize=2;
        if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7' && line.length()==2){
            line.erase(0,1);
            val=std::stoi(line);
            s2=this->toBinaryString(val);
            s2=this->fillZero(s2,4);
            s2="001"+s2+"0"; 
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else {
            if(line.at(0)=='$'){//pc rel
               line.erase(0,1);
                pcRel=true;
            }
            exists=this->myTable->symbolExistsName(line);
            if(exists==false){
                cout<<"Error!Symbol doesn't exist in symbol table!";
                std::exit(1);  
            }
            s1=s1+"00 ";
            s3=this->myTable->getSymbol(line)->getType();
            int ef1=this->myTable->getSymbol(line)->getEquFlag();
            id=this->myTable->getSymbol(line)->getId();
            if(pcRel){
                if(ef1==1){
                    cout<<"ERROR!Can't use symbol defined with .equ with $symbol!";
                    std::exit(1);
                }
                if(s3.compare("global")==0){
                    s2="FE FF "; //-2
                    this->relTable1->addRelData(new RelData(lc+2,1,id));
                    s1=s1+s2;   
                }
                else {
                    s4=this->myTable->getSymbol(line)->getSection();
                    if(s4.compare(this->currentSection)==0){
                        //ne treba relokacija
                        val=this->myTable->getSymbol(line)->getValue();
                        val=val-4-lc;
                        if(val<0) {
                            val*=-1;
                            s2=this->toHexStringNegative(val);
                            
                        }
                        else {
                        s2=this->toBinaryString(val);
                        s2=this->fillZero(s2,16);
                        s2=this->toHexString(s2);
                        
                        }
                        s1=s1+s2;
                    }
                    else {
                        val=this->myTable->getSymbol(line)->getValue();
                        val-=2; 
                        if(val<0){
                            val*=-1;
                            s2=this->toHexStringNegative(val);
                        }
                        else {
                           s2=this->toBinaryString(val);
                           s2=this->fillZero(s2,16);
                           s2=this->toHexString(s2); 
                        }
                        id=this->myTable->getSymbol(s4)->getId();
                        this->relTable1->addRelData(new RelData(lc+2,1,id));
                        s1=s1+s2;
                    }
                }    
            }
            else {// aps relokacija
                if(s3.compare("global")==0){
                    s2="00 00 ";
                    this->relTable1->addRelData(new RelData(lc+2,0,id));
                    s1=s1+s2;
                }
                else {
                    s4=this->myTable->getSymbol(line)->getSection();
                    id=this->myTable->getSymbol(s4)->getId();
                    val=this->myTable->getSymbol(line)->getValue();
                    s2=this->toBinaryString(val);
                    s2=this->fillZero(s2,16);
                    s2=this->toHexString(s2);
                    s1=s1+s2;
                    this->relTable1->addRelData(new RelData(lc+2,0,id));
                }
                
            }
            inSize=4;
        }
        this->binCodeText.push_back(s1);
        this->lc+=inSize;
    }
    else if(word.compare("xchg")==0){
        s1=this->getOpCode(word);
        pos=line.find(del1);
        op1=line.substr(0,pos); 
        line.erase(0,pos+1);
        while(line.front()==' ') line.erase(0,1); 
        int sizein=3;
        if(op1.at(0)=='p') { s1=s1+"3E ";}
        else if(op1.at(0)=='r' && op1.at(1)>='0' && op1.at(1)<='7' && op1.length()==2){
            s2="001";
            op1.erase(0,1);
            val=std::stoi(op1);
            s3=this->toBinaryString(val);
            s3=this->fillZero(s3,4);
            s2=s2+s3+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(op1.at(0)=='['){
            s2="010";
            op1.erase(0,2);
            op1.pop_back();
            val=std::stoi(op1);
            s3=this->toBinaryString(val);
            s3=this->fillZero(s3,4);
            s2=s2+s3+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;    
        }
        else{ 
            sizein+=2;
            op1.erase(0,1);
            s3=op1.substr(0,1);
            val=std::stoi(s3);
            op1.erase(0,2);
            op1.pop_back(); 
            s2="100";
            s3=this->toBinaryString(val);
            s3=this->fillZero(s3,4);
            s2=s2+s3+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
            if(op1.at(0)=='-'){
                negVal=true;
                op1.erase(0,1);
            }
            if(std::regex_match(op1,num)){
                val=std::stoi(op1);
                if(negVal){
                    negVal=false;
                    s2=this->toHexStringNegative(val);
                }
                else {
                s2=this->toBinaryString(val);
                s2=this->fillZero(s2,16);
                s2=this->toHexString(s2); 
                }
                s1=s1+s2;
            }
            else {
                 exists=this->myTable->symbolExistsName(op1);
                 if(exists==false){
                  cout<<"ERROR! Symbol doesn't exist in symbol table!";
                   std::exit(1);
                 }
                 if(this->myTable->getSymbol(op1)->getSection().compare("eund")==0){
                    id=this->myTable->getSymbol(op1)->getId();
                    RelData* rd=new RelData(lc+2,0,id);
                    s2=" 00 00 ";
                    this->relTable1->addRelData(rd);
                 }
                 else{
                    val=this->myTable->getSymbol(op1)->getValue();
                    s2=this->toBinaryString(val);
                    s2=this->fillZero(s2,16);
                    s2=this->toHexString(s2); 
                    
                } 
                s1=s1+s2;          
            }
        }
        
        if(line.at(0)=='p') {s1=s1+"3E ";}
        else if(line.at(0)=='r' && line.at(1)>='0' && line.at(1)<='7' && line.length()==2){
            s2="001";
            line.erase(0,1);
            val=std::stoi(line);
            s3=this->toBinaryString(val);
            s3=this->fillZero(s3,4);
            s2=s2+s3+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
        }
        else if(line.at(0)=='['){
            s2="010";
            line.erase(0,2);
            line.pop_back();
            val=std::stoi(line);
            s3=this->toBinaryString(val);
            s3=this->fillZero(s3,4);
            s2=s2+s3+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;    
        }
        else{
            sizein+=2;
            line.erase(0,1);
            s3=line.substr(0,1);
            val=std::stoi(s3);
            line.erase(0,2);
            line.pop_back(); 
            s2="100";
            s3=this->toBinaryString(val);
            s3=this->fillZero(s3,4);
            s2=s2+s3+"0";
            s2=this->toHexString(s2);
            s1=s1+s2;
            if(line.at(0)=='-'){
                negVal=true;
                line.erase(0,1);
            }
            if(std::regex_match(line,num)){
                val=std::stoi(line);
                if(negVal){
                    negVal=false;
                    s2=this->toHexStringNegative(val);
                }
                else {
                s2=this->toBinaryString(val);
                s2=this->fillZero(s2,16);
                s2=this->toHexString(s2);
                }
                s1=s1+s2;
            }
            else {
                 exists=this->myTable->symbolExistsName(line);
                 if(exists==false){
                  cout<<"ERROR! Symbol doesn't exist in symbol table!";
                   std::exit(1);
                 }
                 if(this->myTable->getSymbol(line)->getSection().compare("eund")==0){
                    id=this->myTable->getSymbol(line)->getId();
                    RelData* rd=new RelData(lc+3,0,id);
                    s2=" 00 00 ";
                    this->relTable1->addRelData(rd);
                 }
                 else{
                    val=this->myTable->getSymbol(line)->getValue();
                    s2=this->toBinaryString(val);
                    s2=this->fillZero(s2,16);
                    s2=this->toHexString(s2); 
                    
                } 
                s1=s1+s2;          
            }
        }
        this->binCodeText.push_back(s1);
        this->lc+=sizein;
        
    }
}

string MyCompiler::toHexStringNegative(int a){
    int b=65536-a;
    string s=this->toBinaryString(b);
    if(s.length()<16) s=this->fillZero(s,16);
    s=this->toHexString(s);
    return s;    
}

    
    



