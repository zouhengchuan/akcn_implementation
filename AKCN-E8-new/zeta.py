# def br(j, k):
#     i = 0
#     while k != 0:
#         i = i * 2 + j % 2
#         j = j // 2
#         k = k - 1
#     return i

# a = []
# b = []
# c = []
# for k in range(1,7):
#     for j in range(2**(k-1)):
#         a.append((192+1152*br(j, k - 1)) // 2**k)
#         b.append((960+1152*br(j, k - 1)) // 2**k)

# print(a)
# print(b)
# for (i,j) in zip(a,b):
#     print(j - i,end = " ")

for k in range(2**8,2**16):
    if k * 3457 % 2**16 == 1:
        print(k)