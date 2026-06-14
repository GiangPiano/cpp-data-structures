#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

class Trie {
  struct TrieNode {
    std::array<std::unique_ptr<TrieNode>, 26> children;
    bool is_word;
  };

  std::unique_ptr<TrieNode> root;
  int size_;

public:
  Trie() : root{std::make_unique<TrieNode>()}, size_{0} {}

  void insert(const std::string& word) {
    TrieNode* node = root.get();
    for (char c : word) {
      int cur = c - 'a';
      if (!node->children[cur]) node->children[cur] = std::make_unique<TrieNode>();
      node = node->children[cur].get();
    }
    node->is_word = true;
  }

  void erase(const std::string& word) {
    TrieNode* node = root.get();
    for (char c : word) {
      int cur = c - 'a';
      node = node->children[cur].get();
      if (!node) return;
    }
    node->is_word = false;
  }

  bool contains(const std::string& word) {
    TrieNode* node = root.get();
    for (char c : word) {
      int cur = c - 'a';
      node = node->children[cur].get();
      if (!node) return false;
    }
    return node->is_word;
  }

  std::vector<std::string> search(const std::string& prefix) {
    std::vector<std::string> res{};
    TrieNode* node = root.get();

    for (char c : prefix) {
      int cur = c - 'a';
      node = node->children[cur].get();
      if (!node) return res;
    }

    std::queue<std::pair<TrieNode*, std::string>> bfs({{node, prefix}});
    while (!bfs.empty()) {
      auto& [node, str] = bfs.front();
      bfs.pop();

      if (node->is_word) res.push_back(str);

      for (char c = 'a'; c <= 'z'; c++) {
        int cur = c - 'a';
        if (!node->children[cur]) continue;
        TrieNode* next = node->children[cur].get();
        bfs.push({next, str + c});
      }
    }

    return res;
  }

  size_t size() {
    return size_;
  }
};
