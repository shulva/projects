#include "spellcheck.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

template <typename Iterator, typename UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(std::string& source) {
    auto index = find_all(source.begin(),source.end(), isspace);
    auto binary_op = [&source](std::string::iterator s1,std::string::iterator s2){
        return Token(source,s1,s2);
    };
    Corpus tokens;
    auto output_index = std::inserter(tokens,tokens.end());
    std::transform(index.begin(),index.end()-1,index.begin()+1,output_index,binary_op);

   std::erase_if(tokens,[](const Token &t){
        return t.content.empty();
    });

    return tokens;
}

std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary) {
    namespace rv = std::ranges::views;

    auto if_contain = [&dictionary](const Token& token){
        return !dictionary.contains(token.content);
    };

    auto distance = [&dictionary](const Token& token){
        auto suggestion = dictionary | rv::filter([token](const std::string& dict_word){
            return levenshtein(token.content,dict_word)==1;
        });
        std::set<std::string> set_from_view(suggestion.begin(),suggestion.end());
        return Misspelling { token, set_from_view };
    };

    auto res = source | rv::filter(if_contain) | rv::transform(distance) | rv::filter([](const Misspelling& m){
        return !m.suggestions.empty();
    });

    std::set<Misspelling> ans(res.begin(),res.end());
    return ans;
};

/* Helper methods */

#include "utils.cpp"