#include "Lexer.h"
#include <cctype>


namespace minic {

static bool isAlpha(char c){ //this check if the letter we are reading is in alphabet(a-Z) or a underscore (_)
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; 
}

static bool isDigit(char c){  //this check if the letter we are using is a number
    return std::isdigit(static_cast<unsigned char>(c)); 
}
static bool isAlnum(char c){ // this check if the char we are reading is alphabetical or a number
    return isAlpha(c) || isDigit(c); 
}

const char* tokenTypeName(TokenType t){ //this just return the string of type names
    switch (t) {
        case TokenType::KW_INT: return "KW_INT";
        case TokenType::KW_BOOL: return "KW_BOOL";
        case TokenType::KW_VOID: return "KW_VOID";
        case TokenType::KW_FINAL: return "KW_FINAL";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_WHILE: return "KW_WHILE";
        case TokenType::KW_FOR: return "KW_FOR";
        case TokenType::KW_RETURN: return "KW_RETURN";
        case TokenType::KW_PRINT: return "KW_PRINT";
        case TokenType::KW_TRUE: return "KW_TRUE";
        case TokenType::KW_FALSE: return "KW_FALSE";
        case TokenType::KW_AND: return "KW_AND";
        case TokenType::KW_OR: return "KW_OR";
        case TokenType::KW_NOT: return "KW_NOT";
        case TokenType::KW_BREAK: return "KW_BREAK";
        case TokenType::KW_CONTINUE: return "KW_CONTINUE";
        case TokenType::IDENT: return "IDENT";
        case TokenType::INT_LIT: return "INT_LIT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::LT: return "LT";
        case TokenType::GT: return "GT";
        case TokenType::LE: return "LE";
        case TokenType::GE: return "GE";
        case TokenType::EQ: return "EQ";
        case TokenType::NE: return "NE";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::INC: return "INC";
        case TokenType::DEC: return "DEC";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACK: return "LBRACK";
        case TokenType::RBRACK: return "RBRACK";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMI: return "SEMI";
        case TokenType::END_OF_FILE: return "EOF";
    }
    return "UNKNOWN";
}

const std::unordered_map<std::string, TokenType>& Lexer::keywords(){ //map for the keywords
    static const std::unordered_map<std::string, TokenType> kw = {
        {"int", TokenType::KW_INT},
        {"bool", TokenType::KW_BOOL},
        {"void", TokenType::KW_VOID},     
        {"final", TokenType::KW_FINAL},
        {"if", TokenType::KW_IF},         
        {"else", TokenType::KW_ELSE},
        {"while", TokenType::KW_WHILE},   
        {"for", TokenType::KW_FOR},
        {"return", TokenType::KW_RETURN}, 
        {"print", TokenType::KW_PRINT},
        {"true", TokenType::KW_TRUE},     
        {"false", TokenType::KW_FALSE},
        {"and", TokenType::KW_AND},       
        {"or", TokenType::KW_OR},
        {"not", TokenType::KW_NOT},       
        {"break", TokenType::KW_BREAK},
        {"continue", TokenType::KW_CONTINUE}};
    return kw;
}

Lexer::Lexer(const std::string& source, ErrorReporter& errors)
    : source_code(source), error(errors) {}
//lexer:: means that this function belongs to the lexer class
//const std::string& source this means that we take a reference of the source code and the constructor can not change its content
//ErrorReporter& errors this means that we take a reference of the object of ErrorReporter
//source_code means that we initialize it(i couldnt use error = errors)

char Lexer::peek(size_t offset) const{ //this let us read the position + offset char without changing the position
    //and const tells that we can not change the lexer object
    if(position + offset < source_code.size()){ //check if we can move that much
        return source_code[position + offset]; //we return the char
    }
    return '\0'; //if we cant move  that much we return null
}

char Lexer::advance(){ //this function reads a char and change the position 
    char c = source_code[position++];
    if(c == '\n'){ //if we go the next line
        ++line; 
        col = 1; 
    }
    else{ //unless we go the next col
        ++col; 
    }
    return c;
}

bool Lexer::match(char expected){//this function check the current char with the expected char and compare them
    if(atEnd() || source_code[position] != expected){ 
        return false;
    }
    //if we were the same, then we use it and go to the next char and return true
    advance();
    return true;
}

void Lexer::lexError(int line, int col, const std::string& msg){//here we are going to report a an error
    //this doesn't detect the error itself but it use other functions to detect the problem and call lexError() to report them
    error.report(ErrorKind::Lexical, line, col, msg); 
}

void Lexer::skipWhitespaceAndComments(){//this function doesnt pass comments and whitespace to the parser
    while(!atEnd()) {
        char c = peek();//take the current char without using it
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n'){ //check if it is a white space
            advance();//then we use it
        } 
        else if(c == '/' && peek(1) == '/'){//here we check if we have //(single line comment)
            while(!atEnd() && peek() != '\n'){//until we didnt reach the eof or the next line we ignore the comments
                advance();
            }
        } 
        else if (c == '/' && peek(1) == '*'){//here we check if we have /*(multi line comment)
            int comment_line = line; //we take the line and col
            int comment_col = col;

            advance(); //here we use / and * to reach the letters
            advance();
            bool closed = false;//flag for understanding when the comments ends

            while (!atEnd()){
                if(peek() == '*' && peek(1) == '/'){ //when we reach */ the end of the comment we take out of the loop
                    advance(); //here we use / and * to reach the letters
                    advance();
                    closed = true; //we reach the end
                    break;
                }
                advance();//read the next char
            }
            if(!closed)//if the comment never end with */, this is a error
                lexError(comment_line, comment_col, "unterminated multi line comment (missing '*/')");
        } 
        else{ //other chars
            break;
        }
    }
}

