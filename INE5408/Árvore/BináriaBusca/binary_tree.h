//! Copyright [2018] Maria Eduarda de Melo Hang

#ifndef STRUCTURES_BINARY_TREE_H
#define STRUCTURES_BINARY_TREE_H

#include "array_list.h"

namespace structures {

//! Classe de Arvore Binaria
template <typename T>
class BinaryTree {
 public:
    //! Constructor
    BinaryTree() {
        root = nullptr;
        size_ = 0;
    }

    //! Destrutor
    ~BinaryTree() = default;

    //! Inserir novo dado
    void insert(const T& data) {
        if (root == nullptr) {
            root = new Node(data);

        } else if (data > root->data) {
            if (root->right != nullptr) {
                root->right->insert(data);
            } else {
                root->right = new Node(data);
            }
        } else {
            if (root->left != nullptr) {
                root->left->insert(data);
            } else {
                root->left = new Node(data);
            }
        }

       size_++;
    }

    //! Remove um Dado
    bool remove(const T& data) {
        if (root == nullptr) {
            return false;
        }
        Node* temp = new Node();
        temp->left = root;
        temp->right = root;
        bool result = root->remove(data, temp);
        root = temp->left;
        delete temp;
        size_--;
        return result;
    }

    //! Verifica se o dado estah na arvore
    bool contains(const T& data) const {
        if (size_ == 0) {
            return false;
        }
        return root->contains(data);
    }

    //! Verifica se estah vazia
    bool empty() const {
        return (size_ == 0);
    }

    //! Quantidade de elementos
    size_t size() const {
        return size_;
    }

    //! Percurso em Pre-Ordem
    std::string pre_order() const {
    }

 private:
    struct Node {
        //! Construtor padrao
        Node() = default;

        //! Construtor com dado
        explicit Node(const T& data_) {
            data = data_;
        }

        //! Dado
        T data;

        //! Filho Esquerdo
        Node* left;

        //! Filho Direito
        Node* right;

        //! Inserir um dado
        void insert(const T& data_) {
            if (data_ > data) {
                if (right == nullptr) {
                    right = new Node(data_);
                } else {
                    right->insert(data_);
                }
            } else {
                if (left == nullptr) {
                    left = new Node(data_);
                } else {
                    left->insert(data_);
                }
            }
        }

        //! Remove um dado
        bool remove(const T& data_, Node* parent) {
            if (data_ == data) {
                // Folha
                if (left == nullptr && right == nullptr) {
                    if ((parent->left != nullptr) &&
                        (data == parent->left->data)) {
                        parent->left = nullptr;
                    } else {
                        parent->right = nullptr;
                    }
                    return true;

                // 1 Filho:
                } else if ((left != nullptr) ||
                           (right != nullptr)) {
                    if (left != nullptr) {
                        if ((parent->left != nullptr) &&
                            (data == parent->left->data)) {
                            parent->left = left;
                        } else {
                            parent->right = left;
                        }

                        return true;
                    } else {
                        if ((parent->left != nullptr) &&
                            (data == parent->left->data)) {
                            parent->left = right;
                        } else {
                            parent->right = right;
                        }

                        return true;
                    }

                // 2 Filhos:
                } else {
                    data = right->minValue();
                    return right->remove(data, this);
                }
            } else {
                if (data > data_) {
                    return left->remove(data_, this);
                } else {
                    return right->remove(data_, this);
                }
            }
        }

        //! Pega o minimo
        T minValue() {
            if (left != nullptr) {
                return left->minValue();
            } else {
                return data;
            }
        }

        //! Verifica se esta na subarvore
        bool contains(const T& data_) const {
            if (data == data_) {
                return true;

            } else if (data_ < data) {
                if (left == nullptr) {
                    return false;
                } else {
                    return left->contains(data_);
                }
            } else {
                if (right == nullptr) {
                    return false;
                } else {
                    return right->contains(data_);
                }
            }
        }

        //! Percurso em Pre-Ordem
        void pre_order(ArrayList<T>& v) const {
            v.push_back(data);
            if (left != nullptr)
                left->pre_order(v);
            if (right != nullptr)
                right->pre_order(v);
        }

        //! Percurso em Ordem
        void in_order(ArrayList<T>& v) const {
            if (left != nullptr)
                left->in_order(v);
            v.push_back(data);
            if (right != nullptr)
                right->in_order(v);
        }

         //! Percuso Pos-Ordem
        void post_order(ArrayList<T>& v) const {
            if (left != nullptr)
                left->post_order(v);
            if (right != nullptr)
                right->post_order(v);
            v.push_back(data);
        }
    };
    //! Quantidade de elementos
    size_t size_;

    //! Raiz
    Node* root;
};

}  // namespace structures

#endif
