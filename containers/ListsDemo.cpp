#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "../types.h"
#include "linkedlist.h"
#include "circularlinkedlist.h"
#include "doublelinkedlist.h"
#include "circulardoublelinkedlist.h"

using namespace std;

template <typename Container>
void DemoList(Container& list, string fileName){
    list.insert(28, 15);
    list.insert(17, 25);
    list.insert(8, 35);
    list.insert(4, 45);
    list.insert(35, 55);
    cout << list << endl;
    
    // Grabar la lista en un archivo
    ofstream os(fileName);
    os << list << endl;
    
    // Leer la lista desde un archivo
    ifstream is(fileName);
    is >> list;
    cout << list << endl;
}

void LinkedListDemo(){
    cout << "\n--- LinkedList Demo ---" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;
    DemoList(list, "AscLL.txt");
    LinkedList<DescendingLinkedListTrait<T1>> list2;
    DemoList(list2, "DescLL.txt");
}

// T: Hacer un Demo
void CircularLinkedListDemo(){
    cout << "\n--- CircularLinkedList Demo ---" << endl;
    CircularLinkedList<AscendingCLLTrait<T1>> list;
    DemoList(list, "AscCLL.txt");
    
}

// T: Hacer un Demo
void DoubleLinkedListDemo(){
    cout << "\n--- DoubleLinkedList Demo ---" << endl;
    
    DoubleLinkedList<AscendingDLLTrait<T1>> list;
    DemoList(list, "AscDLL.txt");
    
    DoubleLinkedList<DescendingDLLTrait<T1>> list2;
    DemoList(list2, "DescDLL.txt");
    
    cout << "Iterador Forward (for-each nativo):" << endl;
    for (auto& item : list) {
        cout << item << " ";
    }
    cout << endl;
    
    cout << "Iterador Backward (rbegin/rend):" << endl;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    cout << "ReverseForEach con lambda:" << endl;
    list.ReverseForEach([](auto& elem, ostream& os) {
        os << elem << " ";
    }, std::ref(cout));
    cout << endl;
    
    cout << "Pop back (O(1) gracias a m_prev):" << endl;
    auto [val, ref] = list.pop_back();
    cout << "Extraido: (" << val << "," << ref << ")" << endl;
    cout << "Lista despues del pop_back: " << list << endl;
    
    DoubleLinkedList<AscendingDLLTrait<T1>> listCopy(list);
    cout << "Copy: " << listCopy << endl;
    
    DoubleLinkedList<AscendingDLLTrait<T1>> listMoved(std::move(listCopy));
    cout << "Moved: " << listMoved << endl;
    cout << "Original despues de move (vacia): " << listCopy << endl;
}

// T: Hacer un Demo
void CircularDoubleLinkedListDemo(){
    cout << "\n--- CircularDoubleLinkedList Demo ---" << endl;
    
    // Demo con ordenamiento ascendente
    CircularDoubleLinkedList<AscendingCDLLTrait<T1>> list;
    DemoList(list, "AscCDLL.txt");
}

void TestConcurrencia() {
    cout << "\nTEST DE CONCURRENCIA" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;

    auto worker = [&list](int thread_id) {
        for(int i = 0; i < 1000; i++) {
            list.push_front(i, thread_id);
        }
    };

    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    thread t4(worker, 4);
    thread t5(worker, 5);

    t1.join(); t2.join(); t3.join(); t4.join(); t5.join();

    cout << "Se lanzaron 5 hilos insertando 1000 elementos simultaneamente." << endl;
    cout << "Tamano de la lista (Esperado 5000): " << list.size() << endl;
    if(list.size() == 5000) {
        cout << "ESTADO: EXITO - El shared_mutex previno condiciones de carrera perfectamente." << endl;
    } else {
        cout << "ESTADO: FALLO - Hubo corrupcion de memoria." << endl;
    }
}

void TestOperators() {
    cout << "\nTEST DE OPERADORES" << endl;
    LinkedList<AscendingLinkedListTrait<T1>> list;
    
    cout << "Simulando lectura desde formato: [(10, 100), (20, 200), (30, 300)]" << endl;
    stringstream simulador_input("[(10, 100), (20, 200), (30, 300)]");
    simulador_input >> list;

    cout << "Lista luego de la lectura (operator<<): " << list << endl;
    cout << "Accediendo al indice [0] (operator[]): Dato -> " << list[0] << endl;
    cout << "Accediendo al indice [2] (operator[]): Dato -> " << list[2] << endl;
}

void ListsDemo(){
    LinkedListDemo();
    CircularLinkedListDemo();
    DoubleLinkedListDemo();
    CircularDoubleLinkedListDemo();
    TestConcurrencia();
    TestOperators();
    cout << "\n=== FIN DE LAS PRUEBAS ===" << endl;
}