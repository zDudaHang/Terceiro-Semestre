//! Copyright [2018] <Maria Eduarda de Melo Hang>

#ifndef STRUCTURES_DOUBLY_CIRCULAR_LIST_H
#define STRUCTURES_DOUBLY_CIRCULAR_LIST_H

#include <cstdint>
#include <stdexcept>

namespace structures {

template<typename T>
//! Lista Circular Duplamente Encadeada
class DoublyCircularList {
 public:
    //! Construtor
    DoublyCircularList();
    //! Destrutor
    ~DoublyCircularList();
    //! Limpa Lista
    void clear();
    //! Insere no final
    void push_back(const T& data);
    //! Insere no inicio
    void push_front(const T& data);
    //! Insere em uma posicao
    void insert(const T& data, std::size_t index);
    //! Insere em uma ordem, nesse caso maior que
    void insert_sorted(const T& data);
    //! Retira de uma posicao
    T pop(std::size_t index);
    //! Retira do final
    T pop_back();
    //! Retira do inicio
    T pop_front();
    //! Retira um dado
    void remove(const T& data);
    //! Verifica se esta vazia
    bool empty() const;
    //! Verifica se contem o dado
    bool contains(const T& data) const;
    //! Retorna o dado naquele indice
    T& at(std::size_t index);
    //! Retorna o dado naquele indice de forma constante
    const T& at(std::size_t index) const;
    //! Retorna o indice desse dado
    //! Se nao for encontrado, retorna o tamanho da lista
    std::size_t find(const T& data) const;
    //! Retorna o tamanho da lista
    std::size_t size() const;

 private:
    //! Classe do Elemento
    class Node {
     public:
        //! Construtor com o dado
        explicit Node(const T& data) :
            data_{data}
        {}
        //! Construtor com o dado e o proximo
        Node(const T& data, Node* next) :
            data_{data},
            next_{next}
        {}
        //! Construtor com o dado, anterior e proximo
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
        //! Setter do Proximo
        void next(Node* node) {
            next_ = node;
        }

     private:
        //! Dado do Elemento
        T data_;
        //! Ponteiro para o elemento anterior
        Node* prev_;
        //! Ponteiro para o proximo elemento
        Node* next_;
    };
    //! Inicio da Lista
    Node* head;
    //! Tamanho da Lista
    std::size_t size_;
};

}  // namespace structures //

template <typename T>
structures::DoublyCircularList<T>::DoublyCircularList() {
    size_ = 0;
    head = nullptr;
}

template <typename T>
structures::DoublyCircularList<T>::~DoublyCircularList() {
    clear();
}

template <typename T>
void structures::DoublyCircularList<T>::clear() {
    while (!empty()) {
        pop_front();
    }
}

template <typename T>
void structures::DoublyCircularList<T>::push_back(const T& data) {
    Node* new_node = new Node(data, nullptr, nullptr);
    if (new_node == nullptr) {
        std::out_of_range("Full List");
    }
    if (!empty()) {
        Node* it = head->prev();
        new_node->next(head);
        new_node->prev(it);
        it->next(new_node);
        head->prev(new_node);
    } else {
        new_node->next(new_node);
        new_node->prev(new_node);
        head = new_node;
    }
    size_++;
}

template <typename T>
void structures::DoublyCircularList<T>::push_front(const T& data) {
    Node* new_node = new Node(data, nullptr, nullptr);
    if (new_node == nullptr) {
        std::out_of_range("Full List");
    }
    if (size_ != 0) {
        new_node->next(head);
        if (size_ == 1) {
            new_node->prev(head);
            head->next(new_node);
        } else {
            new_node->prev(head->prev());
            head->prev()->next(new_node);
        }
        head->prev(new_node);
        head = new_node;
    } else {
        new_node->next(new_node);
        new_node->prev(new_node);
        head = new_node;
    }
    size_++;
}

