#include "binary_tree.h"
#include <iostream>

int main() {
	structures::BinaryTree<int> int_tree{};
	int values[10] = {10,5,8,20,25,15,-5,-10,30,-15};
	std:: cout << int_tree.empty() << std:: endl;
	for (int i = 0; i < 10; i++) {
		std:: cout << "Inserindo: " << values[i] << std:: endl;
		int_tree.insert(values[i]);
	}
	int_tree.pre_order();
	int_tree.in_order();
	int_tree.post_order();
	return 0;
}