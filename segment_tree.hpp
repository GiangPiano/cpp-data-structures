#pragma once
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

/**
 * @brief A generic segment tree supporting point updates and range queries.
 *
 * Stores elements of type T and answers range queries using a binary associative
 * operator. The operator determines what the tree computes — sum, min, max, XOR, etc.
 *
 * @tparam T        The element type. Must be default-constructible (T{} is used
 *                  as the identity/empty value).
 * @tparam Operator A callable type with signature `T(T, T)`. Must be:
 *                    - Binary: takes exactly two T arguments
 *                    - Associative: op(a, op(b, c)) == op(op(a, b), c)
 *                  Defaults to std::plus<T> (range sum queries).
 *
 * @note The operator does NOT need to be commutative.
 * @note For correct query results the operator should be associative. Non-associative
 *       operators will compile but produce undefined query behaviour.
 *
 * Complexity:
 *   - Build:  O(n)
 *   - Update: O(log n)
 *   - Query:  O(log n)
 *   - Space:  O(n)
 *
 * Example — range sum (default):
 * @code
 *   std::vector<int> nums = {1, 2, 3, 4, 5};
 *   SegmentTree<int> tree(nums);
 *   tree.query(1, 3);     // returns 9  (2+3+4)
 *   tree.update(2, 10);   // nums[2] = 10
 *   tree.query(1, 3);     // returns 16 (2+10+4)
 * @endcode
 *
 * Example — range max with custom operator:
 * @code
 *   struct MaxOp {
 *       int operator()(int a, int b) const { return std::max(a, b); }
 *   };
 *   SegmentTree<int, MaxOp> tree(nums);
 *   tree.query(0, 4);     // returns max element in [0, 4]
 * @endcode
 */
template <typename T, typename Operator = std::plus<int>>
class SegmentTree {
private:
  struct Node {
    std::unique_ptr<Node> left, right;
    T val;
    Node(T val = T{}) : left(nullptr), right(nullptr), val(val) {}
  };

  std::unique_ptr<Node> root;
  size_t size_;
  Operator oper_;

  std::unique_ptr<Node> build_tree(const std::vector<T>& elems, size_t l, size_t r) {
    if (l == r) return std::make_unique<Node>(elems[l]);
    auto node = std::make_unique<Node>();
    size_t mid = (l + r) >> 1;
    node->left = build_tree(elems, l, mid);
    node->right = build_tree(elems, mid + 1, r);
    node->val = oper_(node->left->val, node->right->val);
    return node;
  }

  void update_tree(size_t l, size_t r, std::unique_ptr<Node>& node, size_t index, T val) {
    if (l == r) {
      node->val = val;
      return;
    }
    size_t mid = (l + r) >> 1;
    if (index <= mid) update_tree(l, mid, node->left, index, val);
    else update_tree(mid + 1, r, node->right, index, val);
    node->val = oper_(node->left->val, node->right->val);
  }

  T query_tree(size_t l, size_t r, std::unique_ptr<Node>& node, size_t left, size_t right) {
    if (l == left && r == right) return node->val;
    size_t mid = (l + r) >> 1;
    if (right <= mid) return query_tree(l, mid, node->left, left, right);
    if (left > mid) return query_tree(mid + 1, r, node->right, left, right);
    return oper_(query_tree(l, mid, node->left, left, mid),
                 query_tree(mid + 1, r, node->right, mid + 1, right));
  }

public:
  /**
   * @brief Constructs an empty segment tree.
   *
   * Only valid if Operator is default-constructible. Calling update() or
   * query() on an empty tree is undefined behaviour.
   */
  SegmentTree() : size_(0), oper_(Operator{}) {}

  /**
   * @brief Constructs a segment tree from a vector of elements.
   *
   * @param elems The initial elements. The tree is built in O(n) time.
   *              If empty, the tree is valid but update/query must not be called.
   * @param oper  The combining operator instance. Only needs to be passed
   *              explicitly for stateful operators (e.g. weighted sum).
   *              Stateless operators (std::plus, MaxOp, etc.) are
   *              default-constructed automatically.
   */
  SegmentTree(const std::vector<T>& elems, Operator oper = Operator{})
      : size_(elems.size()), oper_(oper) {
    if (!elems.empty()) root = build_tree(elems, 0, size_ - 1);
  }

  /**
   * @brief Updates the element at the given index to a new value.
   *
   * @param index 0-based index of the element to update. Must be < size().
   * @param val   The new value to assign.
   *
   * @note Behaviour is undefined if index >= size() or the tree is empty.
   */
  void update(size_t index, T val) {
    update_tree(0, size_ - 1, root, index, val);
  }

  /**
   * @brief Queries the combined value over the range [left, right] (inclusive).
   *
   * Returns op(elems[left], op(elems[left+1], ... op(elems[right-1], elems[right]))).
   *
   * @param left  0-based index of the range start. Must be <= right.
   * @param right 0-based index of the range end (inclusive). Must be < size().
   * @return      The result of applying Operator across all elements in [left, right].
   *
   * @note Behaviour is undefined if left > right, right >= size(), or the tree is empty.
   */
  T query(size_t left, size_t right) {
    return query_tree(0, size_ - 1, root, left, right);
  }

  /**
   * @brief Returns the number of elements in the tree.
   */
  size_t size() const { return size_; }

  /**
   * @brief Returns true if the tree contains no elements.
   */
  bool empty() const { return size_ == 0; }
};
