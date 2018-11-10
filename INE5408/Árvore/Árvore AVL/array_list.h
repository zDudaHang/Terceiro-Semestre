/// Copyright 2018 Maria Eduarda de Melo Hang

#ifndef STRUCTURES_ARRAY_LIST_H
#define STRUCTURES_ARRAY_LIST_H

#include <cstdint>
#include <stdexcept>  // C++ Exceptions


namespace structures {

template<typename T>
/// Classe Lista
class ArrayList {
 public:
    /// Construtor sem parametros
    ArrayList();

    /// Construtor com o parametro para determina o tamanho
    explicit ArrayList(std::size_t max_size);

    /// Destrutor
    ~ArrayList();

    /// Limpa a lista
    void clear();

    /// Insere no final
    void push_back(const T& data);

    /// Insere no inicio
    void push_front(const T& data);

    /// Insere na posicao determinada
    void insert(const T& data, std::size_t index);

    /// Insere de forma ordenada: Maior que
    void insert_sorted(const T& data);

    /// Retira de uma posicao especifica
    T pop(std::size_t index);

    /// Retira do final
    T pop_back();

    /// Retira do inicio
    T pop_front();

    /// Retira um elemento especifico
    void remove(const T& data);

    /// Teste para verificar se a lista esta cheia
    bool full() const;

    /// Teste para verificar se a lista esta vazia
    bool empty() const;

    /// Teste se essse elemento existe na lista
    bool contains(const T& data) const;

    /// Retorna a posicao do elemento especifico
    std::size_t find(const T& data) const;

    /// Retorna o tamanho atual da lista
    std::size_t size() const;

    /// Retorna o tamanho maximo da lista
    std::size_t max_size() const;

    /// Retorna o elemento de uma posicao especifica
    T& at(std::size_t index);

    /// (?)
    T& operator[](std::size_t index);

    /// (?)
    const T& at(std::size_t index) const;

    /// (?)
    const T& operator[](std::size_t index) const;

 private:
    T* contents;
    std::size_t size_;
    std::size_t max_size_;

    static const auto DEFAULT_MAX = 10u;
};

}  // namespace structures //

template <typename T>
structures::ArrayList<T>::ArrayList() {	 	  	 	      	      	    	 	  	     	      	 	
    this->max_size = DEFAULT_MAX;
    this->size_ = 0;
    this->contents = new T[max_size];
}

template <typename T>
structures::ArrayList<T>::ArrayList(std::size_t max_size) {
    this->max_size_ = max_size;
    this->size_ = 0;
    this->contents = new T[max_size];
}

template <typename T>
structures::ArrayList<T>::~ArrayList() {
    delete[] this->contents;
}

template <typename T>
void structures::ArrayList<T>::clear() {
    this->size_ = 0;
}

template <typename T>
void structures::ArrayList<T>::push_back(const T& data) {
    if (this->full()) {
        throw std::out_of_range("Full List");
    } else {
        this->insert(data, this->size_);
    }
}

template <typename T>
void structures::ArrayList<T>::push_front(const T& data) {
    if (this->full()) {
        throw std::out_of_range("Full List");
    } else {	 	  	 	      	      	    	 	  	     	      	 	
        this->insert(data, 0);
    }
}

template <typename T>
void structures::ArrayList<T>::insert(const T& data, std::size_t index) {
    if (this->full()) {
        throw std::out_of_range("Full List");
    } else {
        if (index < 0 || index > this->size_) {
            throw std::out_of_range("Invalid Index");
        } else {
            int i = this->size_;
            while (i > index) {
                this->contents[i] = this->contents[i -1];
                i--;
            }
            this->size_ += 1;
            this->contents[index] = data;
        }
    }
}

template <typename T>
/// Dividir e conquistar
void structures::ArrayList<T>::insert_sorted(const T& data) {
    if (this->full()) {
        throw std::out_of_range("Full List");
    } else {
        std::size_t index = 0;
        while (index < this->size_ && data > contents[index]) {
            index++;
        }
        this->insert(data, index);
    }
}	 	  	 	      	      	    	 	  	     	      	 	

template <typename T>
T structures::ArrayList<T>::pop(std::size_t index) {
     T temp;
    if (this->empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (index < 0 || index > this->size_ - 1) {
            throw std::out_of_range("Invalid Index, Out of the Range");
        } else {
            temp = this->contents[index];
            while (index < this->size_ - 1) {
                this->contents[index] = this->contents[index+1];
                index++;
            }
            this->size_ -= 1;
        }
    }
    return temp;
}

template <typename T>
T structures::ArrayList<T>::pop_back() {
    if (this->empty()) {
        throw std::out_of_range("Empty List");
    } else {
        return this->pop(this->size_-1);
    }
}

template <typename T>
T structures::ArrayList<T>::pop_front() {
    if (this->empty()) {
        throw std::out_of_range("Empty List");
    } else {
        return this->pop(0);
    }	 	  	 	      	      	    	 	  	     	      	 	
}

template <typename T>
void structures::ArrayList<T>::remove(const T& data) {
    if (this->empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (!this->contains(data)) {
            throw std::out_of_range("Was not found");
        } else {
            std::size_t index = this->find(data);
            this->pop(index);
        }
    }
}

template <typename T>
bool structures::ArrayList<T>::full() const {
    if (this->size_ == this->max_size_) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
bool structures::ArrayList<T>::empty() const {
    if (this->size_ == 0) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
bool structures::ArrayList<T>::contains(const T& data) const {	 	  	 	      	      	    	 	  	     	      	 	
    int counter = 0;
    for (int i = 0; i < this->size_ ; i++) {
        if (this->contents[i] != data) {
            counter++;
        }
    }
    if (counter == this->size_) {
        return false;
    } else {
        return true;
    }
}

template <typename T>
std::size_t structures::ArrayList<T>::find(const T& data) const {
    for (std::size_t i = 0; i < this->size_; i++) {
        if (this->contents[i] == data) {
            return i;
        }
    }
    return this->size_;
}

template <typename T>
std::size_t structures::ArrayList<T>::size() const {
    return this->size_;
}

template <typename T>
std::size_t structures::ArrayList<T>::max_size() const {
    return this->max_size_;
}

template <typename T>
T& structures::ArrayList<T>::at(std::size_t index) {
    if (this->empty()) {	 	  	 	      	      	    	 	  	     	      	 	
        throw std::out_of_range("Empty List");
    } else {
        if (index < 0 || index > this->size_ - 1) {
            throw std::out_of_range("Invalid Index");
        } else {
            return this->contents[index];
        }
    }
}

template <typename T>
T& structures::ArrayList<T>::operator[](std::size_t index) {
    return this->contents[index];
}

template <typename T>
const T& structures::ArrayList<T>::at(std::size_t index) const {
    if (this->empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (index < 0 || index > this->size_- 1) {
            throw std::out_of_range("Invalid Index");
        } else {
            return this->contents[index];
        }
    }
}

template <typename T>
const T& structures::ArrayList<T>::operator[](std::size_t index) const {
    return this->contents[index];
}


#endif
	 	  	 	      	      	    	 	  	     	      	 	
