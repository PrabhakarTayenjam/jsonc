/**********************************************************************************
 *  This json validator uses a PUSH-DOWN AUTOMATA to validate the json string.
 * It is fast. Its validates in O(L) time, where L = lenght of json string
 * It uses an extra space of the depth of the json
 * Unicode support
 * There is no limit to the depth of the json
 **********************************************************************************/


#include "jsonvalidator.h"

#include <stdio.h>
#include <stdlib.h>

#define __ -1
#define POP_FAIL -1

struct stack
{
    int *array;
    int top;
    int capacity;
};

enum enum_classes
{
    c_space,        // space
    c_wspace,       // other whitespace \t \n \r
    c_lcurly,       // {
    c_rcurly,       // }
    c_lsqr,         // [
    c_rsqr,         // ]
    c_dquote,       // "
    c_colon,        // : colon
    c_comma,        // , comma
    c_zero,         // 0
    c_digit,        // 1-9
    c_plus,         // +
    c_minus,        // -
    c_point,        // .
    c_lowt,         // t
    c_lowr,         // r
    c_lowu,         // u
    c_lowe,         // e
    c_lowf,         // f
    c_lowa,         // a
    c_lowl,         // l
    c_lows,         // s
    c_lown,         // n
    c_lowb,         // b
    c_uni,          // cdABCDF
    c_upe,          // E
    c_bslash,       /* \ */
    c_fslash,       // /
    c_other,        // other characters

    total_class    
};

enum enum_states
{
    ST = 0, // STAT
    O1,     // { object start
    O2,     // } object end
    O3,     // object value end
    O4,     // object start after comma
    OV,     // object value
    CL,     // colon
    A1,     // [ array start
    A2,     // ] array end
    A3,     // array value end
    A4,     // array start after comma
    SR,     // string
    ZR,     // zero
    D1,     // number start
    EX,     // exponent
    FR,     // fraction
    NG,     // negative number
    D2,     // number after frac
    D3,     // number after exp
    ES,     // \ escape
    U1 ,    // unicode
    U2 ,    // unicode
    U3 ,    // unicode
    U4 ,    // unicode
    T1 ,    // t
    T2 ,    // tr
    T3 ,    // tru
    F1 ,    // f
    F2 ,    // fa
    F3 ,    // fal
    F4 ,    // fals
    N1 ,    // n
    N2 ,    // nu
    N3 ,    // nul

    total_states    
};

enum stack_states
{
    o1 = 1000,  // {
    o2,         // }
    a1,         // [
    a2,         // ]
    sr,         // "
    pp,         // true, false, null, num
    p4          // num, (comma just after a number)
};

enum modes
{
    m_st , // start
    m_obj,
    m_key,
    m_arr
};

