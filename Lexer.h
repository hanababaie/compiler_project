#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "ErrorReporter.h"
#include "Token.h"

namespace minic {

class Lexer {
public:
    Lexer(const std::string& source, ErrorReporter& errors); //this is the constructor of the lexer
    // 

    //returns the next valid token. invalid characters are reported and skipped so no token is produced for them. returns END_OF_FILE once the input is exhausted.
    Token nextToken();

    //all tokens up to and including END_OF_FILE.
    std::vector<Token> tokenize();


//private means that we can not use these outside of the class
private:
    std::string source_code;//the code that we want to check is stored in this variable
    ErrorReporter& error; //this a reference to an object that reports the lexical errors
    size_t position = 0;//this keep the postion of the lexer in the source code
    //size_t is an unsigned integer
    int line = 1; //this tells in which line we are
    int col = 1; //this tells in which column we are

    static const std::unordered_map<std::string, TokenType>& keywords();
    //this is a map that its key is a string and its value is the token type
    //we use this to recognize the keywords
    //static means that this function belongs to the class
    //const means that we can not change the content of the map using the reference
    //& is for that the output of the function is a reference to the map(function keyword)
    //so in this map like when we see int we check the map and see that its key is KW_INT


    bool atEnd() const { return position >= source_code.size(); } //here we check if we reach the end of the file or not
    

    char peek(size_t offset = 0) const; //this function read a char with out changing and adding the position
    //offset is the distance of the char we want to the current position
    //const means that we dont change lexer when it return

    char advance();//this function read the next char and add one point to the position 


    bool match(char expected);//this check if the current char is equal to the desire char or not

    void skipWhitespaceAndComments();//this function ignore and dont pass comments and whitespaces

    Token lexNumber(int line, int col);//this create token for numbers

    Token lexIdentifier(int line, int col);//this create token for identifiers and keywords

    Token makeToken(TokenType type, const std::string& lexeme,int line, int col); //this is used of detecting chars that are not num or letters(like +)

    void lexError(int line, int col, const std::string& msg); //this report the lexical errors we have
};

}  // namespace minic
