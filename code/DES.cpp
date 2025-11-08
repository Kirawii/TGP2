#include<bits/stdc++.h>
using namespace std;

vector<string> Key0(16);

vector<int> LS = {
            1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1},
			EX_K1 = {
              57, 49, 41, 33, 25, 17, 9,
			   1, 58, 50, 42, 34, 26, 18,
			  10,  2, 59, 51, 43, 35, 27,
			  19, 11,  3, 60, 52, 44, 36,
			  63, 55, 47, 39, 31, 23, 15,
			   7, 62, 54, 46, 38, 30, 22,
			  14,  6, 61, 53, 45, 37, 29,
			  21, 13,  5, 28, 20, 12, 4},
		    EX_K2 = {
                14, 17, 11, 24, 1, 5,
			   3, 28, 15,  6, 21, 10,
			  23, 19, 12,  4, 26,  8,
			  16,  7, 27, 20, 13,  2,
			  41, 52, 31, 37, 47, 55,
			  30, 40, 51, 45, 33, 48,
			  44, 49, 39, 56, 34, 53,
			  46, 42, 50, 36, 29, 32},
			IP = {
             58, 50, 42, 34, 26, 18, 10, 2,
			 60, 52, 44, 36, 28, 20, 12, 4,
			 62, 54, 46, 38, 30, 22, 14, 6,
			 64, 56, 48, 40, 32, 24, 16, 8,
			 57, 49, 41, 33, 25, 17, 9,  1,
			 59, 51, 43, 35, 27, 19, 11, 3,
			 61, 53, 45, 37, 29, 21, 13, 5,
			 63, 55, 47, 39, 31, 23, 15, 7 },
			IPR = {
              40, 8, 48, 16, 56, 24, 64, 32,
			  39, 7, 47, 15, 55, 23, 63, 31,
			  38, 6, 46, 14, 54, 22, 62, 30,
			  37, 5, 45, 13, 53, 21, 61, 29,
			  36, 4, 44, 12, 52, 20, 60, 28,
			  35, 3, 43, 11, 51, 19, 59, 27,
			  34, 2, 42, 10, 50, 18, 58, 26,
			  33, 1, 41,  9, 49, 17, 57, 25},
			E = {
                32, 1, 2, 3, 4, 5,
				4, 5, 6, 7, 8, 9,
				8, 9, 10, 11, 12, 13,
			    12, 13, 14, 15, 16, 17,
			    16, 17, 18, 19, 20, 21,
			    20, 21, 22, 23, 24, 25,
			    24, 25, 26, 27, 28, 29,
			    28, 29, 30, 31, 32, 1},
		    P = {
                16, 7, 20, 21,
			    29, 12, 28, 17,
				1, 15, 23, 26,
				5, 18, 31, 10,
				2, 8, 24, 14,
			    32, 27, 3, 9,
			    19, 13, 30, 6,
			    22, 11, 4, 25};
