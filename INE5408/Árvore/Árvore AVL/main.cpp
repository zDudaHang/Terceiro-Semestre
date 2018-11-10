#include "avl_tree.h"
#include <iostream>

int main() {
	structures::AVLTree<int> int_tree{};
	std:: cout << "Está vazia? " << int_tree.empty() << std:: endl;
	int values[10] = {10, 5, 8, 20, 25, 15, -5, -10, 30, -15};
	for(int i = 0; i < 10; i++) {
		int_tree.insert(values[i]);
	}
	for (int i = 0; i < 10; i++) {
		std:: cout << "Contém " << values[i] << " ? " << int_tree.contains(values[i]) << std:: endl;
	}
	std:: cout << "Tamanho: " << int_tree.size() << std:: endl;
	std:: cout << "Está vazia? " << int_tree.empty() << std:: endl;
	int_tree.remove(-15);
	std:: cout << "Contém -15 ? " << int_tree.contains(-15) << std:: endl;
	return 0;
}