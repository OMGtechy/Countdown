/*
    This file is part of Countdown.

    Countdown is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Countdown is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Countdown.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <thread>
#include <future>

template <typename IterLhs, typename IterRhs>
auto findUnionOf(IterLhs lhsIter, const IterLhs& lhsEnd,
                 const IterRhs rhsBegin, const IterRhs& rhsEnd) {
    std::vector<std::string> result;

    for(; lhsIter != lhsEnd; ++lhsIter) {
        if(std::find(rhsBegin, rhsEnd, *lhsIter) != rhsEnd) {
            result.push_back(*lhsIter);
        }
    }

    return result;
}

int main(int argc, char* argv[]) {
    // program name and word list
    if(argc < 3) {
        std::cout << "Please supply a word to check anagrams of and a newline delimited list of words as a file" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const char* const wordlistPath = argv[2];

    std::ifstream wordlistFile(wordlistPath, std::ios_base::in);

    if(! wordlistFile.is_open()) {
        std::cout << "Could not open file " << wordlistPath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string currentWord;
    std::vector<std::string> words;

    std::cout << "Loading words..." << std::endl;

    while(wordlistFile >> currentWord) {
        words.emplace_back(std::move(currentWord));
    }

    std::cout << words.size() << " words loaded!" << std::endl;

    std::string anagram = argv[1];
    std::transform(anagram.begin(), anagram.end(), anagram.begin(), tolower);

    std::cout << "Searching for anagrams of " << anagram << std::endl;

    std::sort(anagram.begin(), anagram.end());

    std::vector<std::string> anagramPermutations;

    do {
        anagramPermutations.push_back(anagram);
    } while(std::next_permutation(anagram.begin(), anagram.end()));

    std::cout << "There are " << anagramPermutations.size() << " possible words" << std::endl;

    std::vector<std::future<std::vector<std::string>>> futures;

    const int jump = 20000;
    auto wordIter = words.cbegin();

    for(;;) {
        decltype(words)::const_iterator nextEnd;

        if(std::distance(wordIter, words.cend()) <= jump) {
            nextEnd = words.cend();
        } else {
            nextEnd = wordIter + jump;
        }

        futures.emplace_back(std::async(std::launch::async, [&anagramPermutations, wordIter, nextEnd](){
            return findUnionOf(wordIter, nextEnd, anagramPermutations.cbegin(), anagramPermutations.cend());
        }));

        if(nextEnd == words.cend()) {
            break;
        }

        wordIter += jump;
    }

    bool matchFound = false;
    
    for(auto& future : futures) {
        for(auto& word : future.get()) {
            matchFound = true;
            std::cout << "MATCH: " << word << std::endl;
        }
    }

    if(! matchFound) {
        std::cout << "No matches found" << std::endl;
    }
}
