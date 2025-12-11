import time
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from rsa_core import RSACore
import platform

# --- Mac 字体与绘图风格设置 ---
def set_style():
    # 使用 Seaborn 的高级风格
    sns.set_theme(style="whitegrid")
    
    # 针对 MacOS 的字体设置，解决中文乱码
    system_name = platform.system()
    if system_name == 'Darwin':  # macOS
        plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'PingFang HK', 'Heiti TC']
    else:
        plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei'] # Windows fallback
        
    plt.rcParams['axes.unicode_minus'] = False # 解决负号显示问题
    
    # 设置图表清晰度 (Retina 屏优化)
    plt.config_inline = {'figure.format': 'retina'}


def run_benchmark():
    rsa = RSACore()
    bit_sizes = [512, 1024, 2048] 
    rounds = 10 

    results_keygen = []
    results_decrypt = []

    print(f"开始基准测试 (每组跑 {rounds} 轮)...")

    for bits in bit_sizes:
        print(f"正在测试 {bits} bits...")
        start_time = time.time()
        for _ in range(rounds):
            pub, priv, crt = rsa.generate_keypair(bits)
        avg_keygen = (time.time() - start_time) / rounds
        results_keygen.append({"Bits": str(bits), "Time(s)": avg_keygen})

        msg = 123456789
        cipher = rsa.encrypt(msg, pub)

        # 普通解密计时
        start_std = time.time()
        for _ in range(rounds * 5): 
            rsa.decrypt_standard(cipher, priv)
        avg_std = (time.time() - start_std) / (rounds * 5)

        # CRT 解密计时
        start_crt = time.time()
        for _ in range(rounds * 5):
            rsa.decrypt_crt(cipher, crt)
        avg_crt = (time.time() - start_crt) / (rounds * 5)

        results_decrypt.append({"Bits": str(bits), "Method": "Standard RSA", "Time(ms)": avg_std * 1000})
        results_decrypt.append({"Bits": str(bits), "Method": "CRT Accelerated", "Time(ms)": avg_crt * 1000})

    return pd.DataFrame(results_keygen), pd.DataFrame(results_decrypt)


def plot_results(df_keygen, df_decrypt):
    set_style()
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    sns.lineplot(data=df_keygen, x="Bits", y="Time(s)", marker='o', 
                 linewidth=2.5, color='#3498db', ax=axes[0])
    axes[0].set_title("密钥生成平均耗时 (Key Generation)", fontsize=14, fontweight='bold', pad=15)
    axes[0].set_xlabel("密钥长度 (Bits)", fontsize=12)
    axes[0].set_ylabel("耗时 (秒)", fontsize=12)
    for x, y in zip(df_keygen['Bits'], df_keygen['Time(s)']):
        axes[0].text(x, y, f'{y:.3f}s', ha='center', va='bottom', fontsize=10)


    palette = {"Standard RSA": "#e74c3c", "CRT Accelerated": "#2ecc71"}
    sns.barplot(data=df_decrypt, x="Bits", y="Time(ms)", hue="Method", 
                palette=palette, ax=axes[1])
    
    axes[1].set_title("解密性能对比: 普通模式 vs CRT加速", fontsize=14, fontweight='bold', pad=15)
    axes[1].set_xlabel("密钥长度 (Bits)", fontsize=12)
    axes[1].set_ylabel("耗时 (毫秒)", fontsize=12)
    axes[1].legend(title="Decryption Method")

    # 自动优化布局
    plt.tight_layout()
    
    # 保存图片，分辨率300dpi，适合论文打印
    plt.savefig("rsa_performance_analysis.png", dpi=300)
    print("图表已生成: rsa_performance_analysis.png")
    plt.show()

if __name__ == "__main__":
    df_k, df_d = run_benchmark()
    plot_results(df_k, df_d)