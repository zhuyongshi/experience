#include "pr_filter.h"

// hash函数 带盐值
std::string H1(const std::string message, std::string key)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = key;
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}

// hash函数，不带盐值
std::string H(const std::string message)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = "01";
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return std::string((const char *)buf, (size_t)SHA256::DIGESTSIZE);
}

// hash函数，输出Int
int hash_k_int(const std::string message, const std::string key)
{
    byte buf[SHA256::DIGESTSIZE];
    std::string salt = key;
    SHA256().CalculateDigest(buf, (byte *)((message + salt).c_str()), message.length() + salt.length());
    return bytesToInt(buf, 4);
}

// byte转Int
int bytesToInt(byte *bytes, int size = 4)
{
    int addr = bytes[0] & 0xFF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    addr |= ((bytes[2] << 16) & 0xFF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

// Int转byte
byte *IntToBytes(int num)
{
    byte *ans=new byte[4];
    ans[0] = (byte)(num);
    ans[1] = (byte)(num >> 8);
    ans[2] = (byte)(num >> 16);
    ans[3] = (byte)(num >> 24);
    return ans;
}

// 整数转二进制
std::string toBinary(int n)
{
    std::string r;
    while (n != 0)
    {
        r += (n % 2 == 0 ? "0" : "1");
        n /= 2;
    }
    reverse(r.begin(), r.end());
    return r;
}

// 填充算法: 后面填0
std::string padding(std::string s, int len)
{
    std::string r;
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

// 产生随机数密钥串，类型为byte
int gen_key(byte *key1)
{
    //产生一个随机数密钥串，长度为16字节
    AutoSeededRandomPool rand;
    SecByteBlock Key(0x00, AES::DEFAULT_KEYLENGTH);
    rand.GenerateBlock(Key, Key.size());
    key1 = Key;
    return 1;
}

// 产生随机数密钥串，类型为string
std::string Gen_RandKey(int len)
{
    srand(time(NULL));
    std::string key = "";
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

// string 异或
std::string Xor(const std::string s1, const std::string s2)
{
    if (s1.length() != s2.length())
    {
        std::cout << "[Xor] not sufficient size: " << s1.length() << ", " << s2.length() << std::endl;
        return "";
    }
    std::string ans = s1;
    for (int i = 0; i < ans.length(); i++)
    {
        ans[i] = ans[i] ^ s2[i];
    }
    return ans;
}

// 排列加密算法: 将pin按kep顺序排列，pin为string
std::string Permutation(int n, std::vector<int> kep, std::string pin)
{
    std::string pout = "";
    for (size_t i = 0; i < n; i++)
    {
        pout += (pin[kep[i]]);
    }
    return pout;
}

// 排列加密算法: 将pin按kep顺序重排回原列
std::string De_Permutation(int n, std::vector<int> kep, std::string dpin)
{
    char *dpout = new char(n);
    for (size_t i = 0; i < n; i++)
    {
        dpout[kep[i]] = dpin[i];
    }
    std::string ret = "";
    for (int i = 0; i < n; i++)
    {
        ret += dpout[i];
    }
    return ret;
}

// 排列加密算法：将pin按kep顺序排列，pin为string数组，start为开始位置
int Permutation2(int start, std::vector<int> kep, std::vector<std::string> pin, std::vector<std::string> &dpin)
{
    for (size_t i = 0; i < kep.size(); i++)
    {
        dpin[i+start] = (pin[start+kep[i]]);
    }
    return 0;
}

// 排列加密算法: 将pin按kep顺序重排回原列
int De_Permutation2(int start, std::vector<int> kep, std::vector<std::string> dpin, std::vector<std::string> &pin)
{
    for (size_t i = 0; i < kep.size(); i++)
    {
        pin[kep[i]+start] = dpin[i+start];
    }
    return 0;
}

// 根据kpa和kpb调整顺序
std::vector<int> Find_CK(int n, std::vector<int> kpa, std::vector<int> kpb)
{
    std::map<int, int> ck_mpa;
    for (int i = 0; i < n; i++)
    {
        ck_mpa.insert(std::pair<int, int>(kpa[i], i));
    }
    std::vector<int> ret(n);
    for (int i = 0; i < n; i++)
    {
        ret[i] = ck_mpa[kpb[i]];
    }
    return ret;
}

// 密钥扩展函数，将密钥从key长度扩展到n长度
int Permutationkey_Gen(std::string key, int n, std::vector<int> &ret)
{
    if (ret.size() != n)
    {
        std::cout << "[Permutationkey_Gen] ret size is not n: " << ret.size() << std::endl;
        return -1;
    }
    for (int i = 0; i < n; i++)
    {
        ret[i] = i;
    }
    unsigned int tmp, tempo;
    for (int i = 0; i < n; i++)
    {
        std::string iTostr = toBinary(i);
        tmp = hash_k_int(iTostr, key);
        tmp = tmp % n;
        tempo = ret[tmp];
        ret[tmp] = ret[i];
        ret[i] = tempo;
    }
    return 0;
}

int ANOTH(int ctr, int len, std::vector<std::string> m, std::vector<std::string> &mplus)
{
    if (m.size() + 1 != mplus.size())
    {
        std::cout << "[ANOTH] not sufficient size: " << m.size() << ", " << mplus.size() << std::endl;
        return -1;
    }
    int n = m.size(); // id个数
    std::vector<std::string> x(n);
    std::string allX = "";

    std::string keyp = Gen_RandKey(len);

    // x[i] = m[i] ^ Fk'(ctr+i)
    for (int i = 0; i < n; i++)
    {
        std::string ctr_plus = std::to_string(ctr + i + 1);
        std::string digest = H1(ctr_plus, keyp).substr(0, m[i].size());
        x[i] = Xor(m[i], digest);
        allX += x[i];
    }

    // m'[n+1] = K' ^ H(x[1]...x[n])
    std::string hash_allx = H(allX).substr(0, keyp.size());
    mplus[n] = Xor(keyp, hash_allx);

    // m'[i] = x[i] ^ H(m'[n+1] ^ (ctr+i))
    for (int i = 0; i < n; i++)
    {
        std::string bina_ctr = toBinary(ctr + i + 1);
        std::string pad_ctr = padding(bina_ctr, mplus[n].size());
        std::string xor_param = H(Xor(mplus[n], pad_ctr)).substr(0, x[i].size());
        mplus[i] = Xor(x[i], xor_param);
    }
    return 0;
}

int D_AONTH(int ctr, std::vector<std::string> mplus, std::vector<std::string> &m)
{
    if (m.size() + 1 != mplus.size())
    {
        std::cout << "[D_AONTH] not sufficient size: " << m.size() << ", " << mplus.size() << std::endl;
        return -1;
    }
    int n = m.size();
    std::vector<std::string> x(n);
    std::string allX = "";
    std::string keyp;

    // x[i] = m'[i] ^ H(m'[n+1] ^ (ctr+i))
    for (int i = 0; i < n; i++)
    {
        std::string bina_ctr = toBinary(ctr + i + 1);
        std::string pad_ctr = padding(bina_ctr, mplus[n].size());
        std::string xor_param = H(Xor(mplus[n], pad_ctr)).substr(0, mplus[i].size()); //
        x[i] = Xor(mplus[i], xor_param);
        allX += x[i];
    }

    // K' = m'[n+1] ^ H(x[1]...x[n])
    std::string hash_allx = H(allX).substr(0, mplus[n].size());
    keyp = Xor(mplus[n], hash_allx);

    // m[i] = x[i] ^ Fk'(ctr+i)
    for (int i = 0; i < n; i++)
    {
        std::string ctr_plus = std::to_string(ctr + i + 1);
        std::string digest = H1(ctr_plus, keyp).substr(0, x[i].size());
        m[i] = Xor(x[i], digest);
    }
    return 0;
}

int Pr_Gen(std::vector<std::string> key, std::vector<std::string> w, int len, int n,
           std::vector<int> &P1, std::vector<int> &P2, std::vector<int> &P3, std::string &keyphi)
{
    if (key.size() != 3 || w.size() != 2)
    {
        std::cout << "[Pr_Gen] wrong input, the lenght of key should be 3 and w should be 2" << std::endl;
        return -1;
    }
    if (P1.size() != len || P2.size() != len || P3.size() != n)
    {
        std::cout << "[Pr_Gen] the lenght of P1 and P2 is not len, and P3 is not n" << std::endl;
        return -1;
    }
    std::string keyw1 = H1(key[0], w[0]);
    std::string keyw2 = H1(key[0], w[1]);
    std::string key3 = H1(key[1], w[0] + w[1]);
    keyphi = H1(key[2], w[0] + w[1]);

    Permutationkey_Gen(keyw1, len, P1);
    Permutationkey_Gen(keyw2, len, P2);
    Permutationkey_Gen(key3, n, P3);
    return 0;
}

int Pr_Enc(std::vector<std::string> key, std::vector<std::string> w, std::vector<std::string> m, int ctr, int len,
           std::vector<std::string> &c)
{
    if (m.size() + 1 != c.size())
    {
        std::cout << "[Pr_Enc] wrong input, len(m) + 1 = len(c) " << std::endl;
        return -1;
    }
    int s = m.size(), n = s + 1;
    // call Pr-Gen
    std::vector<int> P1(len);
    std::vector<int> P2(len);
    std::vector<int> P3(s);
    std::string keyphi;
    if (Pr_Gen(key, w, len, s, P1, P2, P3, keyphi) != 0)
    {
        std::cout << "[Pr_Enc] run Pr_Gen err" << std::endl;
        return -1;
    }
    // m1'...mn'=AONTH(ctr, len, m1...mn)
    std::vector<std::string> mplus(n);
    ANOTH(ctr, len, m, mplus);
    // m1''...mn''= Perm(P3, m1'...ms')
    std::vector<std::string> mplusplus(s);
    if (Permutation2(0, P3, mplus, mplusplus) != 0)
    {
        std::cout << "[Pr_Enc] run Permutation2 err" << std::endl;
        return -1;
    }
    // co = Perm(P1, mn') ^ Perm(P2, Kfai)
    c[0] = Xor(Permutation(P1.size(), P1, mplus[n - 1]), Permutation(P2.size(), P2, keyphi));
    // for i = 1 to s do
    // ci = Perm(P1, mi'') ^ Perm(P2, ci-1)
    for (int i = 1; i < n; i++)
    {
        c[i] = Xor(Permutation(P1.size(), P1, mplusplus[i - 1]), Permutation(P2.size(), P2, c[i - 1]));
    }
    return 0;
}

int Pr_Dec(std::vector<std::string> key, std::vector<std::string> w, std::vector<std::string> c, int ctr, int len,
           std::vector<std::string> &m)
{
    if (m.size() + 1 != c.size())
    {
        std::cout << "[Pr_Dec] wrong input, len(m) + 1 = len(c) " << std::endl;
        return -1;
    }
    int n = c.size(), s = n - 1;
    // call Pr-Gen
    std::vector<int> P1(len);
    std::vector<int> P2(len);
    std::vector<int> P3(s);
    std::string keyphi;
    if (Pr_Gen(key, w, len, s, P1, P2, P3, keyphi) == -1)
    {
        std::cout << "[Pr_Dec] run Pr_Gen err" << std::endl;
        return -1;
    }
    // mi''=DePerm(P1, ci ^ Perm(P2, ci-1))
    std::vector<std::string> mplus(n);
    std::vector<std::string> mplusplus(s);
    for (int i = s; i > 0; i--)
    {
        mplusplus[i - 1] = De_Permutation(P1.size(), P1, Xor(c[i], Permutation(P2.size(), P2, c[i - 1])));
    }
    // mn' = DePerm(P1, c0 ^ Perm(P2, keypai))
    mplus[n - 1] = De_Permutation(P1.size(), P1, Xor(c[0], Permutation(P2.size(), P2, keyphi)));
    // m1'...mn'= DePerm(P3,m1''...ms'')
    if (De_Permutation2(0, P3, mplusplus, mplus) != 0)
    {
        std::cout << "[Pr_Dec] run De_Permutation2 err" << std::endl;
        return -1;
    }
    // m1..ms=D-AONTH(ctr, m1'...mn')
    D_AONTH(ctr, mplus, m);
    return 0;
}

int Pr_ReGen(std::vector<std::string> key, std::vector<std::string> w, int len, int s,
             std::vector<std::vector<int>> &RetCK, std::vector<std::vector<int>> &RetP2, std::vector<std::string> &RetKeyPhi)
{
    if (key.size() != 3 || w.size() != 3)
    {
        std::cout << "[Pr_ReGen] wrong input, the lenght of key should be 3 and w should be 3" << std::endl;
        return -1;
    }
    if (RetCK.size() != 2 || RetP2.size() != 2 || RetKeyPhi.size() != 2)
    {
        std::cout << "[Pr_ReGen] wrong output, the lenght of P2 should be 2 and keypai should be 2 " << std::endl;
        return -1;
    }
    std::vector<int> P1(len);
    std::vector<int> P2(len);
    std::vector<int> P3(s);
    std::string keyphi;
    std::vector<std::string> w_input(2);
    w_input[0] = w[0];
    w_input[1] = w[1];
    // call Pr-Gen
    if (Pr_Gen(key, w_input, len, s, P1, P2, P3, keyphi) == -1)
    {
        std::cout << "[Pr_ReGen] run Pr_Gen err" << std::endl;
        return -1;
    }

    std::vector<int> P1plus(len);
    std::vector<int> P2plus(len);
    std::vector<int> P3plus(s);
    std::string keyphiplus;
    w_input[1] = w[2];
    // call Pr-Gen
    if (Pr_Gen(key, w_input, len, s, P1plus, P2plus, P3plus, keyphiplus) == -1)
    {
        std::cout << "[Pr_ReGen] run Pr_Gen err" << std::endl;
        return -1;
    }

    RetCK[0] = Find_CK(len, P1, P1plus); // CK1=FindCk(P1, P1')
    RetCK[1] = Find_CK(s, P3, P3plus);   // CK3=FindCk(P3, P3')
    RetP2[0] = P2;                       // P2
    RetP2[1] = P2plus;                   // P2'
    RetKeyPhi[0] = keyphi;               // keyfai
    RetKeyPhi[1] = keyphiplus;           // keyfai'
    return 0;
}

int Pr_ReEnc(std::vector<std::vector<int>> CK, std::vector<std::vector<int>> P2, std::vector<std::string> KeyPhi, std::vector<std::string> c,
             std::vector<std::string> &ret_c)
{
    if (CK.size() != 2 || P2.size() != 2 || KeyPhi.size() != 2)
    {
        std::cout << "[Pr_ReEnc] wrong output, the lenght of CK should be 2, P2 should be 2, KeyPhi should be 2 " << std::endl;
        return -1;
    }
    if (c.size() != ret_c.size())
    {
        std::cout << "[Pr_ReEnc] the length of c and ret_c should be equal" << std::endl;
        return -1;
    }
    int n = c.size(), s = n - 1;
    std::vector<std::string> cplus(n);
    std::vector<std::string> cplusplus(n);
    // ci'=Perm( CK1, ci ^ Perm(P2, ci-1))
    for (int i = s; i > 0; i--)
    {
        cplus[i] = Permutation(CK[0].size(), CK[0], Xor(c[i], Permutation(P2[0].size(), P2[0], c[i - 1])));
    }
    // c1''..cs''= Perm(CK3, c1'...cs')
    if(Permutation2(1, CK[1], cplus, cplusplus))
    {
        std::cout << "[Pr_ReEnc] run Permutation2 err" << std::endl;
        return -1;
    }
    // c0'' = Perm(CK1, c0 ^ Perm(P2, keypai))
    cplusplus[0] = Permutation(CK[0].size(), CK[0], Xor(c[0], Permutation(P2[0].size(), P2[0], KeyPhi[0])));
    // ret_c0 = c0'' ^ Perm(P2', keypai')
    ret_c[0] = Xor(cplusplus[0], Permutation(P2[1].size(), P2[1], KeyPhi[1]));
    // ret_ci = ci'' ^ Perm(P2', ci-1'')
    for (int i = 1; i < n; i++)
    {
        ret_c[i] = Xor(cplusplus[i], Permutation(P2[1].size(), P2[1], ret_c[i - 1]));
    }
    return 0;
}