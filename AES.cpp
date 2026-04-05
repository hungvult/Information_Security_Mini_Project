#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

const string Message = "AB5BFF34115C963B835CAF027EBE0B53";
const string Key = "CDAB0FC51CACBCF9A8A348C3D2D0247A";

// S-box
static const unsigned char Sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Round Constant
static const unsigned char Rcon[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

// Hàm hỗ trợ chuyển đổi hex string sang vector byte
vector<unsigned char> hexToBytes(const string& hex) {
    vector<unsigned char> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

// Hàm hỗ trợ in vector byte ra hex
void printBytes(const string& name, const vector<unsigned char>& bytes) {
    cout << name << " = ";
    for (unsigned char b : bytes) cout << hex << uppercase << setw(2) << setfill('0') << (int)b;
    cout << endl;
}

void printStateRow(const string& name, const vector<unsigned char>& state) {
    cout << name << " = ";
    for(size_t i=0; i<16; i++){
        cout << hex << uppercase << setw(2) << setfill('0') << (int)state[i];
    }
    cout << endl;
}

// Key Expansion Helper
vector<unsigned char> RotWord(const vector<unsigned char>& w) {
    return {w[1], w[2], w[3], w[0]};
}

vector<unsigned char> SubWord(const vector<unsigned char>& w) {
    return {Sbox[w[0]], Sbox[w[1]], Sbox[w[2]], Sbox[w[3]]};
}

vector<unsigned char> XorBytes(const vector<unsigned char>& a, const vector<unsigned char>& b) {
    vector<unsigned char> res(a.size());
    for(size_t i = 0; i < a.size(); ++i) res[i] = a[i] ^ b[i];
    return res;
}

vector<vector<unsigned char>> KeyExpansion(const vector<unsigned char>& key) {
    vector<vector<unsigned char>> w(44, vector<unsigned char>(4));
    cout << "--- PHAN 1: SINH KHOA ---" << endl;
    for (int i = 0; i < 4; i++) {
        w[i] = {key[4*i], key[4*i+1], key[4*i+2], key[4*i+3]};
        printBytes("w" + to_string(i), w[i]);
    }
    
    for (int i = 4; i < 44; i++) {
        vector<unsigned char> temp = w[i-1];
        if (i % 4 == 0) {
            cout << "\n--- Sinh w" << i << " den w" << i+3 << " (K" << i/4 << ") ---" << endl;
            vector<unsigned char> rw = RotWord(temp);
            printBytes("rw = RotWord(w" + to_string(i-1) + ")", rw);
            vector<unsigned char> sw = SubWord(rw);
            printBytes("sw = SubWord(rw)", sw);
            vector<unsigned char> rc = {Rcon[i/4 - 1], 0x00, 0x00, 0x00};
            cout << "Rcon[" << i/4 << "] = " << hex << setw(2) << setfill('0') << (int)rc[0] << "000000" << endl;
            temp = XorBytes(sw, rc);
            printBytes("xcsw = XorRcon(sw, Rcon)", temp);
        }
        w[i] = XorBytes(w[i-4], temp);
        printBytes("w" + to_string(i), w[i]);
    }
    return w;
}

// Chuyen M thanh State cong mang
vector<unsigned char> getAddRoundKey(const vector<unsigned char>& state, const vector<vector<unsigned char>>& w, int round) {
    vector<unsigned char> res = state;
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            res[r + 4*c] = state[r + 4*c] ^ w[round * 4 + c][r];
        }
    }
    return res;
}

vector<unsigned char> SubBytes(const vector<unsigned char>& state) {
    vector<unsigned char> res = state;
    for (int i = 0; i < 16; i++) res[i] = Sbox[state[i]];
    return res;
}

vector<unsigned char> ShiftRows(const vector<unsigned char>& state) {
    vector<unsigned char> res(16);
    // Row 0
    res[0] = state[0]; res[4] = state[4]; res[8] = state[8]; res[12] = state[12];
    // Row 1
    res[1] = state[5]; res[5] = state[9]; res[9] = state[13]; res[13] = state[1];
    // Row 2
    res[2] = state[10]; res[6] = state[14]; res[10] = state[2]; res[14] = state[6];
    // Row 3
    res[3] = state[15]; res[7] = state[3]; res[11] = state[7]; res[15] = state[11];
    return res;
}

unsigned char gmul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    for (int counter = 0; counter < 8; counter++) {
        if ((b & 1) != 0) p ^= a;
        bool hi_bit_set = (a & 0x80) != 0;
        a <<= 1;
        if (hi_bit_set) a ^= 0x1B;
        b >>= 1;
    }
    return p;
}

vector<unsigned char> MixColumns(const vector<unsigned char>& state) {
    vector<unsigned char> res(16);
    for (int c = 0; c < 4; c++) {
        unsigned char s0 = state[4*c+0], s1 = state[4*c+1], s2 = state[4*c+2], s3 = state[4*c+3];
        res[4*c+0] = gmul(0x02, s0) ^ gmul(0x03, s1) ^ s2 ^ s3;
        res[4*c+1] = s0 ^ gmul(0x02, s1) ^ gmul(0x03, s2) ^ s3;
        res[4*c+2] = s0 ^ s1 ^ gmul(0x02, s2) ^ gmul(0x03, s3);
        res[4*c+3] = gmul(0x03, s0) ^ s1 ^ s2 ^ gmul(0x02, s3);
    }
    return res;
}

int main() {
    string M_str = Message;
    string K_str = Key;

    vector<unsigned char> M = hexToBytes(M_str);
    vector<unsigned char> K = hexToBytes(K_str);
    
    // Sinh khóa
    vector<vector<unsigned char>> w = KeyExpansion(K);

    cout << "\n--- PHAN 2: MA HOA ---" << endl;
    cout << "Message: " << M_str << endl;
    cout << "Key: " << K_str << endl;

    // Arrange state (column-major order)
    vector<unsigned char> state(16);
    for(int c=0; c<4; c++){
        for(int r=0; r<4; r++){
            state[r + 4*c] = M[4*c + r];
        }
    }

    // AddRoundKey ban đầu
    state = getAddRoundKey(state, w, 0);
    printStateRow("AddRoundKey(0) State", state);

    // 9 Vong lap
    for (int round = 1; round <= 9; round++) {
        cout << "\n=== VONG LAP THU " << round << " ===" << endl;
        state = SubBytes(state);
        printStateRow("Sau SubBytes", state);
        
        state = ShiftRows(state);
        printStateRow("Sau ShiftRows", state);
        
        state = MixColumns(state);
        printStateRow("Sau MixColumns", state);
        
        state = getAddRoundKey(state, w, round);
        printStateRow("Sau AddRoundKey", state);
    }

    // Vong 10 lap
    cout << "\n=== VONG LAP THU 10 ===" << endl;
    state = SubBytes(state);
    printStateRow("Sau SubBytes", state);
    
    state = ShiftRows(state);
    printStateRow("Sau ShiftRows", state);
    
    state = getAddRoundKey(state, w, 10);
    printStateRow("Sau AddRoundKey (Ciphertext)", state);

    return 0;
}
