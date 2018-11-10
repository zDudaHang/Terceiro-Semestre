//#include <gtest/gtest.h>

#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "fork-tree.h"

class TestingForks : public ::testing::Test {
public:
  std::string base_csv;

  // Executed always before test case
  void SetUp() {
    std::ifstream input("test.out", std::ifstream::in);
    base_csv = "";
    std::string tmp;
    while (!input.eof() && !input.fail()) {
      input >> tmp;
      tmp += "\n";
      base_csv += tmp;
    }
    input.close();
  }

  // Executed always after test case
  void TearDown() {
    base_csv = "";
  }

};

bool assert_decrescent_n_children_on(std::vector<fork_t*> parents, int count_start) {
  while (count_start >= 0) {
    auto iterator = parents.begin();
	for (unsigned i = 0; i < parents.size(); ++i) {
	  if (parents[i]->children.size() == count_start) {
		break;
	  }
	  iterator++;
	  if (i == parents.size()) {
		return false;
	  }
	}
	parents.erase(iterator);
	count_start--;
  }

  return true;
}

TEST_F(TestingForks, Exercise1NumProcs) {
  // Prepare test
  std::string test_st = ForkTree::extract_test_from_csv(base_csv, 1);
  ForkTree three_children = ForkTree(test_st);
  int fpid = three_children.root->pid;
  auto children = three_children.root->children;
  int n_children = children.size();
  ASSERT_EQ(3, n_children);

  int child_ids[3] = {children[0]->pid, children[1]->pid, children[2]->pid};
  ASSERT_TRUE(child_ids[0] > fpid);
  ASSERT_TRUE(child_ids[1] > fpid);
  ASSERT_TRUE(child_ids[2] > fpid);
}

TEST_F(TestingForks, Exercise2NumProcs) {
  std::string test_st = ForkTree::extract_test_from_csv(base_csv, 2);
  ForkTree triple_forked = ForkTree(test_st);

  int fpid = triple_forked.root->pid;
  auto children = triple_forked.root->children;
  int n_children = children.size();
  ASSERT_EQ(3, n_children);

  int child_ids[3] = {children[0]->pid, children[1]->pid, children[2]->pid};
  ASSERT_TRUE(child_ids[0] > fpid);
  ASSERT_TRUE(child_ids[1] > fpid);
  ASSERT_TRUE(child_ids[2] > fpid);

  auto proc = children[0];
  auto _children = proc->children;
  n_children = _children.size();
  ASSERT_TRUE(assert_decrescent_n_children_on(children, 2));

  int _child_ids[2] = {_children[0]->pid, _children[1]->pid};
  ASSERT_TRUE(_child_ids[0] > proc->pid);
  ASSERT_TRUE(_child_ids[1] > proc->pid);
  ASSERT_TRUE(assert_decrescent_n_children_on(_children, 1));
}

TEST_F(TestingForks, Exercise3NumProcs) {
    std::string test_st = ForkTree::extract_test_from_csv(base_csv, 3);
    ForkTree balanced_tree = ForkTree(test_st);

    int fpid = balanced_tree.root->pid;
    auto children = balanced_tree.root->children;
    ASSERT_EQ(2, children.size());
    ASSERT_EQ(2, children[0]->children.size());
    ASSERT_EQ(2, children[1]->children.size());

    for (auto pid : balanced_tree.pid_list) {
        ASSERT_TRUE(pid >= fpid);
    }

    ASSERT_EQ(7, balanced_tree.pid_list.size());
}

TEST_F(TestingForks, Exercise4NumProcs) {
    std::string test_st = ForkTree::extract_test_from_csv(base_csv, 4);
    ForkTree balanced_tree_right = ForkTree(test_st);

    int fpid = balanced_tree_right.root->pid;
    auto children = balanced_tree_right.root->children;
    ASSERT_EQ(3, children.size());

    // Determine the index of the child with no children
    int last_pindex = -1;
    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->children.size() == 0) {
            last_pindex = i;
            break;
        }
    }

    for (int i = 0; i < children.size(); ++i) {
        if (i == last_pindex) {
            ASSERT_EQ(0, children[i]->children.size());
        } else {
            ASSERT_EQ(2, children[i]->children.size());
        }
    }

    for (auto pid : balanced_tree_right.pid_list) {
        ASSERT_TRUE(pid >= fpid);
    }

    ASSERT_EQ(8, balanced_tree_right.pid_list.size());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
