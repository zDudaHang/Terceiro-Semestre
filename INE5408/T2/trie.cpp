//! Copyright [2018] Maria Eduarda de Melo Hang

#ifndef STRUCTURES_TRIE_H
#define STRUCTURES_TRIE_H

#include <string>
#include <cstring>
#include <map>
#include <iostream>

#define K 26


namespace structures {

class Trie {
 public:
    char alphabet_[K] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't' , 'u', 'v', 'w', 'x', 'y', 'z'};

    //! Mapa para direcionar corretamente o índice de acordo com o caractere
    std::map<char, int> alphabet_decoder {};

    //! Inicializando o mapa
    void initMap () {
        for (int i = 0; i < K; i++) {
            alphabet_decoder.insert(std::pair<char, int>(alphabet_[i], i));
        }
    }

    //! Construtor
    Trie() {
        this->root = new Node('*');
    }

    //! Destrutor
    ~Trie() = default;

    //! Inserir um elemento
    void insert(std::string word, unsigned long position, unsigned long length) {
        Node* it = this->root;
        for (size_t i = 0; i < word.length(); i++) {
            char current = word.at(i);
            int index = alphabet_decoder[current];
            if (it->children[index] == nullptr) {
                it->children[index] = new Node(current);
            }
            it = it->children[index];
        }
        it->dic_position = position;
        it->dic_length = length;

    }

    bool isPrefix(std::string word) {
        Node* it = this->root;
        for (size_t i = 0; i < word.length(); i++) {
            char current = word.at(i);
            int index = alphabet_decoder[current];
            Node* child = it->children[index];
            if (child == nullptr) {
                std:: cout << "is not prefix " << std:: endl;
                return false;
            } else {
                it = child;
            } 
        }
        if (it->isWord()) {
            std:: cout << it->dic_position << " " << it->dic_length << std:: endl;
            return false;
        } else {
            std:: cout << "is prefix " << std:: endl;
            return true;
        }
    }

 private:
    struct Node {
        //! Dado
        char data;

        //! Array de ponteiros para os filhos
        Node* children[K];

        //! Posicao em que está a palavra e sua definição
        unsigned long dic_position;

        //! Comprimento da String
        unsigned long dic_length;

        //! Construtor padrao
        Node() {
            this->data = ' ';
            this->dic_position = 0;
            this->dic_length = 0;
            for (int i = 0; i < K; i++) {
                this->children[i] = NULL;
            }
        }

        //! Construtor com dado
        explicit Node(const char& data_) {
            this->data = data_;
            this->dic_position = 0;
            this->dic_length = 0;
            for (int i = 0; i < K; i++) {
                this->children[i] = NULL;
            }
        }

        bool isWord() {
            if (this->dic_position >= 0 && this->dic_length > 0) {
                return true;
            }
            return false;
        }
    };
    //! Nodo raiz
    Node* root;
};

}  // namespace structures


#endif
