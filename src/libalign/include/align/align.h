#pragma once

#include <functional>
#include <memory>
#include <variant>

namespace align {

template <typename T>
struct options {
  template <typename F>
  options(F f, int indel)
      : match_score(f)
      , indel_cost(indel)
  {
  }

  std::function<int(T const&, T const&)> match_score;
  int indel_cost;
};

template <typename T>
struct alignment {
  template <typename Iterator>
  alignment(
      options<T> opts, Iterator a_begin, Iterator a_end, Iterator b_begin,
      Iterator b_end);

private:
  options<T> opts_;

  int rows_;
  int cols_;

  int& score_at(int row, int col);

  std::unique_ptr<int[]> data_;
};

template <typename T, typename Iterator>
alignment(options<T>, Iterator, Iterator, Iterator, Iterator) -> alignment<T>;

template <typename T>
template <typename Iterator>
alignment<T>::alignment(
    options<T> opts, Iterator a_begin, Iterator a_end, Iterator b_begin,
    Iterator b_end)
    : opts_(opts)
    , rows_(std::distance(a_begin, a_end) + 1)
    , cols_(std::distance(b_begin, b_end) + 1)
    , data_(std::make_unique<int[]>(rows_ * cols_))
{
  for (auto i = 0; i < cols_; ++i) {
    score_at(0, i) = i * opts_.indel_cost;
  }

  for (auto j = 0; j < rows_; ++j) {
    score_at(j, 0) = j * opts_.indel_cost;
  }

  auto a_it = a_begin;
  for (auto row = 1; row < rows_; ++row, ++a_it) {
    auto b_it = b_begin;
    for (auto col = 1; col < cols_; ++col, ++b_it) {
      auto match = opts_.match_score(*a_it, *b_it);
      auto a_gap = score_at(row, col - 1) + opts_.indel_score;
      auto b_gap = score_at(row - 1, col) + opts_.indel_score;

      score_at(row, col) = std::max({match, a_gap, b_gap});
    }
  }
}

template <typename T>
int& alignment<T>::score_at(int row, int col)
{
  return data_[row * cols_ + col];
}

} // namespace align
