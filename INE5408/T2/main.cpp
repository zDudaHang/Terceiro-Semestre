//! Copyright [2018] Maria Eduarda de Melo Hang

#include <iostream>
#include <string>
#include <cstring>
#include "parser.cpp"

int main() {

    std::string filename;
    std::string word;
    std::cin >> filename;  // entrada

    structures::Parser parser = structures::Parser(filename);

    structures::Trie trie {};

    trie.initMap();

    std::string dicText = parser.getDicText();

    parser.parsing(dicText, trie);

    while (1) {  // leitura das palavras ate' encontrar "0"
        std:: cin >> word;
        if (word.compare("0") == 0) {
            break;
        }
        trie.verifyTypeOfWord(word);
    }

    return 0;
}
