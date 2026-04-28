#ifndef __CIRCULARDOUBLELINKEDLIST_H__
#define __CIRCULARDOUBLELINKEDLIST_H__

#include "doublelinkedlist.h"

template <typename T>
struct AscendingCDLLTrait : BaseTrait<T, less<T>>{
    using Node = DLLNode<T>;
};

template <typename T>
struct DescendingCDLLTrait : BaseTrait<T, greater<T>>{
    using Node = DLLNode<T>;
};

template <typename Container>
class CDLLForwardIterator : public general_iterator<Container, CDLLForwardIterator<Container>> {
public:
    using MySelf = CDLLForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
private:
    Node *m_start;
    bool  m_started;
public:
    CDLLForwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode), m_start(pNode), m_started(false) {}
    CDLLForwardIterator(Container *pContainer, Node *pNode, bool sentinel)
        : Parent(pContainer, pNode), m_start(nullptr), m_started(sentinel) {}

    MySelf& operator++() {
        if (this->m_pNode) {
            m_started = true;
            this->m_pNode = this->m_pNode->getNext();
            if (this->m_pNode == m_start)
                this->m_pNode = nullptr;
        }
        return *this;
    }
};

template <typename Container>
class CDLLBackwardIterator : public general_iterator<Container, CDLLBackwardIterator<Container>> {
public:
    using MySelf = CDLLBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
private:
    Node *m_start;
    bool  m_started;
public:
    CDLLBackwardIterator(Container *pContainer, Node *pNode)
        : Parent(pContainer, pNode), m_start(pNode), m_started(false) {}
    CDLLBackwardIterator(Container *pContainer, Node *pNode, bool sentinel)
        : Parent(pContainer, pNode), m_start(nullptr), m_started(sentinel) {}

    MySelf& operator++() {
        if (this->m_pNode) {
            m_started = true;
            this->m_pNode = this->m_pNode->getPrev();
            if (this->m_pNode == m_start)
                this->m_pNode = nullptr;
        }
        return *this;
    }
};

template <typename Trait>
class CircularDoubleLinkedList : public LinkedList<Trait> {
public:
    using value_type        = typename Trait::value_type;
    using Node              = typename Trait::Node;
    using Comp              = typename Trait::Comp;
    using MySelf            = CircularDoubleLinkedList<Trait>;
    using forward_iterator  = CDLLForwardIterator<MySelf>;
    using backward_iterator = CDLLBackwardIterator<MySelf>;
    friend forward_iterator;
    friend backward_iterator;

private:
    void internal_insert(const value_type &value, Ref ref);

public:
    CircularDoubleLinkedList() : LinkedList<Trait>() {}

    CircularDoubleLinkedList(const CircularDoubleLinkedList &other);

    CircularDoubleLinkedList(CircularDoubleLinkedList &&other);

    CircularDoubleLinkedList& operator=(const CircularDoubleLinkedList &other);

    CircularDoubleLinkedList& operator=(CircularDoubleLinkedList &&other);

    virtual ~CircularDoubleLinkedList();

    void clear();

    void    push_front(value_type value, Ref ref) override;
    void    push_back(value_type value, Ref ref) override;
    void    insert(const value_type &value, Ref ref) override;
    std::tuple<value_type, Ref> pop_front() override;
    std::tuple<value_type, Ref> pop_back() override;
    value_type& operator[](size_t index) override;
    size_t  size() const override;

