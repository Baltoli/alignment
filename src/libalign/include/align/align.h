#pragma once

#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <variant>

namespace align {

struct gap {
};

template <typename T>
struct options {
  template <typename F>
  options(F f, int indel)
      : match_score(f)
      , indel_score(indel)
  {
  }

  std::function<int(T const&, T const&)> match_score;
  int indel_score;
};

template <typename T, typename Iterator>
struct alignment {
  using aligned_element_t = std::variant<T, gap>;

  alignment(
      options<T> opts, Iterator a_begin, Iterator a_end, Iterator b_begin,
      Iterator b_end);

  auto trace_back() const;

  void dump_scores();

private:
  options<T> opts_;

  Iterator a_begin_;
  Iterator a_end_;
  Iterator b_begin_;
  Iterator b_end_;

  int rows_;
  int cols_;

  int& score_at(int row, int col);

  std::unique_ptr<int[]> data_;
};

template <typename T, typename Iterator>
alignment(options<T>, Iterator, Iterator, Iterator, Iterator)
    -> alignment<T, Iterator>;

template <typename T, typename Iterator>
alignment<T, Iterator>::alignment(
    options<T> opts, Iterator a_begin, Iterator a_end, Iterator b_begin,
    Iterator b_end)
    : opts_(opts)
    , a_begin_(a_begin)
    , a_end_(a_end)
    , b_begin_(b_begin)
    , b_end_(b_end)
    , rows_(std::distance(a_begin, a_end) + 1)
    , cols_(std::distance(b_begin, b_end) + 1)
    , data_(std::make_unique<int[]>(rows_ * cols_))
{
  for (auto i = 0; i < cols_; ++i) {
    score_at(0, i) = i * opts_.indel_score;
  }

  for (auto j = 0; j < rows_; ++j) {
    score_at(j, 0) = j * opts_.indel_score;
  }

  auto a_it = a_begin;
  for (auto row = 1; row < rows_; ++row, ++a_it) {
    auto b_it = b_begin;
    for (auto col = 1; col < cols_; ++col, ++b_it) {
      auto match = score_at(row - 1, col - 1) + opts_.match_score(*a_it, *b_it);
      auto a_gap = score_at(row, col - 1) + opts_.indel_score;
      auto b_gap = score_at(row - 1, col) + opts_.indel_score;

      score_at(row, col) = std::max({match, a_gap, b_gap});
    }
  }
}

template <typename T, typename Iterator>
auto alignment<T, Iterator>::trace_back() const
{
  using elt = alignment<T, Iterator>::aligned_element_t;

  auto a_align = std::vector<elt> {};
  auto b_align = std::vector<elt> {};

  // Start at rs-1, cs-1 and check the three neighbours
  // Read off characters of a, b depending on the move
  // Finally reverse the

  return std::pair {a_align, b_align};
}

template <typename T, typename Iterator>
void alignment<T, Iterator>::dump_scores()
{
  constexpr bool use_chars = std::is_same_v<char, T>;

  auto begin = data_.get();
  auto end = data_.get() + (rows_ * cols_);

  auto max = *std::max_element(begin, end);
  auto min = *std::min_element(begin, end);

  auto elt_width
      = std::max({std::to_string(max).size(), std::to_string(min).size()}) + 1;

  std::cout << std::right;

  if constexpr (use_chars) {
    std::cout << "  " << std::setw(elt_width) << ' ' << ' ';

    for (auto col = 1; col < cols_; ++col) {
      std::cout << std::setw(elt_width) << *(b_begin_ + (col - 1)) << ' ';
    }

    std::cout << '\n';
  }

  for (auto row = 0; row < rows_; ++row) {
    if constexpr (use_chars) {
      std::cout << (row > 0 ? *(a_begin_ + (row - 1)) : ' ') << ' ';
    }

    for (auto col = 0; col < cols_; ++col) {
      std::cout << std::setw(elt_width) << score_at(row, col) << ' ';
    }

    std::cout << '\n';
  }
}

template <typename T, typename Iterator>
int& alignment<T, Iterator>::score_at(int row, int col)
{
  return data_[row * cols_ + col];
}

} // namespace align
