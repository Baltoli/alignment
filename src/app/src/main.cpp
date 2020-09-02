#include <align/align.h>

int main()
{
  auto opts = align::options<char>(
      [](auto a, auto b) { return a == b ? 1 : -1; }, -1);
}