Token Lexer::lexNumber(int line, int col){ //here are we are detecting the numbers and create a token for them
    std::string text; //a empty string for storing

    while(isDigit(peek())){//until we are reading numbers we use it and append it the string
        text += advance();
    }

    //if a letter or .. is connected to a number, so that we can understand identifiers can not start with numbers and it is not an identifier
    if(isAlpha(peek())){
        while(isAlnum(peek())){
            text += advance(); //we add them to the text
        }

        lexError(line, col, "invalid token '" + text + "': identifiers cannot start with a digit"); //saying that it can not be a identifier
        return Token{TokenType::END_OF_FILE, "", line, col};  //then we return stop
        //we dont return the token as a normal token to the caller (but this is not the real end of file)
    }

    
    size_t num_zero = text.find_first_not_of('0'); //if there is 0 in the begining we can ignore them,so we delete them and here we return the index of the first number that is not zero

    std::string digits = (num_zero == std::string::npos) ? "0" : text.substr(num_zero);
    //here we say if all the numbers were zero we return 0
    //but if there was atleast one number rather than 0, we retrun it(trim the string from that index)

    if(digits.size() > 10 || std::stoll(digits) > 2147483647LL){ //we take the int 32 bit, so we set a limit for the number (2^31 - 1)
        lexError(line, col, "integer literal '" + text + "' is out of range for int");

        return Token{TokenType::END_OF_FILE, "", line, col}; //again we retrun a eof token
    }
    return Token{TokenType::INT_LIT, text, line, col}; //if everything was ok we send a int_lit token
}

Token Lexer::lexIdentifier(int line, int col){ //here we detect identifiers and keywords and retrun their token
    std::string text;
    while(isAlnum(peek())){ //until it has nums and alphabetical letters or _ we use it(number cant come first we check in the lexNum)
        text += advance(); //we use it and append it to the text
    }
    auto it = keywords().find(text);//if its value has a key in the keyword map
    return Token{it != keywords().end() ? it->second : TokenType::IDENT, text, line, col};
    //if it was a token we retrun a keyword else an identifier token

    //in an unordered_map, when a key is not found, find() returns keywords().end()

}


Token Lexer::makeToken(TokenType type, const std::string& lexeme,int line, int col){ //here we create token for chars that are not num or letter
    return Token{type, lexeme, line, col};
}

