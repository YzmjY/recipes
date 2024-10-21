#include <iostream>
#include <stack>
#include <vector>
using namespace std;

void print(const string& prefix, const vector<int>& arr) {
	cout << prefix << ": [";
	for (int i = 0; i < arr.size(); ++i) {
		cout << arr[i];
		if (i != arr.size() - 1) {
			cout << ", ";
		}
	}
	cout << "]";
	cout << endl;
}

void sort(vector<int>& arr) {
	int n = arr.size();
	// 写一个插入排序
	for (int i = 1; i < n; ++i) {
		int cur = arr[i];
		for (int j = i - 1; j >= 0; --j) {
			if (cur < arr[j]) {
				arr[j + 1] = arr[j];
			} else {
				arr[j + 1] = cur;
				break;
			}
		}
	}
}

vector<int> stackAlgo(const vector<int>& arr) {
	stack<int> stk;
	vector<int> res(arr.size(), -1);
	print("init", arr);

	for (int i = 0; i < arr.size(); ++i) {
		// 递增单调栈
		while (!stk.empty() && arr[stk.top()] > arr[i]) {
			int top = stk.top();
			stk.pop();
			// arr[i]即为arr[top]的下一个更小元素
			res[top] = arr[i]; // 记录结果
		}
		stk.push(i);
	}
	return res;
}

int main() {
	vector<int> arr = {5, 2, 4, 6, 1, 3};
	vector<int> res = stackAlgo(arr);
	print("res", res);
	return 0;
}