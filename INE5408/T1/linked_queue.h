/// Copyright 2018 Maria Eduarda de Melo Hang

#ifndef STRUCTURES_LINKED_QUEUE_H
#define STRUCTURES_LINKED_QUEUE_H

#include <cstdint>
#include <stdexcept>  // C++ Exceptions
#include <iostream>


namespace structures {


template<typename T>
//! Fila Encadeada: FIFO: First-In & First-Out
class LinkedQueue {
 public:
    //! Construtor
    LinkedQueue();

    //! Destrutor
    ~LinkedQueue();

    //! Limpa Fila
    void clear();

    //! Colocar um elemento no final da fila
    void enqueue(const T& data);

    //! Tirar o primeiro elemento da fila
    T dequeue();

    //! Pegar o dado do primeiro elemento
    T& front() const;

    //! Tirar o dado do ultimo elemento
    T& back() const;

    //! Verificar se esta vazia
    bool empty() const;

    //! Retorna o tamano da fila
    std::size_t size() const;

 private:
    //! Classe do Elemento da Pilha Encadeada
    class Node {
     public:
        //! Construtor para inserir o dado
        explicit Node(const T& data):
            data_{data}
        {}

        //! Construtor para inserir o dado e o proximo
        Node(const T& data, Node* next):
            data_{data},
            next_{next}
        {}

        //! Getter do dado
        T& data() {
            return data_;
        }

        //!! Getter-Constante do Dado
        const T& data() const {
            return data_;
        }

        //! Getter do Proximo
        Node* next() {
            return next_;
        }

        //! Getter-Constante do Proximo
        const Node* next() const {
            return next_;
        }

        //! Setter do Proximo
        void next(Node* node) {
            next_ = node;
        }

     private:
        //! Valor guardado pelo elemento
        T data_;
        //! Ponteiro para o proximo elemento
        Node* next_;
    };
    //! Primeiro elemento da fila
    Node* head;

    //! Ultimo elemento da fila
    Node* tail;

    //! Tamanho da fila
    std::size_t size_;
};

}  // namespace structures //

template <typename T>
structures::LinkedQueue<T>::LinkedQueue() {
    head = nullptr;
    tail = nullptr;
    size_ = 0;
}

template <typename T>
structures::LinkedQueue<T>::~LinkedQueue() {
    clear();
}

template <typename T>
void structures::LinkedQueue<T>::clear() {
    while (!empty()) {
        dequeue();
    }
}

template <typename T>
/*
1) Verificar se a alocacao deu certo (data,nullptr)
2) Vericiar se ela era vazia
2.1) Colocar o head apontando para ele
3) Fazer o next do antigo tail apontar para o new_node
4) tail = new_node
5) size++
*/
void structures::LinkedQueue<T>::enqueue(const T& data) {
    Node* new_node = new Node(data, nullptr);
    if (new_node == nullptr) {
        throw std::out_of_range("Full Queue");
    }
    if (empty()) {
        head = new_node;
    } else {
        tail->next(new_node);
    }
    tail = new_node;
    size_++;
}

template <typename T>
T structures::LinkedQueue<T>::dequeue() {
    if (empty()) {
        throw std::out_of_range("Empty Queue");
    }
    Node* out_node = head;
    T return_dada = head->data();
    if (size_ == 1) {
        tail = nullptr;
        head = nullptr;
    } else {
        head = head->next();
    }
    size_--;
    delete(out_node);
    return return_dada;
}

template <typename T>
T& structures::LinkedQueue<T>::front() const {
    if (empty()) {
        throw std::out_of_range("Empty Queue");
    }
    return head->data();
}

template <typename T>
T& structures::LinkedQueue<T>::back() const {
    if (empty()) {
        throw std::out_of_range("Empty List");
    }
    return tail->data();
}

template <typename T>
bool structures::LinkedQueue<T>::empty() const {
    return (size_ == 0);
}

template <typename T>
std::size_t structures::LinkedQueue<T>::size() const {
    return size_;
}

#endif
