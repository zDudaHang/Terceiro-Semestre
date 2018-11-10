/// Copyright 2018 Maria Eduarda de Melo Hang

#ifndef STRUCTURES_LINKED_STACK_H
#define STRUCTURES_LINKED_STACK_H

#include <cstdint>
#include <stdexcept>  // C++ Exceptions

namespace structures {

template<typename T>
//! Classe de Pilha Encadeada
class LinkedStack {
 public:
    //! Construtor
    LinkedStack();

    //! Destrutor
    ~LinkedStack();

    //! Limpa a pilha
    void clear();

    //! Empilha: Coloca um novo elemento no topo
    void push(const T& data);

    //! Desempilha: Tira o último elemento e retorna o dado dele
    T pop();

    //! Retorna o dado do topo
    T& top() const;

    //! Testa se a pilha esta vazia
    bool empty() const;

    //! Retorna o tamanho da pilha
    std::size_t size() const;

 private:
    //! Classe do Elemento da Pilha Encadeada
    class Node {
     public:
        //! Construtor para colocar o dado
        explicit Node(const T& data):
            data_{data}
        {}

        //! Construtor com dado e proximo
        Node(const T& data, Node* next):
            data_{data},
            next_{next}
        {}

        //! Getter do Dado
        T& data() {  // getter: dado
            return data_;
        }

        //! Getter-Constante do Dado
        const T& data() const {  // getter const: dado
            return data_;
        }

        Node* next() {  // getter: próximo
            return next_;
        }

        const Node* next() const {  // getter const: próximo
            return next_;
        }

        void next(Node* node) {  // setter: próximo
            next_ = node;
        }

     private:
        //! Valor guardado pelo elemento
        T data_;
        //! Ponteiro para o proximo elemento
        Node* next_;
    };
    //! Topo da pilha
    Node* top_;

    //! Tamanho da Pilha
    std::size_t size_;
};

}  // namespace structures //

template <typename T>
structures::LinkedStack<T>::LinkedStack() {
    top_ = nullptr;
    size_ = 0;
}

template <typename T>
structures::LinkedStack<T>::~LinkedStack() {
    clear();
}

template <typename T>
void structures::LinkedStack<T>::clear() {
    while (!empty()) {
        pop();
    }
}

template <typename T>
void structures::LinkedStack<T>::push(const T& data) {
    Node* new_node = new Node(data, nullptr);
    if (new_node == nullptr) {
        throw std::out_of_range("Full Stack!");
    }
    if (!empty()) {
        new_node->next(top_);
    }
    top_ = new_node;
    size_++;
}

template <typename T>
T structures::LinkedStack<T>::pop() {
    if (empty()) {
        throw std::out_of_range("Empty Stack!");
    }
    Node* out_node = top_;
    T return_data = out_node->data();
    top_ = top_->next();
    if (size_ == 1) {
        top_ = nullptr;
    }
    size_--;
    delete(out_node);
    return return_data;
}

template <typename T>
T& structures::LinkedStack<T>::top() const {
    if (empty()) {
        throw std::out_of_range("Empty Stack!");
    }
    return top_->data();
}

template <typename T>
bool structures::LinkedStack<T>::empty() const {
    return (size_ == 0);
}

template <typename T>
std::size_t structures::LinkedStack<T>::size() const {
    return size_;
}

#endif
