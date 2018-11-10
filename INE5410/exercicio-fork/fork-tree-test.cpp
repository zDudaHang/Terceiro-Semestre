
//#include <gtest/gtest.h>

#include <string>
#include <fstream>
#include <iostream>
#include "fork-tree.h"

class TestingTree : public ::testing::Test {

};


TEST_F(TestingTree, Creation) {
  ForkTree* f = new ForkTree(0);
  ASSERT_EQ(0, f->root->pid);
  ASSERT_EQ(0, f->root->children.size());

  delete f;
}

TEST_F(TestingTree, NodeRemoval) {
  ForkTree* f = new ForkTree(0);
  f->insert_process(1, 0);
  f->insert_process(2,1);
  f->remove_process(2);
  ASSERT_EQ(1, f->root->children.size());
  ASSERT_EQ(1, f->root->children[0]->pid);
  ASSERT_EQ(0, f->root->children[0]->children.size());

  delete f;
}

TEST_F(TestingTree, TwoNodeRemoval) {
  ForkTree* f = new ForkTree(0);
  f->insert_process(1, 0);
  f->insert_process(2,1);
  f->insert_process(3,1);
  f->remove_process(1);
  ASSERT_EQ(2, f->root->children.size());
  ASSERT_EQ(2, f->root->children[0]->pid);
  ASSERT_EQ(3, f->root->children[1]->pid);
  ASSERT_EQ(3, f->pid_list.size());

  delete f;
}

TEST_F(TestingTree, InsertNode) {
  ForkTree* f = new ForkTree(0);
  f->insert_process(1, 0);
  ASSERT_EQ(1, f->root->children.size());
  ASSERT_EQ(1, f->root->children[0]->pid);

  delete f;
}

TEST_F(TestingTree, Insert2Nodes) {
  ForkTree* f = new ForkTree(0);
  f->insert_process(1, 0);
  f->insert_process(2,1);
  ASSERT_EQ(1, f->root->children.size());
  ASSERT_EQ(1, f->root->children[0]->pid);
  ASSERT_EQ(1, f->root->children[0]->children.size());
  ASSERT_EQ(2, f->root->children[0]->children[0]->pid);

  delete f;
}

TEST_F(TestingTree, TestToCSV) {
  std::string csv("start,0,0\nfork,1,0\nfork,2,0\nfork,3,0\nend,0,0\n");
  std::string testable = ForkTree::extract_test_from_csv(csv, 0);
  std::string ready_csv("start,0,0,fork,1,0,fork,2,0,fork,3,0,end");
  ASSERT_EQ(ready_csv, testable);
}

TEST_F(TestingTree, SecondTestToCSV) {
  std::string csv("start,0,0\nfork,1,0\nfork,2,0\nfork,3,0\nend,0,0\nstart,1,0\nfork,1,0\nfork,2,0\nfork,3,0\nend,0,0\nstart,2,0\nfork,1,0\nfork,2,0\nfork,3,0\nend,0,0\n");
  std::string testable = ForkTree::extract_test_from_csv(csv, 1);
  std::string ready_csv("start,1,0,fork,1,0,fork,2,0,fork,3,0,end");
  ASSERT_EQ(ready_csv, testable);
}

TEST_F(TestingTree, CSVCreation) {
  std::string csv("start,0,0\nfork,1,0\nfork,2,0\nfork,3,0\nend,0,0\n");
  std::string testable = ForkTree::extract_test_from_csv(csv, 0);
  ForkTree* f = new ForkTree(testable);

  ASSERT_EQ(4, f->pid_list.size());
  ASSERT_EQ(0, f->root->pid);
  ASSERT_EQ(1, f->root->children[0]->pid);
  ASSERT_EQ(2, f->root->children[1]->pid);
  ASSERT_EQ(3, f->root->children[2]->pid);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
