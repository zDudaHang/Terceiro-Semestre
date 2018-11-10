#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <list>


struct fork_t {
  int pid;
  int ppid;
  fork_t* parent;
  std::vector<fork_t*> children;
};

class ForkTree {
 public:
  ForkTree();
  ForkTree(int pid);
  ForkTree(std::string csv); // Uses parse_csv_into_tree
  ~ForkTree();
  fork_t* insert_process(int pid, int ppid);
  void remove_process(int pid);
  std::string to_string();

  static std::string extract_test_from_csv(std::string csv, int test_num);

  // private:
  fork_t* root;
  std::vector<int> pid_list;

 protected:
  void parse_csv_into_tree(std::string in);

};
