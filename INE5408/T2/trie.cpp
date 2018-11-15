/**
*    @file trie.cpp
*    @brief Árvore de Prefixos que suporta os caracteres de a até z como nodos. 
*    Não suporta acentos ou caracteres especiais para a inserção.
*    @author Maria Eduarda de Melo Hang.
*    @date 15 November 2018.
*/

#ifndef STRUCTURES_TRIE_H
#define STRUCTURES_TRIE_H

#include <string>
#include <cstring>
#include <iostream>

#define K 26 /**< constante para guardar a quantidade de caracteres do alfabeto. */ 


namespace structures {
/**
*   @brief Classe da Trie.
*   Estrutura de Dados responsável pela indexação das palavras.
*/
class Trie {
 public:
    /**
    *   @brief Construtor.
    *   A Trie é construída com a raiz já instanciada, não pode existir nenhum caractere na raiz.
    */
    Trie() {
        this->root = new Node('*');
    }

    /** Destrutor */
    ~Trie() = default;

    /**
    *   @brief Inserção na Trie.
    *
    *   A inserção será por caractere da palavra recebida no parâmetro, sendo que a cada iteração o ponteiro it
    *   receberá o filho correspondente ao caractere da palavra para inserir no próximo corretamente. O último
    *   caractere da palavra receberá a posição e o comprimento dela, garantindo que ele é o final de uma palavra.
    *
    *   @param word A palavra que será inserida.
    *   @param position A posição em que se encontra o primeiro caractere dessa palavra no dicionário. 
    *   @param length O comprimento do texto correspondente da palavra no dicionário.
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
    *   @brief Busca ma palavra na Trie e verifica se é um prefixo ou uma palavra.
    *
    *   A busca será caractere por caractere da palavra, caso o nó atual da árvore tenha
    *   um ponteiro nulo para a letra selecionada durante a iteração, retornará falso, ou seja, 
    *   a palavra desejada não é um prefixo para nenhuma das palavras do dicionário e enviará ao terminal "is not prefix"
    *   Caso contrário, percorrerá os filhos dos nós até chegar na última letra e verificará se é o final de uma palavra,
    *   através do método isEndOfWord(). Se retornar verdadeiro, é uma palavra e enviará para o terminal aposição e o 
    *   comrprimento da palavra, caso contrário, "is prefix".
    *
    *   @param word A palavra que será buscada.
    *   @return Se é um prefixo.
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
                if ( (i == word.size() - 1) && (child->isEndOfWord())) {
                    std:: cout << it->dic_position << " " << it->dic_length << std:: endl;
                    return false;
                } else if ( (i == word.size() - 1) && !(child->isEndOfWord()) ){
                    std:: cout << "is prefix" << std:: endl;
                    return true;
                }
            } 
        }
    }

 private:
    /** Classe para o nó da Trie. */
    struct Node {
        char data; /**< Caractere do alfabeto. */ 

        Node* children[K]; /**< Vetor de ponteiros para os próximos nós. */ 

        unsigned long dic_position; /**< Posição no arquivo do dicionário. */ 
 
        unsigned long dic_length; /**< Compriemento da linha correspondente à palavra no arquivo do dicionário. */ 

        /** Construtor padrão. */
        Node() {
            this->data = ' ';
            this->dic_position = 0;
            this->dic_length = 0;
            for (int i = 0; i < K; i++) {
                this->children[i] = NULL;
            }
        }

        /**
        *   @brief Construtor com o dado
        *   @param data_ O caractere para ser colocado como data do nó.
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
        *   @brief Verificação do término de uma palavra em um nó.
        *   @return Se é o final de uma palavra.
        */
        bool isEndOfWord() {
            if (this->dic_position >= 0 && this->dic_length > 0) {
                return true;
            }
            return false;
        }
    };

    Node* root; /**< Nó raiz da Trie. */ 
};

}  // namespace structures


#endif
