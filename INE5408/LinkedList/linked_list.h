//! Copyright [2018] <Maria Eduarda de Melo Hang>

#ifndef STRUCTURES_LINKED_LIST_H
#define STRUCTURES_LINKED_LIST_H

#include <cstdint>
#include <stdexcept>


namespace structures {

//! ...
template<typename T>
class LinkedList {
 public:
    //! ...
    LinkedList();  // construtor padrão
    //! ...
    ~LinkedList();  // destrutor
    //! ...
    void clear();  // limpar lista
    //! ...
    void push_back(const T& data);  // inserir no fim
    //! ...
    void push_front(const T& data);  // inserir no início
    //! ...
    void insert(const T& data, std::size_t index);  // inserir na posição
    //! ...
    void insert_sorted(const T& data);  // inserir em ordem
    //! ...
    T& at(std::size_t index);  // acessar um elemento na posição index
    //! ...
    T pop(std::size_t index);  // retirar da posição
    //! ...
    T pop_back();  // retirar do fim
    //! ...
    T pop_front();  // retirar do início
    //! ...
    void remove(const T& data);  // remover específico
    //! ...
    bool empty() const;  // lista vazia
    //! ...
    bool contains(const T& data) const;  // contém
    //! ...
    std::size_t find(const T& data) const;  // posição do dado
    //! ...
    std::size_t size() const;  // tamanho da lista

 private:
    //! Classe Elemento
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
        T data_;
        Node* next_{nullptr};
    };

    Node* end() {  // último nodo da lista
        auto it = head;
        for (auto i = 1u; i < size(); ++i) {
            it = it->next();
        }
        return it;
    }

    Node* head{nullptr};
    std::size_t size_{0u};
};

}  // namespace structures

template <typename T>
structures::LinkedList<T>::LinkedList() {
    head = nullptr;
    size_ = 0;
}

template <typename T>
structures::LinkedList<T>::~LinkedList() {
    clear();
}

template <typename T>
void structures::LinkedList<T>::clear() {
    while (!empty()) {
        pop_front();
    }
}

template <typename T>
void structures::LinkedList<T>::push_back(const T& data) {
    Node* new_node = new Node(data, nullptr);
    if (new_node == nullptr) {
        throw std::out_of_range("Full Linked List\n");
    }
    if (empty()) {
        head = new_node;
    } else {
        Node* it = end();
        it->next(new_node);
    }
    size_++;
}

template <typename T>
void structures::LinkedList<T>::push_front(const T& data) {
    Node* new_node = new Node(data, nullptr);
    if (new_node == nullptr) {
        throw std::out_of_range("Lista cheia!");
    }
    if (!empty()) {
        new_node->next(head);
    }
    head = new_node;
    size_++;
}

template <typename T>
void structures::LinkedList<T>::insert(const T& data, std::size_t index) {
    if (index > size_) {
        throw std::out_of_range("Índice inválido!");
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
                Node* new_node = new Node(data, nullptr);
                if (new_node == nullptr) {
                    throw std::out_of_range("Lista cheia!");
                }
                Node* it = head;
                Node* previous = nullptr;
                for (size_t i = 0; i < index; i++) {
                    previous = it;
                    it = it->next();
                }
                previous->next(new_node);
                new_node->next(it);
                size_++;
            }
        }
    }
}

template <typename T>
void structures::LinkedList<T>::insert_sorted(const T& data) {
    Node* it = head;
    int index = 0;
    while (index < size_ && it != nullptr && it->data() < data) {
        index++;
        it = it->next();
    }
    insert(data, index);
}

template <typename T>
T& structures::LinkedList<T>::at(std::size_t index) {
    if (empty()) {
        throw std::out_of_range("Empty Linked List");
    }
    if (index >= size_) {
        throw std::out_of_range("Invalid Index");
    }
    Node* it = head;
    for (size_t i = 0; i < index; i++) {
        it = it->next();
    }
    return it->data();
}

template <typename T>
T structures::LinkedList<T>::pop(std::size_t index) {
    if (empty()) {
        throw std::out_of_range("Empty Linked List!");
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
                Node* out_node = head;
                Node* previous = nullptr;
                T return_data;
                for (size_t i = 0; i < index; i++) {
                    previous = out_node;
                    out_node = out_node->next();
                }
                previous->next(out_node->next());
                return_data = out_node->data();
                size_--;
                delete(out_node);
                return return_data;
            }
        }
    }
}

template <typename T>
T structures::LinkedList<T>::pop_back() {
    if (empty()) {
        throw std::out_of_range("Empty Linked List!");
    }
    T return_data;
    Node* out_node = head;
    if (size_ != 1) {
        Node* previous = nullptr;
        for (int i = 0; i < size_ ; i++) {
            if (out_node->next() != nullptr) {
                previous = out_node;
                out_node = out_node->next();
            }
        }
        return_data = out_node->data();
        previous->next(nullptr);
    } else {
        return_data = head->data();
        head = nullptr;
    }
    size_--;
    delete(out_node);
    return return_data;
}

template <typename T>
T structures::LinkedList<T>::pop_front() {
    if (empty()) {
      throw std::out_of_range("Empty Linked List!");
    }
    Node* out_node = head;
    T return_data = out_node->data();
    head = out_node->next();
    size_--;
    delete(out_node);
    return return_data;
}

template <typename T>
void structures::LinkedList<T>::remove(const T& data) {
    if (empty()) {
        throw std::out_of_range("Empty Linked List!");
    }
    if (!contains(data)) {
        throw std::out_of_range("Was not found");
    }
    size_t index = find(data);
    pop(index);
}

template <typename T>
bool structures::LinkedList<T>::empty() const {
    return (size_ == 0);
}

template <typename T>
bool structures::LinkedList<T>::contains(const T& data) const {
    if (empty()) {
        throw std::out_of_range("Empty Linked List!");
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
std::size_t structures::LinkedList<T>::find(const T& data) const {
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
std::size_t structures::LinkedList<T>::size() const {
    return size_;
}

#endif
