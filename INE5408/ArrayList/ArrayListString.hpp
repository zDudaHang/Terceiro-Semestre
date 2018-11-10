//! Copyright [2018] <Copyright Maira Eduarda de Melo Hang>

#ifndef STRUCTURES_STRING_LIST_H
#define STRUCTURES_STRING_LIST_H

#include <cstdint>
#include <stdexcept>  // C++ exceptions
#include <cstring>


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

 protected:
    /// Array dos dados
    T* contents;

    /// Tamanho atual
    std::size_t size_;

    /// Tamanho maximo
    std::size_t max_size_;

    /// Constante para o maximo de espaco do Array
    static const auto DEFAULT_MAX = 10u;
};

//! ...
//! ArrayListString e' uma especializacao da classe ArrayList
class ArrayListString : public ArrayList<char *> {
 public:
    //! Construtor sem parametros
    ArrayListString() : ArrayList() {}

    //! Construtor com parametros
    explicit ArrayListString(std::size_t max_size) : ArrayList(max_size) {}

    //! Destrutor
    ~ArrayListString();

    //! Limpa o vetor
    void clear();

    //! Insere um ponteiro no final
    void push_back(const char *data);

    //! Insere um ponteiro no inicio
    void push_front(const char *data);

    //! Insere na posicao desejada
    void insert(const char *data, std::size_t index);

    //! Insere em uma ordem especificada (Maior que)
    void insert_sorted(const char *data);

    //! Retira um elemento do endereco especificado
    char *pop(std::size_t index);

    //! Retira um elemento do final
    char *pop_back();

    //! Retira um elemento do inicio
    char *pop_front();

    //! Retira um determinado elemento
    void remove(const char *data);

    //! Verifica se o elemento existe na lsita
    bool contains(const char *data);

    //! Encontra o indice de um determinado elemento
    std::size_t find(const char *data);
};
}  // namespace structures //

structures::ArrayListString::~ArrayListString() {
    //  for (std::size_t i = 0; i < ArrayList::size_; i++) {
        //  delete[] (contents[i]);
    //  }
    //  delete[] contents;
}

void structures::ArrayListString::clear() {
    //  for (std::size_t i = 0; i < size_; i++) {
        //  delete[] (contents[i]);
    //  }
    size_ = 0;
}

template <typename T>
structures::ArrayList<T>::ArrayList() {
    ArrayList(DEFAULT_MAX);
}

template <typename T>
structures::ArrayList<T>::ArrayList(std::size_t max_size) {
    max_size_ = max_size;
    size_ = 0;
    contents = new T[max_size];
}

template <typename T>
structures::ArrayList<T>::~ArrayList() {
    //  delete[] contents;
}

template <typename T>
void structures::ArrayList<T>::clear() {
    size_ = 0;
}

template <typename T>
void structures::ArrayList<T>::push_back(const T& data) {
    if (full()) {
        throw std::out_of_range("Full List");
    } else {
        insert(data, size_);
    }
}

template <typename T>
void structures::ArrayList<T>::push_front(const T& data) {
    if (full()) {
        throw std::out_of_range("Full List");
    } else {
        insert(data, 0);
    }
}

template <typename T>
void structures::ArrayList<T>::insert(const T& data, std::size_t index) {
    if (full()) {
        throw std::out_of_range("Full List");
    } else {
        if (index > size_) {
            throw std::out_of_range("Invalid Index");
        } else {
            int i = size_;
            while (i > index) {
                contents[i] = contents[i -1];
                i--;
            }
            size_ += 1;
            contents[index] = data;
        }
    }
}

template <typename T>
/// Dividir e conquistar
void structures::ArrayList<T>::insert_sorted(const T& data) {
    if (full()) {
        throw std::out_of_range("Full List");
    } else {
        std::size_t index = 0;
        while (index < size_ && data > contents[index]) {
            index++;
        }
        insert(data, index);
    }
}

template <typename T>
T structures::ArrayList<T>::pop(std::size_t index) {
     T temp;
    if (empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (index > size_ - 1) {
            throw std::out_of_range("Invalid Index, Out of the Range");
        } else {
            temp = contents[index];
            while (index < size_ - 1) {
                contents[index] = contents[index+1];
                index++;
            }
            size_ -= 1;
        }
    }
    return temp;
}

template <typename T>
T structures::ArrayList<T>::pop_back() {
    if (empty()) {
        throw std::out_of_range("Empty List");
    } else {
        return pop(size_-1);
    }
}

template <typename T>
T structures::ArrayList<T>::pop_front() {
    if (empty()) {
        throw std::out_of_range("Empty List");
    } else {
        return pop(0);
    }
}

