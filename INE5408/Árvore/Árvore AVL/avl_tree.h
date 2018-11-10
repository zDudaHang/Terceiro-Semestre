//! Copyright [2018] Maria Eduarda de Melo Hang

#ifndef STRUCTURES_AVLTREE_TREE_H
#define STRUCTURES_AVLTREE_TREE_H

#include "array_list.h"

namespace structures {

//! Classe de Arvore Binaria
template <typename T>
class AVLTree {
 public:
    //! Constructor
    AVLTree() {
        root = nullptr;
        size_ = 0;
    }

    //! Destrutor
    ~AVLTree() = default;

    //! Inserir novo dado
    void insert(const T& data_) {
        if (root == nullptr) {
            root = new Node(data_);
        } else if (data_ < root->data) {
            if (root->left != nullptr) {
                root->left->insert(data_);
            } else {
                root->left = new Node(data_);
                root->left->updateHeight();
            }
        } else {
            if (root->right != nullptr) {
                root->right->insert(data_);
            } else {
                root->right = new Node(data_);
                root->right->updateHeight();
            }
        }
        size_++;
    }

    //! Remove um Dado
    bool remove(const T& data_) {
        if (root == nullptr) {
            return false;
        }
        if (data_ == root->data) {
            delete root;
            size_--;
            return true;
        } else {
            if (data_ < root->data) {
                size_--;
                return root->left->remove(data_, root);
            } else {
                size_--;
                return root->right->remove(data_, root);
            }
        }
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

    /*!Visite a subarvores*/
    ArrayList<T> pre_order() const {
         ArrayList<T> v{size_};
        if (!empty())
            root->pre_order(v);
        return v;
    }

    /*!Visite a subarvores*/
    ArrayList<T> in_order() const {
        ArrayList<T> v{size_};
        if (!empty())
            root->in_order(v);
        return v;
    }
    /*!Visite a raiz depois das subarvores*/
    ArrayList<T> post_order() const {
        ArrayList<T> v{size_};
        if (!empty())
            root->post_order(v);
        return v;
    }

 private:
    struct Node {
        //! Construtor padrao
        Node() = default;

        //! Construtor com dado
        explicit Node(const T& data_) {
            data = data_;
            left = nullptr;
            right = nullptr;
            height = 0;
        }
        //! Dado
        T data;

        //! Filho Esquerdo
        Node* left;

        //! Filho Direito
        Node* right;

        //! Altura
        std::size_t height;

        int getHeight(Node* subtree) {
            if (subtree == nullptr) {
                return 0;
            } else {
                return subtree->height;
            }
        }

        //! Rotacao simples a esquerda
        Node* rotacaoLL() {
            Node* aux = right;
            aux->left = this;
            right = right->left;
            aux->updateHeight();
            aux->left->updateHeight();
            aux->right->updateHeight();
            return aux;
        }

        //!
        Node* rotacaoRR() {
            Node* aux = left;
            aux->right = this;
            left = left->right;
            aux->updateHeight();
            aux->left->updateHeight();
            aux->right->updateHeight();
            return aux;
        }

        //! Rotacao dupla a esquerda
        Node* rotacaoRL() {
            right = right->rotacaoLL();  //! LL
            return rotacaoRR();  //! RR
        }

        //! Rotacao dupla a direita
        Node* rotacaoLR() {
            left = left->rotacaoRR();  //! RR
            return rotacaoLL();  //! LL
        }

        //! Inserir um dado
       void insert(const T& data_) {
           if (data_ < data) {
                if (left == nullptr)
                    left = new Node(data_);
                else
                    left->insert(data_);
            } else {
                if (right == nullptr)
                    right = new Node(data_);
                else
                    right->insert(data_);
            }

            updateHeight();

            int fb_node = fb();
            if (fb_node != 0) {  // No' desbalanceado
                if (fb_node > 0) {
                    if (left->fb() > 0)
                        left = rotacaoRR();
                    else
                        left = rotacaoLR();
                } else {
                    if (right->fb() > 0)
                        right = rotacaoRL();
                    else
                        right = rotacaoLL();
                }
            }
        }


        int maximo(int data1, int data2) {
            if (data1 >= data2)
                return data1;
            else
                return data2;
        }
        void updateHeight() {
            if ((left != nullptr) && (right != nullptr)) {
                height = maximo(left->height, right->height);
            } else if (left != nullptr) {
                height = left->height;
            } else if (right != nullptr) {
                height = right->height;
            } else {
                height = 0;
            }
        }
        int fb() {
            if ((left != nullptr) && (right != nullptr))
                return left->height - right->height;
            if (left != nullptr)
                return left->height;
            if (right != nullptr)
                return 0 - right->height;
            return 0;
        }
        //! Remove um dado
        bool remove(const T& data_, Node* parent) {
            bool remove_return = false;
            if (data_ == data) {
                // Folha
                if (left == nullptr && right == nullptr) {
                    if ((parent->left != nullptr) &&
                        (data == parent->left->data)) {
                        parent->left = nullptr;
                    } else {
                        parent->right = nullptr;
                    }
                    //  delete this;
                    remove_return = true;

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
                        //  delete this;
                        remove_return = true;
                    } else {
                        if ((parent->left != nullptr) &&
                            (data == parent->left->data)) {
                            parent->left = right;
                        } else {
                            parent->right = right;
                        }
                        //  delete this;
                        remove_return = true;
                    }

                // 2 Filhos:
                } else {
                    data = right->minValue();
                    remove_return = right->remove(data, this);
                }
            } else {
                if (data > data_) {
                    remove_return = left->remove(data_, this);
                } else {
                    remove_return = right->remove(data_, this);
                }
            }
             updateHeight();
            //! Verificando o desbalanceamento
            int fb_node = fb();
            if (fb_node != 0) {  // No' desbalanceado
                if (fb_node > 0) {
                    if (left->fb() > 0)
                        left = rotacaoRR();
                    else
                        left = rotacaoLR();
                } else {
                    if (right->fb() > 0)
                        right = rotacaoRL();
                    else
                        right = rotacaoLL();
                }
            }
            return remove_return;
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
