#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
using u64 = uint64_t;
using u32 = uint32_t;
using u8 = uint8_t;
using u16 = uint16_t;

constexpr ll ADD_MOD = 65536LL;
constexpr ll MUL_MOD = 65537LL;

u16 key[60];
u16 key2[60];
string my_key_bin_str;

static inline u16 add(u16 a, u16 b) { return a + b; }

u16 mul(u16 a, u16 b)
{
    u64 a1 = (a == 0) ? ADD_MOD : a;
    u64 b1 = (b == 0) ? ADD_MOD : b;
    u64 res = (a1 * b1) % MUL_MOD;
    return (res == ADD_MOD) ? 0 : static_cast<u16>(res);
}

static inline u16 yihuo(u16 a, u16 b) { return a ^ b; }

u16 bin_str_to_u16(string_view s)
{
    u16 res = 0;
    for (int i = 0; i < 16; ++i)
        res = (res << 1) | (s[i] - '0');
    return res;
}

// 128比特循环左移25位
string roll_left(string t)
{
    string s(128, '0');
    for (int i = 0; i < 128; i++)
        s[i] = t[(i + 25) % 128];
    return s;
}

// 生成52个子密钥
void extend(string t)
{
    int k_idx = 1;
    for (int i = 0; i < 6; i++)
    {
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 0, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 16, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 32, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 48, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 64, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 80, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 96, 16));
        key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 112, 16));
    }
    key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 0, 16));
    key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 16, 16));
    key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 32, 16));
    key[k_idx++] = bin_str_to_u16(string_view(t.c_str() + 48, 16));
}

void exgcd(ll a, ll b, ll &d, ll &x, ll &y)
{
    if (!b)
    {
        d = a;
        x = 1;
        y = 0;
    }
    else
    {
        exgcd(b, a % b, d, y, x);
        y -= x * (a / b);
    }
}

u16 inv(u16 a_u16, ll p)
{
    ll a = (a_u16 == 0) ? ADD_MOD : a_u16;
    ll d, x, y;
    exgcd(a, p, d, x, y);
    ll res = (d == 1) ? (x % p + p) % p : -1;
    return (res == ADD_MOD) ? 0 : static_cast<u16>(res);
}

u16 add_ni(u16 a, ll p)
{
    ll b = -static_cast<ll>(a);
    b = (b % p + p) % p;
    return static_cast<u16>(b);
}

// 生成解密子密钥
void dkey()
{
    for (int i = 1; i <= 9; i++)
    {
        for (int j = 1; j <= 4; j++)
        {
            int t = 6 * (10 - i - 1) + j;
            if (j == 1 || j == 4)
                key2[(i - 1) * 6 + j] = inv(key[t], MUL_MOD);
            else
            {
                if (i == 1 || i == 9)
                    key2[(i - 1) * 6 + j] = add_ni(key[t], ADD_MOD);
                else
                {
                    if (j == 2)
                        key2[(i - 1) * 6 + j] = add_ni(key[t + 1], ADD_MOD);
                    else
                        key2[(i - 1) * 6 + j] = add_ni(key[t - 1], ADD_MOD);
                }
            }
        }
    }
    for (int i = 1; i <= 8; i++)
    {
        key2[(i - 1) * 6 + 5] = key[(9 - i - 1) * 6 + 5];
        key2[(i - 1) * 6 + 6] = key[(9 - i - 1) * 6 + 6];
    }
}

// 加密块
void IDEA_encrypt_block(u16 x[4])
{
    for (int i = 1; i <= 8; i++)
    {
        int k_idx = (i - 1) * 6 + 1;
        u16 z1 = key[k_idx++], z2 = key[k_idx++], z3 = key[k_idx++],
            z4 = key[k_idx++], z5 = key[k_idx++], z6 = key[k_idx++];

        u16 t1 = mul(x[0], z1);
        u16 t2 = add(x[1], z2);
        u16 t3 = add(x[2], z3);
        u16 t4 = mul(x[3], z4);
        u16 t5 = yihuo(t1, t3);
        u16 t6 = yihuo(t2, t4);
        u16 t7 = mul(t5, z5);
        u16 t8 = add(t6, t7);
        u16 t9 = mul(t8, z6);
        u16 t10 = add(t7, t9);

        u16 w1 = yihuo(t1, t9);
        u16 w2 = yihuo(t3, t9);
        u16 w3 = yihuo(t2, t10);
        u16 w4 = yihuo(t4, t10);

        x[0] = w1;
        x[1] = w2;
        x[2] = w3;
        x[3] = w4;
    }

    u16 y1 = mul(x[0], key[49]);
    u16 y2 = add(x[2], key[50]);
    u16 y3 = add(x[1], key[51]);
    u16 y4 = mul(x[3], key[52]);
    x[0] = y1;
    x[1] = y2;
    x[2] = y3;
    x[3] = y4;
}

