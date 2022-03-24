//
// Created by yjs on 2022/1/2.
//
#include <iostream>
#include <fstream>


using namespace std;


int main(){
    pid_t status;
    string tmpName="img.base64";
    status = system(("base64 zh1.jpg > "+tmpName).c_str());
    if (-1 == status)
    {
        printf("system error!");
    }
    fstream in;
    in.open(tmpName.c_str(),std::ios::in);
    in.seekg(0,std::ios_base::end);
    streampos sp=in.tellg();
    int size=sp;
    char *buffer=(char *) malloc(sizeof(char )*size);
    in.seekg(0,std::ios_base::beg);

    in.read(buffer,size);
    cout << buffer<<endl;
    if(remove(tmpName.c_str())!=0){
        cerr<< "rm failed  . . ."<<endl;

    }


    return 0;
}