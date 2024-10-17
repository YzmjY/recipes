#include <spdlog/spdlog.h>

#include <iostream>
#include <string>
#include <vector>
void hanoi(int n, std::string from, std::string to, std::string other) {
  if (n == 1) {
    SPDLOG_INFO("move {} to {}", from, to);
    return;
  }
  hanoi(n - 1, from, other, to);
  SPDLOG_INFO("move {} to {}", from, to);
  hanoi(n - 1, other, to, from);
}
int main() {
  int tower_num = 0;
  std::cout << "请输入塔的层数：";
  std::cin >> tower_num;
  hanoi(tower_num, "A", "C", "B");
  return 0;
}