template <typename T>
void structures::DoublyCircularList<T>::
insert(const T& data, std::size_t index) {
    if (index > size_) {
        throw std::out_of_range("Invalid Index");
    }
    if (index == 0) {
        push_front(data);
    } else {
        if (index == size_) {
            push_back(data);
        } else {
            if (empty()) {
                push_front(data);
            } else {
                Node* new_node = new Node(data, nullptr, nullptr);
                if (new_node == nullptr) {
                    throw std::out_of_range("Full List");
                }
                Node* it = nullptr;
                size_t interval = size_/2;
                if (index <= interval) {
                    it = head;
                    for (size_t i = 0; i < index; i++) {
                        it = it->next();
                    }
                } else {
                    it = head;
                    for (size_t i = 0; i < (size_ - index); i++) {
                        it = it->prev();
                    }
                }
                new_node->next(it);
                new_node->prev(it->prev());
                it->prev()->next(new_node);
                it->prev(new_node);
                size_++;
            }
        }
    }
}

template <typename T>
void structures::DoublyCircularList<T>::insert_sorted(const T& data) {
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
T structures::DoublyCircularList<T>::pop(std::size_t index) {
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
            Node* node_out = nullptr;
            T return_data;
            size_t interval = size_/2;
            if (index <= interval) {
                node_out = head;
                for (size_t i = 0; i < index; i++) {
                    node_out = node_out->next();
                }
            } else {
                node_out = head->prev();
                for (size_t i = 0; i < (size_ - index - 1); i++) {
                    node_out = node_out->prev();
                }
            }
            return_data = node_out->data();
            node_out->prev()->next(node_out->next());
            node_out->next()->prev(node_out->prev());
            size_--;
            delete(node_out);
            return return_data;
        }
    }
}

template <typename T>
T structures::DoublyCircularList<T>::pop_back() {
    if (empty()) {
        throw std::out_of_range("Empty List");
    }
    Node* node_out = nullptr;
    T return_data;
    if (size_ > 1) {
        node_out = head->prev();
        return_data = node_out->data();
        node_out->prev()->next(head);
        head->prev(node_out->prev());
    } else {
        node_out = head;
        return_data = node_out->data();
        head = nullptr;
    }
    size_--;
    delete(node_out);
    return return_data;
}

template <typename T>
T structures::DoublyCircularList<T>::pop_front() {
    if (empty()) {
        throw std::out_of_range("Empty List");
    }
    Node* node_out = head;
    T return_data = node_out->data();
    if (size_ > 1) {
        node_out->prev()->next(node_out->next());
        node_out->next()->prev(node_out->prev());
        head = node_out->next();
    } else {
        head = nullptr;
    }
    size_--;
    delete(node_out);
    return return_data;
}

template <typename T>
void structures::DoublyCircularList<T>::remove(const T& data) {
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
bool structures::DoublyCircularList<T>::empty() const {
    return (size_ == 0);
}

template <typename T>
bool structures::DoublyCircularList<T>::contains(const T& data) const {
    if (empty()) {
        throw std::out_of_range("Empty List");
    }
    Node* it = head;
    for (size_t i = 0; i < size_; i++) {
        if (it->data() == data) {
            return true;
        } else {
            it = it->next();
        }
    }
    return false;
}

template <typename T>
T& structures::DoublyCircularList<T>::at(std::size_t index) {
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
        it = head->prev();
        for (size_t i = 0; i < (size_ - index - 1); i++) {
            it = it->prev();
        }
    }
    return it->data();
}

template <typename T>
const T& structures::DoublyCircularList<T>::at(std::size_t index) const {
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
        it = head->prev();
        for (size_t i = 0; i < (size_ - index - 1); i++) {
            it = it->prev();
        }
    }
    return it->data();
}

template <typename T>
std::size_t structures::DoublyCircularList<T>::find(const T& data) const {
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
std::size_t structures::DoublyCircularList<T>::size() const {
    return size_;
}

#endif
