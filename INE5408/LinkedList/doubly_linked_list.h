//! Copyright [2018] <Maria Eduarda de Melo Hang>

#ifndef STRUCTURES_DOUBLY_LINKED_LIST_H
#define STRUCTURES_DOUBLY_LINKED_LIST_H

#include <cstdint>
#include <stdexcept>


namespace structures {

template<typename T>
//! Classe de Lista Duplamente Encadeada
class DoublyLinkedList {
 public:
    //! Construtor
    DoublyLinkedList();

    //! Destrutor
    ~DoublyLinkedList();

    //! Limpa Lista
    void clear();

    //! Insere no final da lista
    void push_back(const T& data);

    //! Insere no inicio
    void push_front(const T& data);

    //! Insere em qualquer posicao
    void insert(const T& data, std::size_t index);

    //! Insere em uma ordem especifica (a>b)
    void insert_sorted(const T& data);

    //! Retira de uma posicao
    T pop(std::size_t index);

    //! Retira do final
    T pop_back();

    //! Retira do inicio
    T pop_front();

    //! Retira um elemento especifico
    void remove(const T& data);

    //! Verifica se a lista esta vazia
    bool empty() const;

    //! Verifica se o elemento esta na lista
    bool contains(const T& data) const;

    //! Retorna o dado do indice escolhido
    //! Checa os limites
    T& at(std::size_t index);

    //! Retorna o dado do indice escolhido - Constante
    //! Checa os limites
    const T& at(std::size_t index) const;

    //! Encontra o indice do dado
    std::size_t find(const T& data) const;

    //! Tamanho
    std::size_t size() const;

 private:
    //! Classe do Elemento
    class Node {
     public:
        //! Construtor com dado
        explicit Node(const T& data) :
            data_{data}
        {}

        //! Construtor com dado e proximo
        Node(const T& data, Node* next) :
            data_{data},
            next_{next}
        {}

        //! Construtor com dado, anterior e proximo
        Node(const T& data, Node* prev, Node* next) :
            data_{data},
            prev_{prev},
            next_{next}
        {}

        //! Getter do Dado
        T& data() {
            return data_;
        }

        //! Getter-Constante do Dado
        const T& data() const {
            return data_;
        }

        //! Getter do Anterior
        Node* prev() {
            return prev_;
        }

        //! Getter-Constante do Anterior
        const Node* prev() const {
            return prev_;
        }

        //! Setter do Anterior
        void prev(Node* node) {
            prev_ = node;
        }

        //! Getter do Proximo
        Node* next() {
            return next_;
        }

        //! Getter-Constante do Proximo
        const Node* next() const {
            return next_;
        }

        //! Setter do proximo
        void next(Node* node) {
            next_ = node;
        }

     private:
        //! Dado do Elemento
        T data_;

        //! Anterior do Elemento
        Node* prev_;

        //! Proximo do Elemento
        Node* next_;
    };

    //! Inicio da Lista
    Node* head;

    //! Fim da Lista
    Node* tail;
    //! Tamanho da Lista
    std::size_t size_;
};
}  // namespace structures //

template <typename T>
structures::DoublyLinkedList<T>::DoublyLinkedList() {
    size_ = 0;
    head = nullptr;
    tail = nullptr;
}

template <typename T>
structures::DoublyLinkedList<T>::~DoublyLinkedList() {
    clear();
}

template <typename T>
void structures::DoublyLinkedList<T>::clear() {
    while (!empty()) {
        pop_front();
    }
}

template <typename T>
void structures::DoublyLinkedList<T>::push_back(const T& data) {
    // Node(const T& data, Node* prev, Node* next)
    Node* new_node = new Node(data, nullptr, nullptr);
    if (new_node == nullptr) {
        throw std::out_of_range("Full Doubly Linked List");
    }
    if (empty()) {
        head = new_node;
    } else {
        tail->next(new_node);
        new_node->prev(tail);
    }
    tail = new_node;
    size_++;
}

template <typename T>
void structures::DoublyLinkedList<T>::push_front(const T& data) {
    // Node(const T& data, Node* prev, Node* next)
    Node* new_node = new Node(data, nullptr, nullptr);
    if (new_node == nullptr) {
        throw std::out_of_range("Full Doubly Linked List");
    }
    if (empty()) {
        tail = new_node;
    } else {
        head->prev(new_node);
        new_node->next(head);
    }
    head = new_node;
    size_++;
}

