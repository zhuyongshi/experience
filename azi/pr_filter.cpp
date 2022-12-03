#include "pr_filter.h"

int find_w12_from_w(std::string word, std::string &w1, std::string &w2)
{
    if (word == "")
    {
        w1 = "";
        w2 = "";
        return 0;
    }
    int idx = word.find("∩");
    if (idx == -1)
    {
        std::cout << "substr without ∩, wrong msg: " << word << std::endl;
        return -1;
    }
    w1 = word.substr(0, idx);
    w2 = word.substr(idx + 3, word.size() - idx);
    return 0;
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
        dpin[i + start] = (pin[start + kep[i]]);
    }
    return 0;
}

// 排列加密算法: 将pin按kep顺序重排回原列
int De_Permutation2(int start, std::vector<int> kep, std::vector<std::string> dpin, std::vector<std::string> &pin)
{
    for (size_t i = 0; i < kep.size(); i++)
    {
        pin[kep[i] + start] = dpin[i + start];
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
    if (Permutation2(1, CK[1], cplus, cplusplus))
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

void sEMM_Setup(int lambda, std::map<std::string, std::vector<std::string>> MMp,
                std::string &mskp, std::map<std::string, std::vector<std::string>> &EMMp)
{
    mskp = Gen_RandKey(lambda);
    for (auto mmp : MMp)
    {
        std::string key_cipher;
        encrypt(mskp, mmp.first, key_cipher);
        EMMp[key_cipher] = mmp.second;
    }
}

void sEMM_Token(std::string mskp, std::string w1, std::string w2, std::string &token)
{
    encrypt(mskp, w1 + "∩" + w2, token);
}

void sEMM_Search(std::string tokp, std::map<std::string, std::vector<std::string>> EMMp, std::vector<std::string> &tags)
{
    tags = EMMp[tokp];
}

int PR_Filter_Setup(pr_filter_setup_param param, pr_filter_setup_res &res)
{
    srand(time(NULL));
    // generate random key
    res.mk.kv = Gen_RandKey(param.lambda);
    res.mk.kt = Gen_RandKey(param.lambda);
    res.mk.kp = Gen_RandKey(param.lambda);
    res.mk.kx = Gen_RandKey(param.lambda);
    res.mk.kxor = Gen_RandKey(param.mu);
    // init mmp and dx
    std::map<std::string, std::vector<std::string>> MMp;
    for (auto mm : param.MM)
    {
        int s = mm.second.size(), n = s + 1;
        // get a b
        std::string a, b;
        if (find_w12_from_w(mm.first, a, b) != 0)
        {
            std::cout << "[PR_Filter_Setup] call find_w12_from_w err: " << mm.first << std::endl;
            return -1;
        }
        // keyab_enc=F(Kp, a||b)
        std::string key_ab_enc = H1(res.mk.kp, a + b);
        // DX[(a,b)]<-ctr
        unsigned int ctr = rand() % 1000000;
        res.DX.insert(std::pair<std::string, int>{mm.first, ctr});
        // c0..cs = Pr.Enc(Kv, kt, kxor, a, b, ctr, v1...vs, len, s)
        std::vector<std::string> c(n);
        std::vector<std::string> key{res.mk.kv, res.mk.kt, res.mk.kxor};
        std::vector<std::string> w{a, b};
        int len = 0;
        if (s > 0)
        {
            len = mm.second[0].size();
        }
        if (Pr_Enc(key, w, mm.second, ctr, len, c) != 0)
        {
            std::cout << "[PR_Filter_Setup] call Pr_Enc failed" << std::endl;
            return -1;
        }
        // encrypt tag(a,b,v)=Aes_Enc(keyab_enc, ci)
        std::vector<std::string> tag(n);
        for (int i = 0; i < n; i++)
        {
            encrypt(key_ab_enc, c[i], tag[i]);
        }
        MMp[mm.first] = tag;

        // keyab_x=F(Kp, a||b)
        std::string key_ab_x = H1(res.mk.kx, a + b);
        // tag <- xset
        for (int i = 0; i < n; i++)
        {
            res.emm.Xset.insert(H1(key_ab_x, c[i]));
        }
    }
    // mskp, EMMp = sEMM_Setup(1_lamda, MMp)
    sEMM_Setup(param.lambda, MMp, res.mk.mskp, res.emm.EMMp);
    return 0;
}

int PR_Filter_Token(pr_filter_token_param param, pr_filter_token_res &res)
{
    if (param.words.size() < 2)
    {
        std::cout << "[PR_Filter_Token] words size too little err: " << param.words.size() << std::endl;
        return -1;
    }
    // tokp1=sEMM.Token(mskp, (w1, w2))
    sEMM_Token(param.mk.mskp, param.words[0], param.words[1], res.tokp);
    // k_w12_enc = hash(kp, w1+w2)
    res.k_w12_enc = H1(param.mk.kp, param.words[0] + param.words[1]);
    // n = word.size()
    int n = param.words.size();
    for (int i = 2; i < n; i++)
    {
        // kred = PR.ReGen(kv, kt, kxor, w1, wd-1, wd, len, s)
        std::vector<std::string> input_key{param.mk.kv, param.mk.kt, param.mk.kxor};
        std::vector<std::string> input_w{param.words[0], param.words[i - 1], param.words[i]};
        std::vector<std::vector<int>> CK;
        std::vector<std::vector<int>> P2;
        std::vector<std::string> KeyPhi;
        Pr_ReGen(input_key, input_w, param.len, param.s, CK, P2, KeyPhi);
        // split
        key_re_d k_re_d;
        k_re_d.CK = CK;
        k_re_d.P2 = P2;
        k_re_d.KeyPhi = KeyPhi;
        // kxi = hash(kx, w1+wd)
        k_re_d.kx = H1(param.mk.kx, param.words[0] + param.words[i]);
        res.tokp_vec.push_back(k_re_d);
    }
    // done
    return 0;
}

int PR_Filter_Search(pr_filter_search_param param, pr_filter_search_res &res)
{
    // tagl = sEMM.Search(tokp, EMMp)
    std::vector<std::string> tags_l;
    sEMM_Search(param.tokq.tokp, param.emm.EMMp, tags_l);
    // ci = Dec(k_w1_w2_enc, tagl)
    std::vector<std::string> c(tags_l.size());
    for (int i = 0; i < tags_l.size(); i++)
    {
        descrypt(param.tokq.k_w12_enc, tags_l[i], c[i]);
    }
    // optimization
    // for l=1..L
    // for d=3..nl
    // cl = PR.ReEnc(k_re_d, cl)
    // dtage(l,d) = F(kx, cl')
    int n = c.size(), s = n - 1;
    std::vector<bool> vaild(s, true);
    std::string dtag;
    for (int i = 0; i < param.tokq.tokp_vec.size(); i++)
    {
        std::vector<std::string> cplus(c.size());
        Pr_ReEnc(param.tokq.tokp_vec[i].CK, param.tokq.tokp_vec[i].P2, param.tokq.tokp_vec[i].KeyPhi, c, cplus);
        for (int j = 0; j < c.size(); j++)
        {
            c[j] = cplus[j];
        }
        for (int j = 1; j < n; j++)
        {
            if (!vaild[j - 1])
                continue;
            dtag = H1(param.tokq.tokp_vec[i].kx, cplus[j]);
            if (param.emm.Xset.find(dtag) == param.emm.Xset.end())
            {
                vaild[j - 1] = false;
            }
        }
    }
    // done
    return 0;
}

int PR_Filter_Resolve(pr_filter_resolve_param param, std::vector<std::string> &res)
{
    if (param.c.size() != param.vaild.size() + 1)
    {
        std::cout << "[PR_Filter_Resolve] vaild size wrong: " << param.vaild.size() << std::endl;
        return -1;
    }
    // ctr=Dx[(w1, wn)]
    int ctr = param.DX[param.w1 + "∩" + param.wn];
    // v1..vs=PR.Dec(kv, kt, kxor, w1, wn, ctr, c0..cs)
    int n = param.c.size(), s = n - 1, len = 0;
    if (n > 0)
    {
        len = param.c[0].size();
    }
    std::vector<std::string> v(s);
    std::vector<std::string> key{param.mk.kv, param.mk.kt, param.mk.kxor};
    std::vector<std::string> w{param.w1, param.wn};
    Pr_Dec(key, w, param.c, ctr, len, v);
    // get vaild value
    for (int i = 0; i < s; i++)
    {
        if (param.vaild[i])
        {
            res.push_back(v[i]);
        }
    }
    // done
    return 0;
}