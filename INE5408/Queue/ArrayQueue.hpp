/// Copyright 2018 Maria Eduarda de Melo Hang

#ifndef STRUCTURES_ARRAY_QUEUE_H
#define STRUCTURES_ARRAY_QUEUE_H

#include <cstdint>  // std::size_t
#include <stdexcept>  // C++ Exceptions

namespace structures {

template<typename T>
/// Classe fila
class ArrayQueue {
 public:
    /// Construtor sem parametros
    ArrayQueue();

    /// Construtor com o parametro para definir o tamanho da fila
    explicit ArrayQueue(std::size_t max);

    /// Destrutor da fila
    ~ArrayQueue();

    /// Adiciona um elemento a fila
    /// Se a fila estiver cheia ira gerar uma excecao
    void enqueue(const T& data);

    /// Retira um elemento da fila
    /// Se a fila estiver vazia ira gerar uma excecao
    T dequeue();

    /// Pega o ultimo elemento da fila
    T& back();

    /// Limpa a fila
    void clear();

    /// Retorna o tamanho atual da fila
    std::size_t size();

    /// Retorna o tamanho maximo da fila
    std::size_t max_size();

    /// Retorna verdadeiro se a fila estiver vazia
    bool empty();

    /// Retorna verdadeiro se a fila estiver cheia
    bool full();

 private:
    T* contents;
    std::size_t size_;
    std::size_t max_size_;

    static const auto DEFAULT_SIZE = 10u;
};

template <typename T>
structures::ArrayQueue<T>::ArrayQueue() {
    this->ArrayQueue(DEFAULT_SIZE);
}

template <typename T>
structures::ArrayQueue<T>::ArrayQueue(std::size_t max) {
    this->max_size_ = max;
    this->contents = new T[max];
    this->size_ = 0;
}

template <typename T>
structures::ArrayQueue<T>::~ArrayQueue() {
    delete[] this->contents;
}


template <typename T>
void structures::ArrayQueue<T>::enqueue(const T& data) {
  if (this->full()) {
       throw std::out_of_range("Full Queue");
  } else {
       this->size_ += 1;
       this->contents[this->size_ -1] = data;
  }
}


template <typename T>
T structures::ArrayQueue<T>::dequeue() {
    if (this->empty()) {
        throw std::out_of_range("Empty Queue");
    } else {
        T auxiliar = this->contents[0];
        for (int i = 0; i < this->size_ - 1; i++) {
            this->contents[i] = this->contents[i+1];
        }
        this->size_ -= 1;
        return auxiliar;
    }
}

template <typename T>
T& structures::ArrayQueue<T>::back() {
    if (this->empty()) {
        throw std::out_of_range("Empty Queue");
    } else {
        return (this->contents[this->size_ -1 ]);
    }
}

template <typename T>
void structures::ArrayQueue<T>::clear() {
    this->size_ = 0;
}

template <typename T>
std::size_t structures::ArrayQueue<T>::size() {
    return (this->size_);
}

template <typename T>
std::size_t structures::ArrayQueue<T>::max_size() {
    return (this->max_size_);
}

template <typename T>
bool structures::ArrayQueue<T>::empty() {
    if (this->size_ == 0) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
bool structures::ArrayQueue<T>::full() {
    if (this->size_ == this->max_size_) {
        return true;
    } else {
        return false;
    }
}

}  // namespace structures //

#endif
