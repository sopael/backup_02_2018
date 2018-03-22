#ifndef MyNtuple_h
#define MyNtuple_h 1

#include <string>
#include <vector>
#include <map>


using namespace std ;

class MyNtuple{

 public:

  initTree(map <string,vector> );

 private:
  
  TObjArray* _trees; 
  TTree* _Tree;
  std::string _outFileName;



};

#endif
