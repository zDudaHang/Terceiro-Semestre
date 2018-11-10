//! Copyright [2018] <Maria Eduarda de Melo Hang>
#ifndef STRUCTURES_CIRCULAR_LIST_H
#define STRUCTURES_CIRCULAR_LIST_H

#include <cstdint>
#include <stdexcept>

namespace structures {

template<typename T>
//! Classe Lista Circula Simplesmente Encadeada
class CircularList {
 public:
    //! Construtor
    CircularList();

    //! Destrutor
    ~CircularList();

    //! Limpa Lista
    void clear();

    //! Insere no final da lista
    void push_back(const T& data);

    //! Insere no inicio da lista
    void push_front(const T& data);

    //! Insere em uma posicao especifica
    void insert(const T& data, std::size_t index);

    //! Insere em uma ordem especifica (Maior que)
    void insert_sorted(const T& data);

    //! Retorna o conteudo naquele indice
    T& at(std::size_t index);

    //! Retorna-Constante o conteudo naquele indice
    const T& at(std::size_t index) const;

    //! Retira de um endereco especifico
    T pop(std::size_t index);

    //! Retira do final
    T pop_back();

    //! Retira do inicio
    T pop_front();

    //! Retira um dado especifico
    void remove(const T& data);

    //! Testa se esta vazia
    bool empty() const;

    //! Testa se o dado esta na lista
    bool contains(const T& data) const;

    //! Encontra o indice em que o dado se encontra, senao retorna o tamanho
    std::size_t find(const T& data) const;

    //! Retorna o tamanho
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

      //! Construtor com dado, proximo e boolean do ward
      Node(const T& data, Node* next, bool isWard):
        data_{data},
        next_{next},
        isWard_{isWard}
      {}

      //! Getter do Dado
      T& data() {
        return data_;
      }

      //! Getter-Constante do Dado
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

      //! Getter do isWard
      bool isWard() {
        return isWard_;
      }

      //! Getter-Constante do isWard
      const bool isWard() const {
        return isWard_;
      }

      //! Setter do proximo
      void next(Node* node) {
        next_ = node;
      }

      //! Setter do bool
      void isWard(bool isWard) {
        isWard_ = isWard;
      }

   private:
      //! Dado do Elemento
      T data_;

      //! Proximo do Elemento
      Node* next_;

      //! Valor para determinar se eh ward
      bool isWard_;
  };

  //! Inicio da Lista
  Node* head;

  //! Tamanho da Lista
  std::size_t size_;
};
}  // namespace structures //

template <typename T>
structures::CircularList<T>::CircularList() {
  size_ = 0;
  Node* ward = new Node(0, nullptr, true);
  ward->next(ward);
  head = ward;
}

template <typename T>
structures::CircularList<T>::~CircularList() {
  clear();
  delete(head);
}

template <typename T>
void structures::CircularList<T>::clear() {
  while (!empty()) {
    pop_front();
  }
}

template <typename T>
void structures::CircularList<T>::push_back(const T& data) {
  Node* new_node = new Node(data, head, false);
  if (new_node == nullptr) {
    throw std::out_of_range("Full List");
  }
  if (!empty()) {
    Node* it = head->next();
    for (size_t i = 0; i < size_ - 1; i++) {
      it = it->next();
    }
    it->next(new_node);
  } else {
    head->next(new_node);
  }
  size_++;
}

template <typename T>
void structures::CircularList<T>::push_front(const T& data) {
  Node* new_node = new Node(data, nullptr, false);
  if (new_node == nullptr) {
    throw std::out_of_range("Full List");
  }
  if (!empty()) {
    new_node->next(head->next());
  } else {
    new_node->next(head);
  }
  head->next(new_node);
  size_++;
}

