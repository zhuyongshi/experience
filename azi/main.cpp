#include "pr_filter.h"

void Test_GenKey()
{
    cout << Gen_RandKey(10) << endl;
}

//测试permutation
void Test_Permutation()
{
    vector<string> kep{"2", "3", "4", "0", "1"};
    string pin = "abcde";
    string pout = Permutation(pin.size(), kep, pin);
    for (size_t i = 0; i < pin.size(); i++)
    {
        cout << pout[i] << " ";
    }
    cout << endl;
    string out = De_Permutation(pin.size(), kep, pout);
    for (size_t i = 0; i < pin.size(); i++)
    {
        cout << out[i] << " ";
    }
    cout << endl;
}

//测试 FindConversion Key
void Test_FindConversionKey()
{
    vector<string> kpa{"3","1","2","0"};
    vector<string> kpb{"2","3","1","0"};
    int size = kpa.size();
    vector<string> ret = Find_CK(size, kpa, kpb);
    for (size_t i = 0; i < ret.size(); i++)
    {
        cout << ret[i] << " ";
    }
    cout << endl;
}

//测试PGen
void Test_PGen()
{
    string key = "2310";
    int n = 20;
    vector<string> ans(n);
    Permutationkey_Gen(key, n, ans);
    for (size_t i = 0; i < ans.size(); i++)
    {
        cout << ans[i] << " ";
    }
    cout << endl;
}

//测试Aonth和Daonth
void Test_AONTH_DAONTH()
{
    // 测试AONTH
    int ctr = 20, len = 8;
    vector<string> m{"cdg", "efg", "sad", "aqw", "cvb", "fhj"};
    vector<string> mplus(m.size() + 1);
    ANOTH(ctr, len, m, mplus);
    for (int i = 0; i < mplus.size(); i++)
    {
        cout << mplus[i] << " ";
    }
    cout << endl;

    // 测试D-AONTH
    vector<string> origin_m(mplus.size() - 1);
    D_AONTH(ctr, mplus, origin_m);
    for (int i = 0; i < m.size(); i++)
    {
        cout << origin_m[i] << " ";
    }
    cout << endl;
}

//测试PR_Enc和Pr_dec
void Test_Pr_ED()
{
    vector<string> key{"01010", "10101", "11100"};
    vector<string> w{"name", "age"};
    vector<string> m{"id1", "id2", "id4", "id5", "id6"};
    int ctr = 20, len = m[0].size();

    int s = m.size(), n = s + 1;
    vector<string> c(n);
    Pr_Enc(key, w, m, ctr, len, c);
    cout << "E:" << endl;
    for (size_t i = 0; i < n; i++)
    {
        cout << c[i] << " ";
    }
    cout << endl;

    vector<string> mm(s);
    Pr_Dec(key, w, c, ctr, len, mm);
    cout << "D:" << endl;
    for (size_t i = 0; i < s; i++)
    {
        cout << mm[i] << " ";
    }
    cout << endl;
}

//测试RG和RE
void Test_RG_RE(){
    vector<string> key{"01010", "10101", "11100"};
    vector<string> w1{"name", "age"};
    vector<string> w2{"name", "sex"};
    vector<string> w3{"name", "age", "sex"};
    vector<string> m{"id1", "id2", "id4", "id5", "id6"};
    int ctr = 20, len = m[0].size();

    int s = m.size(), n = s + 1;
    vector<string> c(n);
    Pr_Enc(key, w1, m, ctr, len, c);
    
    vector<vector<string>> CK(2);
    vector<vector<string>> P2(2);
    vector<string> Keyfai(2);
    RG(key, w3, len, s, CK, P2, Keyfai);

    vector<string> cc(n);
    RE(CK, P2, Keyfai, c, cc);

    vector<string> mm(s);
    Pr_Dec(key, w2, cc, ctr, len, mm);
    for (size_t i = 0; i < s; i++)
    {
        cout << mm[i] << " ";
    }
    cout << endl;
}

int main()
{
    // Test_GenKey();
    // Test_Permutation();
    // Test_FindConversionKey();
    // Test_PGen();
    // Test_AONTH_DAONTH();
    // Test_Pr_ED();
    Test_RG_RE();
    return 0;
}