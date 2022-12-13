#include "pr_filter_server.h"
#include "pr_filter_client.h"

void Test_GenKey()
{
    std::cout << Gen_RandKey(10) << std::endl;
}

// 测试截取word
void Test_SubWord()
{
    std::string str = "0001∩15444";
    std::string w1, w2;
    find_w12_from_w(str, w1, w2);
    std::cout << w1 << " " << w2 << std::endl;

    str = "000115444";
    find_w12_from_w(str, w1, w2);
}

// 测试permutation
void Test_Permutation()
{
    std::vector<int> kep{2, 3, 4, 0, 1};
    std::string pin = "abcde";
    std::string pout = Permutation(pin.size(), kep, pin);
    for (size_t i = 0; i < pin.size(); i++)
    {
        std::cout << pout[i] << " ";
    }
    std::cout << std::endl;
    std::string out = De_Permutation(pin.size(), kep, pout);
    for (size_t i = 0; i < pin.size(); i++)
    {
        std::cout << out[i] << " ";
    }
    std::cout << std::endl;
}

// 测试permutation2
void Test_Permutation2()
{
    std::vector<int> kep{4, 2, 1, 3, 0};
    std::vector<std::string> pin{"id1", "id2", "id3", "id4", "id5"};
    std::vector<std::string> dpin(kep.size());
    Permutation2(0, kep, pin, dpin);
    for (int i = 0; i < dpin.size(); i++)
    {
        std::cout << dpin[i] << " ";
    }
    std::cout << std::endl;

    De_Permutation2(0, kep, dpin, pin);
    for (int i = 0; i < pin.size(); i++)
    {
        std::cout << pin[i] << " ";
    }
    std::cout << std::endl;

    std::vector<int> kep2{2, 1, 3, 0};
    Permutation2(1, kep2, pin, dpin);
    for (int i = 0; i < dpin.size(); i++)
    {
        std::cout << dpin[i] << " ";
    }
    std::cout << std::endl;

    De_Permutation2(1, kep2, dpin, pin);
    for (int i = 0; i < pin.size(); i++)
    {
        std::cout << pin[i] << " ";
    }
    std::cout << std::endl;
}

// 测试 FindConversion Key
void Test_FindConversionKey()
{
    int size = 4;
    std::vector<int> kpa{3, 1, 2, 0};
    std::vector<int> kpb{2, 3, 1, 0};
    std::vector<int> expect{2, 0, 1, 3};
    std::vector<int> ret = Find_CK(size, kpa, kpb);
    for (size_t i = 0; i < ret.size(); i++)
    {
        std::cout << ret[i] << " ";
    }
    std::cout << std::endl;
}

// 测试PGen
void Test_PGen()
{
    std::string key = "yangziyi";
    int n = 20;
    std::vector<int> ans(n);
    Permutationkey_Gen(key, n, ans);
    for (size_t i = 0; i < ans.size(); i++)
    {
        std::cout << ans[i] << " ";
    }
    std::cout << std::endl;
}

// 测试Aonth和Daonth
void Test_AONTH_DAONTH()
{
    // 测试AONTH
    int ctr = 20;
    std::string m = "abc";
    std::string mplus;
    std::string dmplus;
    AONTH(ctr, m, mplus, dmplus);
    std::cout << "mplus: " << mplus << ", dmplus: " << dmplus << std::endl;

    // 测试D-AONTH
    std::string origin_m;
    D_AONTH(ctr, mplus, dmplus, origin_m);
    std::cout << "origin m: " << origin_m << std::endl;
}

// 测试PR_Enc和Pr_dec
void Test_Pr_ED()
{
    std::vector<std::string> key{"01010", "10101", "11100"};
    std::vector<std::string> w{"name", "age"};
    std::vector<std::string> m{"id1", "id2", "id3", "id4", "id5"};
    int s = m.size(), len = m[0].size(), ctr = 20;
    std::map<std::string, int> DX;
    std::map<std::string, int> ZX{
        {"id1", 1001},
        {"id2", 1},
        {"id3", 64132},
        {"id4", 2345},
        {"id5", 98},
    };

    std::vector<std::string> c(s);
    std::vector<std::string> dc(s);
    Pr_Enc(key, w, m, len, ZX, c, dc, DX);

    std::vector<std::string> mm(s);
    Pr_Dec(key, w, c, dc, len, DX, mm);
    std::cout << "Pr_Dec:" << std::endl;
    for (size_t i = 0; i < s; i++)
    {
        std::cout << mm[i] << " ";
    }
    std::cout << std::endl;
}

// 测试PR_Enc和Pr_dec，测试m数组为空情况
void Test_Pr_ED_NUL()
{
    std::vector<std::string> key{"01010", "10101", "11100"};
    std::vector<std::string> w{"name", "age"};
    std::vector<std::string> m;
    int s = m.size(), len = 0, ctr = 20;
    std::map<std::string, int> DX;
    std::map<std::string, int> ZX;

    std::vector<std::string> c(s);
    std::vector<std::string> dc(s);
    Pr_Enc(key, w, m, len, ZX, c, dc, DX);

    std::vector<std::string> mm(s);
    Pr_Dec(key, w, c, dc, len, DX, mm);
    std::cout << "Pr_Dec:" << std::endl;
    for (size_t i = 0; i < s; i++)
    {
        std::cout << mm[i] << " ";
    }
    std::cout << std::endl;
}

