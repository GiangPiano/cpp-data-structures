#pragma once
#include <array>
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief A trie (prefix tree) storing lowercase ASCII words ('a'–'z').
 *
 * Provides O(k) insert, erase, contains, and hasPrefix where k is the word
 * length, and O(k + output) prefix search.
 *
 * Uses a tree of heap-allocated nodes, each holding 26 child pointers and
 * a boolean end-of-word marker. Memory is managed automatically via unique_ptr.
 *
 * @note Behaviour is undefined for strings containing characters outside 'a'–'z'.
 * @note Erased words' nodes are not reclaimed; the tree never shrinks in memory.
 *
 * Complexity:
 *   - Insert:    O(k)
 *   - Erase:     O(k)
 *   - Contains:  O(k)
 *   - HasPrefix: O(k)
 *   - Search:    O(k + output)
 *   - Space:     O(alphabet * total characters inserted)
 *
 * Example:
 * @code
 *   Trie trie;
 *   trie.insert("apple");
 *   trie.insert("app");
 *   trie.contains("app");        // true
 *   trie.hasPrefix("appl");      // true
 *   trie.search("app");          // {"app", "apple"}
 *   trie.erase("app");
 *   trie.contains("app");        // false
 *   trie.contains("apple");      // true
 * @endcode
 */
class Trie {
  struct TrieNode {
    std::array<std::unique_ptr<TrieNode>, 26> children;
    bool is_word = false;
  };

  std::unique_ptr<TrieNode> root;
  std::size_t size_;

public:
  Trie() : root{std::make_unique<TrieNode>()}, size_{0} {}

  /**
   * @brief Inserts a word into the trie.
   * @param word Lowercase ASCII word to insert.
   * @note  Inserting a duplicate has no effect.
   */
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

  /**
   * @brief Removes a word from the trie if present.
   * @param word Lowercase ASCII word to remove.
   * @note  Has no effect if the word is not in the trie.
   * @note  Does not reclaim memory from now-unused nodes.
   */
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

  /**
   * @brief Returns true if the exact word is stored in the trie.
   * @param word Lowercase ASCII word to look up.
   */
  [[nodiscard]] bool contains(const std::string& word) const {
    const TrieNode* node = root.get();
    for (char c : word) {
      int idx = c - 'a';
      if (!node->children[idx]) return false;
      node = node->children[idx].get();
    }
    return node->is_word;
  }

  /**
   * @brief Returns true if any stored word begins with the given prefix.
   * @param prefix Prefix to search for. An empty prefix always returns true.
   */
  [[nodiscard]] bool hasPrefix(const std::string& prefix) const {
    const TrieNode* node = root.get();
    for (char c : prefix) {
      int idx = c - 'a';
      if (!node->children[idx]) return false;
      node = node->children[idx].get();
    }
    return true;
  }

  /**
   * @brief Returns all words that begin with the given prefix, in BFS (shortest-first) order.
   * @param prefix Common prefix to search under. Pass "" to retrieve every word.
   * @return Vector of matching words, empty if none share the prefix.
   */
  [[nodiscard]] std::vector<std::string> search(const std::string& prefix) const {
    std::vector<std::string> res{};
    const TrieNode* node = root.get();
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

  /**
   * @brief Returns the number of unique words currently stored.
   */
  [[nodiscard]] std::size_t size() const {
    return size_;
  }
};
