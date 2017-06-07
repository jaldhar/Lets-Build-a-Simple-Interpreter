// interp.cc -- c++ port of Ruslan Spivaks "Let's Build a Simple Interpreter"
//              tutorial.
//
// By Jaldhar H. Vyas <jaldhar@braincells.com>
// Copyright (C) 2017, Consolidated Braincells Inc.  All rights reserved.
// "Do what thou wilt shall be the whole of the license."

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
    PLUS
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
    Token   _current_token; // current token instance

    void  eat(TOKENTYPE token_type);
    Token get_next_token();
};

// Constructor
Interpreter::Interpreter(string& text) : _text{text}, _pos{0},
_current_token{TOKENTYPE::ENDOFFILE, 0} {
}

// expr -> INTEGER PLUS INTEGER
long Interpreter::expression() {

    // set current token to the first token taken from the input
    _current_token = get_next_token();

    // we expect the current token to be a single-digit integer
    Token left = _current_token;
    eat(TOKENTYPE::INTEGER);

    // we expect the current token to be a '+' token
    Token op = _current_token;
    eat(TOKENTYPE::PLUS);

    // we expect the current token to be a single-digit integer
    Token right = _current_token;
    eat(TOKENTYPE::INTEGER);
    // after the above call _current_token is set to ENDOFFILE token

    // at this point INTEGER PLUS INTEGER sequence of tokens has been
    // successfully found and the method can just return the result of adding
    // two integers, thus effectively interpreting client input
    return left.value + right.value;
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
    // is _pos index past the end of _text ?
    // if so, then return ENDOFFILE token because there is no more
    // input left to convert into tokens
    if (_pos >= _text.length()) {
        Token token(TOKENTYPE::ENDOFFILE, 0);
        cerr << token << endl;
        return token;
    }

    // get a character at the position _pos and decide what token to create
    // based on the single character
    char current_char = _text[_pos];

    // if the next characters are digits then convert them to an integer, create
    // an INTEGER token, increment _pos index to point to the next character
    // after the digits, and return the INTEGER token
    if (isdigit(current_char)) {
        long total = 0;
        while (isdigit(_text[_pos])) {
            total *= 10;
            total += (_text[_pos] - '0');
            _pos++;
        }
        Token token(TOKENTYPE::INTEGER, total);
        cerr << token << endl;
        return token;
    }

    if (current_char == '+') {
        Token token(TOKENTYPE::PLUS, '+');
        cerr << token << endl;
        _pos++;
        return token;
    }

    ostringstream out;
    out << "Error parsing input. Got: " << current_char;
    throw(out.str().c_str());
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