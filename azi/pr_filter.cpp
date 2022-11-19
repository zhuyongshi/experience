#include "pr_filter.h"
using namespace CryptoPP;

string H1(const string message, string key)
{
    byte buf[SHA256::DIGESTSIZE];
    string salt = key;
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}

string H(const string message)
{
    byte buf[SHA256::DIGESTSIZE];
    string salt = "01";
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}

int bytesToInt(byte *bytes, int size = 4)
{
    int addr = bytes[0] & 0xFF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    addr |= ((bytes[2] << 16) & 0xFF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

int hash_k_int(const string message, const string key)
{
    byte buf[SHA256::DIGESTSIZE];
    string salt = key;
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return bytesToInt(buf);
}

string toBinary(int n)
{
    string r;
    while (n != 0)
    {
        r += (n % 2 == 0 ? "0" : "1");
        n /= 2;
    }
    reverse(r.begin(), r.end());
    return r;
}

string padding(string s, int len)
{
    string r;
    if (s.size() < len)
    {
        int count = len - s.size();
        while (count)
        {
            r += "0";
            count--;
        }
        r += s;
    }
    else
    {
        r = s.substr(0, len);
    }
    return r;
}

int gen_key(byte *key1)
{
    //产生一个随机数密钥串，长度为16字节
    AutoSeededRandomPool rand;
    SecByteBlock Key(0x00, AES::DEFAULT_KEYLENGTH);
    rand.GenerateBlock(Key, Key.size());
    key1 = Key;
    return 1;
}

string Xor(const string s1, const string s2)
{
    string result = s1;
    if (s1.length() != s2.length())
    {
        // ERROR
        cout << "not sufficient size: " << s1.length() << ", " << s2.length() << endl;
        return "";
    }
    for (int i = 0; i < result.length(); i++)
    {
        result[i] = result[i] ^ s2[i];
    }
    return result;
}

string Permutation(int n, vector<string> kep, string pin)
{
    string pout;
    for (size_t i = 0; i < n; i++)
    {
        pout += (pin[atoi(kep[i].c_str())]);
    }
    return pout;
}
int Permutation2(int n, std::vector<std::string> kep, std::vector<std::string> pin, std::vector<std::string> &pout)
{

    for (size_t i = 0; i < n; i++)
    {
        pout[i] = (pin[atoi(kep[i].c_str())]);
    }
    return 0;
}

string De_Permutation(int n, vector<string> kep, string dpin)
{
    char *dpout = new char(n);
    for (size_t i = 0; i < n; i++)
    {
        dpout[atoi(kep[i].c_str())] = dpin[i];
    }
    string ret = "";
    for (int i = 0; i < n; i++)
    {
        ret += dpout[i];
    }
    return ret;
}

vector<string> Find_CK(int n, vector<string> kpa, vector<string> kpb)
{
    map<string, int> ck_mpa;
    for (int i = 0; i < n; i++)
    {
        ck_mpa.insert(pair<string, int>(kpa[i], i));
    }
    vector<string> ret(n);
    for (int i = 0; i < n; i++)
    {
        ret[i] = to_string(ck_mpa[kpb[i]]);
    }
    return ret;
}

int Permutationkey_Gen(string key, int n, vector<string> &ret)
{
    vector<int> A(n, 0);
    unsigned int tmp, tempo;
    for (int i = 0; i < n; i++)
    {
        A[i] = i;
    }

    for (int j = 0; j < n; j++)
    {
        string iTostr = toBinary(j);
        tmp = hash_k_int(iTostr, key);

        tmp = tmp % n;

        tempo = A[tmp];
        A[tmp] = A[j];
        A[j] = tempo;
    }

    for (int i = 0; i < n; i++)
    {
        ret[i] = std::to_string(A[i]);
    }
    return 0;
}

void ANOTH(int ctr, int len, vector<string> m, vector<string> &mplus)
{
    int n = m.size(); // id个数
    vector<string> x(n);
    string allX = "";

    string keyp = Gen_RandKey(len);

    // x[i] = m[i] ^ Fk'(ctr+i)
    for (int i = 0; i < n; i++)
    {
        string ctrPlus = std::to_string(ctr + i + 1);
        string digest = H1(ctrPlus, keyp).substr(0, m[i].size());
        x[i] = Xor(m[i], digest);
        allX += x[i];
    }

    // m'[n+1] = K' ^ H(x[1]...x[n])
    string hash_allx = H(allX).substr(0, keyp.size());
    mplus[n] = Xor(keyp, hash_allx);

    // m'[i] = x[i] ^ H(m'[n+1] ^ (ctr+i))
    for (int i = 0; i < n; i++)
    {
        string bina_ctr = toBinary(ctr + i + 1);
        string pad_ctr = padding(bina_ctr, mplus[n].size());
        string xor_param = H(Xor(mplus[n], pad_ctr)).substr(0, x[i].size());
        mplus[i] = Xor(x[i], xor_param);
    }
    return;
}

void D_AONTH(int ctr, vector<string> mplus, vector<string> &m)
{
    int n = m.size();
    vector<string> x(n);
    string allX = "";
    string keyp;

    // x[i] = m'[i] ^ H(m'[n+1] ^ (ctr+i))
    for (int i = 0; i < n; i++)
    {
        string bina_ctr = toBinary(ctr + i + 1);
        string pad_ctr = padding(bina_ctr, mplus[n].size());
        string xor_param = H(Xor(mplus[n], pad_ctr)).substr(0, mplus[i].size()); //
        x[i] = Xor(mplus[i], xor_param);
        allX += x[i];
    }

    // K' = m'[n+1] ^ H(x[1]...x[n])
    string hash_allx = H(allX).substr(0, mplus[n].size());
    keyp = Xor(mplus[n], hash_allx);

    // m[i] = x[i] ^ Fk'(ctr+i)
    for (int i = 0; i < n; i++)
    {
        string ctrPlus = std::to_string(ctr + i + 1);
        string digest = H1(ctrPlus, keyp).substr(0, x[i].size());
        m[i] = Xor(x[i], digest);
    }
    return;
}

string Gen_RandKey(int len)
{
    srand(time(NULL));
    string key = "";
    int rn;
    while (len != 0)
    {
        rn = rand();
        while (rn != 0)
        {
            if (len == 0)
                break;
            key += std::to_string((rn % 10) % 2);
            rn /= 10;
            --len;
        }
    }
    return key;
}

int Pr_Gen(vector<string> key, vector<string> w, int len, int n,
           vector<string> &P1, vector<string> &P2, vector<string> &P3, string &retkey)
{
    if (key.size() != 3 || w.size() != 2)
    {
        cout << "wrong input, the lenght of key should be 3 and w should be 2" << endl;
        return -1;
    }
    string keyfai1 = H1(key[0], w[0]);
    string keyfai2 = H1(key[0], w[1]);
    string key3 = H1(key[1], w[0] + w[1]);
    retkey = H1(key[2], w[0] + w[1]);

    Permutationkey_Gen(keyfai1, len, P1);
    Permutationkey_Gen(keyfai2, len, P2);
    Permutationkey_Gen(key3, len, P3);
    return 0;
}

int Pr_Enc(vector<string> key, vector<string> w, vector<string> m, int ctr, int len,
           vector<string> &c)
{
    if (m.size() + 1 != c.size())
    {
        cout << "wrong input, len(m) + 1 = len(c) " << endl;
        return -1;
    }
    int s = m.size(), n = s + 1;
    // call Pr-Gen
    vector<string> P1(len);
    vector<string> P2(len);
    vector<string> P3(len);
    string keyfai;
    if (Pr_Gen(key, w, len, s, P1, P2, P3, keyfai) == -1)
    {
        cout << "[Pr_Enc] run Pr_Gen err" << endl;
        return -1;
    }
    // m1'...mn'=AONTH(ctr, len, m1...mn)
    vector<string> mplus(n);
    ANOTH(ctr, len, m, mplus);
    // m1''...mn''= Perm(P3,m1'...ms')
    vector<string> mplusplus(s);
    for (int i = 0; i < s; i++)
    {
        mplusplus[i] = Permutation(P3.size(), P3, mplus[i]);
    }
    // co = Perm(P1, mn') ^ Perm(P2, Kfai)
    c[0] = Xor(Permutation(P1.size(), P1, mplus[n - 1]), Permutation(P2.size(), P2, keyfai));
    // for i = 1 to s do
    // ci = Perm(P1, mi'') ^ Perm(P2, ci-1)
    for (int i = 1; i < n; i++)
    {
        c[i] = Xor(Permutation(P1.size(), P1, mplusplus[i - 1]), Permutation(P2.size(), P2, c[i - 1]));
    }
    return 0;
}

int Pr_Dec(vector<string> key, vector<string> w, vector<string> c, int ctr, int len,
           vector<string> &m)
{
    if (m.size() + 1 != c.size())
    {
        cout << "wrong input, len(m) + 1 = len(c) " << endl;
        return -1;
    }
    int n = c.size(), s = n - 1;
    // call Pr-Gen
    vector<string> P1(len);
    vector<string> P2(len);
    vector<string> P3(len);
    string keyfai;
    if (Pr_Gen(key, w, len, s, P1, P2, P3, keyfai) == -1)
    {
        cout << "[Pr_Dec] run Pr_Gen err" << endl;
        return -1;
    }
    // mi''=DePerm(P1, ci ^ Perm(P2, ci-1))
    vector<string> mplus(n);
    vector<string> mplusplus(s);
    for (int i = s; i > 0; i--)
    {
        mplusplus[i - 1] = De_Permutation(P1.size(), P1, Xor(c[i], Permutation(P2.size(), P2, c[i - 1])));
    }
    // mn' = DePerm(P1, c0 ^ Perm(P2, keypai))
    mplus[n - 1] = De_Permutation(P1.size(), P1, Xor(c[0], Permutation(P2.size(), P2, keyfai)));
    // m1'...mn'= DePerm(P3,m1''...ms'')
    for (int i = 0; i < s; i++)
    {
        mplus[i] = De_Permutation(P3.size(), P3, mplusplus[i]);
    }
    // m1..ms=D-AONTH(ctr, m1'...mn')
    D_AONTH(ctr, mplus, m);
    return 0;
}

int RG(vector<string> key, vector<string> w, int len, int s,
       vector<vector<string>> &RetCK, vector<vector<string>> &RetP2, vector<string> &RetKeyfai)
{
    if (key.size() != 3 || w.size() != 3)
    {
        cout << "wrong input, the lenght of key should be 3 and w should be 3" << endl;
        return -1;
    }
    if (RetCK.size() != 2 || RetP2.size() != 2 || RetKeyfai.size() != 2)
    {
        cout << "wrong output, the lenght of P2 should be 2 and keypai should be 2 " << endl;
        return -1;
    }
    vector<string> P1(len);
    vector<string> P2(len);
    vector<string> P3(len);
    string keyfai;
    vector<string> w_input(2);
    w_input[0] = w[0];
    w_input[1] = w[1];
    // call Pr-Gen
    if (Pr_Gen(key, w_input, len, s, P1, P2, P3, keyfai) == -1)
    {
        cout << "[RG] run Pr_Gen err" << endl;
        return -1;
    }

    vector<string> P1plus(len);
    vector<string> P2plus(len);
    vector<string> P3plus(len);
    string keyfaiplus;
    w_input[1] = w[2];
    // call Pr-Gen
    if (Pr_Gen(key, w_input, len, s, P1plus, P2plus, P3plus, keyfai) == -1)
    {
        cout << "[RG] run Pr_Gen err" << endl;
        return -1;
    }

    RetCK[0] = Find_CK(len, P1, P1plus); // CK1=FindCk(P1, P1')
    RetCK[1] = Find_CK(len, P3, P3plus); // CK3=FindCk(P3, P3')
    RetP2[0] = P2;                       // P2
    RetP2[1] = P2plus;                   // P2'
    RetKeyfai[0] = keyfai;               // keyfai
    RetKeyfai[1] = keyfaiplus;           // keyfai'
    return 0;
}

int RE(vector<vector<string>> CK, vector<vector<string>> P2, vector<string> Keyfai, vector<string> c,
       vector<string> &ret_c)
{
    if (CK.size() != 2 || P2.size() != 2 || Keyfai.size() != 2)
    {
        cout << "wrong output, the lenght of CK should be 2, P2 should be 2, keypai should be 2 " << endl;
        return -1;
    }
    if (c.size() != ret_c.size())
    {
        cout << "the length of c and ret_c should be equal" << endl;
        return -1;
    }
    int n = c.size(), s = n - 1;
    vector<string> cplus(n);
    vector<string> cplusplus(n);
    // ci'=Perm( CK1, ci ^ Perm(P2, ci-1))
    for (int i = s; i > 0; i--)
    {
        cplus[i] = Permutation(CK[0].size(), CK[0], Xor(c[i], Permutation(P2[0].size(), P2[0], c[i - 1])));
    }
    // c1''..cs''= Perm(CK3, c1'...cs')
    for (int i = 1; i <= s; i++)
    {
        cplusplus[i] = Permutation(CK[1].size(), CK[1], cplus[i]);
    }
    // c0'' = Perm(CK1, c0 ^ Perm(P2, keypai))
    cplusplus[0] = Permutation(CK[0].size(), CK[0], Xor(c[0], Permutation(P2[0].size(), P2[0], Keyfai[0])));
    // ret_c0 = c0'' ^ Perm(P2', keypai')
    ret_c[0] = Xor(cplusplus[0], Permutation(P2[1].size(), P2[1], Keyfai[1]));
    // ret_ci = ci'' ^ Perm(P2', ci-1'')
    for (int i = 1; i < n; i++)
    {
        ret_c[i] = Xor(cplusplus[i], Permutation(P2[1].size(), P2[1], cplusplus[i - 1]));
    }
    return 0;
}