Token Lexer::nextToken(){//this function decides what token should be produced next
    while(true){
        skipWhitespaceAndComments(); //we ignore the whitespaces and comments
        int line_char = line; 
        int col_char = col;

        if(atEnd()){
            return Token{TokenType::END_OF_FILE, "", line_char, col_char}; //when we reach the end of the code we return a eof token 
        }

        char c = peek();//we read the current token

        if(isDigit(c)){ //if it was a digit
            Token t = lexNumber(line_char, col_char); //we call the lexNum to find the token(if it is a num or we have error)
            if(t.type == TokenType::END_OF_FILE){ //of the returned token was eof(there is an error)
                continue; //error already reported
            }  
            return t; //here we return the valid number
        }


        if (isAlpha(c)){ //if we had a letter it is an identifier or a keyword so we retrun it
            return lexIdentifier(line_char, col_char);
        }

        advance(); //use it and update the position


        switch(c){ //so here we check other chars that we might encounter
            case '+': //here we check the next char, if it is + we have ++ unless we just have +
                return match('+')
                    ? makeToken(TokenType::INC, "++", line_char, col_char)
                    : makeToken(TokenType::PLUS, "+", line_char, col_char);

            case '-':
                return match('-')
                    ? makeToken(TokenType::DEC, "--", line_char, col_char)
                    : makeToken(TokenType::MINUS, "-", line_char, col_char);

            case '*':
                return makeToken(TokenType::STAR, "*", line_char, col_char);

            case '/':
                return makeToken(TokenType::SLASH, "/", line_char, col_char);

            case '%':
                return makeToken(TokenType::PERCENT, "%", line_char, col_char);

            case '<':
                return match('=')
                    ? makeToken(TokenType::LE, "<=", line_char, col_char)
                    : makeToken(TokenType::LT, "<", line_char, col_char);

            case '>':
                return match('=')
                    ? makeToken(TokenType::GE, ">=", line_char, col_char)
                    : makeToken(TokenType::GT, ">", line_char, col_char);

            case '=':
                return match('=')
                    ? makeToken(TokenType::EQ, "==", line_char, col_char)
                    : makeToken(TokenType::ASSIGN, "=", line_char, col_char);

            case '!'://we use not for like !a in this language so it is an error to have it
                if (match('=')) {
                    return makeToken(TokenType::NE, "!=", line_char, col_char);
                }

                lexError(line_char, col_char,
                        "unexpected '!': use 'not' for logical negation");
                continue;

            case '&':
                lexError(line_char, col_char,
                        "unexpected '&': use 'and' for logical conjunction");

                if (peek() == '&') {
                    advance();
                }
                continue;

            case '|':
                lexError(line_char, col_char,
                        "unexpected '|': use 'or' for logical disjunction");

                if (peek() == '|') {
                    advance();
                }
                continue;

            case '(':
                return makeToken(TokenType::LPAREN, "(", line_char, col_char);

            case ')':
                return makeToken(TokenType::RPAREN, ")", line_char, col_char);

            case '[':
                return makeToken(TokenType::LBRACK, "[", line_char, col_char);

            case ']':
                return makeToken(TokenType::RBRACK, "]", line_char, col_char);

            case '{':
                return makeToken(TokenType::LBRACE, "{", line_char, col_char);

            case '}':
                return makeToken(TokenType::RBRACE, "}", line_char, col_char);

            case ',':
                return makeToken(TokenType::COMMA, ",", line_char, col_char);

            case ';':
                return makeToken(TokenType::SEMI, ";", line_char, col_char);

            default: {
                std::string message = "Invalid character: ";
                message += c;

                lexError(line_char, col_char, message);
                continue;
            }
        }
    }
}

std::vector<Token> Lexer::tokenize(){ //this function collect all the tokens that nextToken created and return them
    std::vector<Token> out; //we create an empty vector to store the tokens(vector let us to append and use the index for access)
    while(true) {
        Token t = nextToken(); //create the next token
        out.push_back(t);//store it
        if(t.type == TokenType::END_OF_FILE){ //when we reach the end of the code
            break;
        }
    }
    return out;
}

}  // namespace minic
