#include <bits/stdc++.h>
using namespace std;

using u64 = uint64_t;
using u32 = uint32_t;
using u8 = uint8_t;

static const int LS[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1}; // 每轮左移位数

// 密钥置换 PC-1（64->56，去奇偶校验位）
static const int PC1[56] = {
    57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};
// 压缩置换 PC-2（56->48）
static const int PC2[48] = {
    14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

// 初始/逆初始置换
static const int IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};
static const int FP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};

// E 扩展（32->48）
static const int E_tab[48] = {
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

// P 置换（F 函数输出置换）
static const int P_tab[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25};

// 8 个 S 盒
static const int SBOX[8][4][16] = {
    {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

template <int N>
static inline u64 permute(u64 input, const int (&tbl)[N])
{
    u64 out = 0;
    for (int i = 0; i < N; ++i)
    {
        int src = tbl[i] - 1;  
        u64 bit = (input >> (64 - 1 - src)) & 1u; 
        out = (out << 1) | bit;
    }
    return out;
}

static inline u64 permute_n(u64 input, const int *tbl, int n, int in_width)
{
    u64 out = 0;
    for (int i = 0; i < n; ++i)
    {
        int src = tbl[i] - 1;
        u64 bit = (input >> (in_width - 1 - src)) & 1u;
        out = (out << 1) | bit;
    }
    return out;
}

static inline u32 rol28(u32 v, int r)
{
    v &= 0x0FFFFFFFu;
    return ((v << r) | (v >> (28 - r))) & 0x0FFFFFFFu;
}

static inline u64 load64_be(const u8 *b)
{
    return ((u64)b[0] << 56) | ((u64)b[1] << 48) | ((u64)b[2] << 40) | ((u64)b[3] << 32) |
           ((u64)b[4] << 24) | ((u64)b[5] << 16) | ((u64)b[6] << 8) | ((u64)b[7]);
}
static inline void store64_be(u64 v, u8 *b)
{
    b[0] = (u8)(v >> 56);
    b[1] = (u8)(v >> 48);
    b[2] = (u8)(v >> 40);
    b[3] = (u8)(v >> 32);
    b[4] = (u8)(v >> 24);
    b[5] = (u8)(v >> 16);
    b[6] = (u8)(v >> 8);
    b[7] = (u8)(v);
}

static inline void des_key_schedule(const u8 key8[8], u64 subkey[16])
{
    u64 key64 = load64_be(key8);              // 原始 64 位密钥
    u64 k56 = permute<56>(key64, PC1);        // PC-1 去校验位
    u32 C = (u32)((k56 >> 28) & 0x0FFFFFFFu); // 左 28 位
    u32 D = (u32)(k56 & 0x0FFFFFFFu);         // 右 28 位
    for (int r = 0; r < 16; ++r)
    {
        C = rol28(C, LS[r]); // 左移
        D = rol28(D, LS[r]);
        u64 CD = (((u64)C) << 28) | (u64)D;   // 合并 56 位
        u64 k48 = permute_n(CD, PC2, 48, 56); // 压缩到 48 位
        subkey[r] = k48;
    }
}

static inline u32 des_f(u32 R, u64 k48)
{
    u64 ER = permute_n((u64)R, E_tab, 48, 32); // 扩展 32->48
    u64 x = ER ^ k48;                          // 与子密钥异或
    u32 out32 = 0;
    for (int i = 0; i < 8; ++i)
    { // 8 个 S 盒
        int shift = 42 - 6 * i;
        u8 six = (u8)((x >> shift) & 0x3Fu);
        int row = ((six & 0x20) >> 4) | (six & 0x01);
        int col = (six >> 1) & 0x0F;
        u8 s = (u8)SBOX[i][row][col];
        out32 = (out32 << 4) | s;
    }
    u64 Pout = permute_n((u64)out32, P_tab, 32, 32); // P 置换
    return (u32)Pout;
}

static inline void des_encrypt_block(const u8 in[8], u8 out[8], const u64 subkey[16])
{
    u64 B = load64_be(in);
    u64 IPB = permute<64>(B, IP);           // 初始置换
    u32 L = (u32)(IPB >> 32), R = (u32)IPB; // 拆成 L/R
    for (int r = 0; r < 16; ++r)
    { // 16 轮 Feistel
        u32 f = des_f(R, subkey[r]);
        u32 newL = R;
        u32 newR = L ^ f;
        L = newL;
        R = newR;
    }
    u64 preout = (((u64)R) << 32) | L; // 交换 L/R
    u64 C = permute<64>(preout, FP);   // 逆初始置换
    store64_be(C, out);
}

static inline void des_decrypt_block(const u8 in[8], u8 out[8], const u64 subkey[16])
{
    u64 rev[16];
    for (int i = 0; i < 16; ++i)
        rev[i] = subkey[15 - i]; // 反向子密钥
    des_encrypt_block(in, out, rev);
}

static inline vector<u8> pkcs7_pad_vec(const vector<u8> &data, size_t block = 8)
{
    size_t n = data.size();
    size_t pad = block - (n % block);
    if (pad == 0)
        pad = block;
    vector<u8> out;
    out.reserve(n + pad);
    out.insert(out.end(), data.begin(), data.end());
    out.insert(out.end(), pad, (u8)pad);
    return out;
}
static inline bool pkcs7_unpad_inplace(vector<u8> &buf, size_t block = 8)
{
    if (buf.empty() || (buf.size() % block) != 0)
        return false;
    u8 p = buf.back();
    if (p == 0 || p > block || p > buf.size())
        return false;
    for (size_t i = 0; i < p; ++i)
        if (buf[buf.size() - 1 - i] != p)
            return false;
    buf.resize(buf.size() - p);
    return true;
}

static inline vector<u8> des_ecb_encrypt_vec(const vector<u8> &plain, const u8 key8[8])
{
    u64 subkey[16];
    des_key_schedule(key8, subkey);
    vector<u8> in = pkcs7_pad_vec(plain, 8);
    vector<u8> out(in.size());
    for (size_t i = 0; i < in.size(); i += 8)
        des_encrypt_block(&in[i], &out[i], subkey);
    return out;
}
static inline bool des_ecb_decrypt_vec(const vector<u8> &cipher, const u8 key8[8], vector<u8> &plain_out)
{
    if (cipher.size() % 8)
        return false;
    u64 subkey[16];
    des_key_schedule(key8, subkey);
    plain_out.assign(cipher.size(), 0);
    for (size_t i = 0; i < cipher.size(); i += 8)
        des_decrypt_block(&cipher[i], &plain_out[i], subkey);
    return pkcs7_unpad_inplace(plain_out, 8);
}

static inline string to_hex(const vector<u8> &v)
{
    static const char *H = "0123456789abcdef";
    string s;
    s.resize(v.size() * 2);
    for (size_t i = 0; i < v.size(); ++i)
    {
        u8 b = v[i];
        s[2 * i] = H[b >> 4];
        s[2 * i + 1] = H[b & 0xF];
    }
    return s;
}
static inline vector<u8> from_hex(const string &h)
{
    auto hexv = [](char c) -> int
    {
        if ('0' <= c && c <= '9')
            return c - '0';
        if ('a' <= c && c <= 'f')
            return c - 'a' + 10;
        if ('A' <= c && c <= 'F')
            return c - 'A' + 10;
        return -1;
    };
    vector<u8> v;
    if (h.size() % 2)
        return v;
    v.resize(h.size() / 2);
    for (size_t i = 0; i < v.size(); ++i)
    {
        int hi = hexv(h[2 * i]), lo = hexv(h[2 * i + 1]);
        if (hi < 0 || lo < 0)
        {
            v.clear();
            return v;
        }
        v[i] = (u8)((hi << 4) | lo);
    }
    return v;
}

static inline bool is_space_le(char c) { return static_cast<unsigned char>(c) <= ' '; }

vector<u8> get_plain()
{
    string all((istreambuf_iterator<char>(cin)), istreambuf_iterator<char>());
    size_t i = 0;
    while (i < all.size() && is_space_le(all[i]))
        ++i;
    if (i < all.size() && all[i] == '@')
    {
        size_t j = i + 1;
        while (j < all.size() && is_space_le(all[j]))
            ++j;
        size_t k = all.size();
        while (k > j && is_space_le(all[k - 1]))
            --k;
        string path = all.substr(j, k - j);
        FILE *fp = fopen(path.c_str(), "rb");
        if (!fp)
            return {};
        fseek(fp, 0, SEEK_END);
        long n = ftell(fp);
        if (n < 0)
        {
            fclose(fp);
            return {};
        }
        fseek(fp, 0, SEEK_SET);
        vector<u8> buf(static_cast<size_t>(n));
        if (n > 0)
            fread(buf.data(), 1, static_cast<size_t>(n), fp);
        fclose(fp);
        return buf;
    }
    return vector<u8>(all.begin(), all.end());
}

// int main()
// {
// ios::sync_with_stdio(false);
// cin.tie(nullptr);
//
// // 读取 8 字节密钥：不足补 0，多余截断
// string key_in;
// if (!getline(cin, key_in))
// return 0;
// if (key_in.size() < 8)
// key_in.append(8 - key_in.size(), '\0');
// if (key_in.size() > 8)
// key_in.resize(8);
// u8 key8[8];
// for (int i = 0; i < 8; ++i)
// key8[i] = (u8)key_in[i];
//
// // 读取明文，ECB 演示：加密再解密
// vector<u8> plain = get_plain();
// vector<u8> cipher = des_ecb_encrypt_vec(plain, key8);
// vector<u8> plain2;
// bool ok = des_ecb_decrypt_vec(cipher, key8, plain2);
// if (!ok)
// {
// cerr << "unpad failed\n";
// return 1;
// }
// // for (auto &x : cipher)
// // cout << (int)(static_cast<unsigned char>(x)) << " ";
// // cout << "\n";
// // cout << to_hex(cipher) << "\n";
// // cout.write((const char *)plain2.data(), (streamsize)plain2.size());
// // cout << "\n";
// return 0;
// }

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto now_us = []() -> uint64_t
    {
        return (uint64_t)chrono::duration_cast<chrono::microseconds>(
                   chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    };
    auto read_status_kb = [](const char *key) -> uint64_t
    {
#ifdef __linux__
        ifstream f("/proc/self/status");
        string line;
        while (getline(f, line))
        {
            if (line.rfind(key, 0) == 0)
            {
                for (char &c : line)
                    if (!isdigit((unsigned char)c))
                        c = ' ';
                stringstream ss(line);
                uint64_t v = 0;
                ss >> v;
                return v; // kB
            }
        }
#endif
        return 0;
    };
    auto rss_kb = [&]()
    { return read_status_kb("VmRSS:"); };
    auto hwm_kb = [&]()
    { return read_status_kb("VmHWM:"); };
    auto read_proc_ticks = []() -> uint64_t
    {
#ifdef __linux__
        ifstream f("/proc/self/stat");
        string s;
        if (!getline(f, s))
            return 0;
        size_t rparen = s.rfind(')');
        if (rparen == string::npos)
            return 0;
        string rest = s.substr(rparen + 2);
        string tok;
        vector<string> a;
        stringstream ss(rest);
        while (ss >> tok)
            a.push_back(tok);
        if (a.size() < 13)
            return 0;
        uint64_t ut = strtoull(a[11].c_str(), nullptr, 10);
        uint64_t st = strtoull(a[12].c_str(), nullptr, 10);
        return ut + st;
#else
        return 0;
#endif
    };
    auto ticks_per_sec = []() -> long
    {
#ifdef __linux__
        long v = sysconf(_SC_CLK_TCK);
        return v > 0 ? v : 100;
#else
        return 100;
#endif
    };

    uint64_t t_all_begin = now_us();
    uint64_t cpu_ticks_begin = read_proc_ticks();
    uint64_t rss_begin = rss_kb();

    string key_in;
    if (!getline(cin, key_in))
        return 0;
    if (key_in.size() < 8)
        key_in.append(8 - key_in.size(), '\0');
    if (key_in.size() > 8)
        key_in.resize(8);
    u8 key8[8];
    for (int i = 0; i < 8; ++i)
        key8[i] = (u8)key_in[i];

    vector<u8> plain = get_plain();

    uint64_t t_enc_begin = now_us();
    vector<u8> cipher = des_ecb_encrypt_vec(plain, key8);
    uint64_t t_enc_end = now_us();

    uint64_t t_dec_begin = now_us();
    vector<u8> plain2;
    bool ok = des_ecb_decrypt_vec(cipher, key8, plain2);
    uint64_t t_dec_end = now_us();
    if (!ok)
    {
        cerr << "去填充失败\n";
        return 1;
    }

    uint64_t t_all_end = now_us();
    uint64_t cpu_ticks_end = read_proc_ticks();
    uint64_t rss_end = rss_kb();
    uint64_t peak_kb = hwm_kb();

    double enc_ms = (t_enc_end - t_enc_begin) / 1000.0;
    double dec_ms = (t_dec_end - t_dec_begin) / 1000.0;
    double all_ms = (t_all_end - t_all_begin) / 1000.0;
    double wall_s = (t_all_end - t_all_begin) / 1e6;
    double cpu_s = 0.0;
#ifdef __linux__
    cpu_s = double(cpu_ticks_end - cpu_ticks_begin) / double(ticks_per_sec());
#endif
    unsigned ncpu = thread::hardware_concurrency();
    if (!ncpu)
        ncpu = 1;
    double cpu_percent = wall_s > 0 ? (cpu_s / wall_s) * 100.0 / ncpu : 0.0;
    if (cpu_percent < 0)
        cpu_percent = 0;
    if (cpu_percent > 100)
        cpu_percent = 100;

    uint64_t blocks = cipher.size() / 8;

    cerr << "\n【DES 性能指标 CPP（统一口径）】\n"
         << " 输入字节数   ：" << plain.size() << " 字节\n"
         << " 输出字节数   ：" << cipher.size() << " 字节\n"
         << " 数据块数量   ：" << blocks << "\n"
         << " 加密耗时    ：" << enc_ms << " 毫秒\n"
         << " 解密耗时    ：" << dec_ms << " 毫秒\n"
         << " 总耗时     ：" << all_ms << " 毫秒\n"
         << " 起始内存占用 ：" << rss_begin << " KB\n"
         << " 结束内存占用 ：" << rss_end << " KB\n"
         << " 峰值内存占用 ：" << peak_kb << " KB\n"
         << fixed << setprecision(1)
         << " CPU 使用率  ：" << cpu_percent << " %\n";

    return 0;
}

/*
jielycat
Hello IDEA!

jielycat
@data/The_Story_of_the_Stone.txt
*/