int SBOX[8][4][16] = {
	{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
	 {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
	 {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
	 {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
	{{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
	 {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
	 {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
	 {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
	{{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
	 {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
	 {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
	 {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
	{{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
	 {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
	 {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
	 {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
	{{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
	 {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
	 {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
	 {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
	{{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
	 {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
	 {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
	 {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
	{{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
	 {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
	 {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
	 {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
	{{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
	 {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
	 {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
	 {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}
};		 

string EX(string s, vector<int> & ex) {
	string s2;
	for(auto & x : ex) s2 += s[x - 1];
	return s2;
}

string movel(string s, int i) {
	return s.substr(LS[i]) + s.substr(0, LS[i]);
}

string B2b(vector<char> & block) {
    string out;
    out.reserve(64);
    for (int i = 0; i < 8; ++i) {
    	char u = static_cast<unsigned char>(block[i]);
        for (int b = 7; b >= 0; --b) {
            out.push_back( ((u >> b) & 1) ? '1' : '0' );
        }
    }
    return out;
}

void b2B(string & bits, vector<char> & block) {
    for (int i = 0; i < 8; ++i) {
        char v = 0;
        for (int b = 0; b < 8; ++b) {
            char c = bits[i*8 + b];
            v = (char)((v << 1) | (c == '1'));
        }    
        block[i] = v;
    }    
}

void make_key(vector<char> & key8) {
    string key = B2b(key8);
	string key2 = EX(key, EX_K1);
	for(int i = 0; i < 16; i ++) {
		string l = key2.substr(0, 28), r = key2.substr(28);
		l = movel(l, i);
		r = movel(r, i);
		key2 = l + r;
		Key0[i] = EX(key2, EX_K2);
	}
}

string XOR(string s1, string s2) {
	string s;
	for(size_t i = 0; i < s1.size(); i ++) s += '0' + (s1[i] != s2[i]);
	return s;
}

string S(string s) {
	string s2;
	for(int i = 0; i < 48; i += 6) {
		int x = ((s[i] - '0') << 1) + (s[i + 5] - '0'), y = 0;
		for(int j = i + 4, base = 1; j > i; j --, base *= 2) y += (s[j] - '0') * base;
		int z = SBOX[i / 6][x][y];
		for(int j = 3; j >= 0; j --) s2 += '0' + ((z >> j) & 1);
	}
	return s2;
}

string F(string s, string key) {
	s = EX(s, E);
	s = XOR(s, key);
	s = S(s);
	s = EX(s, P);
	return s;
}

string DES(string con, bool f) {
	con = EX(con, IP);
	string l0 = con.substr(0, 32), r0 = con.substr(32), mid;
	for(int i = 0; i < 16; i ++) {
        mid = r0;
		if(f) r0 = XOR(l0, F(r0, Key0[i]));
		else r0 = XOR(l0, F(r0, Key0[15 - i]));
		l0 = mid;
	}    
	con = EX(r0 + l0, IPR);
	return con;
}    

vector<char> pad(const vector<char> & data) {
    size_t pad_len = 8 - (data.size() % 8);
    if (pad_len == 0) pad_len = 8;
    vector<char> out = data;
    out.insert(out.end(), pad_len, (char)pad_len);
    return out;
}

vector<char> unpad(const vector<char> & data) {
    char pad = data.back();
    return vector<char>(data.begin(), data.end() - pad);
}

vector<char> DES_EN(vector<char> & in) {
	in = pad(in);
    vector<char> out;
    out.resize(in.size());
    for (size_t i = 0; i < in.size(); i += 8) {
        vector<char> block(8); 
        memcpy(block.data(), &in[i], 8);
        string bits = B2b(block);
        string enc_bits = DES(bits, 1);
        b2B(enc_bits, block);
        memcpy(&out[i], block.data(), 8);
    }
    return out;
}

vector<char> DES_DE(const vector<char>& in) {
    vector<char> out;
    out.resize(in.size());
    for (size_t i = 0; i < in.size(); i += 8) {
        vector<char> block(8); 
        memcpy(block.data(), &in[i], 8);
        string bits = B2b(block);
        string enc_bits = DES(bits, 0);
        b2B(enc_bits, block);
        memcpy(&out[i], block.data(), 8);
    }
    return unpad(out);
}

vector<char> get_key() {
    // cout << "input key(8 bytes)\n";
    string key;
    cin >> key;
    while(key.size() < 8) key += "0";
    if (key.size() > 8) key.resize(8);
    vector<char> key_char(key.begin(), key.end());
	return key_char;
}

vector<char> get_plain() {
    // cout << "input plaintext or @<path> to file\n";
    string all, line; 
    bool first = true;
    while (getline(cin, line)) {
        if (!first) all.push_back('\n');
        all += line;
        first = false;
    }
    auto ltrim = [](const string & s) -> string {
        size_t i = 0;
        while (i < s.size() && (unsigned char)s[i] <= ' ') ++i;
        return s.substr(i);
    };
    string trimmed = ltrim(all);
    string content = trimmed;
    if (!trimmed.empty() && trimmed[0] == '@') {
        string path = trimmed.substr(1);
        while (!path.empty() && (unsigned char)path.front() <= ' ') path.erase(path.begin());
        while (!path.empty() && (unsigned char)path.back()  <= ' ') path.pop_back();
        vector<char> bytes;
        FILE* fp = fopen(path.c_str(), "rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            long n = ftell(fp);
            if (n < 0) n = 0;
            fseek(fp, 0, SEEK_SET);
            bytes.resize((size_t)n);
            if (n) fread(bytes.data(), 1, (size_t)n, fp);
            fclose(fp);
            if (!bytes.empty()) content.assign((const char*)bytes.data(), bytes.size());
        }
    }
	return vector<char> (content.begin(), content.end());
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    
    vector<char> key = get_key();
    make_key(key);
    
    vector<char> plain = get_plain();
    
    vector<char> cipher = DES_EN(plain);
	for(auto & x : cipher) cout << (int)(static_cast<unsigned char>(x)) << " ";
	cout << "\n";
    
    vector<char> plain2 = DES_DE(cipher);
    
    string p(plain2.begin(), plain2.end());
    cout << p << "\n";
    
    return 0;
}