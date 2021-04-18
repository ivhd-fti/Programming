#include <iostream>
using namespace std;

void create(int** arr, int arr_len, int arr_first_elem = 0, int arifm_step = 0) {
    *arr = new int[arr_len];
    for (int i = 0; i < arr_len; i++) {
        (*arr)[i] = arr_first_elem;
        arr_first_elem = arr_first_elem + arifm_step;
    }
}

void sort(int* arr, int arr_len) {
    for (int i = 1; i < arr_len; i++)
        for (int j = i; j > 0 && arr[j - 1] > arr[j]; j--)
            swap(arr[j - 1], arr[j]);
}

void print(int* arr, int arr_len) {
    for (int i = 0; i < arr_len; i++) {
        cout << ((!i) ? "[" : "") << arr[i] << ((i < arr_len - 1) ? ", " : "]\n");
    }
}

void destroy(int* arr) {
    if (arr != nullptr) {
        delete[] arr;
        int* arr = nullptr;
    }
}

int main()
{
    int arr_len, arr_first_elem, arifm_step;
    int* arr;
    cin >> arr_len >> arr_first_elem >> arifm_step;
    create(&arr, arr_len, arr_first_elem, arifm_step);
    sort(arr, arr_len);
    print(arr, arr_len);
    destroy(arr);
    return 0;
}
