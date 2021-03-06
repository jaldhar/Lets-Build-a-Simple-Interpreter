// calc5.cc -- c++ port of Ruslan Spivaks "Let's Build a Simple Interpreter"
//              tutorial.
//
// By Jaldhar H. Vyas <jaldhar@braincells.com>
// Copyright (C) 2017, Consolidated Braincells Inc.  All rights reserved.
// "Do what thou wilt shall be the whole of the license."

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Token types
enum class TOKENTYPE {
    ENDOFFILE = 0, // EOF can't be used as it is already defined in std
                   // ENDOFFILE token is used to indicate that
                   // there is no more input left for lexical analysis
    INTEGER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAREN,
    RPAREN
};

ostream& operator<<(ostream& out, const TOKENTYPE& t) {
    string repr;
    switch(t) {
        case TOKENTYPE::ENDOFFILE:
            repr = "ENDOFFILE";
            break;
        case TOKENTYPE::INTEGER:
            repr = "INTEGER";
            break;
        case TOKENTYPE::PLUS:
            repr = "PLUS";
            break;
        case TOKENTYPE::MINUS:
            repr = "MINUS";
            break;
        case TOKENTYPE::MUL:
            repr = "MUL";
            break;
        case TOKENTYPE::DIV:
            repr = "DIV";
            break;
        case TOKENTYPE::LPAREN:
            repr = "LPAREN";
            break;
        case TOKENTYPE::RPAREN:
            repr = "RPAREN";
            break;
    }

    out << repr;
    return out;
}

struct Token {
    Token(TOKENTYPE t, long v) : type{t}, value{v} {
    }

    TOKENTYPE       type;   // token type: INTEGER, PLUS, or EOF
    long            value;  // token value: long int or '+'

    friend ostream& operator<<(ostream& out, const Token& t);
};

// String representation of the Token instance.
// Examples:
//      Token(INTEGER, 3)
//      Token(MUL '*')
//
ostream& operator<<(ostream& out, const Token& t) {
    out << "Token(" << t.type << "," << (long)t.value << ")";
    return out;
}

class Lexer {
public:
    Lexer(string& text);
    Token get_next_token();
    long  integer();
private:
    string& _text;          // client string input, e.g. "3+5"
    size_t  _pos;           // an index into _text
    char    _current_char;  // the character at _text[_pos]

    void  advance();
    void  skip_whitespace();
};

// Constructor
Lexer::Lexer(string& text) : _text{text}, _pos{0}, _current_char{_text[_pos]} {
}

// Lexical analyzer (also known as scanner or tokenizer)
//
// This method is responsible for breaking a sentence apart into tokens. One
// token at a time.
//
Token Lexer::get_next_token() {

    while (_current_char != '\0') {

        if (isspace(_current_char)) {
            skip_whitespace();
            continue;
        }

        if (isdigit(_current_char)) {
            Token token(TOKENTYPE::INTEGER, integer());
            cerr << token << endl;
            return token;
        }

        if (_current_char == '*') {
            advance();
            Token token(TOKENTYPE::MUL, '*');
            cerr << token << endl;
            return token;
        }

        if (_current_char == '/') {
            advance();
            Token token(TOKENTYPE::DIV, '/');
            cerr << token << endl;
            return token;
        }

        if (_current_char == '+') {
            advance();
            Token token(TOKENTYPE::PLUS, '+');
            cerr << token << endl;
            return token;
        }

        if (_current_char == '-') {
            advance();
            Token token(TOKENTYPE::MINUS, '-');
            cerr << token << endl;
            return token;
        }

        if (_current_char == '(') {
            advance();
            Token token(TOKENTYPE::LPAREN, '(');
            cerr << token << endl;
            return token;
        }

        if (_current_char == ')') {
            advance();
            Token token(TOKENTYPE::RPAREN, ')');
            cerr << token << endl;
            return token;
        }

        ostringstream out;
        out << "Error parsing input. Got: " << _current_char;
        throw(out.str().c_str());
    }

    Token token(TOKENTYPE::ENDOFFILE, '\0');
    cerr << token << endl;
    return token;
}

// Return a (multidigit) integer consumed from the input.
long Lexer::integer() {
    string result("");

    while (_current_char != '\0' && isdigit(_current_char)) {
        result += _current_char;
        advance();
    }

    return stol(result);
}

// Advance the '_pos' pointer and set the '_current_char' variable.
void Lexer::advance() {
    _pos++;
    if (_pos > _text.length() - 1) {
        _current_char = '\0';   // Indicates end of input
    } else {
        _current_char = _text[_pos];
    }
}

// Skip leading white space.
void Lexer::skip_whitespace() {
    while (_current_char != '\0' && isspace(_current_char)) {
        advance();
    }
}

class Interpreter {
public:
    Interpreter(Lexer& lexer);
    long expression();
private:
    Lexer&  _lexer;
    Token   _current_token; // current token instance

    void  eat(TOKENTYPE token_type);
    long  factor();
    long  term();
};

// Constructor
Interpreter::Interpreter(Lexer& lexer) : _lexer{lexer},
_current_token{_lexer.get_next_token()} {
}

// Arithmetic expression parser / interpreter.
// expr    : term ((PLUS | MINUS) term)*
// term    : factor ((MUL | DIV) factor)*
// factor  : INTEGER | LPAREN expr RPAREN
long Interpreter::expression() {
    long result = term();

    while (_current_token.type == TOKENTYPE::PLUS ||
    _current_token.type == TOKENTYPE::MINUS) {
        Token token = _current_token;
        if (token.type == TOKENTYPE::PLUS) {
            eat(TOKENTYPE::PLUS);
            result += term();
        } else if (token.type == TOKENTYPE::MINUS) {
            eat(TOKENTYPE::MINUS);
            result -= term();
        }
    }

    return result;
}

// compare the current token type with the passed token type and if they match
// then "eat" the current token and assign the next token to _current_token,
// otherwise raise an exception.
void Interpreter::eat(TOKENTYPE token_type) {
    if (_current_token.type == token_type) {
        _current_token = _lexer.get_next_token();
    } else {
        ostringstream out;
        out << "Error parsing input. Wanted: " << token_type;
        throw(out.str().c_str());
    }
}

// factor : INTEGER | LPAREN expr RPAREN
long Interpreter::factor() {
    Token token = _current_token;
    if (token.type == TOKENTYPE::LPAREN) {
        eat(TOKENTYPE::LPAREN);
        long result = expression();
        eat(TOKENTYPE::RPAREN);
        return result;
    } else {
        eat(TOKENTYPE::INTEGER);
        return token.value;
    }
}

// term : factor ((MUL | DIV) factor)*
long Interpreter::term() {
    long result = factor();
                                                                      
    while (_current_token.type == TOKENTYPE::MUL
    || _current_token.type == TOKENTYPE::DIV) {
        Token token = _current_token;
        if (token.type == TOKENTYPE::MUL) {
            eat(TOKENTYPE::MUL);
            result *= factor();
        } else if (token.type == TOKENTYPE::DIV) {
            eat(TOKENTYPE::DIV);
            long rhs = factor();
            if (rhs == 0) {
                throw("Division by zero");
            }
            result /= rhs;
        }
    }

    return result;
}

int main() {
    string text;
    while(cin) {
        cout << "calc> ";
        getline(cin, text);

        try {
            Lexer lexer(text);
            Interpreter interpreter(lexer);
            long result = interpreter.expression();
            cout << result << endl;
        }
        catch(const char* error) {
            cerr << error << endl;
            break;
        }
    }

    return EXIT_SUCCESS;
}