#include <iostream>
#include <vector>
using namespace std;
void sort(vector<int>& arr)
{
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

int main()
{
    vector<int> arr = {5, 2, 4, 6, 1, 3};
    sort(arr);
    for (int i = 0; i < arr.size(); ++i)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}