// 解密块
void IDEA_decrypt_block(u16 x[4])
{
    for (int i = 1; i <= 8; i++)
    {
        int k_idx = (i - 1) * 6 + 1;
        u16 z1 = key2[k_idx++], z2 = key2[k_idx++], z3 = key2[k_idx++],
            z4 = key2[k_idx++], z5 = key2[k_idx++], z6 = key2[k_idx++];

        u16 t1 = mul(x[0], z1);
        u16 t2 = add(x[1], z2);
        u16 t3 = add(x[2], z3);
        u16 t4 = mul(x[3], z4);
        u16 t5 = yihuo(t1, t3);
        u16 t6 = yihuo(t2, t4);
        u16 t7 = mul(t5, z5);
        u16 t8 = add(t6, t7);
        u16 t9 = mul(t8, z6);
        u16 t10 = add(t7, t9);

        u16 w1 = yihuo(t1, t9);
        u16 w2 = yihuo(t3, t9);
        u16 w3 = yihuo(t2, t10);
        u16 w4 = yihuo(t4, t10);

        x[0] = w1;
        x[1] = w2;
        x[2] = w3;
        x[3] = w4;
    }

    u16 y1 = mul(x[0], key2[49]);
    u16 y2 = add(x[2], key2[50]);
    u16 y3 = add(x[1], key2[51]);
    u16 y4 = mul(x[3], key2[52]);
    x[0] = y1;
    x[1] = y2;
    x[2] = y3;
    x[3] = y4;
}

// 字节转二进制字符串
string B2b(vector<char> &block)
{
    string out;
    out.reserve(128);
    for (int i = 0; i < 16; ++i)
    {
        u8 u = static_cast<u8>(block[i]);
        for (int b = 7; b >= 0; --b)
            out.push_back(((u >> b) & 1) ? '1' : '0');
    }
    return out;
}

// 读取16字节密钥
vector<char> get_key()
{
    string key;
    getline(cin, key);
    while (key.size() < 16)
        key += "0";
    if (key.size() > 16)
        key.resize(16);
    return vector<char>(key.begin(), key.end());
}

static inline bool is_space_le(char c) { return static_cast<unsigned char>(c) <= ' '; }

