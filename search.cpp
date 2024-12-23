#include "include/search.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string cleanToken(const string& token) {
  // TODO student
  string cleaned;
  bool hasLetter = false;

    // Remove leading punctuation and convert to lowercase
  for (char c : token) {
      if (!cleaned.empty() || !std::ispunct(c)) {
          cleaned += std::tolower(c);
          if (std::isalpha(c)) {
              hasLetter = true;
          }
      }
  }

    // Remove trailing punctuation
  while (!cleaned.empty() && std::ispunct(cleaned.back())) {
      cleaned.pop_back();
  }

    // Return empty string if no letters found
  return hasLetter ? cleaned : "";
}

set<string> gatherTokens(const string& text) {
  // TODO student
  set<string> tokens;
    istringstream iss(text);
    string token;

    while (iss >> token) {
        string cleaned = cleanToken(token);
        if (!cleaned.empty()) {
            tokens.insert(cleaned);
        }
    }

    return tokens;
}

int buildIndex(const string& filename, map<string, set<string>>& index) {
  // TODO student
  ifstream file(filename);
  if (!file.is_open()) {
      return 0;
  }

  int numPages = 0;
  string url, content;

  while (getline(file, url) && getline(file, content)) {
      set<string> tokens = gatherTokens(content);
        
      for (const string& token : tokens) {
          index[token].insert(url);
      }

      numPages++;
  }

  return numPages;
}

set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence) {
  // TODO student
  istringstream iss(sentence);
  string term;
  set<string> result;
  bool isFirstTerm = true;

  while (iss >> term) {
      char modifier = ' ';
      if (term[0] == '+' || term[0] == '-') {
          modifier = term[0];
          term = term.substr(1);
      }

      term = cleanToken(term);
        
      set<string> termMatches;
      auto it = index.find(term);
      if (it != index.end()) {
          termMatches = it->second;
      }

      if (isFirstTerm) {
          result = termMatches;
          isFirstTerm = false;
      } else if (modifier == '+') {
          for (auto it = result.begin(); it != result.end(); ) {
              if (termMatches.find(*it) == termMatches.end()) {
                  it = result.erase(it);
              } else {
                  ++it;
              }
          }
        } 
        else if (modifier == '-') {
          for (const auto& url : termMatches) {
              result.erase(url);
          }
        } 
        else {
          result.insert(termMatches.begin(), termMatches.end());
        }
  }

  return result;
}

void searchEngine(const string& filename) {
  // TODO student
  map<string, set<string>> index;
  int numPages = buildIndex(filename, index);
  if (numPages == 0) {
      cout << "Invalid filename." << endl;
  }

  int uniqueTerms = index.size();

  cout << "Stand by while building index..." << endl;
  cout << "Indexed " << numPages << " pages containing " << uniqueTerms << " unique terms" << endl;
  
  string query;
  while (true) {
      cout << "Enter query sentence (press enter to quit): ";
      getline(cin, query);

      if (query.empty()) {
          break;
      }

      set<string> matches = findQueryMatches(index, query);
        
      cout << "Found " << matches.size() << " matching pages" << endl;
      for (const auto& url : matches) {
          cout << url << endl;
      }
  }

  cout << "Thank you for searching!" << endl;
}
