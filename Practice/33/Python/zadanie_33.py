def create(arr_len, arr_1st_el=0, arr_step=0):
    return [ i for i in range(arr_1st_el, arr_1st_el + arr_len*arr_step, arr_step) ] \
        if arr_step else [arr_1st_el]*arr_len

    #if not arifm_step: return [arr_first_elem]*arr_len
    #return [ i for i in range(arr_first_elem, arr_first_elem + arr_len*arifm_step, arifm_step) ]

    #return [arr_first_elem + i*arifm_step for i in range(arr_len)]

def sort(arr): # намного быстрее просто использовать встроенную функцию сортировки: return sorted(arr)
    for i in range(len(arr)):
        for j in range(i, 0,-1):
            if arr[j - 1] <= arr[j]: break
            arr[j - 1], arr[j]=arr[j], arr[j - 1]
    return arr

_prn=print
def print(arr):
    _prn(arr)
    return arr

print(sort(create(arr_len=int(input()), arr_1st_el=int(input()), arr_step=int(input()))))

#for i in range(1000000):
#    create(8,4,-1)
