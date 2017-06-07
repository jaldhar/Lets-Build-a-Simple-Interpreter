// calc2.cc -- c++ port of Ruslan Spivaks "Let's Build a Simple Interpreter"
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
    MINUS
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
//      Token(PLUS '+')
//
ostream& operator<<(ostream& out, const Token& t) {
    out << "Token(" << t.type << "," << (long)t.value << ")";
    return out;
}

class Interpreter {
public:
    Interpreter(string& text);
    long expression();
private:
    string& _text;          // client string input, e.g. "3+5"
    size_t  _pos;           // an index into _text
    char    _current_char;  // the character at _text[_pos]
    Token   _current_token; // current token instance

    void  advance();
    void  eat(TOKENTYPE token_type);
    Token get_next_token();
    long  integer();
    void  skip_whitespace();
};

// Constructor
Interpreter::Interpreter(string& text) : _text{text}, _pos{0},
_current_char{_text[_pos]}, _current_token{TOKENTYPE::ENDOFFILE, 0} {
}

// Parser/Interpreter
//
// expr -> INTEGER PLUS INTEGER
// expr -> INTEGER MINUS INTEGER
long Interpreter::expression() {

    // set current token to the first token taken from the input
    _current_token = get_next_token();

    // we expect the current token to be an integer
    Token left = _current_token;
    eat(TOKENTYPE::INTEGER);

    // we expect the current token to be a '+' or '-' token
    Token op = _current_token;
    if (op.type == TOKENTYPE::PLUS) {
        eat(TOKENTYPE::PLUS);
    } else {
        eat(TOKENTYPE::MINUS);
    }

    // we expect the current token to be an integer
    Token right = _current_token;
    eat(TOKENTYPE::INTEGER);
    // after the above call _current_token is set to ENDOFFILE token

    // at this point either the INTEGER PLUS INTEGER or the INTEGER MINUS
    // INTEGER sequence of tokens has been successfully found and the method can
    // just return the result of adding or subtracting two integers, thus
    // effectively interpreting client input
    if (op.type == TOKENTYPE::PLUS) {
        return left.value + right.value;
    } else {
        return left.value - right.value;
    }
}

// Advance the '_pos' pointer and set the '_current_char' variable.
void Interpreter::advance() {
    _pos++;
    if (_pos > _text.length() - 1) {
        _current_char = '\0';   // Indicates end of input
    } else {
        _current_char = _text[_pos];
    }
}

// compare the current token type with the passed token type and if they match
// then "eat" the current token and assign the next token to _current_token,
// otherwise raise an exception.
void Interpreter::eat(TOKENTYPE token_type) {
    if (_current_token.type == token_type) {
        _current_token = get_next_token();
    } else {
        ostringstream out;
        out << "Error parsing input. Wanted: " << token_type;
        throw(out.str().c_str());
    }
}

// Lexical analyzer (also known as scanner or tokenizer)
//
// This method is responsible for breaking a sentence apart into tokens. One
// token at a time.
//
Token Interpreter::get_next_token() {
 
    while (_current_char != '\0') {

        if (isspace(_current_char)) {
            skip_whitespace();
        }

        if (isdigit(_current_char)) {
            Token token(TOKENTYPE::INTEGER, integer());
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

        ostringstream out;
        out << "Error parsing input. Got: " << _current_char;
        throw(out.str().c_str());
    }

    Token token(TOKENTYPE::ENDOFFILE, '\0');
    cerr << token << endl;
    return token;
}

// Return a (multidigit) integer consumed from the input.
long Interpreter::integer() {
    string result("");

    while (_current_char != '\0' && isdigit(_current_char)) {
        result += _current_char;
        advance();
    }

    return stol(result);
}

// Skip leading white space.
void Interpreter::skip_whitespace() {
    while (_current_char != '\0' && isspace(_current_char)) {
        advance();
    }
}

int main() {
    string text;
    while(cin) {
        cout << "calc> ";
        getline(cin, text);

        try {
            Interpreter interpreter(text);
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