template <typename T>
void structures::CircularList<T>::insert(const T& data, std::size_t index) {
  if (index > size_) {
    throw std::out_of_range("Full List");
  }
  if (index == 0) {
    return push_front(data);
  } else {
    if (index == size_) {
      return push_back(data);
    } else {
      if (empty()) {
        return push_front(data);
      } else {
        Node* new_node = new Node(data, nullptr, false);
        if (new_node == nullptr) {
          throw std::out_of_range("Full List");
        }
        Node* it = head->next();
        Node* previous = nullptr;
        for (size_t i = 0; i < index; i++) {
          if (!it->isWard()) {
            previous = it;
            it = it->next();
          }
        }
        new_node->next(it);
        previous->next(new_node);
        size_++;
      }
    }
  }
}

template <typename T>
void structures::CircularList<T>::insert_sorted(const T& data) {
  size_t index = 0;
  Node* it = head->next();
  while ( index < size_ && it != nullptr && data > it->data() ) {
    index++;
    it = it->next();
  }
  insert(data, index);
}

template <typename T>
T& structures::CircularList<T>::at(std::size_t index) {
  if (empty()) {
    throw std::out_of_range("Empty List");
  }
  if (index > size_) {
    throw std::out_of_range("Invalid Index");
  }
  Node* it = head->next();
  for (size_t i = 0; i < index; i++) {
    it = it->next();
  }
  return it->data();
}

template <typename T>
const T& structures::CircularList<T>::at(std::size_t index) const {
  if (empty()) {
    throw std::out_of_range("Empty List");
  }
  if (index > size_) {
    throw std::out_of_range("Invalid Index");
  }
  Node* it = head->next();
  for (size_t i = 0; i < index; i++) {
    it = it->next();
  }
  return it->data();
}

template <typename T>
T structures::CircularList<T>::pop(std::size_t index) {
  if (empty()) {
    throw std::out_of_range("Empty List");
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
      if (size_ == 1) {
        return pop_front();
      } else {
        Node* node_out = head->next();
        Node* previous = nullptr;
        T return_data;
        for (size_t i = 0; i < index; i++) {
          previous = node_out;
          node_out = node_out->next();
        }
        return_data = node_out->data();
        previous->next(node_out->next());
        size_--;
        delete(node_out);
        return return_data;
      }
    }
  }
}

template <typename T>
T structures::CircularList<T>::pop_back() {
  if (empty()) {
    throw std::out_of_range("Empty List");
  }
  Node* node_out = head->next();
  T return_data;
  if (size_ > 1) {
    Node* previous = nullptr;
    for (size_t i = 0; i < size_ -1; i++) {
      if (!(node_out->isWard())) {
        previous = node_out;
        node_out = node_out->next();
      }
      return_data = node_out->data();
    }
    previous->next(head);
  } else {
    return_data = node_out->data();
    head->next(head);
  }
  delete(node_out);
  size_--;
  return return_data;
}

template <typename T>
T structures::CircularList<T>::pop_front() {
  if (empty()) {
    throw std::out_of_range("Empty List");
  }
  Node* node_out = head->next();
  T return_data = node_out->data();
  if (size_ > 1) {
    head->next(node_out->next());
  } else {
    head->next(head);
  }
  delete(node_out);
  size_--;
  return return_data;
}

template <typename T>
void structures::CircularList<T>::remove(const T& data) {
  if (empty()) {
    throw std::out_of_range("Empty List");
  }
  if (!contains(data)) {
    throw std::out_of_range("Was not found");
  }
  size_t index = find(data);
  pop(index);
}

template <typename T>
bool structures::CircularList<T>::empty() const {
  return (size_ == 0);
}

template <typename T>
bool structures::CircularList<T>::contains(const T& data) const {
  if (empty()) {
    throw std::out_of_range("Empty Linked List!");
  }
  Node* it = head->next();
  for (size_t i = 0; i < size_; i++) {
    if (it->data() == data && !it->isWard()) {
      return true;
    }
    it = it->next();
  }
  return false;
}

template <typename T>
std::size_t structures::CircularList<T>::find(const T& data) const {
  Node* it = head->next();
  for (size_t i = 0; i < size_; i++) {
    if (it->data() == data && !it->isWard()) {
        return i;
    }
    it = it->next();
  }
  return size_;
}

template <typename T>
std::size_t structures::CircularList<T>::size() const {
  return size_;
}


#endif
