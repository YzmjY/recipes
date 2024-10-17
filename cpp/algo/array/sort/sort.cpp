#include <assert.h>
#include <iostream>
#include <vector>
using namespace std;

vector<int> bubble_sort(vector<int> arr) {
	bool unsorted = true;
	int size = arr.size();
	while (unsorted) {
		unsorted = false;
		for (int i = 1; i < size; ++i) {
			if (arr[i] < arr[i - 1]) {
				swap(arr[i], arr[i - 1]);
				unsorted = true;
			}
		}
	}
	return arr;
}

vector<int> select_sort(vector<int> arr) {
	int size = arr.size();

	for (int i = 0; i < size; ++i) {
		int cur = arr[i];
		int min_idx = i;
		for (int j = i + 1; j < size; ++j) {
			if (cur > arr[j]) {
				cur = arr[j];
				min_idx = j;
			}
		}

		swap(arr[i], arr[min_idx]);
	}
	return arr;
}

vector<int> insert_sort(vector<int> arr) {
	int size = arr.size();

	for (int i = 1; i < size; ++i) {
		int cur = arr[i];
        // [0,i-1]已经是有序数组
		for (int j = i - 1; j >= 0; j--) {
			if (cur > arr[j]) {
				arr[j + 1] = cur;
				break;
			} else {
				arr[j + 1] = arr[j];
			}
		}
	}
	return arr;
}

struct test_case {
	vector<int> test;
	vector<int> want;
};

void Log(const vector<int>& arr) {
	int size = arr.size();
	cout << "{";
	for (int i = 0; i < size; i++) {
		if (i != arr.size() - 1)
			cout << arr[i] << ",";
		else
			cout << arr[i];
	}
	cout << "}" << endl;
}

int main() {
	vector<test_case> cases = {
	    {
	        .test = {1, 2, 3, 4, 5, 6, 7},
	        .want = {1, 2, 3, 4, 5, 6, 7},

	    },
	    {
	        .test = {1, 3, 4, 2, 444, 22, 44, 55},
	        .want = {1, 2, 3, 4, 22, 44, 55, 444},
	    },
	    {
	        .test = {1, 3, 4, 2, 444, 22, 44, 3, 55},
	        .want = {1, 2, 3, 3, 4, 22, 44, 55, 444},
	    },
	};

	printf("test bubble sort\n");
	for (int i = 0; i < cases.size(); ++i) {
		auto ans = bubble_sort(cases[i].test);
		Log(ans);
		assert(ans == cases[i].want);
	}

	printf("test select sort\n");
	for (int i = 0; i < cases.size(); ++i) {
		auto ans = select_sort(cases[i].test);
		Log(ans);
		assert(ans == cases[i].want);
	}
	printf("test insert sort\n");
	for (int i = 0; i < cases.size(); ++i) {
		auto ans = insert_sort(cases[i].test);
		Log(ans);
		assert(ans == cases[i].want);
	}
}