    forward_iterator  begin()  { return forward_iterator(this, this->m_pRoot); }
    forward_iterator  end()    { return forward_iterator(this, nullptr, true); }
    backward_iterator rbegin() { return backward_iterator(this, this->m_tail); }
    backward_iterator rend()   { return backward_iterator(this, nullptr, true); }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (this->m_size == 0) return;
        for (auto &item : *this)
            func(item, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&...args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (this->m_size == 0) return;
        for (auto it = rbegin(); it != rend(); ++it)
            func(*it, std::forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void circularForEach(size_t vueltas, Func func, Args &&...args) {
        unique_lock<shared_mutex> lock(this->m_mtx);
        if (!this->m_pRoot || vueltas == 0) return;
        Node  *act   = this->m_pRoot;
        size_t pasos = this->m_size * vueltas;
        for (size_t i = 0; i < pasos; ++i) {
            func(act->getDataRef(), std::forward<Args>(args)...);
            act = act->getNext();
        }
    }

    friend ostream& operator<<(ostream &os, const CircularDoubleLinkedList &list) {
        shared_lock<shared_mutex> lock(list.m_mtx);
        os << "[";
        if (list.m_pRoot) {
            Node *act = list.m_pRoot;
            do {
                os << "(" << act->getData() << "," << act->getRef() << ")";
                act = act->getNext();
                if (act != list.m_pRoot) os << ",";
            } while (act != list.m_pRoot);
        }
        os << "]";
        if (list.m_pRoot)
            os << " ->root(" << list.m_pRoot->getData() << "," << list.m_pRoot->getRef() << ")";
        return os;
    }

    friend istream& operator>>(istream &is, CircularDoubleLinkedList &list) {
        char ch;
        if (!(is >> ch) || ch != '[') {
            is.clear(ios_base::failbit);
            return is;
        }
        value_type val;
        Ref        ref;
        char       comma, parenClose;
        while (is >> ch && ch != ']')
            if (ch == '(')
                if (is >> val >> comma >> ref >> parenClose)
                    if (comma == ',' && parenClose == ')')
                        list.insert(val, ref);
        is.ignore(numeric_limits<streamsize>::max(), '\n');
        return is;
    }
};

// T: Adaptar la insercion (internal_insert)
template <typename Trait>
void CircularDoubleLinkedList<Trait>::internal_insert(const value_type &value, Ref ref) {
    Node *newNode = new Node(value, ref);
    this->m_size++;

    if (!this->m_pRoot) {
        newNode->setNext(newNode);
        newNode->setPrev(newNode);
        this->m_pRoot = newNode;
        this->m_tail  = newNode;
        return;
    }

    if (this->m_comp(value, this->m_pRoot->getDataRef())) {
        newNode->setNext(this->m_pRoot);
        newNode->setPrev(this->m_tail);
        this->m_pRoot->setPrev(newNode);
        this->m_tail->setNext(newNode);
        this->m_pRoot = newNode;
        return;
    }

    Node *act = this->m_pRoot;
    while (act->getNext() != this->m_pRoot && !this->m_comp(value, act->getNext()->getDataRef())) {
        act = act->getNext();
    }

    Node *siguiente = act->getNext();
    newNode->setNext(siguiente);
    newNode->setPrev(act);
    act->setNext(newNode);
    siguiente->setPrev(newNode);

    if (act == this->m_tail)
        this->m_tail = newNode;
}

template <typename Trait>
CircularDoubleLinkedList<Trait>::CircularDoubleLinkedList(const CircularDoubleLinkedList &other) : LinkedList<Trait>() {
    shared_lock<shared_mutex> lock(other.m_mtx);
    if (!other.m_pRoot) return;
    Node *curr = other.m_pRoot;
    do {
        push_back(curr->getData(), curr->getRef());
        curr = curr->getNext();
    } while (curr != other.m_pRoot);
}

template <typename Trait>
CircularDoubleLinkedList<Trait>::CircularDoubleLinkedList(CircularDoubleLinkedList &&other) : LinkedList<Trait>() {
    unique_lock<shared_mutex> lock(other.m_mtx);
    this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
    this->m_tail  = std::exchange(other.m_tail, nullptr);
    this->m_size  = std::exchange(other.m_size, 0);
}

template <typename Trait>
CircularDoubleLinkedList<Trait>& CircularDoubleLinkedList<Trait>::operator=(const CircularDoubleLinkedList &other) {
    if (this != &other) {
        clear();
        shared_lock<shared_mutex> lock(other.m_mtx);
        if (!other.m_pRoot) return *this;
        Node *curr = other.m_pRoot;
        do {
            push_back(curr->getData(), curr->getRef());
            curr = curr->getNext();
        } while (curr != other.m_pRoot);
    }
    return *this;
}

template <typename Trait>
CircularDoubleLinkedList<Trait>& CircularDoubleLinkedList<Trait>::operator=(CircularDoubleLinkedList &&other) {
    if (this != &other) {
        clear();
        unique_lock<shared_mutex> lock(other.m_mtx);
        this->m_pRoot = std::exchange(other.m_pRoot, nullptr);
        this->m_tail  = std::exchange(other.m_tail, nullptr);
        this->m_size  = std::exchange(other.m_size, 0);
    }
    return *this;
}

template <typename Trait>
CircularDoubleLinkedList<Trait>::~CircularDoubleLinkedList() {
    clear();
}

template <typename Trait>
void CircularDoubleLinkedList<Trait>::clear() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_pRoot) return;

