#pragma once

#include <cstddef>
#include <functional>
#include <vector>

template <typename T, typename Operator = std::plus<T>>
class SegmentTreeArray {
private:
  std::vector<T> tree;
  size_t size_;
  Operator oper_;

  void build_tree(const std::vector<T>& elems, int l, int r, int cur) {
    if (l == r) {
      tree[cur] = elems[l];
      return;
    }
    int mid = (l + r) / 2;
    build_tree(elems, l, mid, 2 * cur);
    build_tree(elems, mid + 1, r, 2 * cur + 1);
    tree[cur] = oper_(tree[2 * cur], tree[2 * cur + 1]);
  }

  void update_tree(int l, int r, int cur, int index, int val) {
    if (l == r) {
      tree[cur] = val;
      return;
    }
    int mid = (l + r) / 2;
    if (mid <= index) update_tree(l, mid, 2 * cur, index, val);
    else update_tree(mid + 1, r, 2 * cur + 1, index, val);
    tree[cur] = oper_(tree[2 * cur], tree[2 * cur + 1]);
  }

  T query_tree(int l, int r, int cur, int left, int right) {
    if (l == left && r == right) return tree[cur];
    int mid = (l + r) / 2;
    if (mid >= right) return query_tree(l, mid, 2 * cur, left, right);
    if (mid < left) return query_tree(mid + 1, r, 2 * cur + 1, left, right);
    return oper_(query_tree(l, mid, 2 * cur, left, mid),
                 query_tree(mid + 1, r, 2 * cur + 1, mid + 1, right));
  }

public:
  /**
   * @brief Constructs an empty segment tree.
   *
   * Only valid if Operator is default-constructible. Calling update() or
   * query() on an empty tree is undefined behaviour.
   */
  SegmentTreeArray() : size_(0), oper_(Operator{}) {}

  SegmentTreeArray(const std::vector<T>& elems, Operator oper = Operator{})
      : tree(elems.size() * 4), size_{elems.size()}, oper_{oper} {
    build_tree(elems, 0, size_ - 1, 1);
  }

  void update(int index, int val) { update_tree(0, size_ - 1, 1, index, val); }

  T query(int left, int right) { return query_tree(0, size_ - 1, 1, left, right); }

  size_t size() { return size_; }

  bool empty() { return size_ == 0; }
};
