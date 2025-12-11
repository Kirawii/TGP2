import time
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import random
import platform
import numpy as np
from rsa_core import RSACore

def naive_mod_pow(base, exponent, modulus):
    """
    朴素模幂算法: O(n)
    """
    result = 1
    for _ in range(exponent):
        result = (result * base) % modulus
    return result

def fast_mod_pow(base, exponent, modulus):
    """
    快速模幂算法: O(log n)
    """
    return pow(base, exponent, modulus)


def set_style():
    sns.set_theme(style="whitegrid")
    system_name = platform.system()
    if system_name == 'Darwin':
        plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'PingFang HK']
    else:
        plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei']
    plt.rcParams['axes.unicode_minus'] = False
    plt.config_inline = {'figure.format': 'retina'}

def experiment_miller_rabin():
    print(">>> 正在进行 Miller-Rabin 灵敏度测试...")
    rsa = RSACore()
    test_num = random.getrandbits(1024) | 1
    
    rounds_list = range(1, 60, 2) # 测试轮数 1, 3, 5 ... 59
    timing_data = []

    for k in rounds_list:
        start_time = time.time()
        for _ in range(50):
            rsa._is_prime_miller_rabin(test_num, k=k)
        avg_time = (time.time() - start_time) / 50
        
        error_prob = 1.0 / (4**k)
        
        timing_data.append({
            "Rounds (k)": k,
            "Time (ms)": avg_time * 1000,
            "Error Probability": error_prob
        })
    
    return pd.DataFrame(timing_data)

def experiment_mod_pow_complexity():
    print(">>> 正在进行 模幂算法复杂度 对比测试...")
    
    base = 123456789
    modulus = 1000000007
    exponents = list(range(1000, 50000, 2000)) 
    
    complexity_data = []

    for exp in exponents:
        start_naive = time.time()
        naive_mod_pow(base, exp, modulus)
        time_naive = time.time() - start_naive
        start_fast = time.time()
        for _ in range(1000):
            fast_mod_pow(base, exp, modulus)
        time_fast = (time.time() - start_fast) / 1000

        complexity_data.append({"Exponent Size": exp, "Method": "Naive O(n)", "Time(s)": time_naive})
        complexity_data.append({"Exponent Size": exp, "Method": "Fast O(log n)", "Time(s)": time_fast})

    return pd.DataFrame(complexity_data)

def run_and_plot():
    set_style()
    df_mr = experiment_miller_rabin()
    df_mp = experiment_mod_pow_complexity()

    fig = plt.figure(figsize=(16, 8))
    gs = fig.add_gridspec(2, 2)

    ax1 = fig.add_subplot(gs[0, 0])
    sns.lineplot(data=df_mr, x="Rounds (k)", y="Time (ms)", ax=ax1, marker='o', color='#e74c3c')
    ax1.set_title("Miller-Rabin: 迭代次数与耗时关系", fontsize=14, fontweight='bold')
    ax1.set_ylabel("耗时 (ms)")
    
    ax2 = fig.add_subplot(gs[1, 0])
    sns.lineplot(data=df_mr, x="Rounds (k)", y="Error Probability", ax=ax2, color='#8e44ad', linewidth=2)
    ax2.set_yscale("log") 
    ax2.set_title("Miller-Rabin: 理论错误率 (1/4^k)", fontsize=14, fontweight='bold')
    ax2.set_ylabel("错误概率 (Log Scale)")
    ax2.set_xlabel("迭代次数 (Rounds)")
    ax2.axhline(y=1e-10, color='gray', linestyle='--', alpha=0.5)
    ax2.text(30, 1e-8, '在 k=20 时\n错误率已忽略不计', fontsize=10, color='#2c3e50')

    ax3 = fig.add_subplot(gs[:, 1]) 

    sns.lineplot(data=df_mp, x="Exponent Size", y="Time(s)", hue="Method", style="Method", markers=True, ax=ax3, palette=["#e67e22", "#2980b9"])
    
    ax3.set_title("算法复杂度对比: 朴素乘法 vs 快速模幂", fontsize=14, fontweight='bold')
    ax3.set_ylabel("单次运算耗时 (秒)")
    ax3.set_xlabel("指数大小 (Exponent Value)")

    last_naive = df_mp[df_mp["Method"]=="Naive O(n)"].iloc[-1]
    ax3.annotate(f'O(n) 线性增长\n(实际上不可用)', 
                 xy=(last_naive["Exponent Size"], last_naive["Time(s)"]), 
                 xytext=(last_naive["Exponent Size"]-20000, last_naive["Time(s)"]),
                 arrowprops=dict(facecolor='black', shrink=0.05))

    last_fast = df_mp[df_mp["Method"]=="Fast O(log n)"].iloc[-1]
    ax3.annotate(f'O(log n) 极其平缓\n(RSA的基础)', 
                 xy=(last_fast["Exponent Size"], last_fast["Time(s)"]), 
                 xytext=(last_fast["Exponent Size"]-25000, last_fast["Time(s)"]+0.0005),
                 arrowprops=dict(facecolor='black', shrink=0.05))

    plt.tight_layout()
    plt.savefig("rsa_scientific_analysis.png", dpi=300)
    print("图表已生成: rsa_scientific_analysis.png")
    plt.show()

if __name__ == "__main__":
    run_and_plot()