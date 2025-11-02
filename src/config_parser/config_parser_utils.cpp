#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include"parsing.hpp"

std::vector<std::string> readFileLines(const std::string &filename)
{
    std::vector<std::string> lines;
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return lines; // vector vide si erreur
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    return lines;
}

int err(const std::string &msg, int line) {
    std::cerr << "Error (line " << line + 1 << "): " << msg << std::endl;
    return 1;
}

// Retourne un vector<vector<string>>
// → chaque ligne devient une liste de tokens (mots, symboles, etc.)
std::vector<std::vector<std::string> > tokenizeLines(const std::vector<std::string> &lines)
{
    std::vector<std::vector<std::string> > tokens;

    for (size_t i = 0; i < lines.size(); ++i) {
        std::vector<std::string> lineTokens;
        std::string word;
        for (size_t j = 0; j < lines[i].size(); ++j) {
            char c = lines[i][j];
            // Si le caractère est un séparateur
            if (std::isspace(c) || c == ';' || c == ',' || c == '/' || c == '}' || c == '{') {
                if (!word.empty()) {
                    lineTokens.push_back(word);
                    word.clear();
                }
                // Si c'est un caractère spécial isolé, on le garde comme token
                if (c == ';' || c == ',' || c == '/' || c == '}' || c == '{')
                    lineTokens.push_back(std::string(1, c));
            } 
            else {
                word += c;
            }
        }
        // Ne pas oublier le dernier mot de la ligne
        if (!word.empty())
            lineTokens.push_back(word);

        // Ajouter la ligne tokenisée
        tokens.push_back(lineTokens);
    }
    return tokens;
}

int check_scopes_logic(std::vector<std::vector<std::string> > tokens)
{
    std::stack<std::string> ctx;
    int expected = 0;
    int current = 0;
    int servers = 0;
    bool justClosed = false; // nouveau flag

    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = 0; j < tokens[i].size(); ++j) {
            const std::string &t = tokens[i][j];
            if (t == "server") {
                if (!ctx.empty()) return err("nested server not allowed", i);
                ctx.push("server");
                servers++;
                expected += 2;
                justClosed = false;
                continue;
            }
            if (t == "location") {
                if (ctx.empty() || ctx.top() != "server")
                    return err("location outside server", i);
                ctx.push("location");
                expected += 2;
                continue;
            }
            if (t == "{")
                current++;

            if (t == "}") {
                if (ctx.empty())
                    return err("unexpected }", i);
                ctx.pop();
                current++;
                justClosed = true;
            }
        }
        // ✅ On n’imprime qu’une seule fois quand on vient de fermer
        if (justClosed && ctx.empty() && current == expected) {
            std::cout << "Server " << servers << " closed correctly ✅" << std::endl;
            justClosed = false;
        }
        if (current > expected)
            return err("too many braces", i);
    }
    if (!ctx.empty() || current != expected)
        return err("unclosed block(s) at EOF", tokens.size());
    std::cout << "✓ Structure OK (" << servers << " server block(s))" << std::endl;
    return 0;
}

int check_forbidden_comments(const std::vector<std::vector<std::string> > &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = 0; j < tokens[i].size(); ++j) {
            const std::string &t = tokens[i][j];
            // --- cas 1 : un seul token contient un caractère de commentaire
            if (t == "#" || t == "//" || t == "/*" || t == "*/") {
                std::cerr << "Error (line " << i + 1 << "): forbidden comment syntax '" 
                          << t << "'" << std::endl;
                return 84;
            }
            // --- cas 2 : combinaison de deux tokens formant un commentaire
            if (t == "/" && j + 1 < tokens[i].size()) {
                const std::string &next = tokens[i][j + 1];
                if (next == "/" || next == "*" ) {
                    std::cerr << "Error (line " << i + 1 
                              << "): forbidden comment syntax '" 
                              << t + next << "'" << std::endl;
                    return 84;
                }
            }
            if (t == "*" && j + 1 < tokens[i].size()) {
                const std::string &next = tokens[i][j + 1];
                if (next == "/") {
                    std::cerr << "Error (line " << i + 1 
                              << "): forbidden comment syntax '" 
                              << t + next << "'" << std::endl;
                    return 84;
                }
            }
        }
    }
    std::cout << "✓ No forbidden comments detected ✅" << std::endl;
    return 0;
}


int check_double_semicolon(const std::vector<std::vector<std::string> > &tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = 0; j + 1 < tokens[i].size(); ++j) {
            const std::string &t = tokens[i][j];
            const std::string &next = tokens[i][j + 1];

            if (t == ";" && next == ";") {
                std::cerr << "Error (line " << i + 1
                          << "): invalid token sequence ';;'" << std::endl;
                return 1;
            }
        }
    }

    //std::cout << "✓ No double semicolon detected ✅" << std::endl;
    return 0;
}


int make_checks(std::vector<std::vector<std::string> > tokens)
{
    if(check_scopes_logic(tokens) == 1)
        return (1);
    else if (check_forbidden_comments(tokens) == 1)
        return (1);
    else if(check_double_semicolon(tokens) == 1)
        return (1);
    return (0);
}
;;