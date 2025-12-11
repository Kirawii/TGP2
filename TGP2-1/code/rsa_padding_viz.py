import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import platform
from rsa_padding import RSAPadding

# --- 绘图风格 ---
def set_style():
    plt.style.use('default')
    system_name = platform.system()
    if system_name == 'Darwin':
        plt.rcParams['font.sans-serif'] = ['Arial Unicode MS', 'PingFang HK']
    else:
        plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei']
    plt.rcParams['axes.unicode_minus'] = False
    plt.config_inline = {'figure.format': 'retina'}

# --- 生成测试图像 ---
def generate_secret_image(size=32):
    """生成一个简单的 0/1 矩阵图像（比如一个十字架或方块）"""
    img = np.zeros((size, size), dtype=int)
    # 画一个中间的方块 (代表秘密信息)
    center = size // 2
    radius = size // 4
    img[center-radius:center+radius, center-radius:center+radius] = 1
    # 画几条线
    img[center-radius:center+radius, center] = 0
    img[center, center-radius:center+radius] = 0
    return img

# --- 模拟加密过程 ---
def simulate_ecb_leakage():
    print(">>> 正在初始化 Padding 实验 (可能需要几秒生成密钥)...")
    rsa_pad = RSAPadding()
    
    # 1. 准备原始数据
    original_img = generate_secret_image(size=20) # 20x20 像素，太大了跑得慢
    rows, cols = original_img.shape
    
    print(f"正在加密 {rows*cols} 个像素点...")
    
    # 结果矩阵 (我们将取密文的第一个字节作为可视化的颜色值)
    textbook_cipher_map = np.zeros((rows, cols))
    oaep_cipher_map = np.zeros((rows, cols))
    
    # 为了演示效果，我们将 0 和 1 映射为固定的消息
    msg_white = b'secret_white'
    msg_black = b'secret_black'

    # 预计算 Textbook RSA (因为它是确定的，不需要每次算)
    # 这就是不安全的原因：同样的明文 -> 同样的密文
    c_white_tb = rsa_pad.encrypt_textbook(msg_white)[0] # 取第1个字节用于绘图
    c_black_tb = rsa_pad.encrypt_textbook(msg_black)[0]

    for r in range(rows):
        for c in range(cols):
            pixel = original_img[r, c]
            current_msg = msg_black if pixel == 1 else msg_white
            
            # --- 模式 A: Textbook RSA ---
            # 直接填入预计算的值
            if pixel == 1:
                textbook_cipher_map[r, c] = c_black_tb
            else:
                textbook_cipher_map[r, c] = c_white_tb
            
            # --- 模式 B: OAEP RSA ---
            # 必须每次重新加密！因为内部有随机数，每次结果都不同
            full_cipher = rsa_pad.encrypt_oaep(current_msg)
            # 取密文的哈希值或第一个字节来可视化
            oaep_cipher_map[r, c] = full_cipher[0] 

    return original_img, textbook_cipher_map, oaep_cipher_map

# --- 绘图 ---
def plot_padding_comparison(orig, tb, oaep):
    set_style()
    
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    
    # 1. 原始图像
    sns.heatmap(orig, ax=axes[0], cbar=False, square=True, cmap="Greys", xticklabels=False, yticklabels=False)
    axes[0].set_title("原始明文图像 (Original)", fontsize=14, fontweight='bold')
    axes[0].set_xlabel("明显的结构化信息")

    # 2. Textbook RSA 结果
    # 使用由数据决定的颜色映射，看起来依然有轮廓
    sns.heatmap(tb, ax=axes[1], cbar=False, square=True, cmap="viridis", xticklabels=False, yticklabels=False)
    axes[1].set_title("教科书式 RSA 加密 (Textbook)", fontsize=14, fontweight='bold')
    axes[1].set_xlabel("危险！图案轮廓依然可见\n(确定性加密导致的模式泄露)")

    # 3. OAEP 结果
    sns.heatmap(oaep, ax=axes[2], cbar=False, square=True, cmap="viridis", xticklabels=False, yticklabels=False)
    axes[2].set_title("OAEP 填充模式 (Semantic Security)", fontsize=14, fontweight='bold')
    axes[2].set_xlabel("安全：完全的随机噪声\n(相同的明文 -> 不同的密文)")

    plt.tight_layout()
    plt.savefig("rsa_padding_analysis.png", dpi=300)
    print("图表已生成: rsa_padding_analysis.png")
    plt.show()

if __name__ == "__main__":
    orig, tb, oaep = simulate_ecb_leakage()
    plot_padding_comparison(orig, tb, oaep)