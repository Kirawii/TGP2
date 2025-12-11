import random

class RSACore:
    def __init__(self):
        pass

    def _is_prime_miller_rabin(self, n, k=40):
        if n == 2 or n == 3: return True
        if n % 2 == 0 or n < 2: return False

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

    def generate_large_prime(self, bits):
        while True:
            n = random.getrandbits(bits)
            if n % 2 == 0: 
                n += 1
            if self._is_prime_miller_rabin(n):
                return n

    # --- 2. 扩展欧几里得 (迭代版 - 修复 RecursionError) ---
    def _egcd(self, a, b):
        old_s, s = 1, 0
        old_t, t = 0, 1
        old_r, r = a, b

        while r != 0:
            quotient = old_r // r
            old_r, r = r, old_r - quotient * r
            old_s, s = s, old_s - quotient * s
            old_t, t = t, old_t - quotient * t

        return (old_r, old_s, old_t)

    def _modinv(self, a, m):
        g, x, y = self._egcd(a, m)
        if g != 1:
            raise Exception('Modular inverse does not exist')
        return x % m

    def generate_keypair(self, bits):
        p = self.generate_large_prime(bits // 2)
        q = self.generate_large_prime(bits // 2)
        while p == q:
            q = self.generate_large_prime(bits // 2)

        n = p * q
        phi = (p - 1) * (q - 1)

        e = 65537
        if self._egcd(e, phi)[0] != 1:
            e = 3
            while self._egcd(e, phi)[0] != 1:
                e += 2

        d = self._modinv(e, phi)

        dP = d % (p - 1)
        dQ = d % (q - 1)
        qInv = self._modinv(q, p)

        return ((e, n), (d, n), (p, q, dP, dQ, qInv))


    def encrypt(self, message_int, public_key):
        e, n = public_key
        return pow(message_int, e, n)

    def decrypt_standard(self, ciphertext_int, private_key):
        d, n = private_key
        return pow(ciphertext_int, d, n)

    def decrypt_crt(self, ciphertext_int, crt_params):
        p, q, dP, dQ, qInv = crt_params
        m1 = pow(ciphertext_int, dP, p)
        m2 = pow(ciphertext_int, dQ, q)
        h = (qInv * (m1 - m2)) % p
        m = m2 + h * q
        return m