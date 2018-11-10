/// Copyright 2018 Maria Eduarda de Melo Hang

#ifndef STRUCTURES_ARRAY_STACK_H
#define STRUCTURES_ARRAY_STACK_H

#include <cstdint>  // std::size_t
#include <stdexcept>  // C++ exceptions
#include <memory>  // for std::unique_ptr

namespace structures {

template <typename T>
/// Classe Pilha
class ArrayStack {
 public:
    /// Construtor sem parametros
    ArrayStack();

    /// Construtor com o parametro para determinar o tamanho da pilha
    explicit ArrayStack(std::size_t max);

    /// Destrutor da classe
    ~ArrayStack();

    /// Metodo de empilhamento
    void push(const T& data);

    /// Metodo de desempilhar
    T pop();

    /// Metodo para pegar o ultimo item da pilha
    T& top();

    /// Limpar a pilha
    void clear();

    /// Retorna a quantidade de elementos atual da pilha
    std::size_t size();

    /// Retorna o tamanho maximo da pilha
    std::size_t max_size();

    /// Retorna verdadeiro se a pilha estiver vazia
    bool empty();

    /// Retorna verdadeiro se a pilha estiver cheia
    bool full();

 private:
    // std::unique_ptr<T[]> contents;
    T* contents;

    int top_;
    std::size_t max_size_;

    static const auto DEFAULT_SIZE = 10u;
};

}  // namespace structures //


template <typename T>
structures::ArrayStack<T>::ArrayStack() {
    this->ArrayStack(this->DEFAULT_SIZE);
}

template <typename T>
structures::ArrayStack<T>::ArrayStack(std::size_t max) {
    this->max_size_ = max;
    this->top_ = -1;
    this->contents = new T[max];
}

template <typename T>
structures::ArrayStack<T>::~ArrayStack() {
    delete[] this->contents;
}

template <typename T>
void structures::ArrayStack<T>::push(const T& data) {
    if (this->full()) {
        throw std::out_of_range("Full Stack");
    } else {
        this->top_ += 1;
        this->contents[this->top_] = data;
    }
}

template <typename T>
T structures::ArrayStack<T>::pop() {
    if (this->empty()) {
        throw std::out_of_range("Empty Stack");
    } else {
        this->top_ -= 1;
        return this->contents[this->top_ + 1];
    }
}

template <typename T>
T&  structures::ArrayStack<T>::top() {
    if (this->empty()) {
        throw std::out_of_range("Empty stack");
    } else {
        return this->contents[this->top_];
    }
}

template <typename T>
void  structures::ArrayStack<T>::clear() {
    this->top_ = -1;
}

template <typename T>
std::size_t structures::ArrayStack<T>::size() {
    return (this->top_ + 1);
}

template <typename T>
std::size_t  structures::ArrayStack<T>::max_size() {
    return this->max_size_;
}

template <typename T>
bool  structures::ArrayStack<T>::empty() {
    if (this->top_ == -1) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
bool  structures::ArrayStack<T>::full() {
    if (this->top_ + 1 == this->max_size_) {
        return true;
    } else {
        return false;
    }
}


#endif
