import hashlib
import os
import math
from rsa_core import RSACore

class RSAPadding:
    def __init__(self):
        self.core = RSACore()
        self.pub, self.priv, _ = self.core.generate_keypair(1024)
        self.k = 1024 // 8 

    def _mgf1(self, seed: bytes, length: int, hash_func=hashlib.sha1) -> bytes:
        """
        Mask Generation Function (MGF1) based on PKCS#1
        """
        h_len = hash_func().digest_size
        mask = b''
        counter = 0
        while len(mask) < length:
            # C = seed || counter (4 bytes)
            c = counter.to_bytes(4, byteorder='big')
            mask += hash_func(seed + c).digest()
            counter += 1
        return mask[:length]

    def _xor_bytes(self, b1: bytes, b2: bytes) -> bytes:
        """两个字节串异或"""
        return bytes(x ^ y for x, y in zip(b1, b2))

    def encrypt_textbook(self, message: bytes) -> bytes:
        """
        直接 m^e % n，不做任何填充。
        """
        m_int = int.from_bytes(message, byteorder='big')
        c_int = self.core.encrypt(m_int, self.pub)
        return c_int.to_bytes(self.k, byteorder='big')

    def encrypt_oaep(self, message: bytes, label=b'') -> bytes:
        """
        手写 OAEP 流程:
        """
        hash_func = hashlib.sha1
        h_len = hash_func().digest_size
        
        max_msg_len = self.k - 2 * h_len - 2
        if len(message) > max_msg_len:
            raise ValueError("Message too long for OAEP")

        l_hash = hash_func(label).digest()
        ps = b'\x00' * (self.k - len(message) - 2 * h_len - 2) # Padding String
        db = l_hash + ps + b'\x01' + message

        seed = os.urandom(h_len)
        
        db_mask = self._mgf1(seed, self.k - h_len - 1, hash_func)
        masked_db = self._xor_bytes(db, db_mask)
        
        seed_mask = self._mgf1(masked_db, h_len, hash_func)
        masked_seed = self._xor_bytes(seed, seed_mask)
        
        em = b'\x00' + masked_seed + masked_db
        

        m_int = int.from_bytes(em, byteorder='big')
        c_int = self.core.encrypt(m_int, self.pub)
        
        return c_int.to_bytes(self.k, byteorder='big')