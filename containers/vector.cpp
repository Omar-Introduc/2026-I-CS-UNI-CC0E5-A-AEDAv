#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include "vector.h"

using namespace std;

void DemoVector(){
    Vector<T1> v1(10);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(-1);
    v1.push_back(4);
    cout << v1.toString() << endl;
    cout << v1 << endl;
    // cout << "hola" << 5 << endl;
    // cout.operator<<("hola")
    // ==============
    //           cout << 5 << endl;
    //           =========
    //                cout << endl;

    Vector<string> v2(10);
    v2.push_back("Hola");
    v2.push_back("Mundo");
    v2.push_back("!");
    cout << v2 << endl;
    cout << v2.toString() << endl;

    ofstream of("temp.txt");
    of << v1 << endl;
    of << v2 << endl;
    // of.close();
    
}

void DemoReadVector() {
    ifstream in("temp.txt");

    Vector<T1> v1_recuperado(2);
    Vector<string> v2_recuperado(3);
    Vector<string> v3_recuperado(4);

    in >> v1_recuperado >> v2_recuperado;

    cout << "t1 : " << v1_recuperado << endl;
    cout << "str : " << v2_recuperado << endl;
    cout << "str : " << v3_recuperado << endl;
}