    this->m_tail->setNext(nullptr);
    this->m_pRoot->setPrev(nullptr);

    Node *curr = this->m_pRoot;
    while (curr) {
        Node *next = curr->getNext();
        delete curr;
        curr = next;
    }

    this->m_pRoot = nullptr;
    this->m_tail  = nullptr;
    this->m_size  = 0;
}

// T: Adaptar la insercion (push_front)
template <typename Trait>
void CircularDoubleLinkedList<Trait>::push_front(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    Node *newNode = new Node(value, ref);
    if (!this->m_pRoot) {
        newNode->setNext(newNode);
        newNode->setPrev(newNode);
        this->m_pRoot = newNode;
        this->m_tail  = newNode;
    } else {
        newNode->setNext(this->m_pRoot);
        newNode->setPrev(this->m_tail);
        this->m_pRoot->setPrev(newNode);
        this->m_tail->setNext(newNode);
        this->m_pRoot = newNode;
    }
    this->m_size++;
}

// T: Adaptar la insercion (push_back)
template <typename Trait>
void CircularDoubleLinkedList<Trait>::push_back(value_type value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    Node *newNode = new Node(value, ref);
    if (!this->m_pRoot) {
        newNode->setNext(newNode);
        newNode->setPrev(newNode);
        this->m_pRoot = newNode;
        this->m_tail  = newNode;
    } else {
        newNode->setNext(this->m_pRoot);
        newNode->setPrev(this->m_tail);
        this->m_tail->setNext(newNode);
        this->m_pRoot->setPrev(newNode);
        this->m_tail = newNode;
    }
    this->m_size++;
}

// T: Adaptar la insercion (insert)
template <typename Trait>
void CircularDoubleLinkedList<Trait>::insert(const value_type &value, Ref ref) {
    unique_lock<shared_mutex> lock(this->m_mtx);
    internal_insert(value, ref);
}

template <typename Trait>
std::tuple<typename CircularDoubleLinkedList<Trait>::value_type, Ref> CircularDoubleLinkedList<Trait>::pop_front() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_pRoot) throw runtime_error("La lista esta vacia");

    Node *temp  = this->m_pRoot;
    auto result = std::make_tuple(temp->getData(), temp->getRef());

    if (this->m_size == 1) {
        this->m_pRoot = nullptr;
        this->m_tail  = nullptr;
    } else {
        this->m_pRoot = temp->getNext();
        this->m_pRoot->setPrev(this->m_tail);
        this->m_tail->setNext(this->m_pRoot);
    }

    delete temp;
    this->m_size--;
    return result;
}

template <typename Trait>
std::tuple<typename CircularDoubleLinkedList<Trait>::value_type, Ref> CircularDoubleLinkedList<Trait>::pop_back() {
    unique_lock<shared_mutex> lock(this->m_mtx);
    if (!this->m_pRoot) throw runtime_error("La lista esta vacia");

    auto result = std::make_tuple(this->m_tail->getData(), this->m_tail->getRef());

    if (this->m_size == 1) {
        delete this->m_tail;
        this->m_pRoot = nullptr;
        this->m_tail  = nullptr;
    } else {
        Node *prev = this->m_tail->getPrev();
        delete this->m_tail;
        this->m_tail = prev;
        this->m_tail->setNext(this->m_pRoot);
        this->m_pRoot->setPrev(this->m_tail);
    }

    this->m_size--;
    return result;
}

template <typename Trait>
typename CircularDoubleLinkedList<Trait>::value_type& CircularDoubleLinkedList<Trait>::operator[](size_t index) {
    shared_lock<shared_mutex> lock(this->m_mtx);
    if (index >= this->m_size) throw out_of_range("Indice fuera de rango");
    Node *act = this->m_pRoot;
    for (size_t i = 0; i < index; ++i)
        act = act->getNext();
    return act->getDataRef();
}

template <typename Trait>
size_t CircularDoubleLinkedList<Trait>::size() const {
    shared_lock<shared_mutex> lock(this->m_mtx);
    return this->m_size;
}

#endif // __CIRCULARDOUBLELINKEDLIST_H__
