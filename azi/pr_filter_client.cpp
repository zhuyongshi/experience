#include "pr_filter_client.h"

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
    std::string word = w1 + "∩" + w2;
    encrypt(mskp, word, token);
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
        // c0..cs = Pr.Enc(Kv, kt, kxor, a, b, v1...vs, len, s)
        std::vector<std::string> c(n);
        std::vector<std::string> key{res.mk.kv, res.mk.kt, res.mk.kxor};
        std::vector<std::string> w{a, b};
        int len = 0;
        if (s > 0)
        {
            len = mm.second[0].size();
        }
        if (Pr_Enc(key, w, mm.second, len, c) != 0)
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
        std::vector<std::vector<int>> CK(2);
        std::vector<std::vector<int>> P2(2);
        std::vector<std::string> KeyPhi(2);
        if (Pr_ReGen(input_key, input_w, param.len, param.s, CK, P2, KeyPhi) != 0)
        {
            std::cout << "[PR_Filter_Token] call Pr_ReGen failed" << std::endl;
            return -1;
        }
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

int PR_Filter_Resolve(pr_filter_resolve_param param, std::vector<std::string> &res)
{
    if (param.c.size() != param.vaild.size() + 1)
    {
        std::cout << "[PR_Filter_Resolve] vaild size wrong: " << param.vaild.size() << std::endl;
        return -1;
    }
    // v1..vs=PR.Dec(kv, kt, kxor, w1, wn, c0..cs)
    int n = param.c.size(), s = n - 1, len = 0;
    if (n > 0)
    {
        len = param.c[0].size();
    }
    std::vector<std::string> v(s);
    std::vector<std::string> key{param.mk.kv, param.mk.kt, param.mk.kxor};
    std::vector<std::string> w{param.w1, param.wn};
    if (Pr_Dec(key, w, param.c, len, v) != 0)
    {
        std::cout << "[PR_Filter_Resolve] call Pr_Dec failed" << std::endl;
        return -1;
    }
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