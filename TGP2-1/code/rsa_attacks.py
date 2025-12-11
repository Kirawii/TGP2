import random
from rsa_core import RSACore

class RSAAttacks:
    def __init__(self):
        self.core = RSACore()

    def _extended_gcd(self, a, b):
        """
        扩展欧几里得算法 (迭代版)
        """
        # 初始化
        old_s, s = 1, 0
        old_t, t = 0, 1
        old_r, r = a, b

        # 循环直到余数为 0
        while r != 0:
            quotient = old_r // r
            old_r, r = r, old_r - quotient * r
            old_s, s = s, old_s - quotient * s
            old_t, t = t, old_t - quotient * t

        return old_r, old_s, old_t

    def _modinv(self, a, m):
        """求模逆元 (迭代版)"""
        g, x, y = self._extended_gcd(a, m)
        if g != 1:
            raise Exception('Modular inverse does not exist')
        return x % m

    def _cube_root(self, n):
        """计算整数立方根 (二分法)"""
        low = 0
        high = n
        while low < high:
            mid = (low + high) // 2
            if mid**3 < n:
                low = mid + 1
            else:
                high = mid
        return low

    def broadcast_attack(self, c1, n1, c2, n2, c3, n3):
        """
        利用 CRT 求解 M^3 = C (mod N1*N2*N3)
        """
        N = n1 * n2 * n3
        N1, N2, N3 = N // n1, N // n2, N // n3

        u1 = (c1 * N1 * self._modinv(N1, n1))
        u2 = (c2 * N2 * self._modinv(N2, n2))
        u3 = (c3 * N3 * self._modinv(N3, n3))
        
        C_combined = (u1 + u2 + u3) % N
        
        m = self._cube_root(C_combined)
        return m

    def _continued_fractions_expansion(self, numerator, denominator):
        e = []
        while denominator != 0:
            e.append(numerator // denominator)
            numerator, denominator = denominator, numerator % denominator
        return e

    def _convergents(self, expansion):
        n = [] 
        d = [] 

        for i in range(len(expansion)):
            if i == 0:
                ni = expansion[i]
                di = 1
            elif i == 1:
                ni = expansion[i] * expansion[i-1] + 1
                di = expansion[i]
            else:
                ni = expansion[i] * n[i-1] + n[i-2]
                di = expansion[i] * d[i-1] + d[i-2]
            
            n.append(ni)
            d.append(di)
            yield (ni, di)

    def wieners_attack(self, e, n):
        expansion = self._continued_fractions_expansion(e, n)
        steps = 0
        
        for k, d_guess in self._convergents(expansion):
            steps += 1
            if k == 0: continue
            if d_guess % 2 == 0: continue 
            
            test_m = 2
            if pow(pow(test_m, e, n), d_guess, n) == test_m:
                return d_guess, steps
                
        return None, steps

    def generate_weak_key(self, bits):
        while True:
            p = self.core.generate_large_prime(bits // 2)
            q = self.core.generate_large_prime(bits // 2)
            if p * q < (2**(bits-1)): continue 
            
            n = p * q
            phi = (p - 1) * (q - 1)
            d = random.getrandbits(30) | 1 
            if self._extended_gcd(d, phi)[0] != 1:
                continue
                
            e = self._modinv(d, phi)
            return (e, n), (d, n)