template <typename T>
void structures::ArrayList<T>::remove(const T& data) {
    if (empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (!contains(data)) {
            throw std::out_of_range("Was not found");
        } else {
            std::size_t index = find(data);
            pop(index);
        }
    }
}

template <typename T>
bool structures::ArrayList<T>::full() const {
    if (size_ == max_size_) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
bool structures::ArrayList<T>::empty() const {
    if (size_ == 0) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
bool structures::ArrayList<T>::contains(const T& data) const {
    for (int i = 0; i < size_ ; i++) {
        if (contents[i] == data) {
            return true;
        }
    }
    return false;
}

template <typename T>
std::size_t structures::ArrayList<T>::find(const T& data) const {
    for (std::size_t i = 0; i < size_; i++) {
        if (contents[i] == data) {
            return i;
        }
    }
    return size_;
}

template <typename T>
std::size_t structures::ArrayList<T>::size() const {
    return size_;
}

template <typename T>
std::size_t structures::ArrayList<T>::max_size() const {
    return max_size_;
}

template <typename T>
T& structures::ArrayList<T>::at(std::size_t index) {
    if (empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (index > size_ - 1) {
            throw std::out_of_range("Invalid Index");
        } else {
            return contents[index];
        }
    }
}

template <typename T>
T& structures::ArrayList<T>::operator[](std::size_t index) {
    return contents[index];
}

template <typename T>
const T& structures::ArrayList<T>::at(std::size_t index) const {
    if (empty()) {
        throw std::out_of_range("Empty List");
    } else {
        if (index > size_- 1) {
            throw std::out_of_range("Invalid Index");
        } else {
            return contents[index];
        }
    }
}

template <typename T>
const T& structures::ArrayList<T>::operator[](std::size_t index) const {
    return contents[index];
}


/// Insere no final. Parametro: Ponteiro para uma String. Primeiro char dela
void structures::ArrayListString::push_back(const char *data) {
    std::size_t string_length = strlen(data);
    if (string_length < 10000) {
        char* new_data = new char[string_length+1];
        snprintf(new_data, strlen(data)+1, "%s", data);
        ArrayList::push_back(new_data);
    } else {
        throw std::out_of_range("String's length bigger than 10.000");
    }
}

void structures::ArrayListString::push_front(const char *data) {
    std::size_t string_length = strlen(data);
    if (string_length < 10000) {
        char* new_data = new char[string_length+1];
        snprintf(new_data, strlen(data)+1, "%s", data);
        ArrayList::push_front(new_data);
    } else {
        throw std::out_of_range("String's length bigger than 10.000");
    }
}

void structures::ArrayListString::insert(const char *data, std::size_t index) {
    std::size_t string_length = strlen(data);
    if (string_length < 10000) {
        char* new_data = new char[string_length+1];
        snprintf(new_data, strlen(data)+1, "%s", data);
        ArrayList::insert(new_data, index);
    } else {
        throw std::out_of_range("String's length bigger than 10.000");
    }
}

void structures::ArrayListString::insert_sorted(const char *data) {
    if (ArrayList::full()) {
        throw std::out_of_range("Full List");
    } else {
        std::size_t string_length = strlen(data);
        if (string_length < 10000) {
            std::size_t index = 0;
            while (index < size_ && strcmp(contents[index], data) < 0) {
                index++;
            }
            char *new_data = new char[string_length+1];
            snprintf(new_data, strlen(data)+1, "%s", data);
            ArrayList::insert(new_data, index);
        } else {
            throw std::out_of_range("String's length bigger than 10.000");
        }
    }
}

char *structures::ArrayListString::pop(std::size_t index) {
    char* content = ArrayList::pop(index);
    return content;
}

char *structures::ArrayListString::pop_back() {
    char* content = ArrayList::pop(size_ -1);
    return content;
}

char *structures::ArrayListString::pop_front() {
    char* content = ArrayList::pop(0);
    return content;
}

void structures::ArrayListString::remove(const char *data) {
    if (ArrayList::empty()) {
        throw std::out_of_range("Empty List");
    } else {
        std::size_t index = find(data);
        if (index != size_) {
            //  delete[] ArrayList::pop(index)
            ArrayList::pop(index);
        } else {
            throw std::out_of_range("Was not found");
        }
    }
}

bool structures::ArrayListString::contains(const char *data) {
    for (std::size_t i = 0; i < size_; i++) {
        if (strcmp(contents[i], data) == 0) {
            return true;
        }
    }
    return false;
}

std::size_t structures::ArrayListString::find(const char *data) {
    for (std::size_t i = 0; i < size_; i++) {
        if (strcmp(this->contents[i], data) == 0) {
            return i;
        }
    }
    return this->size_;
}

#endif
