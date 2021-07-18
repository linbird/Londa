class Solution {
public:
    int minimumDeleteSum(string s1, string s2) {
///需要保存最长子串，取ascii和最大的子串
        return tot_sum(s1) + tot_sum(s2) - lcm(s1, s2);
    }

    int lcm(string& word1, string& word2){
        if(word1.empty() || word2.empty())  return 0;
        int l1 = word1.size(), l2 = word2.size();
        using node = pair<int, set<string>>;
        node Empty{0, {}};
        vector<vector<node>> dp(l1, vector<node>(l2, Empty));
        if(word1[0] == word2[0]) dp[0][0] = {1, {string(word1[0], 1)}};
        for(int j = 1; j < l2; ++j){
            if(dp[0][j-1].first == 1) dp[0][j] = dp[0][j-1];
            if(word2[j] == word1[0]) dp[0][j] = {1, {string(word1[0], 1)}};
        }
        for(int i = 1; i < l1; ++i){
            if(dp[i-1][0].first == 1) dp[i][0] = dp[i-1][0];
            if(word1[i] == word2[0]) dp[i][0] = {1, {string(word2[0], 1)}};
        }
        for(int i = 1; i < l1; ++i)
            for(int j = 1; j < l2; ++j)
                if(word1[i] == word2[j]){
                    set<string> tmp;
                    for(const string& str : dp[i-1][j-1].second){
                        string x(str);
                        x.push_back(word1[i]);
                        tmp.insert(x);
                    }
                    dp[i][j] = {1 + dp[i-1][j-1].first, tmp};
                }
                else{
                    if(dp[i-1][j].first > dp[i][j-1].first) dp[i][j] = dp[i-1][j];
                    else if(dp[i-1][j].first < dp[i][j-1].first) dp[i][j] = dp[i][j-1];
                    else{
                        set<string> tmp(dp[i][j-1].second);
                        tmp.insert(dp[i-1][j].second.begin(), dp[i-1][j].second.end());
                        dp[i][j] = {dp[i][j-1].first, tmp};
                    }
                }
        // cout << dp.back().back().first << endl;
        auto res = dp.back().back().second;
        copy(res.begin(), res.end(), ostream_iterator<string>(cout, " "));
        cout << endl;
        // if
        // return tot_sum(*res);
        return 0;
    }

    int tot_sum(string res){
        return accumulate(res.begin(), res.end(), 0, [](int& sum, char ch){
            return sum += static_cast<int>(ch);
        });
    }
};
