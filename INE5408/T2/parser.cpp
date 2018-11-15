/**
*    @file parser.cpp
*    @brief Parser dos arquivos dos dicionários.
*    @author Maria Eduarda de Melo Hang
*    @date 15 November 2018
*/

#ifndef STRUCTURES_PARSER_H
#define STRUCTURES_PARSER_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "trie.cpp"

namespace structures {
/** Classe para o Parser. */
class Parser {
 public:
	/**
	*   @brief Construtor do Parser.
	*   @param fileName Nome do arquivo do dicionário.
	*/
 	Parser(std::string fileName) {
 		this->fileName = fileName;
 	}

 	std::string fileName; /**< Nome do arquivo que o Parser realizará a análise, */ 

 	/**
	*   @brief Armazenamento do texto do arquivo do dicionário.
	*   @return O texto do dicionário.
	*/
 	std::string getDicText() {
 		std::ifstream source(fileName);
		if (!source) {
	    	std:: cout << "error" << std:: endl;
	    	exit(1);
	    }
	    char input;
	    std::string dicText;
	    while (source.get(input)) {
	    	dicText += input;
	    }
	    return dicText;
 	}

 	/**
	*   @brief Extração das palavras, posições e comprimentos.
	*
	*   Primeiramente ocorrerá a extração da palavra através da procura pelos caracteres ']' (begin) e '[' (endWord).
	*	Posteriormente, será buscada o índice do caractere de quebra de linha (end). O comprimento será a diferença
	*	entre o índice begin e o end, enquanto que a posição será o próprio begin. A atualização do índice begin
	*	é realizada em saltos conforme o comprimento das linhas do dicionário. Por fim, a palavra será inserida
	*	na Trie.
	*
	*   @param dicText O texto do dicionário.
	*	@param trie A árvore de indexação.
	*/
 	void parsing(std::string dicText, structures::Trie trie) {
 		size_t begin = 0, endWord, end;

        while (1) {
 			begin = dicText.find("[", begin);
 			if (begin == -1) {
 				break;
 			}
 			endWord = dicText.find("]", begin);
 			if (endWord == -1) {
 				break;
 			}
 			end = dicText.find("\n", begin);
 			if (end == -1) {
 				break;
 			}

            std::string word = dicText.substr(begin + 1, endWord - begin - 1);
            
 			unsigned long position = begin;
 			unsigned long length = end - begin;

      		//  std::cout << word << " ";
		    //  std::cout << "[" << position << "] " << "[" << length << "] " << "\n-------------\n";
		    
		    begin += length + 1;

 			trie.insert(word, position, length);
 		}
 	}
};
}  // namespace structures //

#endif
