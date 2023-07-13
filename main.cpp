#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>
using namespace std;

#define MAX_WORD_LENGTH 15
#define MIN_WORD_LENGTH 5
#define SOLUTIONS_PER_STARTING_WORD 1

string file = "Collins Scrabble Words (2019).txt";
string answerFile = "answers.txt";
ifstream fin(file);
ofstream fout(answerFile);

set<string> words;
map<int, vector<char>> adj; // chars adjacent to an index
map<char, int> numAssign;   // int assignment for each char
vector<vector<string>> ans;
set<string> visited;
set<string> possibleWords;
map<char, vector<string>> allWords;

int MXATMPTS, MAX_SOLUTIONS;
int numSolutions = 0;
bool doneFindingSolutions = false;
bool skipToNextWord = false; //prevent solutions with duplicate words in case of mismatch in word banks

void loadWords() {
  auto start = std::chrono::high_resolution_clock::now();
  cout << "Loading words... \n";
  string gbg;
  fin >> gbg; // garbage first line
  while (!fin.eof()) {
    string s;
    fin >> s;
    for (int i = 0; i < s.length(); ++i) {
      s[i] = tolower(s[i]);
    }
    words.insert(s);
    // cout << "Added " << s << "\n";
  }
  auto end = std::chrono::high_resolution_clock::now();
  cout << "Loading finished :) \n";
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << duration.count() << "ms \n";
  return;
}

bool isWord(string s) { return words.find(s) != words.end(); }

bool canContinue(string s) {
  if (words.upper_bound(s) == words.end()) {
    return false;
  }
  string nextHighest = *words.upper_bound(s);
  return nextHighest.find(s) != std::string::npos;
}

bool isSolution(vector<string> vec) {
  // cout << "checking: ";
  // for (auto val : vec) {
  //   cout << val << " ";
  // }
  set<char> st;
  for (auto s : vec) {
    for (int i = 0; i < s.length(); ++i) {
      st.insert(s[i]);
      if (st.size() == 12){
        return true;
      }
    }
  }
  // cout << (st.size() == 12) << "\n";
  return false;
}

vector<char> adjStuff(char c) { return adj[numAssign[c]]; }

void addWord(string s) {
  allWords[s[0]].push_back(s);
  possibleWords.insert(s);
}

void makeWords(string currString) {
  if (visited.find(currString) != visited.end()) {
    return;
  }
  visited.insert(currString);
  if (isWord(currString) && currString.length() >= MIN_WORD_LENGTH) {
    addWord(currString);
  }
  if (!canContinue(currString)) {
    currString.pop_back(); // remove last char
    return;
  }
  char lastChar = currString[currString.length() - 1];
  for (auto c : adjStuff(lastChar)) {
    makeWords(currString + c);
  }
  currString.pop_back(); // remove last char
  return;
}

void generateCombos(string currString, vector<string> currSol) {
  bool outtaSize = currSol.size() == MXATMPTS;
  if (outtaSize || doneFindingSolutions || skipToNextWord) {
    return;
  }
  currSol.push_back(currString);
  if (isSolution(currSol)) {
    ans.push_back(currSol);
    currSol.pop_back();
    ++numSolutions;
    skipToNextWord = true;
    if (numSolutions == MAX_SOLUTIONS) {
      doneFindingSolutions = true;
    }
    return;
  }
  char lastLetter = currString[currString.length() - 1];
  vector<string> adjWords = allWords[lastLetter];
  for (auto val : adjWords) {
    if (doneFindingSolutions || skipToNextWord) {
      break;
    }
    generateCombos(val, currSol);
  }
  currSol.pop_back();
  return;
}

int main() {
  loadWords();
  cout << "How many words do you want in the solution? (recommended 3 max)\n";
  cin >> MXATMPTS;
  cout << "How many solutions do you want (recommended 10 max for quicker times)\n";
  cin >> MAX_SOLUTIONS;
  cout << "type in letters as a string e.g (top, left, right, buttom), order doesn't matter\n";
  cout << "for example:\n";
  cout << " abc \n";
  cout << "d   g\n";
  cout << "e   h\n";
  cout << "f   i\n";
  cout << " jkl \n";
  cout << "could be abcdefghljkl or jkldefabcghi\n";
  string s;
  cin >> s;
  for (int i = 0; i < s.length(); ++i) {
    numAssign[s[i]] = (i / 3);
    // cout << s[i] << " " << numAssign[s[i]] << "\n";
    for (int j = 0; j < 4; ++j) {
      if (i / 3 == j) {
        continue;
      }
      adj[j].push_back(s[i]);
    }
  }
  // debugging stuff
  // for (int i = 0; i < 4; ++i){
  //   cout << i << "\n";
  //   for (char val : adj[i]) {
  //     cout << val << " ";
  //   }
  //   cout << "\n";
  // }

  auto start = std::chrono::high_resolution_clock::now();

  // start the solve
  // generate all possible words
  for (int i = 0; i < s.length(); ++i) {
    string str = "";
    str += s[i]; // convert char to string
    makeWords(str);
  }
  //cout << "done generating stuff\n";
  // more debugging
  // for (auto val : possibleWords){
  //   cout << val << "\n";
  // }
  // cout << possibleWords.size() << "\n";

  for (auto val : possibleWords) {
    if (doneFindingSolutions) {
      break;
    }
    skipToNextWord = false;
    vector<string> currSol;
    generateCombos(val, currSol);
  }
  // print answers
  for (vector<string> vec : ans) {
    for (auto val : vec) {
      fout << val << " ";
    }
    fout << "\n";
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  cout << "Answers in answers.txt"
       << "\n";
  cout << duration.count() << "ms\n";
}