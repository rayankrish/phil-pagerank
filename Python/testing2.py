import wikipedia

array1 = wikipedia.page("List of philosophers (A-C)").links
array2 = wikipedia.page("List of philosophers (D-H)").links

print(array1)
print(array2)

array1 = array1+array2

print(array1)

