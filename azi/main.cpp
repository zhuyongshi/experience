#include "pr_filter.h"

void Test_GenKey()
{
    std::cout << Gen_RandKey(10) << std::endl;
}

//测试permutation
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

//测试permutation2
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

//测试 FindConversion Key
void Test_FindConversionKey()
{
    int size = 4;
    std::vector<int> kpa{3,1,2,0};
    std::vector<int> kpb{2,3,1,0};
    std::vector<int> expect{2,0,1,3};
    std::vector<int> ret = Find_CK(size, kpa, kpb);
    for (size_t i = 0; i < ret.size(); i++)
    {
        std::cout << ret[i] << " ";
    }
    std::cout << std::endl;
}

//测试PGen
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

//测试Aonth和Daonth
void Test_AONTH_DAONTH()
{
    // 测试AONTH
    int ctr = 20, len = 8;
    std::vector<std::string> m{"abc", "def", "ghi", "jkl", "mno", "pqr"};
    std::vector<std::string> mplus(m.size() + 1);
    ANOTH(ctr, len, m, mplus);
    for (int i = 0; i < mplus.size(); i++)
    {
        std::cout << mplus[i] << " ";
    }
    std::cout << std::endl;

    // 测试D-AONTH
    std::vector<std::string> origin_m(mplus.size() - 1);
    D_AONTH(ctr, mplus, origin_m);
    for (int i = 0; i < m.size(); i++)
    {
        std::cout << origin_m[i] << " ";
    }
    std::cout << std::endl;
}

//测试PR_Enc和Pr_dec
void Test_Pr_ED()
{
    std::vector<std::string> key{"01010", "10101", "11100"};
    std::vector<std::string> w{"name", "age"};
    std::vector<std::string> m{"id1", "id2", "id4", "id5", "id6"};
    int ctr = 20, len = m[0].size();

    int s = m.size(), n = s + 1;
    std::vector<std::string> c(n);
    Pr_Enc(key, w, m, ctr, len, c);
    std::cout << "Pr_Enc:" << std::endl;
    for (size_t i = 0; i < n; i++)
    {
        std::cout << c[i] << " ";
    }
    std::cout << std::endl;

    std::vector<std::string> mm(s);
    Pr_Dec(key, w, c, ctr, len, mm);
    std::cout << "Pr_Dec:" << std::endl;
    for (size_t i = 0; i < s; i++)
    {
        std::cout << mm[i] << " ";
    }
    std::cout << std::endl;
}

//测试RG和RE
void Test_RG_RE()
{
    std::vector<std::string> key{"01010", "10101", "11100"};
    std::vector<std::string> w1{"name", "age"};
    std::vector<std::string> w2{"name", "sex"};
    std::vector<std::string> w3{"name", "age", "sex"};
    std::vector<std::string> m{"id1", "id2", "id4", "id5", "id6"};
    int ctr = 20, len = m[0].size();

    int s = m.size(), n = s + 1;
    std::vector<std::string> c(n);
    Pr_Enc(key, w1, m, ctr, len, c);

    std::vector<std::vector<int>> CK(2);
    std::vector<std::vector<int>> P2(2);
    std::vector<std::string> KeyPhi(2);
    Pr_ReGen(key, w3, len, s, CK, P2, KeyPhi);

    std::vector<std::string> cc(n);
    Pr_ReEnc(CK, P2, KeyPhi, c, cc);

    std::vector<std::string> mm(s);
    Pr_Dec(key, w2, cc, ctr, len, mm);
    for (size_t i = 0; i < s; i++)
    {
        std::cout << mm[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    // Test_GenKey();
    // Test_Permutation();
    // Test_Permutation2();
    // Test_FindConversionKey();
    // Test_PGen();
    // Test_AONTH_DAONTH();
    // Test_Pr_ED();
    Test_RG_RE();
    return 0;
}