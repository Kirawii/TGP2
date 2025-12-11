import time
import pandas as pd
import matplotlib.pyplot as plt
import platform
from rsa_core import RSACore
from rsa_attacks import RSAAttacks

# --- 学术级黑白打印风格设置 ---
def set_academic_bw_style():
    plt.style.use('default') 
    
    # 字体配置
    system_name = platform.system()
    if system_name == 'Darwin':
        plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'Times New Roman']
    else:
        plt.rcParams['font.sans-serif'] = ['SimSun', 'Times New Roman']
    
    plt.rcParams['axes.unicode_minus'] = False
    
    # 轴和网格
    plt.rcParams['axes.grid'] = True
    plt.rcParams['grid.alpha'] = 0.5
    plt.rcParams['grid.linestyle'] = '--'
    plt.rcParams['grid.color'] = '#b0b0b0' 
    
    plt.rcParams['lines.linewidth'] = 2.0
    plt.config_inline = {'figure.format': 'retina'}

def run_broadcast_experiment():
    print(">>> 正在进行 广播攻击 性能测试...")
    attacker = RSAAttacks()
    rsa = RSACore()
    
    results = []
    # 测试不同长度的 N
    bit_sizes = [256, 512, 1024, 2048]
    
    for bits in bit_sizes:
        print(f"Testing {bits} bits...")
        
        # 1. 模拟生成3个用户 (e=3)
        n_list = []
        for _ in range(3):
            while True:
                p = rsa.generate_large_prime(bits // 2)
                q = rsa.generate_large_prime(bits // 2)
                n = p * q
                phi = (p-1)*(q-1)
                if rsa._egcd(3, phi)[0] == 1:
                    n_list.append(n)
                    break
        
        msg = 123456789
        c1 = pow(msg, 3, n_list[0])
        c2 = pow(msg, 3, n_list[1])
        c3 = pow(msg, 3, n_list[2])
        
        # 2. 测量攻击时间
        start = time.time()
        cracked_m = attacker.broadcast_attack(c1, n_list[0], c2, n_list[1], c3, n_list[2])
        attack_time = (time.time() - start) * 1000 # ms
        
        assert cracked_m == msg # 确保攻击成功
        
        # 3. 对比：合法用户的解密时间
        pub, priv, _ = rsa.generate_keypair(bits)
        c_std = rsa.encrypt(msg, pub)
        
        start_dec = time.time()
        rsa.decrypt_standard(c_std, priv)
        dec_time = (time.time() - start_dec) * 1000
        
        results.append({"Bits": bits, "Type": "Attacker (Broadcast)", "Time": attack_time})
        results.append({"Bits": bits, "Type": "Legitimate User (Standard)", "Time": dec_time})
        
    return pd.DataFrame(results)

def run_wiener_experiment():
    print(">>> 正在进行 维纳攻击 复杂度测试...")
    attacker = RSAAttacks()
    
    results = []
    bit_sizes = [128, 256, 512, 1024]
    
    for bits in bit_sizes:
        print(f"Testing Weak Key {bits} bits...")
        
        pub, priv = attacker.generate_weak_key(bits)
        e, n = pub
        real_d = priv[0]
        
        start = time.time()
        cracked_d, steps = attacker.wieners_attack(e, n)
        duration = (time.time() - start) * 1000
        
        if cracked_d == real_d:
            results.append({"Bits": bits, "Steps": steps, "Time": duration})
            
    return pd.DataFrame(results)

def plot_attacks(df_broad, df_wiener):
    set_academic_bw_style()
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    # === 图1: 广播攻击耗时对比 ===
    df_att = df_broad[df_broad['Type'] == "Attacker (Broadcast)"]
    df_leg = df_broad[df_broad['Type'] == "Legitimate User (Standard)"]
    
    axes[0].plot(df_att['Bits'], df_att['Time'], 
                 label='Attacker Time (CRT)', color='black', 
                 linestyle='-', marker='o', markersize=8)
    
    axes[0].plot(df_leg['Bits'], df_leg['Time'], 
                 label='Legitimate User Time (Modular Exp)', color='#555555', 
                 linestyle='--', marker='s', markersize=8)
    
    axes[0].set_title("广播攻击效率 vs 标准解密", fontsize=14, fontweight='bold')
    axes[0].set_xlabel("密钥长度 (Bits)")
    axes[0].set_ylabel("耗时 (ms)")
    axes[0].legend()
    
    # === 图2: 维纳攻击的步数 ===
    bars = axes[1].bar(df_wiener['Bits'].astype(str), df_wiener['Steps'], 
                color='white', edgecolor='black', width=0.5, hatch='///')
    
    axes[1].bar_label(bars, padding=3)
    axes[1].plot(df_wiener['Bits'].astype(str), df_wiener['Steps'], 
                 color='black', marker='D', linestyle='-', linewidth=1, label='Convergence Steps')
    
    axes[1].set_title("维纳攻击：连分数逼近步数", fontsize=14, fontweight='bold')
    axes[1].set_xlabel("密钥长度 (Bits)")
    axes[1].set_ylabel("逼近所需的步数 (Steps)")
    
    # 修复了这里的括号语法错误
    axes[1].text(0.05, 0.85, 
                 "破解仅需极少步数\n与密钥长度呈对数关系", 
                 transform=axes[1].transAxes,
                 bbox=dict(facecolor='white', edgecolor='black', boxstyle='round'))

    plt.tight_layout()
    plt.savefig("rsa_attacks_bw.png", dpi=300)
    print("图表已生成: rsa_attacks_bw.png")
    plt.show()

if __name__ == "__main__":
    df_b = run_broadcast_experiment()
    df_w = run_wiener_experiment()
    plot_attacks(df_b, df_w)