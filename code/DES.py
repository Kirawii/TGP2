
import time, os, psutil, platform
try:
    import resource  # Unix ru_maxrss
except ImportError:
    resource = None

from Crypto.Cipher import DES

def des_encrypt(key, plaintext: bytes) -> bytes:
    if isinstance(key, str): key = key.encode('utf-8')
    if len(key) != 8: raise ValueError("DES 密钥必须是 8 字节长！")
    if isinstance(plaintext, str): plaintext = plaintext.encode('utf-8')
    pad = 8 - (len(plaintext) % 8)
    if pad == 0: pad = 8
    plaintext_padded = plaintext + bytes([pad]) * pad
    cipher = DES.new(key, DES.MODE_ECB)
    return cipher.encrypt(plaintext_padded)

def des_decrypt(key, ciphertext: bytes) -> bytes:
    if isinstance(key, str): key = key.encode('utf-8')
    if len(key) != 8: raise ValueError("DES 密钥必须是 8 字节长！")
    cipher = DES.new(key, DES.MODE_ECB)
    plain_padded = cipher.decrypt(ciphertext)
    pad = plain_padded[-1]
    return plain_padded[:-pad]

def _peak_rss_kb() -> float:
    if resource is not None:
        try:
            v = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
            return v/1024.0 if platform.system() == "Darwin" else float(v)
        except Exception:
            pass
    return psutil.Process(os.getpid()).memory_info().rss / 1024.0

def _cpu_percent_between(proc: psutil.Process, wall_start: float, wall_end: float, cpu_start: float, cpu_end: float) -> float:
    wall = max(1e-9, wall_end - wall_start)
    ncpu = psutil.cpu_count(logical=True) or 1
    return max(0.0, min(100.0, (cpu_end - cpu_start) / wall * 100.0 / ncpu))

if __name__ == "__main__":
    key = "jielycat"
    plaintext = "Hello DES! Hello IDEA!"
    with open('./context/The_Story_of_the_Stone.txt', 'r', encoding='utf-8') as f:
        plaintext = f.read()
    proc = psutil.Process(os.getpid())
    rss_begin_kb = proc.memory_info().rss / 1024.0
    wall_start = time.perf_counter()
    cpu_times = proc.cpu_times()
    cpu_start = (cpu_times.user + cpu_times.system)

    t1 = time.perf_counter()
    ciphertext = des_encrypt(key, plaintext)
    t2 = time.perf_counter()

    t3 = time.perf_counter()
    plain2 = des_decrypt(key, ciphertext)
    t4 = time.perf_counter()

    wall_end = time.perf_counter()
    cpu_times_end = proc.cpu_times()
    cpu_end = (cpu_times_end.user + cpu_times_end.system)
    rss_end_kb = proc.memory_info().rss / 1024.0
    peak_kb = _peak_rss_kb()

    # print("密文字节序列：", ' '.join(str(int(x)) for x in ciphertext))
    # print("解密结果：", plain2.decode('utf-8'))

    enc_ms = (t2 - t1) * 1000.0
    dec_ms = (t4 - t3) * 1000.0
    all_ms = (wall_end - wall_start) * 1000.0
    blocks = len(ciphertext) // 8
    cpu_percent = _cpu_percent_between(proc, wall_start, wall_end, cpu_start, cpu_end)

    print("\n【DES 性能指标 Python（统一口径）】")
    print(f" 输入字节数   ：{len(plaintext.encode())} 字节")
    print(f" 输出字节数   ：{len(ciphertext)} 字节")
    print(f" 数据块数量   ：{blocks}")
    print(f" 加密耗时    ：{enc_ms:.3f} 毫秒")
    print(f" 解密耗时    ：{dec_ms:.3f} 毫秒")
    print(f" 总耗时     ：{all_ms:.3f} 毫秒")
    print(f" 起始内存占用 ：{rss_begin_kb:.0f} KB")
    print(f" 结束内存占用 ：{rss_end_kb:.0f} KB")
    print(f" 峰值内存占用 ：{peak_kb:.0f} KB")
    print(f" CPU 使用率  ：{cpu_percent:.1f} %")
