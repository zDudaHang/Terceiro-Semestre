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

 	void parsing(std::string dicText, structures::Trie trie) {
 		size_t begin = 0, endWord, end;
 		while (begin < dicText.size()) {
 			begin = dicText.find("[", begin);
 			if (begin == std::string::npos) {
 				break;
 			}
 			endWord = dicText.find("]", begin);
 			if (endWord == std::string::npos) {
 				break;
 			}
 			end = dicText.find("\n", begin);
 			if (end == std::string::npos) {
 				break;
 			}
 			std::string word = dicText.substr(begin + 1, endWord - begin - 1);
 			unsigned long position = begin;
 			unsigned long length = end - begin;
 			begin++;
 			trie.insert(word, position, length);
 		}
 	}
};
}  // namespace structures //

#endif
