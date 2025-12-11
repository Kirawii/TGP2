import time
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import random
import platform
from rsa_core import RSACore

# --- 绘图风格设置 ---
def set_style():
    sns.set_theme(style="whitegrid")
    system_name = platform.system()
    if system_name == 'Darwin':
        plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'PingFang HK']
    else:
        plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei']
    plt.rcParams['axes.unicode_minus'] = False
    plt.config_inline = {'figure.format': 'retina'}

# --- 修正后的 Miller-Rabin 实验 ---
def experiment_miller_rabin_linear():
    print(">>> 正在准备 Miller-Rabin 线性测试...")
    rsa = RSACore()
    
    # 【关键修改】生成一个真正的素数！
    # 只有输入是素数，算法才会跑满所有 k 轮，才能看到线性增长。
    print("正在生成用于测试的 1024-bit 素数 (可能需要几秒钟)...")
    prime_num = rsa.generate_large_prime(1024)
    print("素数生成完毕，开始压力测试...")

    rounds_list = range(1, 101, 5) # 测试轮数 1, 6, 11 ... 101
    timing_data = []

    for k in rounds_list:
        # 增加重复次数以减少系统抖动噪音
        repeat_times = 20 
        start_time = time.time()
        for _ in range(repeat_times):
            # 对素数进行测试，它会跑完 k 轮并返回 True
            rsa._is_prime_miller_rabin(prime_num, k=k)
        
        avg_time = (time.time() - start_time) / repeat_times
        
        # 记录数据
        timing_data.append({
            "Rounds (k)": k,
            "Time (ms)": avg_time * 1000
        })
    
    return pd.DataFrame(timing_data)

# --- 绘图函数 ---
def run_and_plot_linear():
    set_style()
    df = experiment_miller_rabin_linear()

    plt.figure(figsize=(10, 6))
    
    # 画散点和连线
    sns.lineplot(data=df, x="Rounds (k)", y="Time (ms)", marker='o', 
                 linewidth=2.5, color='#e74c3c', label='Prime Number Input')
    
    # 添加趋势辅助线（拟合直线）以证明线性关系
    sns.regplot(data=df, x="Rounds (k)", y="Time (ms)", scatter=False, 
                color='gray', line_kws={'linestyle': '--', 'alpha': 0.5}, label='Linear Trend')

    plt.title("Miller-Rabin 最坏情况性能分析 (输入为素数)", fontsize=14, fontweight='bold')
    plt.xlabel("迭代次数 (Rounds k)", fontsize=12)
    plt.ylabel("平均耗时 (ms)", fontsize=12)
    plt.legend()
    
    # 添加文字说明
    plt.tight_layout()
    plt.savefig("miller_rabin_linear_proof.png", dpi=300)
    print("图表已生成: miller_rabin_linear_proof.png")
    plt.show()

if __name__ == "__main__":
    run_and_plot_linear()