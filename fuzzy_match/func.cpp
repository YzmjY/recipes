#include <iostream>
#include <string>

using namespace std;

bool isMatch(std::string s, std::string p) {
  int i = 0, j = 0, star = -1, match = -1;
  int s_size = s.size(), p_size = p.size();
  while (i < s_size) {
    if (j < p_size && s[i] == p[j] ||
        p[j] == '?') { // 该位置上两者相同，共同进步
      i++;
      j++;
    } else if (j < p_size && p[j] == '*') { // parttern 串中有匹配任意的字符
      star = j;
      match = i;
      j++;
    } else if (star !=
               -1) { // 回溯，前面有一个*，回退到上一个尝试过的s索引的下一个
      j = star + 1;
      match++;
      i = match;
    } else {
      return false;
    }
  }

  for (; j < p_size; j++) {
    if (p[j] != '*') {
      return false;
    }
  }
  return true;
}

int main() {
  string s = "xzmjx";
  string p = "x*x";
  cout << (isMatch(s, p) ? "true" : "fasle") << endl;

  s = "xzmjx";
  p = "x*y";
  cout << (isMatch(s, p) ? "true" : "fasle") << endl;

  return 0;
}