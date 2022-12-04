#include "pr_filter_server.h"

void sEMM_Search(std::string tokp, std::map<std::string, std::vector<std::string>> EMMp, std::vector<std::string> &tags)
{
    tags = EMMp[tokp];
}

int PR_Filter_Search(pr_filter_search_param param, pr_filter_search_res &res)
{
    // tagl = sEMM.Search(tokp, EMMp)
    std::vector<std::string> tags_l;
    sEMM_Search(param.tokp, param.emm.EMMp, tags_l);
    // ci = Dec(k_w1_w2_enc, tagl)
    std::vector<std::string> c(tags_l.size());
    for (int i = 0; i < tags_l.size(); i++)
    {
        decrypt(param.k_w12_enc, tags_l[i], c[i]);
    }
    // optimization
    // for l=1..L
    // for d=3..nl
    // cl = PR.ReEnc(k_re_d, cl)
    // dtage(l,d) = F(kx, cl')
    int n = c.size(), s = n - 1;
    std::vector<bool> vaild(s, true);
    std::vector<std::string> cplus(c.size());
    std::string dtag;
    for (int i = 0; i < param.tokp_vec.size(); i++)
    {
        if (Pr_ReEnc(param.tokp_vec[i].CK, param.tokp_vec[i].P2, param.tokp_vec[i].KeyPhi, c, cplus) != 0)
        {
            std::cout << "[PR_Filter_Search] call Pr_ReEnc failed" << std::endl;
            return -1;
        }
        for (int j = 0; j < c.size(); j++)
        {
            c[j] = cplus[j];
        }
        for (int j = 1; j < n; j++)
        {
            if (!vaild[j - 1])
                continue;
            dtag = H1(param.tokp_vec[i].kx, cplus[j]);
            if (param.emm.Xset.find(dtag) == param.emm.Xset.end())
            {
                vaild[j - 1] = false;
            }
        }
    }
    res.c = cplus;
    res.vaild = vaild;
    // done
    return 0;
}