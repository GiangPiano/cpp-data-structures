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
    bool is_word = false;
  };

  std::unique_ptr<TrieNode> root;
  size_t size_;

public:
  Trie() : root{std::make_unique<TrieNode>()}, size_{0} {}

  void insert(const std::string& word) {
    TrieNode* node = root.get();
    for (char c : word) {
      int idx = c - 'a';
      if (!node->children[idx]) node->children[idx] = std::make_unique<TrieNode>();
      node = node->children[idx].get();
    }
    if (node->is_word) return;
    node->is_word = true;
    size_++;
  }

  void erase(const std::string& word) {
    TrieNode* node = root.get();
    for (char c : word) {
      int idx = c - 'a';
      if (!node->children[idx]) return;
      node = node->children[idx].get();
    }
    if (!node->is_word) return;
    node->is_word = false;
    size_--;
  }

  [[nodiscard]] bool contains(const std::string& word) const {
    TrieNode* node = root.get();
    for (char c : word) {
      int idx = c - 'a';
      if (!node->children[idx]) return false;
      node = node->children[idx].get();
    }
    return node->is_word;
  }

  [[nodiscard]] bool hasPrefix(const std::string& word) const {
    TrieNode* node = root.get();
    for (char c : word) {
      int idx = c - 'a';
      if (!node->children[idx]) return false;
      node = node->children[idx].get();
    }
    return true;
  }

  [[nodiscard]] std::vector<std::string> search(const std::string& prefix) const {
    std::vector<std::string> res{};
    TrieNode* node = root.get();

    for (char c : prefix) {
      int idx = c - 'a';
      if (!node->children[idx]) return {};
      node = node->children[idx].get();
    }

    std::queue<std::pair<const TrieNode*, std::string>> bfs({{node, prefix}});
    while (!bfs.empty()) {
      auto [curr, str] = bfs.front();
      bfs.pop();
      if (curr->is_word) res.push_back(str);
      for (char c = 'a'; c <= 'z'; c++) {
        const TrieNode* next = curr->children[c - 'a'].get();
        if (next) bfs.push({next, str + c});
      }
    }

    return res;
  }

  [[nodiscard]] size_t size() const {
    return size_;
  }
};
