w = 32
cnt = 0

while(cnt < 1000):
    A = ZZ.random_element(2 ^ w)
    B = ZZ.random_element(2 ^ w)
    # A=B=2^w-1

A1, A0 = A >> (w/2), A % 2 ^ (w/2)
B1, B0 = B >> (w/2), B % 2 ^ (w/2)
T1, T0 = A1*B0, A0*B1
T0 = (T1 + T0) % 2 ^ (w)
T1 = T0 < T1
C1, C0 = A1*B1, A0*B0
T = C0
C0 = (C0 + (T0 << (w/2))) % 2 ^ (w)
C1 = C1+(T1 << (w/2))+(T0 >> (w/2))+(C0 < T) + C0
C = (C1 << w)

print(A*B == C, A, B, C)
cnt = cnt+1
