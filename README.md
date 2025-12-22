# 🔐 RSA-Implementation-Attack-Defense
**针对 RSA 算法的数学攻击向量复现与 OAEP 防御机制构建**

> **西南财经大学 2025 秋季学期《信息安全基础》期末项目**
>
> **作者**: 刘俊宏 (42311180), 许桐恺 (42311038)

## 📖 项目简介 (Introduction)

本项目不仅仅是简单的 RSA 算法调用，而是一次**从底层引擎构建、到漏洞复现、再到防御加固**的完整密码学工程实践。

我们**不依赖第三方密码学库**（如 `pycryptodome`），纯手工基于 Python 实现了 RSA 的核心算子，复现了针对特定参数的经典数学攻击，并最终构建了符合 PKCS#1 v2.2 标准的 OAEP 填充机制，解决了教科书式 RSA (Textbook RSA) 的语义安全问题。

## 核心特性 (Key Features)

### 1. 高性能底层构建 (The Build)
*   **大素数生成**: 手写 **Miller-Rabin** 素性检测算法（$k=40$），误判率低至 $10^{-24}$。
*   **模幂运算优化**: 采用二进制“平方-乘”算法，将复杂度从 $O(N)$ 降至 $O(\log N)$。
*   **CRT 加速**: 利用 **中国剩余定理 (Chinese Remainder Theorem)** 优化解密流程，实测在 2048 位密钥下性能提升约 **300%**。

### 2. 攻击向量复现 (The Break)
*   **广播攻击 (Broadcast Attack)**: 复现 Håstad 攻击场景。当 $e=3$ 且同一明文发送给 3 个不同用户时，通过 CRT 直接还原明文（无需分解 $N$）。
*   **维纳攻击 (Wiener's Attack)**: 复现低私钥指数攻击。当 $d < \frac{1}{3}N^{0.25}$ 时，利用**连分数展开 (Continued Fractions)** 在多项式时间内反推私钥。

### 3. 防御机制构建 (The Fix)
*   **OAEP 填充**: 实现了基于 **MGF1 (Mask Generation Function)** 的最优非对称加密填充。
*   **语义安全验证**: 通过位图加密实验，证明了 OAEP 能有效消除确定性加密带来的模式泄露（ECB 模式问题）。

## 项目结构 (Structure)

```bash
.
├── rsa_core.py        # RSA 核心实现 (KeyGen, Encrypt, Decrypt, CRT, Miller-Rabin)
├── rsa_attacks.py     # 攻击算法实现 (Broadcast Attack, Wiener's Attack)
├── rsa_padding.py     # OAEP 填充与 MGF1 实现
├── experiments.py     # 科学实验与绘图脚本 (性能测试、可视化)
└── README.md
```

##  实验结果 (Results)

### 1. CRT 性能加速
我们在 2048-bit 密钥下进行了基准测试，结果显示 CRT 版本的解密速度是标准版本的 **3.5 倍**。

### 2. 攻击复现
*   **广播攻击**: 攻击耗时甚至低于合法用户的解密耗时（毫秒级）。
*   **维纳攻击**: 对于 1024 位弱密钥，仅需计算前几十个渐进分数即可瞬间还原 $d$。

### 3. OAEP 可视化防御
**结论**: 教科书式 RSA 保留了图像的统计特征（中间图），而 OAEP 将密文转化为纯随机噪声（右图），实现了 IND-CPA 安全。

## 📜 参考文献 (References)

1.  **Rivest, R. L., et al.** (1978). *A method for obtaining digital signatures and public-key cryptosystems*.
2.  **Håstad, J.** (1988). *On solving simultaneous modular equations of low degree*.
3.  **Wiener, M. J.** (1990). *Cryptanalysis of short RSA secret exponents*.
4.  **Bellare, M., & Rogaway, P.** (1994). *Optimal asymmetric encryption padding*.
5.  **RFC 8017**: PKCS #1: RSA Cryptography Specifications Version 2.2.

## ⚠️ 免责声明 (Disclaimer)

本项目仅用于**学术研究与教育目的**（西南财经大学课程作业）。代码中的实现主要侧重于原理验证，未针对侧信道攻击（如缓存计时攻击、功耗分析）进行防护，**严禁用于生产环境**。

---

© 2025 Junhong Liu & Tongkai Xu. All Rights Reserved.
