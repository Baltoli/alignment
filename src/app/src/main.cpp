#include <align/align.h>

#include <string>

using namespace std::string_literals;

int char_match(char a, char b) { return a == b ? 1 : -1; }

int main()
{
  auto opts = align::options<char>(char_match, -1);
  auto str_a = "GATTACA"s;
  auto str_b = "GATTTTCTAGA"s;

  auto align = align::alignment(
      opts, str_a.begin(), str_a.end(), str_b.begin(), str_b.end());

  align.dump_scores();

  auto [a_seq, b_seq] = align.trace_back();

  for (auto c : a_seq) {
    if (std::holds_alternative<char>(c)) {
      std::cout << std::get<char>(c);
    } else {
      std::cout << '-';
    }
  }
  std::cout << '\n';

  for (auto c : b_seq) {
    if (std::holds_alternative<char>(c)) {
      std::cout << std::get<char>(c);
    } else {
      std::cout << '-';
    }
  }
  std::cout << '\n';
}