// 测试RG和RE
void Test_RG_RE()
{
    std::vector<std::string> key{"01010", "10101", "11100"};
    std::vector<std::string> w1{"name", "age"};
    std::vector<std::string> w2{"name", "sex"};
    std::vector<std::string> w3{"name", "age", "sex"};
    std::vector<std::string> m{"id1", "id2", "id3", "id4", "id5"};
    int len = m[0].size(), s = m.size(), ctr = 20;
    std::map<std::string, int> DX;
    std::map<std::string, int> ZX{
        {"id1", 1001},
        {"id2", 1},
        {"id3", 64132},
        {"id4", 2345},
        {"id5", 98},
    };

    std::vector<std::string> c(s);
    std::vector<std::string> dc(s);
    Pr_Enc(key, w1, m, len, ZX, c, dc, DX);

    std::vector<int> CK3;
    std::vector<std::vector<int>> P2(2);
    std::vector<std::string> KeyPhi(2);
    Pr_ReGen(key, w3, len, CK3, P2, KeyPhi);

    std::vector<std::string> cplus(s);
    std::vector<std::string> dcplus(s);
    Pr_ReEnc(CK3, P2, KeyPhi, c, dc, cplus, dcplus);

    // 重加密后的结果
    std::vector<std::string> mm(s);
    Pr_Dec(key, w2, cplus, dcplus, len, DX, mm);
    for (size_t i = 0; i < s; i++)
    {
        std::cout << mm[i] << " ";
    }
    std::cout << std::endl;

    return;
}

// 测试Pr-filter
void Test_Pr_filter()
{
    // setup
    // std::map<std::string, std::vector<std::string>> MM = {
    //     {"name∩age", std::vector<std::string>{"id1", "id2", "id4"}},
    //     {"name∩sex", std::vector<std::string>{"id2"}},
    //     {"name∩old", std::vector<std::string>{"id2"}},
    //     {"name∩address", std::vector<std::string>{"id4"}},
    //     {"age∩sex", std::vector<std::string>{"id2", "id3"}},
    //     {"age∩old", std::vector<std::string>{"id2", "id3"}},
    //     {"age∩address", std::vector<std::string>{"id3", "id4"}},
    //     {"sex∩old", std::vector<std::string>{"id2", "id3"}},
    //     {"sex∩address", std::vector<std::string>{"id3", "id4"}},
    //     {"old∩address", std::vector<std::string>{"id3"}},
    // };
    std::map<std::string, std::vector<std::string>> MM = {
        {"name∩age", std::vector<std::string>{"id1", "id2", "id3"}},
        {"name∩sex", std::vector<std::string>{"id1", "id2", "id3"}},
        {"age∩sex", std::vector<std::string>{"id1", "id2", "id3"}},
    };
    // std::map<std::string, std::vector<std::string>> MM = {
    //     {"name∩age", std::vector<std::string>{"id1id1id1id1id1id1id1id1id1id1id1", "id2id2id2id2id2id2id2id2id2id2id2", "id3id3id3id3id3id3id3id3id3id3id3"}},
    //     {"name∩sex", std::vector<std::string>{"id1id1id1id1id1id1id1id1id1id1id1", "id2id2id2id2id2id2id2id2id2id2id2", "id3id3id3id3id3id3id3id3id3id3id3"}},
    //     {"age∩sex", std::vector<std::string>{"id1id1id1id1id1id1id1id1id1id1id1", "id2id2id2id2id2id2id2id2id2id2id2", "id3id3id3id3id3id3id3id3id3id3id3"}},
    // };
    pr_filter_setup_param setup_param;
    setup_param.lambda = 5;
    setup_param.mu = 10;
    setup_param.MM = MM;
    pr_filter_setup_res setup_res;
    PR_Filter_Setup(setup_param, setup_res);

    // token
    pr_filter_token_param token_param;
    token_param.len = MM["name∩age"][0].size();
    token_param.mk = setup_res.mk;
    token_param.words = std::vector<std::string>{"name", "age", "sex"};
    pr_filter_token_res token_res;
    PR_Filter_Token(token_param, token_res);

    // search
    pr_filter_search_param search_param;
    search_param.tokp = token_res.tokp;
    search_param.k_w12_enc = token_res.k_w12_enc;
    search_param.tokp_vec = token_res.tokp_vec;
    search_param.emm = setup_res.emm;
    pr_filter_search_res search_res;
    PR_Filter_Search(search_param, search_res);

    // resolve
    pr_filter_resolve_param resolve_param;
    resolve_param.w1 = "name";
    resolve_param.wn = "sex";
    resolve_param.mk = token_param.mk;
    resolve_param.c = search_res.c;
    resolve_param.dc = search_res.dc;
    resolve_param.vaild = search_res.vaild;
    resolve_param.DX = setup_res.DX;
    std::vector<std::string> resolve_res;
    PR_Filter_Resolve(resolve_param, resolve_res);
    for (int i = 0; i < resolve_res.size(); i++)
    {
        std::cout << resolve_res[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    // Test_GenKey();
    // Test_SubWord();
    // Test_Permutation();
    // Test_Permutation2();
    // Test_FindConversionKey();
    // Test_PGen();
    // Test_AONTH_DAONTH();
    // Test_Pr_ED();
    // Test_Pr_ED_NUL();
    // Test_RG_RE();
    Test_Pr_filter();
    return 0;
}