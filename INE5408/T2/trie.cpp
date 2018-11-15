/**
*    @file trie.cpp
*    @brief Árvore de Prefixos que suporta os caracteres de a até z como nodos. 
*    Não suporta acentos ou caracteres especiais para a inserção.
*    @author Maria Eduarda de Melo Hang
*    @date 15 November 2018
*/

#ifndef STRUCTURES_TRIE_H
#define STRUCTURES_TRIE_H

#include <string>
#include <cstring>
#include <iostream>

#define K 26 /**< inteiro para guardar a linha do elemento da matriz */ 


namespace structures {
/**
*   @brief Leitura do arquivo xml e armazenamento do respectivo texto.
*
*   Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
*   não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
*   o ifstream source lerá caractere por caractere e concatenará com a variável
*   local XMLtext e terminará quando encontrar o EOF.
*
*   @param xmlfilename Nome do arquivo xml que será aberto e lido.
*   @return O texto do arquivo xml em formate de uma string.
*/
class Trie {
 public:
    /** Construtor */
    Trie() {
        this->root = new Node('*');
    }

    /** Destrutor */
    ~Trie() = default;

    /**
    *   @brief Leitura do arquivo xml e armazenamento do respectivo texto.
    *
    *   Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
    *   não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
    *   o ifstream source lerá caractere por caractere e concatenará com a variável
    *   local XMLtext e terminará quando encontrar o EOF.
    *
    *   @param xmlfilename Nome do arquivo xml que será aberto e lido.
    *   @return O texto do arquivo xml em formate de uma string.
    */
    void insert(std::string word, unsigned long position, unsigned long length) {
        Node* it = this->root;
        for (size_t i = 0; i < word.size(); i++) {
            char current = word.at(i);
            int index = current - 'a';
            Node* child = it->children[index];
            if (child == nullptr) {
                it->children[index] = new Node(current);
            }
            it = it->children[index];
        }
        it->dic_position = position;
        it->dic_length = length;

    }

    /**
    *   @brief Leitura do arquivo xml e armazenamento do respectivo texto.
    *
    *   Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
    *   não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
    *   o ifstream source lerá caractere por caractere e concatenará com a variável
    *   local XMLtext e terminará quando encontrar o EOF.
    *
    *   @param xmlfilename Nome do arquivo xml que será aberto e lido.
    *   @return O texto do arquivo xml em formate de uma string.
    */
    bool isPrefix(std::string word) {
        Node* it = this->root;
        for (size_t i = 0; i < word.size(); i++) {
            char current = word.at(i);
            int index = current - 'a';
            Node* child = it->children[index];
            if (child == nullptr) {
                std:: cout << "is not prefix " << std:: endl;
                return false;
            } else {
                it = child;
                if ( (i == word.size() - 1) && (child->isWord())) {
                    std:: cout << it->dic_position << " " << it->dic_length << std:: endl;
                    return false;
                } else if ( (i == word.size() - 1) && !(child->isWord()) ){
                    std:: cout << "is prefix" << std:: endl;
                    return true;
                }
            } 
        }
    }

 private:
    /**
    *   @brief Classe para armazenar a linha e a coluna de um elemento da matriz.
    *
    *   Utilizada para o processo de rotulação de imagens binárias do trabalho,
    *   guardará a linha e a coluna da respectiva matriz que está sendo rotulada.   
    */
    struct Node {
        char data; /**< inteiro para guardar a linha do elemento da matriz */ 

        Node* children[K]; /**< inteiro para guardar a linha do elemento da matriz */ 

        unsigned long dic_position; /**< inteiro para guardar a linha do elemento da matriz */ 
 
        unsigned long dic_length; /**< inteiro para guardar a linha do elemento da matriz */ 

        /**
        *   @brief Leitura do arquivo xml e armazenamento do respectivo texto.
        *
        *   Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
        *   não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
        *   o ifstream source lerá caractere por caractere e concatenará com a variável
        *   local XMLtext e terminará quando encontrar o EOF.
        *
        *   @param xmlfilename Nome do arquivo xml que será aberto e lido.
        *   @return O texto do arquivo xml em formate de uma string.
        */
        Node() {
            this->data = ' ';
            this->dic_position = 0;
            this->dic_length = 0;
            for (int i = 0; i < K; i++) {
                this->children[i] = NULL;
            }
        }

        /**
        *   @brief Leitura do arquivo xml e armazenamento do respectivo texto.
        *
        *   Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
        *   não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
        *   o ifstream source lerá caractere por caractere e concatenará com a variável
        *   local XMLtext e terminará quando encontrar o EOF.
        *
        *   @param xmlfilename Nome do arquivo xml que será aberto e lido.
        *   @return O texto do arquivo xml em formate de uma string.
        */
        explicit Node(const char& data_) {
            this->data = data_;
            this->dic_position = 0;
            this->dic_length = 0;
            for (int i = 0; i < K; i++) {
                this->children[i] = NULL;
            }
        }

        /**
        *   @brief Leitura do arquivo xml e armazenamento do respectivo texto.
        *
        *   Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
        *   não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
        *   o ifstream source lerá caractere por caractere e concatenará com a variável
        *   local XMLtext e terminará quando encontrar o EOF.
        *
        *   @param xmlfilename Nome do arquivo xml que será aberto e lido.
        *   @return O texto do arquivo xml em formate de uma string.
        */
        bool isWord() {
            if (this->dic_position >= 0 && this->dic_length > 0) {
                return true;
            }
            return false;
        }
    };

    Node* root; /**< inteiro para guardar a linha do elemento da matriz */ 
};

}  // namespace structures


#endif
