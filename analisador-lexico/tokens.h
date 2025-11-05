/* 
Trabalho de Compiladores - Analisador Léxico
nome: Graziele de Cassia Rodrigues 
matricula: 21.1.8120
data: 11/2025
*/


enum  {
    // Palavras reservadas
    DATA = 256, FUNC, CLASS, INSTANCE, FOR, IF, ELSE, ITERATE, RETURN_KW, NEW,
    TRUE_LIT, FALSE_LIT, NULL_LIT, MAIN, PRINT, TYCHR,

    // Tipos
    INT_TYPE, CHAR_TYPE, BOOL_TYPE, FLOAT_TYPE, VOID_TYPE,

    // Literais e identificadores
    INT, FLOAT, CHAR, TYID, ID,

    // Operadores compostos
    DOUBLE_COLON, ARROW, AND_OP, EQ, GT, GE, LT, LE, NE,

    // Operadores simples e símbolos
    ATTR, AMPERSAND, SEMICOLON,
    L_CHAVE, R_CHAVE,
    L_PARENTESE, R_PARENTESE,
    L_COLCHETE, R_COLCHETE,
    COMMA, DOT, COLON,
    NOT_OP, PLUS, MINUS, MULT, DIV, MOD
};

