import random

def is_prime(n, k=5):  # 使用Miller-Rabin素性测试
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0:
        return False

    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2
    
    for _ in range(k):
        a = random.randrange(2, n - 1)
        x = pow(a, d, n)
        if x == 1 or x == n - 1:
            continue
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True

def find_primitive_root(p):  # 寻找原根
    if p == 2:
        return 1
    phi = p - 1
    factors = []
    temp = phi
    factor = 2
    while factor * factor <= temp:
        if temp % factor == 0:
            factors.append(factor)
            while temp % factor == 0:
                temp //= factor
        factor += 1
    if temp > 1:
        factors.append(temp)

    for g in range(2, p):
        for f in factors:
            if pow(g, phi // f, p) == 1:
                break
        else:
            return g
    return None

def find_1152nd_root_of_unity(p, root):  # 计算1152次单位根
    if root is None:
        raise ValueError("No primitive root found.")
    order = (p - 1) // 1152
    return pow(root, order, p)

if __name__ == "__main__":
    if not is_prime(3457):
        print("3457 is not a prime number.")
    else:
        primitive_root = find_primitive_root(3457)
        if primitive_root is None:
            print("Failed to find a primitive root.")
        else:
            unit_root = find_1152nd_root_of_unity(3457, primitive_root)
            print(f"The 1152nd root of unity modulo 3457 is {unit_root}.")