enum enum_states transition_table[total_states][total_class] = {
/*       spc  ws  {   }   [   ]   "   :   ,   0  1-9  +   -   .   t   r   u   e   f   a   l   s   n   b   uni E  \   /  oth */
/* ST */ {ST, ST, o1, __, a1, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* O1 */ {O1, O1, __, o2, __, __, sr, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* O2 */ {O2, O2, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* O3 */ {O3, O3, __, o2, __, __, __, __, O4, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* O4 */ {O4, O4, __, __, __, __, sr, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* OV */ {OV, OV, o1, __, a1, __, sr, __, __, ZR, D1, __, NG, __, T1, __, __, __, F1, __, __, __, N1, __, __, __, __, __, __},
/* CL */ {CL, CL, __, __, __, __, __, OV, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* A1 */ {A1, A1, o1, __, a1, a2, sr, __, __, ZR, D1, __, NG, __, T1, __, __, __, F1, __, __, __, N1, __, __, __, __, __, __},
/* A2 */ {A2, A2, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* A3 */ {A3, A3, __, __, __, a2, __, __, A4, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* A4 */ {A4, A4, o1, __, a1, __, sr, __, __, ZR, D1, __, NG, __, T1, __, __, __, F1, __, __, __, N1, __, __, __, __, __, __},
/* SR */ {SR, __, SR, SR, SR, SR, sr, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, SR, ES, SR, SR},
/* ZR */ {__, __, __, __, __, __, __, __, p4, __, __, __, __, FR, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* D1 */ {pp, pp, __, o2, __, a2, __, __, p4, D1, D1, __, __, FR, __, __, __, EX, __, __, __, __, __, __, __, EX, __, __, __},
/* EX */ {__, __, __, __, __, __, __, __, __, D3, D3, D3, D3, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* FR */ {__, __, __, __, __, __, __, __, __, D2, D2, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* NG */ {__, __, __, __, __, __, __, __, __, D1, D1, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* D2 */ {pp, pp, __, __, __, __, __, __, p4, D2, D2, __, __, __, __, __, __, EX, __, __, __, __, __, __, __, EX, __, __, __},
/* D3 */ {pp, pp, __, __, __, __, __, __, p4, D3, D3, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* ES */ {__, __, __, __, __, __, SR, __, __, __, __, __, __, __, SR, SR, U1, __, SR, __, __, __, SR, SR, __, __, SR, SR, __},
/* U1 */ {__, __, __, __, __, __, __, __, __, U2, U2, __, __, __, __, __, __, U2, U2, U2, __, __, __, U2, U2, U2, __, __, __},
/* U2 */ {__, __, __, __, __, __, __, __, __, U3, U3, __, __, __, __, __, __, U3, U3, U3, __, __, __, U3, U3, U3, __, __, __},
/* U3 */ {__, __, __, __, __, __, __, __, __, U4, U4, __, __, __, __, __, __, U4, U4, U4, __, __, __, U4, U4, U4, __, __, __},
/* U4 */ {__, __, __, __, __, __, __, __, __, SR, SR, __, __, __, __, __, __, SR, SR, SR, __, __, __, SR, SR, SR, __, __, __},
/* T1 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, T2, __, __, __, __, __, __, __, __, __, __, __, __, __},
/* T2 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, T3, __, __, __, __, __, __, __, __, __, __, __, __},
/* T3 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, pp, __, __, __, __, __, __, __, __, __, __, __},
/* F1 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, F2, __, __, __, __, __, __, __, __, __},
/* F2 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, F3, __, __, __, __, __, __, __, __},
/* F3 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, F4, __, __, __, __, __, __, __},
/* F4 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, pp, __, __, __, __, __, __, __, __, __, __, __},
/* N1 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, N2, __, __, __, __, __, __, __, __, __, __, __, __},
/* N2 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, N3, __, __, __, __, __, __, __, __},
/* N3 */ {__, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, pp, __, __, __, __, __, __, __, __}
};

static enum enum_classes class_map[128] = {
    __,	__,	__,	__,	__,	__,	__,	__,	
    __,	c_wspace, c_wspace,	__,	__,	c_wspace, __, __,	
    __,	__,	__,	__,	__,	__,	__,	__,	
    __,	__,	__,	__,	__,	__,	__,	__,	
    c_space,    c_other,	c_dquote,	c_other,	c_other,	c_other,	c_other,	c_other,	
    c_other,	c_other,	c_other,	c_plus,	    c_comma,	c_minus,	c_point,	c_fslash,	
    c_zero,	    c_digit,	c_digit,	c_digit,	c_digit,	c_digit,	c_digit,	c_digit,	
    c_digit,	c_digit,	c_colon,	c_other,	c_other,	c_other,	c_other,	c_other,	
    c_other,	c_uni,  	c_uni,  	c_uni,	    c_uni,	    c_upe,	    c_uni,  	c_other,	
    c_other,	c_other,	c_other,	c_other,	c_other,	c_other,	c_other,	c_other,	
    c_other,	c_other,	c_other,	c_other,	c_other,	c_other,	c_other,	c_other,	
    c_other,	c_other,	c_other,	c_lsqr, 	c_bslash,	c_rsqr,	    c_other,	c_other,	
    c_other,	c_lowa,	    c_lowb,	    c_uni,	    c_uni,	    c_lowe,	    c_lowf,	    c_other,	
    c_other,	c_other,	c_other,	c_other,	c_lowl,	    c_other,	c_lown,	    c_other,	
    c_other,	c_other,	c_lowr, 	c_lows,	    c_lowt,	    c_lowu,	    c_other,	c_other,	
    c_other,	c_other,	c_other,	c_lcurly,	c_other,	c_rcurly,	c_other,	c_other
};

static void
stack_init(struct stack *s)
{
    (*s).top = -1;
    (*s).capacity = 4;
    (*s).array = (int*)malloc(s->capacity * sizeof(int));
}

static void
stack_push(struct stack *s, int elm)
{
    if(s == NULL)
        return;

    if((s->top + 1) >= s->capacity) {
        int realloc_size;
        if(s->capacity == 0)
            realloc_size = 1;
        else
            realloc_size = s->capacity * 2;

        (*s).array = realloc(s->array, realloc_size * sizeof(int) );
        (*s).capacity = realloc_size;
    }

    (*s).top += 1;
    (*s).array[s->top] = elm; 
}

static int
stack_pop(struct stack *s)
{
    if(s == NULL || s->top == -1) {
        return POP_FAIL;
    }
    return s->array[((*s).top)--];
}

static bool
stack_empty(struct stack s)
{
    if(s.top == -1)
        return true;
    return false;
}

static void
stack_free(struct stack s)
{
    free(s.array);
}

bool
json_is_valid(const char *json_str)
{
    int ptr = 0;
    char ch = 0;
    struct stack mode_stack;
    enum enum_classes cls;
    enum enum_states state;
    enum enum_states next_state;
    bool valid = true;
    enum modes mode;

    if(json_str == NULL)
        return false;
        
    stack_init(&mode_stack);
    state = ST;
    mode = m_st;

    ch = *json_str;
    while(ch != '\0') {
        if(ch >= 0 && ch < 128) // ascii
            cls = class_map[(int)ch];
        else // other than ascii
            cls = c_other;
        if(cls == __) {
            valid = false;
            break;
        }

        next_state = transition_table[state][cls];
        if(next_state == __) {
            valid = false;
            break;
        }
        if(next_state < 1000) 
            state = next_state;
        else {
            /* next state according to stack top */
            switch((enum stack_states)next_state) {
                case o1:
                    stack_push(&mode_stack, mode);
                    mode = m_obj;
                    state = O1;
                    break;
                case a2:
                case o2:
                    mode = stack_pop(&mode_stack);
                    if(mode == m_st) {
                        if((enum stack_states)next_state == o2)
                            state = O2;
                        else
                            state = A2;
                    }
                    else if(mode == m_obj)
                        state = O3;
                    else if(mode == m_arr)
                        state = A3;
                    else {
                        valid = false;
                        break;
                    }
                    break;
                case a1:
                    stack_push(&mode_stack, mode);
                    mode = m_arr;
                    state = A1;
                    break;
                case sr:
                    if(state == O1 || state == O4){
                        stack_push(&mode_stack, mode);
                        mode = m_key;
                        state = SR;
                    } else if(state == SR) {
                        if(mode == m_key) {
                            mode = stack_pop(&mode_stack);
                            state = CL;
                        } else if(mode == m_obj) {
                            state = O3;
                        } else if(mode == m_arr) {
                            state = A3;
                        }
                    } else {
                        state = SR;
                    }
                    break;
                case pp:
                    if(mode == m_obj)
                        state = O3;
                    else if(mode == m_arr)
                        state = A3;
                    else{
                        valid = false;
                        break;
                    }
                    break;
                case p4:
                    if(mode == m_obj)
                        state = O4;
                    else if(mode == m_arr)
                        state = A4;
                    else{
                        valid = false;
                        break;
                    }
                    break;
                default: {
                }             
            }
        }
        ptr++;
        ch = *(json_str + ptr);
    }

    if(!stack_empty(mode_stack) || (state != O2 && state != A2)) {
        valid = false;
    }

    stack_free(mode_stack);
    return valid;
}