#include "fork-tree.h"
#include <stdexcept>
#include <cstring>
#include <algorithm>

using std::string;

void destroy_node(fork_t* node) {
  for (auto c : node->children) {
    if (!c->children.empty()) {
      destroy_node(c);
    }
  }

  delete node;
  node = NULL;
}

fork_t* find_node(fork_t* node, int pid) {
  fork_t* ret;
  if (node->pid == pid) {
    return node;
  } else {
    for (fork_t* c : node->children) {
      if (c->pid == pid) {
        return c;
      }
    }
    for (fork_t* c : node->children) {
      ret = find_node(c, pid);
      if (ret){
        if (ret->pid == pid) {
          return ret;
        }
      }
    }
  }
}

ForkTree::ForkTree() {
  ForkTree(0);
}

ForkTree::ForkTree(int pid) : root{new fork_t {pid, -1, NULL, {}}} {
  pid_list.push_back(pid);
}

ForkTree::ForkTree(string csv) {
  parse_csv_into_tree(csv);
}

ForkTree::~ForkTree() {
  destroy_node(root);
}

fork_t* ForkTree::insert_process(int pid, int ppid) {
  if (std::count(pid_list.begin(), pid_list.end(), ppid) != 1) {
    string msg("Bad ppid ");
    msg += std::to_string(ppid);
    msg += ", exists ";
    msg += std::to_string(std::count(pid_list.begin(), pid_list.end(), ppid));
    msg += " times!\n";
    throw std::invalid_argument(msg);
  }

  if (std::count(pid_list.begin(), pid_list.end(), pid) > 0) {
    throw std::invalid_argument("Already used pid\n");
  }

  fork_t* parent = find_node(root, ppid);
  fork_t* node = new fork_t {pid, ppid, parent, {}};
  parent->children.push_back(node);

  pid_list.push_back(pid);
  return node;
}

void ForkTree::remove_process(int pid) {
  if (std::count(pid_list.begin(), pid_list.end(), pid) != 1) {
    throw std::invalid_argument("Bad pid\n");
  }

  fork_t* removable = find_node(root, pid);

  if (removable->children.size() > 0) {
    for (auto c : removable->children) {
      c->parent = removable->parent;
      removable->parent->children.push_back(c);
    }
  }

  // This would be better done with std::set.
  // For the sake of simplicity, this version will use vector
  auto iterator = removable->parent->children.begin();
  for (auto c : removable->parent->children) {
    if (c->pid == pid) {
      removable->parent->children.erase(iterator);
      break;
    }
    iterator++;
  }

  removable->parent = NULL;
  delete removable;

  for (auto it = pid_list.begin(); it != pid_list.end(); it++) {
    if (*it == pid) {
      pid_list.erase(it);
      break;
    }
  }

}

string ForkTree::extract_test_from_csv(string csv, int test_num) {
  const string start_token("start");
  const string end_token("end");
  const string fork_token("fork");

  char* csv_cpy = new char[csv.length()];
  string test_csv(start_token);

  std::strcpy(csv_cpy, csv.c_str());

  char* cur_tok = std::strtok(csv_cpy, ",");
  bool found_test = false;

  while (!found_test && cur_tok != NULL) {
    while (start_token.compare(cur_tok) != 0) {
      cur_tok = std::strtok(NULL, ",\n");
    }
    // This should be the correct test_num
    cur_tok = std::strtok(NULL, ",\n");
    found_test = (std::to_string(test_num).compare(cur_tok) == 0);
  }

  while(end_token.compare(cur_tok) != 0 && start_token.compare(cur_tok) != 0) {
    test_csv += ",";
    test_csv += cur_tok;
    cur_tok = std::strtok(NULL, ",\n");
  }

  delete [] csv_cpy;

  test_csv += ",end";
  return test_csv;
}

void ForkTree::parse_csv_into_tree(string csv) {
  const string start_token("start");
  const string end_token("end");
  const string fork_token("fork");

  char csv_cpy[csv.length()]; //= new char[csv.length()];
  std::strcpy(csv_cpy, csv.c_str());

  char* cur_tok = std::strtok(csv_cpy, ",");

  while (end_token.compare(cur_tok) != 0) {
    // printf("On tree creation from csv, reading: %s\n", cur_tok);
    if (start_token.compare(cur_tok) == 0) {
      // printf("After start from csv, reading: %s\n", cur_tok);
      // Starting ProcTree
      // Get next token, it should be the TestId
      cur_tok = std::strtok(NULL, ",");
      // printf("Start 1 read... %s\n", cur_tok);
      // Next token should be the initial process id
      cur_tok = std::strtok(NULL, ",");
      // printf("Start 2 read... %s\n", cur_tok);
      int initial_proc_pid = std::stoi(cur_tok);
      // Assign our root node
      // delete root; // Delete current root
      root = new fork_t {initial_proc_pid, -1, NULL, {}};
      pid_list.push_back(initial_proc_pid);
    } else if (fork_token.compare(cur_tok) == 0) {
      // printf("After fork from csv, reading: %s\n", cur_tok);
      // Insert new ProcNode
      // Get the next token, the pid of the new process
      cur_tok = std::strtok(NULL, ",");
      // printf("Fork 1 read... %s\n", cur_tok);
      int pid = std::stoi(cur_tok);
      // Get the next token, the parent pid
      cur_tok = std::strtok(NULL, ",");
      // printf("Fork 2 read... %s\n", cur_tok);
      int ppid = std::stoi(cur_tok);

      this->insert_process(pid, ppid);

      cur_tok = std::strtok(NULL, ",");
    } else {
      // This should not happen, skip until a valid token is found
      // printf("Something wrong with the input... %s\n", cur_tok);
      cur_tok = std::strtok(NULL, ",");
    }
  }

  cur_tok = std::strtok(NULL,",");
}

string ForkTree::to_string() {

}