template <typename T>
void structures::DoublyLinkedList<T>::insert(const T& data, std::size_t index) {
    // Node(const T& data, Node* prev, Node* next)
    if (index > size_) {
        throw std::out_of_range("Invalid Index");
    }
    if (index == size_) {
        push_back(data);
    } else {
        if (index == 0) {
            push_front(data);
        } else {
            Node* new_node = new Node(data, nullptr, nullptr);
            if (new_node == nullptr) {
                delete(new_node);
                throw std::out_of_range("Full Doubly Linked List");
            }
            if (empty()) {
                head = new_node;
                tail = new_node;
            } else {
                Node* it = nullptr;
                size_t interval = size_ / 2;
                if (index <= interval) {
                    it = head;
                    for (size_t i = 0; i < index; i++) {
                        it = it->next();
                    }
                } else {
                    it = tail;
                    for (size_t i = 0; i < (size_ - index -1); i++) {
                        it = it->prev();
                    }
                }
                it->prev()->next(new_node);
                new_node->next(it);
                new_node->prev(it->prev());
                it->prev(new_node);  // Eu perderia a referencia do anterior
                size_++;
            }
        }
    }
}

template <typename T>
void structures::DoublyLinkedList<T>::insert_sorted(const T& data) {
    if (empty()) {
        push_front(data);
    } else {
        Node* it = head;
        size_t index = 0;
        while (index < size_ && it != nullptr && data > it->data()) {
            index++;
            it = it->next();
        }
        insert(data, index);
    }
}

template <typename T>
T structures::DoublyLinkedList<T>::pop(std::size_t index) {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    if (index >= size_) {
        throw std::out_of_range("Invalid Index");
    }
    if (index == 0) {
        return pop_front();
    } else {
        if (index == size_ - 1) {
            return pop_back();
        } else {
            Node* node_out = nullptr;
            T return_data;
            if (size_ == 1) {
                node_out = head;
                return_data = node_out->data();
                tail = nullptr;
                head = nullptr;
            } else {
                size_t interval = size_ / 2;
                if (index <= interval) {
                    node_out = head;
                    for (size_t i = 0; i < index; i++) {
                        node_out = node_out->next();
                    }
                } else {
                    node_out = tail;
                    for (size_t i = 0; i < (size_ - index -1); i++) {
                        node_out = node_out->prev();
                    }
                }
                return_data = node_out->data();
                node_out->next()->prev(node_out->prev());
                node_out->prev()->next(node_out->next());
                size_--;
                delete(node_out);
            }
            return return_data;
        }
    }
}

template <typename T>
T structures::DoublyLinkedList<T>::pop_back() {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    Node* node_out = tail;
    T return_data = tail->data();
    if (size_ == 1) {
        tail = nullptr;
        head = nullptr;
    } else {
        node_out->prev()->next(nullptr);
        tail = node_out->prev();
    }
    size_--;
    delete(node_out);
    return return_data;
}

template <typename T>
T structures::DoublyLinkedList<T>::pop_front() {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    Node* node_out = head;
    T return_data = node_out->data();
    if (size_ == 1) {
        tail = nullptr;
        head = nullptr;
    } else {
        node_out->next()->prev(nullptr);
        head = node_out->next();
    }
    size_--;
    delete(node_out);
    return return_data;
}

template <typename T>
void structures::DoublyLinkedList<T>::remove(const T& data) {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    if (!contains(data)) {
        throw std::out_of_range("Was not found");
    }
    size_t index = find(data);
    printf("Index recebido no remove = %ld\n", index);
    pop(index);
}

template <typename T>
bool structures::DoublyLinkedList<T>::empty() const {
    return (size_ == 0);
}

template <typename T>
bool structures::DoublyLinkedList<T>::contains(const T& data) const {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    Node* it = head;
    for (size_t i = 0; i < size_; i++) {
        if (it->data() == data) {
            return true;
        }
        it = it->next();
    }
    return false;
}

template <typename T>
T& structures::DoublyLinkedList<T>::at(std::size_t index) {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    if (index >= size_) {
        throw std::out_of_range("Invalid Index");
    }
    Node* it = nullptr;
    size_t interval = (size_)/ 2;
    if (index <= interval) {
        it = head;
        for (size_t i = 0; i < index; i++) {
            it = it->next();
        }
    } else {
        it = tail;
        for (size_t i = 0; i < (size_ - index - 1); i++) {
            it = it->prev();
        }
    }
    return it->data();
}

template <typename T>
const T& structures::DoublyLinkedList<T>::at(std::size_t index) const {
    if (empty()) {
        throw std::out_of_range("Empty Doubly Linked List");
    }
    if (index >= size_) {
        throw std::out_of_range("Invalid Index");
    }
    Node* it = nullptr;
    size_t interval = size_ / 2;
    if (index <= interval) {
        it = head;
        for (size_t i = 0; i < index; i++) {
            it = it->next();
        }
    } else {
        it = tail;
        for (size_t i = 0; i < (size_ - index - 1); i++) {
            it = it->prev();
        }
    }
    return it->data();
}

template <typename T>
std::size_t structures::DoublyLinkedList<T>::find(const T& data) const {
    Node* it = head;
    for (size_t i = 0; i < size_; i++) {
        if (it->data() == data) {
            return i;
        }
        it = it->next();
    }
    return size_;
}

template <typename T>
std::size_t structures::DoublyLinkedList<T>::size() const {
    return size_;
}

#endif
