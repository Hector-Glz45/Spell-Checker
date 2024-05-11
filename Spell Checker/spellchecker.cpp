#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cctype>

/*----------------------------------------------------------
 * Project: Spell Checker
 *
 * Date: 30-Nov-2022
 * Authors:
 *           A01753863 Héctor González Sánchez
 *           A01747255 Ricardo Valdéz Jasso
 *----------------------------------------------------------*/

// ************************** Word Struct *************************************

struct word {
    std::string text;
    int line;
    int column;
};

// ************************** Get soundex *************************************

std::unordered_map<char, char> letters = {
    {'b', '1'}, {'f', '1'}, {'p', '1'}, {'v', '1'},
    {'c', '2'}, {'g', '2'}, {'j', '2'}, {'k', '2'}, 
    {'q', '2'}, {'s', '2'}, {'x', '2'}, {'z', '2'},
    {'d', '3'}, {'t', '3'}, {'l', '4'}, {'m', '5'}, 
    {'n', '5'}, {'r', '6'}
};

std::string soundex(std::string& s) 
{
    std::string result = s.substr(0,1);
    result = tolower(result[0]);
    
    for (int i = 1; i < s.length(); i++) {
        if (letters.count(tolower(s[i]))) {
            result = result + letters[tolower(s[i])];
        }
    }
    if (result.size() < 7) {
        result.append(7 - result.size(), '0');
    } else if (result.size() > 7) {
        result = result.substr(0,7);
    }
    return result;
}

// ********************** Lectura del archivo *********************************

bool read_words(
        const std::string input_file_name,
        std::vector<word>& words)
{
    std::ifstream input_file(input_file_name);
    if (input_file.fail()) {
        return false;
    }
    std::regex reg_exp("[a-zA-Z]+");
    std::smatch match;
    std::string text;
    int line = 0;
    int column = 0;
    while (std::getline(input_file, text)) {
        ++line;
        column = 1;
        while (std::regex_search(text, match, reg_exp)) {
            column += match.position();
            words.push_back({match.str(), line, column});
            column += match.length();
            text = match.suffix().str();
        }
    }
    return true;
}

void lower_words (std::vector<word>& words, std::vector<word>& wordslow) 
{
    for (word w : words) {
        std::string word = w.text;
        std::string aux = "";
        for (int j = 0; j < word.length(); j++) {
            aux += tolower(word[j]);
        }
        w.text = aux;
        wordslow.push_back(w);
    }
}

// ********* Leer el diccionario y mandarlo a un unordered_set ****************

bool read_dictionary(std::unordered_set<std::string>& dictionary, std::unordered_map<std::string, std::vector<std::string>>& soundex_map) 
{
    std::string s;
    std::ifstream file("words.txt");
    if (file.fail()) {
        return false;
    }
    
    std::string line;
    while (getline(file, line)) {
        dictionary.insert(line);
        s = soundex(line);
        if (not soundex_map.count(line)) {
            soundex_map[s].push_back(line);
        }
    }
    file.close();
    return true;
}

// ********************* Detectar palabras mal escritas ***********************

bool compare(std::unordered_set<std::string>& dictionary, std::vector<word>& wordslow, std::vector<word>& misspelled)
{
    std::unordered_set<std::string> aux;
    for (word w : wordslow) {
        if (not aux.count(w.text) && not dictionary.count(w.text) ) {
            misspelled.push_back(w);
            aux.insert(w.text);
        }
    }
}

// *************************** Suggestions ************************************

void result(std::vector<word>& misspelled, std::unordered_map<std::string, std::vector<std::string>>& soundex_map)
{
    bool first_time = true;
    for(word w : misspelled) {
        if(soundex_map.count(soundex(w.text))) {
            std::cout << "\nUnrecognized word: '" << w.text << "'. First found at line: " << w.line << " column: " << w.column << "\n";
            std::cout << "Suggestions: ";
            for(auto& x : soundex_map[soundex(w.text)]) {
                if (first_time) {
                    first_time = false;
                } else {
                    std::cout << ", ";
                }
                std::cout << x;
            }
            std::cout << "\n";
            first_time = true;
        } else {
            std::cout << "\nUnrecognized word: '" << w.text << "'. First found at line: " << w.line << " column: " << w.column << "\n";
            std::cout << "No suggestions.\n";
        }
    }
}

// ****************************************************************************

int main(int argc, char* argv[]) 
{
    if(argc != 2) {
        std::cerr << "Please specify the name of the input file.\n";
        exit(1);
    }

    std::string file_name = argv[1];
    std::unordered_set<std::string> dictionary;
    std::vector<word> words;
    std::vector<word> wordslow;
    std::vector<word> misspelled;
    std::unordered_map<std::string, std::vector<std::string>> soundex_map;
    
    read_dictionary(dictionary, soundex_map);
    read_words(file_name, words);
    lower_words(words, wordslow);
    compare(dictionary, wordslow, misspelled);
    result(misspelled, soundex_map);
    
    return 0;
}