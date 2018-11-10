//! Copyright [2018] Maria Eduarda de Melo Hang

#ifndef STRUCTURES_PARSER_H
#define STRUCTURES_PARSER_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "trie.cpp"

namespace structures {

class Parser {
 public:
 	Parser(std::string fileName) {
 		this->fileName = fileName;
 	}

 	std::string fileName;

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

 	void parsing(std::string dicText, structures::Trie trie) {  //!!!
 		size_t begin = 0, endWord, end;
 		size_t POS = 0;

 		std::string dicTmp = dicText.substr(begin);

        while (1) {
 			begin = dicTmp.find("[", begin);
 			if (begin == -1)
 				break;
 			endWord = dicTmp.find("]", begin);
 			if (endWord == -1)
 				break;
 			end = dicTmp.find("\n", begin);
 			if (end == -1)
 				break;
            
            std::string word = dicTmp.substr(begin + 1, endWord - 1);  //!!!
            
 			unsigned long position = POS;
 			unsigned long length = end - begin;

      //       std::cout << word << std:: endl;
		    // std::cout << "[" << position << "] " << "[" << length << "] " << "\n-------------\n";

		    POS += length + 1;
            dicTmp = dicText.substr(POS);


 			trie.insert(word, position, length);
 		}
 	}
};
}  // namespace structures //

#endif
