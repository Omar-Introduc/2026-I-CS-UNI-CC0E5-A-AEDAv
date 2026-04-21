#include "linkedlist.h"
using namespace std;

void LinkedListDemo(){
    LinkedList<DescendingLinkedListTrait<T1>> list;
    list.insert(1, 15);
    list.insert(2, 25);
    list.insert(3, 35);
    list.insert(4, 45);
    list.insert(5, 55);
    cout << list << endl;

    list.push_front(10, 1);
    list.push_back(20, 2);
    list.push_front(5, 0);
    list.push_back(25, 3);
    cout << list << endl;
    cout << list[0] << endl;
    cout << list[2] << endl;
    list.ForEach([](int& x) { x += 100; });
    cout << list << endl;
    LinkedList<DescendingLinkedListTrait<int>> copia(list);
    cout << copia << endl;
    
    LinkedList<DescendingLinkedListTrait<int>> movida(std::move(copia));
    cout << movida << endl;
    cout << copia << endl;
    LinkedList<DescendingLinkedListTrait<int>> asignada;
    asignada = movida; 
    cout << asignada << endl;
    
    LinkedList<DescendingLinkedListTrait<int>> asignadaMov;
    asignadaMov = std::move(asignada); 
    cout << asignadaMov << endl;
    cout << asignada << endl;
    asignadaMov.pop_front();
    cout << asignadaMov << endl;
    
    asignadaMov.pop_back();
    cout << asignadaMov << endl;
    istringstream input("999 9"); 
    input >> asignadaMov;
    cout << asignadaMov << endl;
}

void ListsDemo(){
    LinkedListDemo();
    
}
