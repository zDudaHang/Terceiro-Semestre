//! Copyright [2018] Maria Eduarda de Melo Hang

#ifndef STRUCTURES_TRIE_H
#define STRUCTURES_TRIE_H

#include <string>
#include <cstring>
#include <map>
#include <iostream>

#define K 26

static char alphabet_[K] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't' , 'u', 'v', 'w', 'x', 'y', 'z'};

namespace structures {

class Trie {
 public:
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
        Node* root = new Node();
    }

    //! Destrutor
    ~Trie() = default;

    //! Inserir um elemento
    void insert(std::string word, unsigned long position, unsigned long length) {
        Node* it = root;
        for (size_t i = 0; i < word.length(); i++) {
            char current = word.at(i);
            int index = alphabet_decoder[current];
            if (it->children[index] == nullptr) {
                Node* new_child = new Node(current);
                it->children[index] = new_child;
            }
            it = it->children[index];
            if (i == word.length() - 1) {
                it->setAsWord(position, length);
            }
        }
    }

    //! Verifica se é um prefixo
    bool verifyTypeOfWord(std::string word) {
        int index = alphabet_decoder[word.at(0)];
        if (root->children[index] == nullptr) {
            std:: cout << "is not prefix " << std:: endl;
            return false;
        }
        Node* it = root->children[index];
        for (size_t i = 1; i < word.length(); i++) {
            char current = word.at(i);
            int index = alphabet_decoder[current];
            if (it->children[index] == nullptr) {
                std:: cout << "is not prefix " << std:: endl;
                return false;
            }
            if (it->children[index] != nullptr && !(it->children[index]->isWord()) && (i == word.length() - 1)) {
                std:: cout << "is prefix " << std:: endl;
                return true;
            } else if (it->children[index]->isWord()) {
                std:: cout << it->children[index]->dic_position << " " << it->children[index]->dic_length << std:: endl;
                return true;
            }
            it = it->children[index];
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
            data = ' ';
            dic_position = 0;
            dic_length = 0;
            for (int i = 0; i < K; i++) {
                children[i] = nullptr;
            }
        }

        //! Construtor com dado
        explicit Node(const char& data_) {
            data = data_;
            dic_position = 0;
            dic_length = 0;
            for (int i = 0; i < K; i++) {
                children[i] = nullptr;
            }
        }

        void setAsWord(unsigned long position, unsigned long length) {
            dic_position = position;
            dic_length = length;
        }

        bool isWord() {
            if (dic_position >= 0 && dic_length > 0) {
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
