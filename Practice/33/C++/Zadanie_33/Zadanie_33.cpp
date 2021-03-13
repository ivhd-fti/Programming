#include <iostream>
using namespace std;

//длину массива, начальный член и шаг арифметической прогрессии
int* create(int arr_len, int arr_first_elem = 0, int arifm_step = 0) {

    int* new_array = new int[arr_len];

    // Инициализация массива
    for (int i = 0; i < arr_len; i++) {
        new_array[i] = arr_first_elem;
        arr_first_elem = arr_first_elem + arifm_step;
    }
    return new_array;
}

int* sort(int* arr, int arr_len) {
    for (int i = 1; i < arr_len; i++)
        for (int j = i; j > 0 && arr[j - 1] > arr[j]; j--) // пока j>0 и элемент j-1 > j, x-массив int
            swap(arr[j - 1], arr[j]);                      // меняем местами элементы j и j-1
    return arr;
}

int* print(int* arr, int arr_len) {
    cout << "[";
    for (int i = 0; i < arr_len; i++) {
        cout << arr[i] << ((i < arr_len - 1) ? ", " : "");
    }
    cout << "]";
    return arr;
}

int main()
{
    int arr_len, arr_first_elem, arifm_step;
    cin >> arr_len >> arr_first_elem >> arifm_step;
    int* arr = create(arr_len, arr_first_elem, arifm_step);
    sort(arr, arr_len);
    print(arr, arr_len);
    delete[] arr;
    return 0;
}
