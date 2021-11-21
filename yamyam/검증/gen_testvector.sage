testnum = 10000
lbound = 2** 750
ubound = 2** 751

cnt = 1000
file = open("/Users/jewon/Desktop/aap/TEST.txt", 'w')

A = B = C = [0] * cnt

file.write("char* str[{}] ={}".format(testnum, "{"))

for j in range(cnt):
    A[j] =  ZZ.random_element(lbound, ubound)
    B[j] =  ZZ.random_element(lbound, ubound)
    C[j] = A[j] - B[j]
    file.write("{} {} {} \n {}".format('"', hex(C[j]), '"', ','))
    #file.write('"' + hex(C[j]) + '"' + '\n' + ',')
    # print(hex(C[j]))

file.write("};")

file.close()