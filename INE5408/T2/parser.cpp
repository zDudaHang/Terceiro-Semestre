/**
*    @file trie.cpp
*    @brief Árvore de Prefixos que suporta os caracteres de a até z como nodos. 
*    Não suporta acentos ou caracteres especiais para a inserção.
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
class Parser {
 public:
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
 	Parser(std::string fileName) {
 		this->fileName = fileName;
 	}

 	std::string fileName; /**< inteiro para guardar a linha do elemento da matriz */ 

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
