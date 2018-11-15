/**
*    @file main.cpp
*    @brief Segundo trabalho da disciplina de Estrutura de Dados(INE5408).

*    Propósito: Indexação e recuperação eficiente de palavras em grandes arquivos de dicionários (mantidos em memória secundária).
*    @author Maria Eduarda de Melo Hang
*    @date 15 November 2018
*/
#include <iostream>
#include <string>
#include <cstring>
#include "parser.cpp"

/**
*   @brief Recebe o nome do arquivo como entrada, inicializa a trie e o parser e chama os métodos
*   necessários para a realização correta do trabalho.
*/
int main() {

    std::string filename;
    std::string word;
    std::cin >> filename;

    structures::Parser parser = structures::Parser(filename);
    
    std::string dicText = parser.getDicText();

    structures::Trie trie {};

    parser.parsing(dicText, trie);
    
    while (1) {  // leitura das palavras ate' encontrar "0"
        std:: cin >> word;
        if (word.compare("0") == 0) {
            break;
        }
        trie.isPrefix(word);
    }

    return 0;
}