// 获取明文
vector<u8> get_plain()
{
    string all((istreambuf_iterator<char>(cin)), (istreambuf_iterator<char>()));
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
// vector<char> key0 = get_key();
// my_key_bin_str = B2b(key0);
// extend(my_key_bin_str);
// dkey();
//
// vector<u8> plain = get_plain();
// size_t original_len = plain.size();
// size_t padding = (8 - (original_len % 8)) % 8;
// for (size_t i = 0; i < padding; ++i)
// plain.push_back(padding);
//
// size_t num_blocks = plain.size() / 8;
// vector<u8> encrypted_bytes(plain.size());
// vector<u8> decrypted_bytes(plain.size());
// u16 x[4];
//
// // 加密
// for (size_t i = 0; i < num_blocks; ++i)
// {
// u8 *p_in = plain.data() + i * 8;
// x[0] = (p_in[0] << 8) | p_in[1];
// x[1] = (p_in[2] << 8) | p_in[3];
// x[2] = (p_in[4] << 8) | p_in[5];
// x[3] = (p_in[6] << 8) | p_in[7];
// IDEA_encrypt_block(x);
// u8 *p_out = encrypted_bytes.data() + i * 8;
// p_out[0] = (x[0] >> 8);
// p_out[1] = (x[0] & 0xFF);
// p_out[2] = (x[1] >> 8);
// p_out[3] = (x[1] & 0xFF);
// p_out[4] = (x[2] >> 8);
// p_out[5] = (x[2] & 0xFF);
// p_out[6] = (x[3] >> 8);
// p_out[7] = (x[3] & 0xFF);
// }
//
// // 解密
// for (size_t i = 0; i < num_blocks; ++i)
// {
// u8 *p_in = encrypted_bytes.data() + i * 8;
// x[0] = (p_in[0] << 8) | p_in[1];
// x[1] = (p_in[2] << 8) | p_in[3];
// x[2] = (p_in[4] << 8) | p_in[5];
// x[3] = (p_in[6] << 8) | p_in[7];
// IDEA_decrypt_block(x);
// u8 *p_out = decrypted_bytes.data() + i * 8;
// p_out[0] = (x[0] >> 8);
// p_out[1] = (x[0] & 0xFF);
// p_out[2] = (x[1] >> 8);
// p_out[3] = (x[1] & 0xFF);
// p_out[4] = (x[2] >> 8);
// p_out[5] = (x[2] & 0xFF);
// p_out[6] = (x[3] >> 8);
// p_out[7] = (x[3] & 0xFF);
// }
//
// // cout << "明文：" << string(plain.begin(), plain.begin() + original_len) << endl;
// // cout << "密文 (hex)：";
// // cout << hex << setfill('0');
// // for (size_t i = 0; i < encrypted_bytes.size(); ++i)
// // cout << setw(2) << static_cast<int>(encrypted_bytes[i]);
// // cout << dec << endl;
// // string mingwen1(decrypted_bytes.begin(), decrypted_bytes.begin() + original_len);
// // cout << "解密后的明文：" << mingwen1 << endl;
// return 0;
// }
//

namespace fs = std::filesystem;
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fs::current_path() << "\n";
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
            return 0; // 防御
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

    vector<char> key0 = get_key();
    my_key_bin_str = B2b(key0);
    extend(my_key_bin_str);
    dkey();

    vector<u8> plain = get_plain();
    size_t original_len = plain.size();

    size_t padding = (8 - (original_len % 8)) % 8;
    for (size_t i = 0; i < padding; ++i)
        plain.push_back(padding);
    size_t num_blocks = plain.size() / 8;
    vector<u8> encrypted_bytes(plain.size());
    vector<u8> decrypted_bytes(plain.size());
    u16 x[4];

    uint64_t t_enc_begin = now_us();
    for (size_t i = 0; i < num_blocks; ++i)
    {
        u8 *p_in = plain.data() + i * 8;
        x[0] = (p_in[0] << 8) | p_in[1];
        x[1] = (p_in[2] << 8) | p_in[3];
        x[2] = (p_in[4] << 8) | p_in[5];
        x[3] = (p_in[6] << 8) | p_in[7];
        IDEA_encrypt_block(x);
        u8 *p_out = encrypted_bytes.data() + i * 8;
        p_out[0] = (x[0] >> 8);
        p_out[1] = (x[0] & 0xFF);
        p_out[2] = (x[1] >> 8);
        p_out[3] = (x[1] & 0xFF);
        p_out[4] = (x[2] >> 8);
        p_out[5] = (x[2] & 0xFF);
        p_out[6] = (x[3] >> 8);
        p_out[7] = (x[3] & 0xFF);
    }
    uint64_t t_enc_end = now_us();

    uint64_t t_dec_begin = now_us();
    for (size_t i = 0; i < num_blocks; ++i)
    {
        u8 *p_in = encrypted_bytes.data() + i * 8;
        x[0] = (p_in[0] << 8) | p_in[1];
        x[1] = (p_in[2] << 8) | p_in[3];
        x[2] = (p_in[4] << 8) | p_in[5];
        x[3] = (p_in[6] << 8) | p_in[7];
        IDEA_decrypt_block(x);
        u8 *p_out = decrypted_bytes.data() + i * 8;
        p_out[0] = (x[0] >> 8);
        p_out[1] = (x[0] & 0xFF);
        p_out[2] = (x[1] >> 8);
        p_out[3] = (x[1] & 0xFF);
        p_out[4] = (x[2] >> 8);
        p_out[5] = (x[2] & 0xFF);
        p_out[6] = (x[3] >> 8);
        p_out[7] = (x[3] & 0xFF);
    }
    uint64_t t_dec_end = now_us();

    // cout << "明文：" << string(plain.begin(), plain.begin() + original_len) << "\n";
    // cout << "密文 (hex)：";
    // cout << hex << setfill('0');
    // for (size_t i = 0; i < encrypted_bytes.size(); ++i)
    //     cout << setw(2) << static_cast<int>(encrypted_bytes[i]);
    // cout << dec << "\n";
    // cout << "解密后的明文：" << string(decrypted_bytes.begin(), decrypted_bytes.begin() + original_len) << "\n";

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

    cerr << "\n【IDEA 性能指标 CPP（统一口径）】\n"
         << " 输入字节数   ：" << original_len << " 字节\n"
         << " 输出字节数   ：" << encrypted_bytes.size() << " 字节\n"
         << " 数据块数量   ：" << num_blocks << "\n"
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
jielycatjielycat
Hello IDEA!

jielycatjielycat
@data/The_Story_of_the_Stone.txt

*/