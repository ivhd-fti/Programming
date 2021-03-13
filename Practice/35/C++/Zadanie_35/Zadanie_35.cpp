
#include <iostream>
using namespace std;
struct IntArray {
    int* data;
    int size;
} arr;

void create(IntArray* arr, int size) {
    // тут должна была бы быть проверка на то, что массив уже создан. Т.е. создавать новый 
    // массив можно только, если (*arr).data равен nullptr. Но в задании это не требуется.
    int* new_array = new int[size];
    (*arr).data = new_array;
    (*arr).size = size;
}

// Как Вы и говорили, работа по создания функции по шаблону void create(IntArray*, int)
// уже выполнена. Приводим аргументы к соответствию и повторно используем уже написанный код.
void create(IntArray& arr, int size) {
    create(&arr, size);  // приводим аргументы к стандарту функции create(IntArray*, int)
}

void index_check(IntArray* arr, int index, int error_code) {
    if ((index < 0) || (index >= (*arr).size)) { // проверка вылета индекса за пределы массива
        string err = ((error_code == 2) ? "get()" : "set()"); // текст ошибки: "set()" или "get()"
        cout << err << " index error. Program terminated.\n"; // из задания: "вывести сообщение
        exit(error_code);
    }                           // об ошибке и прервать работу программы."
}

int get(IntArray* arr, int index) {
    index_check(arr, index, 2); // проверка, находится ли индекс в пределах массива. Если нет,
    return (*arr).data[index];  //  то будет глобальный exit(2) из программы.
}

int get(IntArray& arr, int index) {
    return get(&arr, index);      // приводим аргумент к соответствию и используем повторно уже
}                                 // написанную по шаблону int get(IntArray*, int) функцию

void set(IntArray* arr, int index, int value) {
    index_check(arr, index, 1);  // проверка, находится ли индекс в пределах массива. Если нет, то
    (*arr).data[index] = value;  // будет глобальный exit(1) из программы.
}

void set(IntArray& arr, int index, int value) {
    set(&arr, index, value);    // приводим аргумент к соответствию и используем повторно уже
}                               // написанную по шаблону int get(IntArray*, int, int) функцию

void print(IntArray* arr) {
    for (int i = 0; i < (*arr).size; i++) {
        cout << ((!i) ? "[" : "") << (*arr).data[i] << ((i < (*arr).size - 1) ? ", " : "]\n");
    }
}

void print(IntArray& arr) {
    print(&arr);            // приведим аргументы к требуемым в void print(IntArray* arr). 
}                           // Используем код повторно.

void resize(IntArray* arr, int newSize) {
    int* ald_arr = (*arr).data;         // запоминаем указатель на старый массив
    int  ald_size = (*arr).size;        // запоминаем старую длину массива
    create(arr, newSize);               // используем нашу же процедуру create

    // копируем минимальное перекрытие массивов между собой из старого в новый
    // созданный функцией create
    // тут может быть ситуация, когда старый массив удален, но тогда и длина у него будет равна
    // нулю и цикл for на копирование даже не запустится.
    for (int i = 0; i < ((newSize < ald_size) ? newSize : ald_size); i++) {
        set(arr, i, ald_arr[i]);
    }      // используем нашу же функцию set для копирования

// если длинна нового массива - больше, чем у старого (у старого может быть дина 0 - удален)
// заполняем хвост нового массива нулями
    if (newSize > ald_size) {
        for (int i = ald_size; i < newSize; i++) {
            set(arr, i, 0);
        } // используем нашу же функцию set
    }
    if (ald_arr != nullptr) {   // старый массив-то был удален. освобождать память не надо (но работает и без этой проверки)
        delete[]ald_arr;      // освобождаем память старого массива, если он был
    }
    else { cout << "Warning. Attempt to resize a nullptr array. A new array has been created instead.\n"; }
}

void resize(IntArray& arr, int newSize) {
    resize(&arr, newSize);  // приводим аргумент к соответствию и используем повторно уже
}                           // написанную по шаблону int resize(IntArray*, int) функцию

void destroy(IntArray* arr) {
    if ((*arr).data != nullptr) {
        delete[](*arr).data;
        (*arr).data = nullptr;
        (*arr).size = 0;
    }
    else { cout << "Warning. Attempt to destroy a nullptr array. Action skiped.\n"; }
}

void destroy(IntArray& arr) {
    destroy(&arr);
    // Можно было и так написать:
    //if (arr.data != nullptr) {
    //    delete[] arr.data;
    //    arr.data = nullptr;
    //    arr.size = 0; }
    // но использовать повторно уже написанную по шаблону void destroy(IntArray*)
    // мне кажется более правильно. Это касается и остальных функций, принимающих
}   // массив по ссылке

int main()
{
    cout << "Test of IntArray& functions\n";
    create(arr, 30);
    for (int i = 0; i < 30; i++) { set(arr, i, i + 1); }
    print(arr);
    resize(arr, 50);
    print(arr);
    resize(arr, 10);
    print(arr);
    destroy(arr);

    cout << "\n\nTest of IntArray* functions\n";
    IntArray* ptr = &arr;
    create(ptr, 30);
    for (int i = 0; i < 30; i++) { set(ptr, i, i + 1); }
    print(ptr);
    //set(ptr, 100, 0); // test set error -> err_msg -> exit(1)
    //get(ptr, 100);    // test get error -> err_msg -> exit(2)
    resize(ptr, 50);
    print(ptr);
    resize(ptr, 10);
    print(ptr);
    destroy(ptr);
    //destroy(ptr);     // test nullptr array destroy warning
    //resize(ptr, 10);  // test nullptr array resize warning
    return 0;
}
