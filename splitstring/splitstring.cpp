#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> split(const string &s, const string &sep) {
  auto found = s.find_first_of(sep);
  auto last(0);
  vector<string> ans;

  while (found != string::npos) {
    string sub = s.substr(last, found - last);
    if (!sub.empty()) {
      ans.push_back(sub);
    }

    last = found + 1;
    found = s.find_first_of(sep, last);
  }

  string sub = s.substr(last);
  if (!sub.empty()) {
    ans.push_back(sub);
  }
  return ans;
}

int main() {
  string s = "skj/sds/sfdf/sdee////efgege/";
  auto ans = split(s, "/");
  for (size_t i = 0; i < ans.size(); i++) {
    /* code */
    cout << ans[i] << endl;
  }

  return 0;
}