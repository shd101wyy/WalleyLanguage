/*

    virtual machine
	opcode

	first element is opcode
	(def x 12)
	(set 0 0 (const INTEGER 12))

	(set! x 15)
	(set 0 0 (const INTEGER 12))

	(add 3 4) ;; suppose add is in 0 1
	(call 0 1 (set 1 0 (const INTEGER 3)) (set 1 1(const INTEGER 4))) ;; push frame

	(def (add a) a)
	(set 0 1 (param-num 1) (get 1 0)) ;; param num, get 1 0 save to accumulator
	*/
/*
  #include <stdio.h>
int main()
{
  long x = 0x7FFFFFFFFFFFFFFF;
  double y = (double)0.1234;
  long i = 0x3fbf972474538ef3;
  double * p = (double*)&i;

  printf("%ld\n\n", x);
  printf("%lx\n\n", *(long*)&y); // hexadecimal double
  printf("%lf\n\n", *p);        // get double from hexadecimal
  return 0;
}
*/
/*
	Contruct Toy Language Data Types
	*/
var TYPE_INTEGER = 1;
var TYPE_FLOAT = 2;
var TYPE_PAIR = 3;
var TYPE_VECTOR = 4;
var TYPE_OBJECT = 5;
var TYPE_STRING = 6;
var TYPE_LAMBDA = 7;
var TYPE_BUILTIN_LAMBDA = 8;
var TYPE_CONTINUATION = 9; // 新增 continuation data type
var TYPE_NULL = 0;
var Value = function() {
        this.type = 0; // this can be changed using set-data-type
        this.num; // int/float
        this.cons; // pair
        this.vector; // vector
        this.object; // object
        this.lambda; // lambda
        this.string; // string
        this.builtin_lambda; // builtin_lambda
        this.continuation; // continuation
        //this.user_defined_type = null; // user defined data type.
    }
var make_null = function() {
        var v = new Value();
        v.type = TYPE_NULL;
        return v;
    }

var make_string = function(s) {
        var v = new Value();
        v.type = TYPE_STRING;
        v.string = s;
        return v;
    }



var GLOBAL_NULL = make_null(); // global null/false
var GLOBAL_TRUE = make_string("true"); // global true
var Cons = function(car_, cdr_) {
        this.car = car_;
        this.cdr = cdr_;
    }
/*
  create cons data type
  */
var cons = function(v0, v1) {
        var v = new Value(); // create Value
        v.cons = new Cons(v0, v1);
        v.type = TYPE_PAIR;
        return v;
        // return new Cons(v0, v1);
    }
var Builtin_Primitive_Procedure = function(func) {
        this.func = func;
    }
var bpp = function(func) {
        var v = new Value();
        v.type = TYPE_BUILTIN_LAMBDA;
        v.builtin_lambda = func;
        return v;
        // return new Builtin_Primitive_Procedure(func)
    }; // create Builtin_Primitive_Procedure
var Lambda = function(param_num, variadic_place, start_pc, env) {
        this.param_num = param_num;
        this.variadic_place = variadic_place;
        this.start_pc = start_pc;
        this.env = env;
    }
/*
  create lambda data type
  */
var make_lambda = function(param_num, variadic_place, start_pc, env) {
        var l = new Lambda(param_num, variadic_place, start_pc, env);
        var v = new Value();
        v.type = TYPE_LAMBDA;
        v.lambda = l;
        return v;
    }
/*
    Continuation data type
*/
var Continuation = function(start_pc, env, frame_list, functions_list, continuation_return_pc, continuation_env){
    this.start_pc = start_pc; 
    this.env = env;
    this.frame_list = frame_list;
    this.functions_list = functions_list;
    this.continuation_return_pc = continuation_return_pc;
    this.continuation_env = continuation_env;
}

var make_continuation = function(start_pc, env, frame_list, functions_list, continuation_return_pc, continuation_env){
    var v = new Value();
    v.type = TYPE_CONTINUATION;
    v.continuation = new Continuation(start_pc, env, frame_list, functions_list, continuation_return_pc, continuation_env);
    return v;
}

/*
  this saved lambda is for tail call optimization and compilation
  */
var Lambda_for_Compilation = function(param_num, variadic_place, start_pc, vt) {
        this.param_num = param_num;
        this.variadic_place = variadic_place;
        this.start_pc = start_pc;
        this.vt = vt;
    }
/*
  create integer data type
  */
var make_integer = function(num) {
        var v = new Value();
        v.num = num;
        v.type = TYPE_INTEGER;
        return v;
    }
/*
  create float data type
  */
var make_float = function(num) {
        var v = new Value();
        v.num = num;
        v.type = TYPE_FLOAT;
        return v;
    }
var Macro = function(macro_name, clauses, variable_table
    , env) {
        this.macro_name = macro_name;
        this.clauses = clauses;
        this.vt = variable_table;
        this.env = env;
    }
/*
  注意。这里的environmen以
  [[1], ["hello"]...] 的形式储存数据
  C语言可用dereference
  */
/*
  var Environment = function(env)
  {
  this.env = env;
  this.esp = env.length - 1; // point to toppest element
  this.push = function(v)
  {
  this.env[this.esp + 1] = [v];
  this.esp += 1;
  }
  this.pop = function()
  {
  var v = this.env[this.esp][0];
  this.esp -= 1;
  return v;
  }
}*/
var car = function(o) {
        return o.cons.car;
        // return o.car;
    }
var cdr = function(o) {
        return o.cons.cdr;
        //return o.cdr;
    }
var cadr = function(o) {
        return car(cdr(o))
    };
var caddr = function(o) {
        return car(cdr(cdr(o)))
    };
var cadddr = function(o) {
        return car(cdr(cdr(cdr(o))))
    };
var cdddr = function(o) {
        return cdr(cdr(cdr(o)))
    };
var cddr = function(o) {
        return cdr(cdr(o))
    };
/*
  check whether string is number
  */

function isNumber(n) {
    return !isNaN(parseFloat(n)) && isFinite(n);
}
/*
  check whether string is integer
  */
var isInteger = function(n) {
        if (n.length == 0) return false;
        if (n[0] == "-") n = n.slice(1);
        return (n === "0" || /^[1-9][0-9]*$/.test(n) || /^0x[0-9A-F]*$/.test(n) || /^0[1-9][0-9]*$/.test(n))
    }
var isFloat = function(n) {
        return isNumber(n) && !(isInteger(n))
    }
var formatQuickAccess = function(ns, keys) {
        var formatQuickAccess_iter = function(keys, output, count) {
                if (count === keys.length) return output;
                return formatQuickAccess_iter(keys, cons(output, cons(cons(make_string("quote"), cons(make_string(keys[count]), GLOBAL_NULL)), GLOBAL_NULL)), count + 1);
            }
        return formatQuickAccess_iter(keys,
				      cons(make_string(ns),
					   cons(cons(make_string("quote"), 
						     cons(make_string(keys[0]), 
							  GLOBAL_NULL)),
						GLOBAL_NULL)),
				      1);
    }

// new lexer in order to support (add 3 4) <=> add[3, 4]
var new_lexer = function(input_string){
    var i = 0;
    var string_length = input_string.length;
    var output_list = [];
    for(i = 0; i < string_length; i++){
        if(input_string[i] === "("){
            output_list.push("(");
        }
        else if (input_string[i] === ")"){
            output_list.push(")");
        }
        else if (input_string[i] == " " || input_string[i] == "\n" || input_string[i] == "\t" || input_string[i] == ","){
            continue;
        }
        else if (input_string[i] == "#" && (input_string[i + 1] == "[" || input_string[i + 1] == "(")) // vector
        {
            output_list.push("(");
            output_list.push("vector");
            i++;
        }
        else if (input_string[i] == "[" || input_string[i] == "{"){
            if(i!== 0 && 
             (input_string[i - 1] != " " && input_string[i - 1] != "\n"  && input_string[i - 1] != "\t"
              && input_string[i-1] != "'" && input_string[i-1]!="`" && input_string[i-1]!="~"
             && input_string[i - 1]!="(" && input_string[i - 1]!="{" && input_string[i - 1]!="[")){
                if(output_list[output_list.length - 1]!==")"){ // +[  => ( +
                    var t = output_list[output_list.length - 1];
                    output_list[output_list.length - 1] = "(";
                    output_list.push(t);
                }
                else{ // ahead is )
                    var count = 1;
                    var start_index = 0;
                    for(var j = output_list.length - 2; j >= 0; j--){
                        if(input_string[j] == ")"){
                            count++ ;
                        }
                        else if(input_string[j] == "("){
                            count--;
                            if(count == 0){
                                start_index = j;
                                break;
                            }
                        }
                    }
                    output_list.push(output_list[output_list.length - 1]); // save last 
                    for(var j = output_list.length - 2; j != start_index; j--){
                        output_list[j] = output_list[j - 1]; // move elements back
                    }
                    output_list[j] = "(";
                }
            }
            else{
                if(input_string[i] === "[")
                    output_list.push("(");
                else{
                    output_list.push("(");
                    output_list.push("object");
                }
            }
        }
        /*
        else if (input_string[i] == "{"){
            output_list.push("(");
            output_list.push("object");
        }
        */
        else if (input_string[i] == "]" || input_string[i] == "}"){
            output_list.push(")")
        }
        else if (input_string[i] == "~" && input_string[i + 1] == "@"){
            output_list.push("~@")
            i++;
        }
        else if (input_string[i] == "'" || input_string[i] == "`" || input_string[i] == "~"){
            output_list.push(input_string[i]);
        }
        else if (input_string[i] == ";") { // comment
            while (i != input_string.length) {
                if (input_string[i] == "\n") break;
                i++;
            }
        } 
        else if (input_string[i] === '"') {
            var a = i + 1;
            while (a != input_string.length) {
                if (input_string[a] === "\\") {
                    a = a + 2;
                    continue;
                }
                if (input_string[a] === "\"") break;
                a++
            }
            output_list.push(input_string.slice(i, a + 1))
        }
        else{
            var end = i;
            var s_;
            while (true) {
                if (end === input_string.length || input_string[end] === " " || input_string[end] === "\n" || input_string[end] === "\t" || input_string[end] === "," || input_string[end] === ")" || input_string[end] === "(" || input_string[end] === "]" || input_string[end] === "[" || input_string[end] === "{" || input_string[end] === "}" || input_string[end] === "'" || input_string[end] === "`" || input_string[end] === "~" || input_string[end] === ";") break;
                end += 1;
            }
            s_ = input_string.slice(i, end);
            output_list.push(s_);
            i = end - 1;
        }
    }
    return output_list;
}

var new_parser_get_tag = function(i){
    var tag;
    if (i === "'") tag = "quote"
    else if (i === "~") tag = "unquote"
    else if (i === "~@") tag = "unquote-splice"
    else tag = 'quasiquote';
    return make_string(tag);
}
var new_parser = function(l){
    var current_list_pointer = GLOBAL_NULL;
    var lists = cons(GLOBAL_NULL, GLOBAL_NULL);
    var temp;
    for(var i = l.length - 1; i >= 0; i--){
        if(l[i] === ")"){
            lists = cons(current_list_pointer, lists); // save current lists
            current_list_pointer = GLOBAL_NULL; // reset current_list_pointer
        }
        else if (l[i] === "("){
            //lists = cdr(lists); // pop top pointer
            if(i!==0 && 
                (l[i-1] === "~@" || l[i-1] === "'" || l[i-1] === "~" || l[i-1] === "`")){
                temp = cons(cons(new_parser_get_tag(l[i-1]), 
                                        cons(current_list_pointer, GLOBAL_NULL))
                                   ,car(lists));
                current_list_pointer = temp;
                i--;
            }
            else{
                temp = cons(current_list_pointer, car(lists)); // append list
                current_list_pointer = temp;
            }
            lists = cdr(lists);
        }
        else{
            // check Math:add like (Math 'add)
            if (l[i][0] === '"') temp = l[i];
            var splitted_ = l[i].split(":");
            if (l[i] === ":" || splitted_.length == 1 || l[i][0] === ":" || l[i][l[i].length - 1] === ":") //  : :abc abc:
            {
                // 暂不支持 "" 
                if(isInteger(l[i])){
                    temp = make_integer(parseInt(l[i]));
                }
                else if (isFloat(l[i])){
                    temp = make_float(parseFloat(l[i]));
                }
                else{
                    temp = make_string(l[i]);
                }
            }
            else{
                var ns = splitted_[0]; // eg x:a => ns 'x'  keys ['a']
                var keys = splitted_.slice(1);
                var formatted_ = formatQuickAccess(ns, keys); // eg x:a => (x :a)
                temp = formatted_;
            }
            if(i!==0 && 
                (l[i-1] === "~@" || l[i-1] === "'" || l[i-1] === "~" || l[i-1] === "`")){
                current_list_pointer = cons(cons(new_parser_get_tag(l[i - 1]), 
                                                cons(temp,
                                                     GLOBAL_NULL)),
                                            current_list_pointer);
                i--;
            }
            else{
                current_list_pointer = cons(temp, current_list_pointer);
            }
        }
    }
    return current_list_pointer;
    // return car(lists);
}

// print list
var new_parser_debug = function(p){
    var output_string = "(";
    var v;
    while(p!==GLOBAL_NULL){
        v = car(p);
        if(v.type === TYPE_PAIR || v === GLOBAL_NULL){
            output_string += new_parser_debug(v);
        }
        else{
            switch (v.type){
                case TYPE_STRING:
                    output_string += v.string   
                    break;
                case TYPE_INTEGER: case TYPE_FLOAT:
                    output_string += v.num;
                    break;
                case TYPE_NULL:
                    output_string += "()";
                    break;
                }
        }
        output_string += " ";
        p = cdr(p);
    }
    output_string+=")";
    return output_string;
}

/*
  Opcode
  */
var SET = 0x0;
var GET = 0x1;
var CONST = 0x2;
var CONST_INTEGER = 0x2100;
var CONST_FLOAT = 0x2200;
var CONST_STRING = 0x2300;
var CONST_NULL = 0x2400;
var CONST_LOAD = 0x2500; // load constant
var MAKELAMBDA = 0x3;
var RETURN = 0x4;
var NEWFRAME = 0x5;
var PUSH_ARG = 0x6;
var CALL = 0x7;
var JMP = 0x8;
var TEST = 0x9;
var PUSH = 0xA;
var INSTRUCTIONS = []; // global variables. used to save instructions
var INSTRUCTIONS_PC = 0; // global variables, used to save current pc location
var CONSTANT_TABLE = {"true":0}; // use to save string index
var CONSTANT_TABLE_LENGTH = 1; //
var _4_digits_hex = function(num) {
        return ("0000" + num.toString(16)).substr(-4);
    }
var printInstructions = function() {
        var num = 1;
        var str = "";
        for (var i = 0; i < INSTRUCTIONS.length; i++) {
            str = str + _4_digits_hex(INSTRUCTIONS[i]) + " ";
            if (num % 4 == 0) {
                console.log(str);
                str = "";
            }
            num++;
        }
        console.log(str);
    }
    // GLOBAL VARIABLES
    // used to save variable name
var VARIABLE_TABLE = [
// frame 0
["cons", "car", "cdr", "vector", "vector-ref", "vector-set!", "vector-length", "vector?", "+", "-", "*", "/", "=", "<", ">", "<=", ">=", "eq?", "string?", "int?", "float?", "pair?", "null?", "string<?", "string=?", "string-ref", "string-slice", "string-length", "vector-slice", "acos", "acosh", "asin", "asinh", "atan", "atanh", "ceil", "cos", "cosh", "exp", "floor", "loge", "pow", "sin", "sinh", "tan", "tanh", "display-string", "->int", "->float", "int->string", "float->string", "string-append", "lambda?", "vector-push!", "vector-pop!", "object", "object?", "object-keys", "bitwise-and", "bitwise-or", "bitwise-<<", "bitwise->>", "bitwise-not", "bitwise-xor", "string->char-code", "char-code->string", "int->string-16", "string->int", "string->float", "apply", "call/cc"]];
var MACROS = [
    []
]; // used to save macros
var BUILTIN_PRIMITIVE_PROCEDURE_NUM = VARIABLE_TABLE[0].length;
// variable environment
var ENVIRONMENT = [
// frame 0
[
bpp(function(stack_param, param_start) { // 0 cons
    return cons(stack_param[param_start+0], stack_param[param_start+1]);
    //return new Cons(stack_param[0], stack_param[1]);
}), bpp(function(stack_param, param_start) { // 1 car
    return car(stack_param[param_start+0]);
}), bpp(function(stack_param, param_start) { // 2 cdr
    return cdr(stack_param[param_start+0]);
}), bpp(function(stack_param, param_start) { // 3 vector
    var v = new Value();
    v.type = TYPE_VECTOR;
    v.vector = stack_param.slice(param_start);
    return v;
}), bpp(function(stack_param, param_start) { // 4 vector-ref
    return stack_param[param_start+0].vector[stack_param[param_start+1].num];
}), bpp(function(stack_param,param_start) { // 5 vector-set!
    stack_param[param_start+0].vector[stack_param[param_start+1].num] = stack_param[param_start+2];
    return stack_param[param_start+2];
}), bpp(function(stack_param,param_start) { // 6 vector-length
    return make_integer(stack_param[param_start+0].vector.length)
}), bpp(function(stack_param,param_start) { // 7 vector?
    if (stack_param[param_start+0].type === TYPE_VECTOR) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 8 +
    if (stack_param[param_start+0].type === TYPE_FLOAT || stack_param[param_start+1].type === TYPE_FLOAT) return make_float(stack_param[param_start+0].num + stack_param[param_start+1].num);
    return make_integer(stack_param[param_start+0].num + stack_param[param_start+1].num);
}), bpp(function(stack_param,param_start) { // 9 -
    if (stack_param[param_start+0].type === TYPE_FLOAT || stack_param[param_start+1].type === TYPE_FLOAT) return make_float(stack_param[0+param_start].num - stack_param[param_start+1].num);
    return make_integer(stack_param[0+param_start].num - stack_param[1+param_start].num);
}), bpp(function(stack_param,param_start) { // 10 *
    if (stack_param[param_start+0].type === TYPE_FLOAT || stack_param[param_start+1].type === TYPE_FLOAT) return make_float(stack_param[param_start+0].num * stack_param[param_start+1].num);
    return make_integer(stack_param[param_start+0].num * stack_param[param_start+1].num);
}), bpp(function(stack_param,param_start) { // 11 /
    if (stack_param[param_start+0].type === TYPE_FLOAT || stack_param[param_start+1].type === TYPE_FLOAT) return make_float(stack_param[param_start+0].num / stack_param[param_start+1].num);
    return make_integer(stack_param[param_start+0].num / stack_param[param_start+1].num);
}), bpp(function(stack_param,param_start) { // 12 = only for number
    if (stack_param[0+param_start].num == stack_param[1+param_start].num) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 13 < only for number
    if (stack_param[0+param_start].num < stack_param[1+param_start].num) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 14 > only for number
    if (stack_param[0+param_start].num > stack_param[1+param_start].num) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 15 <= only for number
    if (stack_param[0+param_start].num <= stack_param[1+param_start].num) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 16 >= only for number
    if (stack_param[0+param_start].num >= stack_param[1+param_start].num) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 17 eq?
    if ((stack_param[0+param_start].type === TYPE_INTEGER || stack_param[0+param_start].type === TYPE_FLOAT) // check number
    && (stack_param[1+param_start].type === TYPE_INTEGER || stack_param[1+param_start].type === TYPE_FLOAT)) {
        if (stack_param[0+param_start].num === stack_param[1+param_start].num) return GLOBAL_TRUE;
        return GLOBAL_NULL;
    }
    //else if (stack_param[0].type === TYPE_STRING && stack_param[1].type === TYPE_STRING)
    //return stack_param[0].string === stack_param[1].string ? GLOBAL_TRUE:GLOBAL_NULL;
    return stack_param[0+param_start] === stack_param[1+param_start] ? GLOBAL_TRUE : GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 18 string?
    if (stack_param[0+param_start].type === TYPE_STRING) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 19 int?
    if (stack_param[0+param_start].type === TYPE_INTEGER) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 20 float?
    if (stack_param[0+param_start].type === TYPE_FLOAT) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 21 pair?
    if (stack_param[0+param_start].type === TYPE_PAIR) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) { // 22 null?
    if (stack_param[0+param_start].type === TYPE_NULL) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) {
    // 23 string<?
    if (stack_param[0+param_start].string < stack_param[1].string) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) {
    // 24 string=?
    if (stack_param[0+param_start].string === stack_param[1].string) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) {
    // 25 string-ref
    var v = new Value();
    v.type = TYPE_STRING;
    v.string = stack_param[0+param_start].string[stack_param[1+param_start].num];
    return v;
}), bpp(function(stack_param,param_start) {
    // 26 string-slice
    var v = new Value();
    v.type = TYPE_STRING;
    v.string = stack_param[0+param_start].string.slice(stack_param[1+param_start].num, stack_param[2+param_start].num);
    return v;
}), bpp(function(stack_param,param_start) {
    // 27 string-length
    return make_integer(stack_param[0+param_start].string.length);
}), bpp(function(stack_param,param_start) {
    // 28 vector-slice
    return stack_param[0+param_start].vector.slice(stack_param[1+param_start].num, stack_param[2+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 29 acos
    return make_float(Math.acos(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 30 acosh
    return make_float(Math.acosh(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 31 asin
    return make_float(Math.asin(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 32 asinh
    return make_float(Math.asinh(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 33 atan
    return make_float(Math.atan(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 34 atanh
    return make_float(Math.atanh(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 35 ceil
    return make_float(Math.ceil(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 36 cos
    return make_float(Math.cos(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 37 cosh
    return make_float(Math.cosh(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 38 exp
    return make_float(Math.exp(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 39 floor
    return make_float(Math.floor(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 40 loge
    return make_float(Math.log(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 41 pow
    return make_float(Math.pow(stack_param[0+param_start].num, stack_param[1+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 42 sin
    return make_float(Math.sin(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 43 sinh
    return make_float(Math.sinh(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 44 tan
    return make_float(Math.tan(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 45 tanh
    return make_float(Math.tanh(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 46 display-string
    console.log(stack_param[0+param_start].string);
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) {
    // 47 ->int
    return make_integer(parseInt(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 48 ->float
    return make_float(parseFloat(stack_param[0+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 49 int->string
    var v = new Value();
    v.type = TYPE_STRING;
    v.string = "" + stack_param[0+param_start].num;
    return v;
}), bpp(function(stack_param,param_start) {
    // 50 float->string
    var v = new Value();
    v.type = TYPE_STRING;
    v.string = "" + stack_param[0+param_start].num;
    return v;
}), bpp(function(stack_param,param_start) {
    // 51 string-append
    var v = new Value();
    v.type = TYPE_STRING;
    v.string = stack_param[0+param_start].string + stack_param[1+param_start].string;
    return v;
}), bpp(function(stack_param,param_start) {
    // 52 lambda?
    if (stack_param[0+param_start].type === TYPE_LAMBDA || stack_param[0+param_start].type === TYPE_BUILTIN_LAMBDA) return GLOBAL_TRUE;
    return GLOBAL_NULL;
}), bpp(function(stack_param,param_start) {
    // 53 vector-push!
    stack_param[0+param_start].vector.push(stack_param[1+param_start]);
    return stack_param[0+param_start];
}), bpp(function(stack_param,param_start) {
    // 54 vector-pop!
    var c = stack_param[0+param_start].vector.pop();
    return c;
}), bpp(function(stack_param,param_start) {
    // 55 object
    var key;
    var value;
    //var output = {type: "object"}; // preserved
    var output = {};
    for (var i = param_start; i < stack_param.length; i = i + 2) {
        key = stack_param[i];
        if (stack_param[i].type !== TYPE_STRING) {
            console.log("ERROR: object invalid key");
            return GLOBAL_NULL;
        }
        value = stack_param[i + 1];
        output[key.string] = value;
    }
    var v = new Value();
    v.type = TYPE_OBJECT;
    v.object = output;
    return v;;
}), bpp(function(stack_param,param_start) {
    // 56 object?
    return stack_param[0+param_start].type === TYPE_OBJECT ? GLOBAL_TRUE : GLOBAL_NULL;
}), bpp(function(stack_param,param_start) {
    // 57 object-keys
    // return cons
    var c = GLOBAL_NULL;
    var keys = Object.keys(stack_param[0+param_start].object);
    for (var i = keys.length - 1; i >= 0; i--) {
        var v_ = new Value();
        v_.type = TYPE_STRING;
        v_.string = keys[i];
        c = cons(v_, c);
    }
    return c;
    //return Object.keys(stack_param[0].object)
}), bpp(function(stack_param,param_start) {
    // 58 bitwise-and
    return make_integer(stack_param[0+param_start].num & stack_param[1+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 59 bitwise-or
    return make_integer(stack_param[0+param_start].num | stack_param[1+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 60 bitwise-<<
    return make_integer(stack_param[0+param_start].num << stack_param[1+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 61 bitwise->>
    return make_integer(stack_param[0+param_start].num >> stack_param[1+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 62 bitwise-not
    return make_integer(~stack_param[0+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 63 bitwise-xor
    return make_integer(stack_param[0+param_start].num ^ stack_param[1+param_start].num);
}), bpp(function(stack_param,param_start) {
    // 64 string->char-code
    return make_integer(stack_param[0+param_start].string.charCodeAt(stack_param[1+param_start].num));
}), bpp(function(stack_param,param_start) {
    // 65 char-code->string
    var v = new Value();
    v.type = TYPE_STRING;
    v.string = String.fromCharCode(stack_param[0+param_start].num);
    return v;
}), bpp(function(stack_param,param_start) {
    // 66 	int->string-16
    return make_string(stack_param[0+param_start].num.toString(16));
}), bpp(function(stack_param,param_start) {
    // 67 string->int
    return make_integer(parseInt(stack_param[0+param_start].string));
}), bpp(function(stack_param,param_start) {
    // 68 string->float
    return make_integer(parseFloat(stack_param[0+param_start].string));
})                   // number 0 is for GET instruction
  , make_integer(1) // 69 apply
  , make_integer(2) // 70 call/cc
]];
var vt_find = function(vt, var_name) // find variable
    {
        for (var i = vt.length - 1; i >= 0; i--) {
            var frame = vt[i];
            for (var j = frame.length - 1; j >= 0; j--) {
                if (frame[j] === var_name) {
                    return [i, j];
                }
            }
        }
        return [-1, -1];
    }
var list_to_array = function(l) // convert list to array
    {
        var return_array = [];
        while (l.type !== TYPE_NULL) {
            //if(car(l) instanceof Value)
            //    return_array.push(list_to_array(car(l)));
            //else
            return_array.push(car(l));
            l = cdr(l);
        }
        return return_array;
    }
/*
  list_append:
  '(3 4) '(5 6) => '(3 4 5 6)
  */
var list_append = function(a, b) {
        if (a.type === TYPE_NULL) return b;
        return cons(car(a), list_append(cdr(a), b));
    }
/*
  check whether macro_match
  eg:
  [a b] [3 4] matches
  */
var macro_match = function(a, b) {
        var macro_match_iter = function(a, b, output) {
                if (a.type === TYPE_NULL && b.type === TYPE_NULL) return output; // finish matching
                else if ((a.type === TYPE_NULL && b.type !== TYPE_NULL) || (a.type !== TYPE_NULL && b.type === TYPE_NULL)) // doesnt match
                return false;
                else if (car(a).type === TYPE_PAIR && car(b).type === TYPE_PAIR) {
                    var m = macro_match_iter(car(a), car(b), output);
                    if (m === false) return false; // doesnt match
                    return macro_match_iter(cdr(a), cdr(b), output);
                } else if (car(a).type === TYPE_PAIR && !(car(b).type === TYPE_PAIR)) return false;
                else {
                    if (car(a).string[0] === "#") { // constant
                        if (car(a).string.slice(1) === car(b).string) return macro_match_iter(cdr(a), cdr(b), output);
                        else return false;
                    }
                    if (car(a).string === ".") {
                        output[cadr(a).string] = b;
                        return output;
                    }
                    output[car(a).string] = car(b);
                    return macro_match_iter(cdr(a), cdr(b), output);
                }
            }
        var output = {};
        return macro_match_iter(a, b, output);
    }
/*
  eg (* ~x ~x) {:x 12} => (* 12 12)
  */
var macro_expand_with_arg_value = function(body, t) {
        if (body.type === TYPE_NULL) return GLOBAL_NULL;
        else if (car(body).string === "unquote") {
            if (cadr(body).string in t) {
                return t[cadr(body)];
            }
            return body;
        } else if ((car(body).type === TYPE_PAIR) && (car(car(body)).string === "unquote-splice")) {
            var n = cadr(car(body)).string;
            if (n in t) {
                var v = t[n];
                return list_append(v, macro_expand_with_arg_value(cdr(body), t));
            }
            return cons(body, macro_expand_with_arg_value(cdr(body), t));
        } else if (car(body).type === TYPE_PAIR) { // cons
            return cons(macro_expand_with_arg_value(car(body), t), macro_expand_with_arg_value(cdr(body), t));
        }
        return cons(car(body), macro_expand_with_arg_value(cdr(body), t))
    }
var macro_expand = function(macro, exps) {
        var clauses = macro.clauses;
        while (clauses.type !== TYPE_NULL) {
            var match = macro_match(car(car(clauses)), exps);
            if (match === false) {
                clauses = cdr(clauses);
                continue;
            } else { // match
                return macro_expand_with_arg_value(cadr(car(clauses)), match);
            }
        }
        console.log("ERROR: Macro: " + macro.macro_name + " expansion failed");
        return GLOBAL_NULL;
    }

var macro_expand_for_compilation = function(macro, exps, macros) {
        var clauses = macro.clauses;
        while (clauses.type !== TYPE_NULL) {
            var match = macro_match(car(car(clauses)), exps);
            if (match === false) {
                clauses = cdr(clauses);
                continue;
            } else { // match
                /*
                    eg (defm square [x] `(* ~x ~x)) (square 12)
                    在macro_match之后得到了 match {x: 12}
                */
                var expanded_value;
                var new_vt = macro.vt.slice(0);
                var new_env = macro.env.slice(0);
                var new_vt_frame = [];
                var new_env_frame = [];
                new_vt.push(new_vt_frame);
                new_env.push(new_env_frame);
                for(i in match){
                    new_vt_frame.push(i);
                    new_env_frame.push(match[i]);
                }
                var start_pc = INSTRUCTIONS.length;
                compiler(cadr(car(clauses)),  // compile instructions
                         new_vt, 
                         macros,
                         false,
                         null,
                         null,
                         new_env);
                expanded_value = VM(INSTRUCTIONS, new_env, start_pc);
                console.log(new_parser_debug(expanded_value));
                // macro expansion replacement
                var macro_expansion_replacement = function(expanded_value, vt, is_head){
                    if(expanded_value.type === GLOBAL_NULL)
                        return GLOBAL_NULL;
                    if(expanded_value.type !== TYPE_PAIR) return expanded_value;
                    var v = car(expanded_value);
                    if(v.type === TYPE_PAIR)
                        return cons(macro_expansion_replacement(v, vt, true),
                                    macro_expansion_replacement(cdr(expanded_value), vt, false));
                    else{
                        if(v.type === TYPE_STRING && is_head){ // check in vt
                            if (v.string === "quote"){ // is quote function, so no replacement
                                return expanded_value;
                            }
                            var find = vt_find(vt, v.string);
                            if(find[0] !== -1) // find
                                return cons(cons(make_integer(0), cons(make_integer(find[0]), cons(make_integer(find[1]), GLOBAL_NULL))) ,
                                            macro_expansion_replacement(cdr(expanded_value), vt, false));
                            else
                                return cons(v, macro_expansion_replacement(cdr(expanded_value), vt, false));
                        }
                        else
                            return cons(v, macro_expansion_replacement(cdr(expanded_value), vt, false));
                    }
                }
                INSTRUCTIONS = INSTRUCTIONS.slice(0, start_pc); 
                return macro_expansion_replacement(expanded_value, macro.vt, true);
            }
        }
        console.log("ERROR: Macro: " + macro.macro_name + " expansion failed");
        return GLOBAL_NULL;
    }
/*
  parent func name:
  eg (def (f n) (f 12)) when calling (f 12), its parent_func_name is "f"
  (def (f1 n) (f2 12)) when calling (f2 12), its parent_func_name is "f1"
  */
var compiler = function(l, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env) {
        if(typeof(l) === "string"){
            console.log("COMPILER L ERROR");
            return;
        }
        switch(l.type){
            case TYPE_NULL:
                INSTRUCTIONS.push(CONST_NULL); // push null
                return;
            case TYPE_STRING:
                if(l.string[0] === '"'){ // string
                    var s = eval(l.string);
                    if (s in CONSTANT_TABLE) { // already defined
                        INSTRUCTIONS.push(CONST_LOAD); // load from table
                        INSTRUCTIONS.push(CONSTANT_TABLE[s]);
                        return;
                    } else { // string is not defined
                        CONSTANT_TABLE[s] = CONSTANT_TABLE_LENGTH;
                        CONSTANT_TABLE_LENGTH++;
                    }
                    var length = s.length;
                    INSTRUCTIONS.push(CONST_STRING); // create string
                    INSTRUCTIONS.push(length + 1); // push string length
                    var find_end = false;
                    for (var i = 0; i < length; i = i + 2) {
                        if (i + 1 === length) {
                            INSTRUCTIONS.push(s.charCodeAt(i) << 8 & 0xFF00);
                            find_end = true;
                            break;
                        } else {
                            INSTRUCTIONS.push(s.charCodeAt(i) << 8 | s.charCodeAt(i + 1));
                        }
                    }
                    if (find_end === false) {
                        INSTRUCTIONS.push(0x0000); // add end
                    }
                    return;
                }
                else{
                    // it is not number
                    // get variable index
                    var index = vt_find(vt, l.string);
                    if (index[0] == -1) {
                        INSTRUCTIONS = []; // clear instructions
                        console.log("ERROR: undefined variable: " + l.string);
                        return;
                    }
                    INSTRUCTIONS.push(GET << 12 | index[0]); // frame index
                    INSTRUCTIONS.push(index[1]); // value index
                    return;
                }
            case TYPE_INTEGER: // integer
                var i = l.num;
                if (i < 0) {
                    return compiler(cons(make_string("-"), 
                                         cons(make_integer(0), cons(l.slice(1), GLOBAL_NULL))), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env)
                }
                INSTRUCTIONS.push(CONST_INTEGER);
                INSTRUCTIONS.push((0xFFFF0000 & i) >> 16);
                INSTRUCTIONS.push(0xFFFF & i);
                return;
                /*
                First 16 bits: integer
                Second 16 bits: float
                */
            case TYPE_FLOAT: // float
                var f = l.num;
                if (f < 0) {
                    return compiler(cons(make_string("-"), cons(make_integer(0), cons(l.slice(1), GLOBAL_NULL))), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env)
                }
                var i = parseInt(f);
                INSTRUCTIONS.push(CONST_FLOAT);
                INSTRUCTIONS.push(i & 0x0000FFFF);
                // decimals 10 digits
                // 10 ^ 10
                var d = parseInt((f - i) * /*Math.pow(10, 9) 1000000000*/ 10000);
                INSTRUCTIONS.push(d & 0x0000FFFF);
                return;
            case TYPE_PAIR:
                if(car(l).type === TYPE_INTEGER && car(l).num === 0){ // set
                    INSTRUCTIONS.push(GET << 12 | (0x0FFF & cadr(l).num)); // frame index
                    INSTRUCTIONS.push(0x0000FFFF & caddr(l).num); // value index
                    return;
                }
                var tag = car(l).string;
                if(tag === "quote"){
                    var v = cadr(l);
                    // check integer float string null
                    if (v.type === TYPE_NULL || v.type === TYPE_INTEGER || v.type === TYPE_FLOAT /*|| v.type === TYPE_STRING*/) return compiler(v, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    else if (v.type === TYPE_PAIR) // pair
                    {
                        var quote_list = function(l) {
                                if (l.type === TYPE_NULL) return GLOBAL_NULL;
                                var v = car(l);
                                if (v.type === TYPE_PAIR) return cons(make_string("cons"), cons( /*cons(*/ quote_list(v) /*, null)*/ , cons(quote_list(cdr(l)), GLOBAL_NULL)));
                                else if (v.type === TYPE_STRING && v.string === ".") return cons(make_string("quote"), cons(cadr(l), GLOBAL_NULL));
                                return cons(make_string("cons"), cons(cons(make_string("quote"), cons(v, GLOBAL_NULL)), cons(quote_list(cdr(l)), GLOBAL_NULL)));
                            }
                        return compiler(quote_list(v), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    }
                    // symbol/string
                    else if (v.string[0] != '"') {
                        v = make_string('"' + v.string + '"')
                        return compiler(v, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    }
                    return compiler(v, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);;
                }
                else if (tag === "quasiquote"){
                    var v = cadr(l);
                    // check integer
                    // check integer float string null
                    if(v.type === TYPE_NULL || v.type === TYPE_INTEGER || v.type === TYPE_FLOAT /*|| v.type === TYPE_STRING*/) return compiler(v, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    else if (v.type === TYPE_PAIR) // pair
                    {
                        var quasiquote = function(l) {
                                if (l.type === TYPE_NULL) return GLOBAL_NULL;
                                var v = car(l);
                                //if(typeof(v) === "string" && v[0] === '"') v = eval(v);
                                if (v.type === TYPE_PAIR) {
                                    if (car(v).type == TYPE_STRING && car(v).string === "unquote") return cons(make_string("cons"), cons(cadr(v), cons(quasiquote(cdr(l)), GLOBAL_NULL)));
                                    else if (car(v).type == TYPE_STRING && car(v) === "unquote-splice") return cons(make_string("append"), cons(cadr(v), cons(quasiquote(cdr(l)), GLOBAL_NULL)));
                                    return cons(make_string("cons"), cons(quasiquote(v), cons(quasiquote(cdr(l)), GLOBAL_NULL)));
                                } else if (v.type === TYPE_STRING && v === ".") return cons(make_string("quote"), cons(cadr(l), GLOBAL_NULL));
                                    return cons(make_string("cons"),
                                                cons(cons(make_string("quote"),
                                                          cons(v, GLOBAL_NULL)),
                                                     cons(quasiquote(cdr(l)),
                                                          GLOBAL_NULL)));
                            }
                        return compiler(quasiquote(v), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    }
                    // symbol/string
                    else if (v.string[0] != '"') {
                        v = make_string('"' + v.string + '"')
                        return compiler(v, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    }
                    return compiler(v, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);;

                }
                // (def x 12) (def (add a b) (+ a b)) => (def add (lambda [a b] (+ a b)))
                else if (tag === "def"){
                    var variable_name = cadr(l);
                    if (variable_name.type === TYPE_PAIR) // it is lambda format like (def (add a b) (+ a b))
                    {
                        var var_name = car(variable_name);
                        var args = cdr(variable_name);
                        var lambda = cons(make_string("lambda"), cons(args, cddr(l)));
                        return compiler(cons(make_string("def"), cons(var_name, cons(lambda, GLOBAL_NULL))), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    }
                    else{
                        variable_name = variable_name.string;
                    }
                    var variable_value;
                    if (cddr(l).type === TYPE_NULL) variable_value = GLOBAL_NULL;
                    else variable_value = caddr(l);
                    // check whether variable already defined
                    var variable_existed = false;
                    var variable_index = -1;
                    var frame = vt[vt.length - 1];
                    for (var j = frame.length - 1; j >= 0; j--) {
                        if (variable_name === frame[j]) {
                            INSTRUCTIONS = []; // clear instructions
                            console.log("ERROR: variable: " + variable_name + " already defined");
                            return;
                            variable_index = j
                            variable_existed = true;
                            break;
                        }
                    }
                    if (variable_existed === false) vt[vt.length - 1].push(variable_name); // add var name to variable table
                    if (variable_value.type === TYPE_PAIR && car(variable_value).string === "lambda") {
                        parent_func_name = variable_name; // set parent_func_name
                    }
                    // compile value
                    compiler(variable_value, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    // add instruction
                    INSTRUCTIONS.push(PUSH << 12);
                    return;
                }
                else if (tag === "set!"){
                    var variable_name = cadr(l);
                    if (variable_name.type === TYPE_PAIR) // it is lambda format like (set! (add a b) (+ a b))
                    {
                        var var_name = car(variable_name);
                        var args = cdr(variable_name);
                        var lambda = cons("lambda", cons(args, cddr(l)));
                        return compiler(cons("set!", cons(var_name, cons(lambda, GLOBAL_NULL))), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    }
                    else{
                        variable_name = variable_name.string;
                    }
                    var variable_value = caddr(l);
                    var index = vt_find(vt, variable_name);
                    if (index[0] === -1) {
                        // set! error
                        console.log("SET! ERROR");
                        return;
                    } else {
                        if (variable_value.type === TYPE_PAIR && car(variable_value).string === "lambda") {
                            parent_func_name = variable_name; // set parent_func_name
                        }
                        compiler(variable_value, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env) // compile value
                        INSTRUCTIONS.push(SET << 12 | (0x0FFF & index[0])); // frame index
                        INSTRUCTIONS.push(0x0000FFFF & index[1]); // value index
                        return;
                    }
                }
                 // (if test conseq alter)
                else if (tag === "if") {
                    var test = cadr(l);
                    var conseq = caddr(l);
                    var alter;
                    if (cdddr(l).type === TYPE_NULL) alter = GLOBAL_NULL;
                    else alter = cadddr(l);
                    compiler(test, vt, macros, false, parent_func_name, functions_for_compilation, env); // compile test, the test is not tail call
                    // push test, but now we don't know jump steps
                    INSTRUCTIONS.push(TEST << 12); // jump over consequence
                    var index1 = INSTRUCTIONS.length;
                    INSTRUCTIONS.push(0x0000); // jump steps
                /*
                  I changed this to compiler_begin for tail_call optimization
                */
                    //var vt_0 = vt.slice(0, vt.length-1);
                    //vt_0.push(vt[vt.length-1].slice(0));
                    compiler_begin(cons(conseq, GLOBAL_NULL), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    // compiler(conseq, vt, macros, tail_call_flag, parent_func_name); // compiler consequence;
                    var index2 = INSTRUCTIONS.length;
                    INSTRUCTIONS.push(JMP << 12) // jmp
                    INSTRUCTIONS.push(0x0000); // jump over alternative
                    INSTRUCTIONS.push(0x0000);
                    var jump_steps = index2 - index1 + 4;
                    INSTRUCTIONS[index1] = jump_steps;
                    //var vt_1 = vt.slice(0, vt.length - 1);
                    //vt_1.push(vt[vt.length-1].slice(0));
                    compiler_begin(cons(alter, GLOBAL_NULL), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    // compiler(alter, vt, macros, tail_call_flag, parent_func_name); // compiler alternative;
                    var index3 = INSTRUCTIONS.length;
                    jump_steps = index3 - index2;
                    INSTRUCTIONS[index2 + 1] = (0xFFFF0000 & jump_steps) >> 16;
                    INSTRUCTIONS[index2 + 2] = (0xFFFF & jump_steps);
                    return;
                }
                else if (tag === "begin") { // begin
                    compiler_begin(cdr(l), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    return;
                }
                // (let [x 0 y 2] . body)
                else if (tag === "let") {
                    var chunks = cadr(l);
                    var body = cddr(l);
                    var var_names = [];
                    var def_array = [];
                    while (chunks.type !== TYPE_NULL) {
                        var v = car(chunks).string;
                        var g = make_string("def");
                        var existed = false;
                        for (var i = 0; i < var_names.length; i++) {
                            if (var_names[i] === v) {
                                existed = true;
                                g = make_string("set!"); // variable already defined
                            }
                        }
                        if (!existed) // 不存在， 保存
                            var_names.push(v);
                        def_array.push(cons(g, cons(make_string(v), cons(cadr(chunks), GLOBAL_NULL)))); // make assignment
                        chunks = cddr(chunks);
                    }
                    // make assignments
                    var assignments = GLOBAL_NULL;
                    for (var i = def_array.length - 1; i >= 0; i--) {
                        assignments = cons(def_array[i], assignments);
                    }
                    var let_ = cons(cons(make_string("lambda"), cons(GLOBAL_NULL, list_append(assignments, body))), GLOBAL_NULL);
                    return compiler(let_, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                }
                // (lambda (a b) ...)
                // (lambda (a . b) ...)
                else if (tag === "lambda") {
                    var params = cadr(l); // get parameters
                    var variadic_place = -1; // variadic place
                    var counter = 0; // count of parameter num
                    var vt_ = vt.slice(0); // new variable table
                    var macros_ = macros.slice(0); // new macro table
                    var env_ = env.slice(0);
                    vt_.push([]) // we add a new frame
                    macros_.push([]); // add new frame
                    env_.push([]); // 必须加上这个， 要不然((lambda [] (defmacro square ([x] `[* ~x ~x])) (square 12))) macro 会有错
                    // 这里不再存了
                    // vt_[vt_.length - 1].push(GLOBAL_NULL); // save space for parent-env.
                    // vt_[vt_.length - 1].push(GLOBAL_NULL); // save space for return address.

                    while (true) {
                        if (params.type === TYPE_NULL) break;
                        if(car(params).type !== TYPE_STRING){
                            console.log("ERROR: Invalid Function Parameter type");
                            return;
                        }
                        if (car(params).string === ".") // variadic
                        {
                            variadic_place = counter;
                            vt_[vt_.length - 1].push(cadr(params).string);
                            counter += 1; // means no parameters requirement
                            break;
                        }
                        vt_[vt_.length - 1].push(car(params).string);
                        counter += 1;
                        params = cdr(params);
                    }
                    // make lambda
                    INSTRUCTIONS.push(MAKELAMBDA << 12 | (counter << 6) | (variadic_place === -1 ? 0x0000 : (variadic_place << 1)) | (variadic_place === -1 ? 0x0000 : 0x0001));
                    var index1 = INSTRUCTIONS.length;
                    INSTRUCTIONS.push(0x0000); // steps that needed to jump over lambda
                    // for tail call optimization
                    var start_pc = INSTRUCTIONS.length; // get start_pc  
                    // compile_body
                    var c_body = compiler_begin(cddr(l), vt_, macros_, parent_func_name, new Lambda_for_Compilation(counter, variadic_place, start_pc, vt_), env_); // set parent_func_name to null
                    // return
                    INSTRUCTIONS.push(RETURN << 12); // return 
                    var index2 = INSTRUCTIONS.length;
                    INSTRUCTIONS[index1] = index2 - index1; // set jump steps
                    return;
                }
                // macro
                else if (tag === "defmacro") {
                    var var_name = cadr(l).string;
                    var clauses = cddr(l);
                    var already_defined = false;
                    for (var i = macros[macros.length - 1].length - 1; i >= 0; i--) {
                        if (var_name === macros[macros.length - 1][i].macro_name) { // macro already exists
                            already_defined = true;
                            macros[macros.length - 1][i].clauses = clauses;
                        }
                    }
                    // ((lambda [] (defmacro square ([x] `[* ~x ~x])) (square 12)))
                    if (already_defined === false) { // not defined, save macro
                        macros[macros.length - 1].push(new Macro(var_name, clauses, vt.slice(0), env.slice(0)));
                    }
                    return;
                }
                // call function
                else {
                    // check whether macro
                    var func = car(l);
                    // (defmacro test ([a] `[car ~a])) ((lambda [] (def car cdr) (test '(3 4))))
                    // (defmacro test ([a] `[car ~a])) ((lambda [] (test '(3 4))))
                    if (func.type === TYPE_STRING) {
                        for (var i = macros.length - 1; i >= 0; i--) {
                            var frame = macros[i];
                            for (var j = frame.length - 1; j >= 0; j--) {
                                if (frame[j].macro_name === func.string) {
                                    var expand = macro_expand_for_compilation(frame[j], cdr(l), macros);
                                    return compiler(expand, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env)
                                }
                            }
                        }
                    }
                    // tail call
                    if (tail_call_flag) {
                        // so no new frame
                        var start_index = vt[vt.length - 1].length;
                        var track_index = start_index;
                        // compile parameters
                        var param_num = 0;
                        var params = cdr(l);
                        // push parameter from right to left
                        params = list_to_array(params); // convert list to array
                        param_num = params.length; // get param num
                        var count_params = 0; // count param num
                        /* compile parameters */
                        for (var i = 0; i < param_num; i++) {
                            if (i === functions_for_compilation.variadic_place) { // variadic param
                                count_params++;
                                var p = GLOBAL_NULL;
                                var j = param_num - 1;
                                for (; j >= i; j--) {
                                    p = cons(make_string("cons"), cons(params[j], cons(p, GLOBAL_NULL)));
                                }
                                compiler(p, vt, macros, false, parent_func_name, functions_for_compilation, env); // each argument is not tail call
                                // set to current frame
                                INSTRUCTIONS.push(SET << 12 | vt.length - 1); // frame index
                                INSTRUCTIONS.push(0x0000FFFF & track_index); // value index
                            } else {
                                count_params++;
                                compiler(params[i], vt, macros, false, parent_func_name, functions_for_compilation, env); // each argument is not tail call
                                // set to current frame
                                INSTRUCTIONS.push(SET << 12 | vt.length - 1); // frame index
                                INSTRUCTIONS.push(0x0000FFFF & track_index); // value index
                            }
                            track_index++;
                        }
                        // move parameters
                        for (var i = 0; i < count_params; i++) {
                            // get value
                            INSTRUCTIONS.push(GET << 12 | vt.length - 1); // frame index
                            INSTRUCTIONS.push(start_index + i); // value index
                            // move to target index
                            INSTRUCTIONS.push(SET << 12 | vt.length - 1); // frame index
                            INSTRUCTIONS.push(i/* + 2*/) // value index 不再加2是因为不再存env 和 pc
                        }
                        if (functions_for_compilation.variadic_place === -1 && i < functions_for_compilation.param_num) {
                            for (; i < functions_for_compilation.param_num; i++) {
                                INSTRUCTIONS.push(CONST_NULL);
                                // move to target index
                                INSTRUCTIONS.push(SET << 12 | vt.length - 1); // frame index
                                INSTRUCTIONS.push(i/* + 2*/); // value index 不再加2是因为不再存env 和 pc
                            }
                        }
                        // jump back
                        var start_pc = functions_for_compilation.start_pc; // start pc for that lambda
                        INSTRUCTIONS.push(JMP << 12);
                        var jump_steps = -(INSTRUCTIONS.length - start_pc); // jump steps
                        INSTRUCTIONS.push((0xFFFF0000 & jump_steps) >> 16);
                        INSTRUCTIONS.push(0xFFFF & jump_steps);
                        return;
                    }
                    // not tail call
                    else {
                        // compile function first
                        compiler(func, vt, macros, false, parent_func_name, functions_for_compilation, env); // compile lambda, save to accumulator
                        INSTRUCTIONS.push(NEWFRAME << 12); // create new frame and set flag
                        // compile parameters
                        var param_num = 0;
                        var params = cdr(l);
                        // push parameter from right to left
                        params = list_to_array(params); // convert list to array
                        param_num = params.length; // get param num
                        for (var i = 0; i < param_num; i++) // compile parameter from ---right to left---, now from left to right
                        {
                            compiler(params[i], vt, macros, false, parent_func_name, functions_for_compilation, env); // each argument is not tail call
                            INSTRUCTIONS.push(PUSH_ARG << 12/* | (i)*/); // push parameter to new frame
                        }
                        //compiler(func, vt, macros, false, parent_func_name, functions_for_compilation); // compile lambda, save to accumulator
                        INSTRUCTIONS.push(CALL << 12 | (0x0FFF & (param_num))); // call function.
                        return;
                    }
                }
                /*
                // macroexpand-1
                else if (tag === "macroexpand-1") {
                    var expand = cadr(l);
                    var macro_name = car(expand);
                    for (var i = macros.length - 1; i >= 0; i--) {
                        var frame = macros[i];
                        for (var j = frame.length - 1; j >= 0; j--) {
                            if (frame[j].macro_name === macro_name) {
                                var e = cons("quote", cons(macro_expand(frame[j], cdr(expand)), GLOBAL_NULL));
                                return compiler(e, vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                            }
                        }
                    }
                    console.log("ERROR: macroexpand-1 invalid macro: " + macro_name);
                    INSTRUCTIONS = [];
                    return;
                }
                // return
                else if (tag === "return") {
                    compiler(cdr(l).type === TYPE_NULL ? GLOBAL_NULL : cadr(l), vt, macros, tail_call_flag, parent_func_name, functions_for_compilation, env);
                    INSTRUCTIONS.push(RETURN << 12);
                    return;
                }
                */
        }
    }

var compiler_begin = function(l, vt, macros, parent_func_name, functions_for_compilation, env) {
        var acc = null;
        if (typeof(parent_func_name) === "undefined") parent_func_name = GLOBAL_NULL;
        if (typeof(functions_for_compilation) === "undefined") functions_for_compilation = GLOBAL_NULL;
        if (typeof(env) === "undefined") env = GLOBAL_NULL;
        while (l.type !== TYPE_NULL) {
            if (cdr(l).type === TYPE_NULL && car(l).type === TYPE_PAIR && car(car(l)) === parent_func_name) {
                // console.log("Tail Call");
                compiler(car(l), vt, macros, 1, GLOBAL_NULL, functions_for_compilation, env)      // tail call
            } 
            else 
                compiler(car(l), vt, macros, 0, parent_func_name, functions_for_compilation, env) // not tail call;
            l = cdr(l);

            /* 
                runtime evaluation support
                run instructions 
                我可能以后会添加一些flag， 例如在编译模式下就不运行，但是macro会有错
            */ 
            
            if(vt.length == 1){
                acc = VM(INSTRUCTIONS, env, INSTRUCTIONS_PC); // run instructions
                INSTRUCTIONS_PC = INSTRUCTIONS.length;  // update instruction pc
                console.log("****** Finish running VM ******");
                console.log(acc);
            }
        }
        return;
    }
    // variables for VM
var constant_table = [GLOBAL_TRUE]; // used to save constant. 
var BUILTIN_PRIMITIVE_PROCEDURE_STACK = [];
var VM = function(INSTRUCTIONS, env, pc, end_pc, frame_list, functions_list, continuation_env, continuation_return_pc, is_continuation_if_less_than_0, accumulator) {
        //printInstructions(INSTRUCTIONS);
        //printInstructions("\n\n");
        var copy_frame_list_for_continuation = function(frame_list){
            if(frame_list === GLOBAL_NULL){
                return GLOBAL_NULL;
            }
            var v = car(frame_list);
            if(v === GLOBAL_NULL)
                return cons(v, copy_frame_list_for_continuation(cdr(frame_list)));
            return cons(v.slice(0), copy_frame_list_for_continuation(cdr(frame_list)));
        }

        if (typeof(pc) === "undefined") pc = 0;
        if (typeof(end_pc) === "undefined") end_pc = INSTRUCTIONS.length;
        var accumulator = typeof(accumulator) === "undefined" ? GLOBAL_NULL : accumulator; // accumulator

        var frame_list = typeof(frame_list) === "undefined" ? cons(GLOBAL_NULL, GLOBAL_NULL) : frame_list; // stack used to save frames    head frame1 frame0 tail, queue
        var current_frame_pointer = car(frame_list); // pointer that points to current new frame

        var functions_list = typeof(functions_list) === "undefined" ? GLOBAL_NULL : functions_list; // used to save functions 

        var continuation_return_pc = typeof(continuation_return_pc) === "undefined" ? GLOBAL_NULL : continuation_return_pc;  //  return pc
        var continuation_env = typeof(continuation_env) === "undefined"? GLOBAL_NULL : continuation_env; // 保存 env

        is_continuation_if_less_than_0 = is_continuation_if_less_than_0 | 0;

        while (pc !== end_pc) {
            if(is_continuation_if_less_than_0 < 0 && functions_list === GLOBAL_NULL){ // for continuation
                console.log("Continuation: ");
                console.log(accumulator);
                return accumulator;
            }
            var inst = INSTRUCTIONS[pc];
            var opcode = (inst & 0xF000) >> 12;
            switch (opcode) {
            case CONST:
                {
                    switch (inst) {
                    case CONST_INTEGER:
                        // integer
                        {
                            //accumulator = (INSTRUCTIONS[pc + 1] * /*Math.pow(2, 48)*/ 281474976710656)+  // couldn't shift left 48
                            //		  (INSTRUCTIONS[pc + 2] * /*Math.pow(2, 32)*/ 4294967296)+
                            //		  (INSTRUCTIONS[pc + 3] * /*Math.pow(2, 16)*/ 65536) +
                            //		   INSTRUCTIONS[pc + 4] - (INSTRUCTIONS[pc + 1] & 0x8000) * Math.pow(2, 64);
                            accumulator = (INSTRUCTIONS[pc + 1] << 16) + INSTRUCTIONS[pc + 2];
                            accumulator = make_integer(accumulator);
                            pc = pc + 3; //5;
                            // console.log("INT accumulator=> " + accumulator.num);
                            continue;
                        }
                    case CONST_FLOAT:
                        {
                            //accumulator = (INSTRUCTIONS[pc + 1] * /*Math.pow(2, 16)*/65536)+
                            //		  (INSTRUCTIONS[pc + 2])
                            //		  - (INSTRUCTIONS[pc + 1] & 0x8000) * Math.pow(2, 32);
                            // console.log((INSTRUCTIONS[pc + 3] * Math.pow(2, 16)) + (INSTRUCTIONS[pc + 4]))
                            //accumulator = accumulator + ((INSTRUCTIONS[pc + 3] * /*Math.pow(2, 16)*/65536) + (INSTRUCTIONS[pc + 4])) / /*Math.pow(10, 9)*/1000000000
                            accumulator = INSTRUCTIONS[pc + 1] + INSTRUCTIONS[pc + 2] / 10000.0
                            accumulator = make_float(accumulator);
                            pc = pc + 3; //5;
                            // console.log("FLOAT accumulator=> " + accumulator);
                            continue;
                        }
                    case CONST_STRING:
                        {
                            var length = INSTRUCTIONS[pc + 1]; // length used to create string. for C language in the future
                            var created_string = "";
                            var s;
                            pc = pc + 2;
                            while (true) {
                                s = INSTRUCTIONS[pc];
                                var s1 = (0xFF00 & s) >> 8;
                                var s2 = (0x00FF & s);
                                if (s1 === 0x00) // reach end
                                break;
                                else created_string += String.fromCharCode(s1);
                                if (s2 === 0x00) // reach end
                                break;
                                else created_string += String.fromCharCode(s2);
                                pc = pc + 1;
                            }
                            // create string
                            accumulator = make_string(created_string);
                            // push to constant table
                            constant_table.push(accumulator);
                            // increase pc
                            pc = pc + 1;
                            continue;
                        }
                    case CONST_LOAD:
                        accumulator = constant_table[INSTRUCTIONS[pc + 1]]; // load constant
                        pc = pc + 2;
                        //console.log(constant_table);
                        continue;
                    default:
                        // null
                        {
                            accumulator = GLOBAL_NULL;
                            pc = pc + 1;
                            // console.log("NULL: ");
                            continue;
                        }
                    }
                }
            case PUSH_ARG:
                // push to environment
                {
                    // console.log("PUSH_ARG");
                    //current_frame_pointer[0x0FFF & inst] = accumulator; // push to current frame
                    current_frame_pointer.push(accumulator);
                    pc = pc + 1;
                    continue;
                }
            case TEST:
                // test
                {
                    if (accumulator.type === TYPE_NULL) // false, jump
                    {
                        pc = pc + (INSTRUCTIONS[pc + 1]);
                        continue;
                    }
                    // run next
                    pc = pc + 2;
                    continue;
                }
            case JMP:
                // jump
                {
                    pc = pc + ((INSTRUCTIONS[pc + 1] << 16) | INSTRUCTIONS[pc + 2]);
                    continue;
                }
            case SET:
                // set
                {
                    // console.log("SET");
                    var frame_index = 0x0FFF & inst; // get frame index
                    var value_index = INSTRUCTIONS[pc + 1]; // get value index
                    env[frame_index][value_index] = accumulator;
                    pc = pc + 2;
                    continue;
                }
            case GET:
                // get
                {
                    var frame_index = 0x0FFF & inst;
                    var value_index = INSTRUCTIONS[pc + 1];
                    accumulator = env[frame_index][value_index];
                    pc = pc + 2;
                    continue;
                }
            case MAKELAMBDA:
                // make lambda
                {
                    // console.log("MAKELAMBDA");
                    var param_num = (0x0FC0 & inst) >> 6;
                    var variadic_place = (0x0001 & inst) ? ((0x003E & inst) >> 1) : -1;
                    var start_pc = pc + 2;
                    var jump_steps = INSTRUCTIONS[pc + 1];
                    // accumulator = new Lambda(param_num, variadic_place, start_pc, env.slice(0)); // set lambda
                    accumulator = make_lambda(param_num, variadic_place, start_pc, env.slice(0)); // set lambda
                    pc = pc + jump_steps + 1;
                    continue;
                }
            case CALL:
                // call function
                {
                    // console.log("CALL FUNCTION");
                    var param_num = (0x0FFF & inst); // get param num
                    var lambda;
                    lambda = car(functions_list); //得到lambda
                    functions_list = cdr(functions_list);  // pop top lambda
                    var type = lambda.type;
                    switch (type) {
                    case TYPE_BUILTIN_LAMBDA:
                        lambda = lambda.builtin_lambda;
                        // builtin lambda
                        pc = pc + 1;
                        accumulator = lambda(current_frame_pointer, current_frame_pointer.length - param_num); // remove saved env and pc
                        for(var i = 0; i < param_num; i++){
                            current_frame_pointer.pop(); // pop params
                        }
                        frame_list = cdr(frame_list); // pop top frame
                        current_frame_pointer = car(frame_list) // update frame_pointer

                        /*if(continuation_return_pc !== GLOBAL_NULL){
                            pc = car(continuation_return_pc);
                            continuation_return_pc = cdr(continuation_return_pc);
                        }*/
                        continue
                    case TYPE_OBJECT: // doesn't support object anymore
                        // object
                        lambda = lambda.object;
                        pc = pc + 1;
                        if (param_num === 1) { // get 
                            var p0 = current_frame_pointer[current_frame_pointer.length - 1];
                            accumulator = lambda[p0.string];
                            current_frame_pointer.pop(); // pop parameters
                            if (typeof(accumulator) === "undefined") accumulator = GLOBAL_NULL;
                        } else { // set
                            var p0 = current_frame_pointer[current_frame_pointer.length - 2];
                            var p1 = current_frame_pointer[current_frame_pointer.length - 1];
                            lambda[p0.string] = p1;
                            accumulator = lambda;
                            // pop parameters
                            current_frame_pointer.pop();
                            current_frame_pointer.pop();
                        }
                        frame_list = cdr(frame_list); // pop top frame
                        current_frame_pointer = car(frame_list) // update frame_pointer
                        continue;

                    case TYPE_VECTOR: 
                        // vector
                        lambda = lambda.vector;
                        pc = pc + 1;
                        if (param_num === 1) { // get 
                            var p0 = current_frame_pointer[current_frame_pointer.length - 1];
                            accumulator = lambda[p0.num];
                            current_frame_pointer.pop(); // pop parameters
                            if (typeof(accumulator) === "undefined") accumulator = GLOBAL_NULL;
                        } else { // set
                            var p0 = current_frame_pointer[current_frame_pointer.length - 2];
                            var p1 = current_frame_pointer[current_frame_pointer.length - 1];
                            lambda[p0.num] = p1;
                            accumulator = lambda;
                            // pop parameters
                            current_frame_pointer.pop();
                            current_frame_pointer.pop();
                        }
                        frame_list = cdr(frame_list); // pop top frame
                        current_frame_pointer = car(frame_list) // update frame_pointer
                        continue;
                    case TYPE_LAMBDA:
                        // user defined lambda
                        lambda = lambda.lambda; // user defined lambda
                        // user defined lambda
                        var required_param_num = lambda.param_num;
                        var required_variadic_place = lambda.variadic_place;
                        var start_pc = lambda.start_pc;
                        var new_env;
                        new_env = lambda.env.slice(0);
                        new_env.push(current_frame_pointer);
                        
                        //current_frame_pointer[0] = env; // save current env to new-frame
                        //current_frame_pointer[1] = pc + 1; // save pc
                        
                        if (required_variadic_place === -1 && param_num - 1 > required_param_num) {
                            console.log("ERROR: Too many parameters provided");
                            return;
                        }
                        if (required_variadic_place !== -1) { // variadic value
                            var v = GLOBAL_NULL;
                            for (var i = current_frame_pointer.length - 1; i >= required_variadic_place/* + 2*/; i--) {
                                v = cons(current_frame_pointer[i], v);
                            }
                            current_frame_pointer[required_variadic_place/* + 2*/] = v;
                        }
                        if (current_frame_pointer.length/* - 2*/ < required_param_num) // not enough parameters
                        {
                            for (var i = param_num; i < required_param_num; i++) {
                                current_frame_pointer[i/* + 2*/] = GLOBAL_NULL; // default value is null
                            }
                        }
                        // save pc to continuation_return_pc
                        // 不再把它存在current_frame_pointer里面了
                        continuation_return_pc = cons(pc+1, continuation_return_pc);
                        // save old env to continuation_env
                        // 不再把它存在current_frame_pointer里面了
                        continuation_env = cons(env, continuation_env);

                        env = new_env; // change env pointer
                        pc = start_pc; // begin to call function
                        frame_list = cdr(frame_list) // update frame list
                        current_frame_pointer = car(frame_list);
                        continue;
                    // test
                    // (def x 0)(+ 1 (call/cc (lambda (a) (set! x a) 2))) (x 3) => 4
                    // CONTINUATION: start_pc, env, frame_list, functions_list, continuation_env, continuation_return_pc
                    case TYPE_CONTINUATION: // continuation
                        accumulator = current_frame_pointer[current_frame_pointer.length - param_num]; // set accumulator (return value)
                        //console.log("accumulator: ");
                        //console.log(accumulator);

                        // two situations
                        // situation 1)
                        // (def x 0)
                        // (+ 3 (call/cc (lambda [return] (set! x return) (return 13) 2)))
                        // situation 2)
                        // (x 3)

                        // for situaion1
                        // break current
                        if(pc < lambda.continuation.start_pc){
                            console.log("SITUATION 1");
                            for(var i = 0; i < param_num; i++){
                                current_frame_pointer.pop(); // pop params
                            }
                            // restore everything
                            env = lambda.continuation.env.slice(0);
                            pc = lambda.continuation.start_pc;
                            //current_frame_pointer = lambda.continuation.current_frame_pointer === GLOBAL_NULL ? GLOBAL_NULL : lambda.continuation.current_frame_pointer.slice(0);
                            //frame_list = cons(current_frame_pointer, lambda.continuation.frame_list);
                            frame_list = copy_frame_list_for_continuation(lambda.continuation.frame_list);
                            current_frame_pointer = car(frame_list);
                            functions_list = lambda.continuation.functions_list;
                            continuation_env = lambda.continuation.continuation_env;
                            continuation_return_pc = lambda.continuation.continuation_return_pc; 

                            //console.log(current_frame_pointer);
                            //console.log(cdr(frame_list));   
                            //console.log((INSTRUCTIONS[pc]).toString(16));
                            continue;
                        }
                        // for situation2
                        // VM = function(INSTRUCTIONS, env, pc, end_pc, frame_list, functions_list, continuation_env, continuation_return_pc, 如果<0就是continuation, accumulator)
                        // (def x 0) (+ 3 (call/cc (lambda (r) (set! x r) (r 12) 13))) (x 4) => 7
                        // (+ 3 (+ (call/cc (lambda (r) (set! x r) (r 12) 3)) 4))
                        else{
                            frame_list = cdr(frame_list);
                            current_frame_pointer = car(frame_list);

                            console.log("THIS CONTINUATION HASNT BEEN IMPLEMENTED YET");
                            /*
                            if(lambda.continuation.continuation_return_pc === GLOBAL_NULL){
                                pc = pc + 1;
                                continue;
                            }
                            else{*/
                                //var end_pc_for_continuation;
                                //var temp_continuation_return_pc = lambda.continuation_return_pc;
                                //var current_frame_pointer_ = lambda.continuation.current_frame_pointer === GLOBAL_NULL ? GLOBAL_NULL : lambda.continuation.current_frame_pointer.slice(0);
                                //while(cdr(temp_continuation_return_pc) !== GLOBAL_NULL){
                                //    temp_continuation_return_pc = cdr(temp_continuation_return_pc);
                                //} 
                                //end_pc_for_continuation = car(temp_continuation_return_pc);
                                //console.log(car(lambda.continuation.frame_list));
                                //console.log(cadr(lambda.continuation.frame_list));

                                accumulator = VM(INSTRUCTIONS, 
                                                 lambda.continuation.env.slice(0), 
                                                 lambda.continuation.start_pc, 
                                                 INSTRUCTIONS.length,
                                                 /*cons(current_frame_pointer_, lambda.continuation.frame_list),*/copy_frame_list_for_continuation(lambda.continuation.frame_list),
                                                 lambda.continuation.functions_list,
                                                 lambda.continuation.continuation_env,
                                                 lambda.continuation.continuation_return_pc,
                                                -1,
                                                accumulator); 
                                pc = pc + 1;
                                continue;
                            //}
                        }
                    default: 
                        switch(lambda.num){
                            case 1:  // apply
                                lambda = current_frame_pointer[current_frame_pointer.length - param_num];
                                if(lambda.type === TYPE_BUILTIN_LAMBDA){
                                    var func = lambda.builtin_lambda;  
                                    var args_list = current_frame_pointer[current_frame_pointer.length - param_num + 1];
                                    var args = []; // get args
                                    while(args_list !== GLOBAL_NULL){
                                        args.push(car(args_list));
                                        args_list = cdr(args_list);
                                    }
                                    pc = pc + 1;
                                    accumulator = func(args, 0);

                                    for(var i = 0; i < param_num; i++){
                                        current_frame_pointer.pop(); // pop params
                                    }
                                    frame_list = cdr(frame_list); // pop top frame
                                    current_frame_pointer = car(frame_list) // update frame_pointer
                                    continue
                                }
                                else{ // user defined , cant be object and vector
                                    // user defined lambda
                                    lambda = lambda.lambda; // user defined lambda
                                    // =============================
                                    // 只有这里不同
                                    var args_list = current_frame_pointer[current_frame_pointer.length - param_num + 1];
                                    // restore stack
                                    for(var i = 0; i < param_num; i++){
                                        current_frame_pointer.pop(); // pop params
                                    }
                                    current_frame_pointer = [null, null] // reset current_frame_pointer
                                    while(args_list !== GLOBAL_NULL){
                                        current_frame_pointer.push(car(args_list));
                                        args_list = cdr(args_list);
                                    }
                                    // =============================

                                    // user defined lambda
                                    var required_param_num = lambda.param_num;
                                    var required_variadic_place = lambda.variadic_place;
                                    var start_pc = lambda.start_pc;
                                    var new_env;
                                    new_env = lambda.env.slice(0);
                                    new_env.push(current_frame_pointer);
                                    //current_frame_pointer[0] = env; // save current env to new-frame
                                    //current_frame_pointer[1] = pc + 1; // save pc
                                    if (required_variadic_place === -1 && param_num - 1 > required_param_num) {
                                        console.log("ERROR: Too many parameters provided");
                                        return;
                                    }
                                    if (required_variadic_place !== -1) { // variadic value
                                        var v = GLOBAL_NULL;
                                        for (var i = current_frame_pointer.length - 1; i >= required_variadic_place/* + 2*/; i--) {
                                            v = cons(current_frame_pointer[i], v);
                                        }
                                        current_frame_pointer[required_variadic_place/* + 2*/] = v;
                                    }
                                    if (current_frame_pointer.length/* - 2*/ < required_param_num) // not enough parameters
                                    {
                                        for (var i = param_num; i < required_param_num; i++) {
                                            current_frame_pointer[i/* + 2*/] = GLOBAL_NULL; // default value is null
                                        }
                                    }

                                     // save pc to continuation_return_pc
                                    // 不再把它存在current_frame_pointer里面了
                                    continuation_return_pc = cons(pc+1, continuation_return_pc);
                                    // save old env to continuation_env
                                    // 不再把它存在current_frame_pointer里面了
                                    continuation_env = cons(env, continuation_env);

                                    env = new_env; // change env pointer
                                    pc = start_pc; // begin to call function
                                    frame_list = cdr(frame_list) // update frame list
                                    current_frame_pointer = car(frame_list);
                                    continue;
                                }
                            // start_pc, env, frame_list, functions_list, continuation_return_pc, continuation_env
                            case 2:  // call cc
                                var p = current_frame_pointer[current_frame_pointer.length - param_num].lambda; // 得到 lambda
                                // ==========================================
                                for(var i = 0; i < param_num; i++){
                                    current_frame_pointer.pop(); // pop params
                                }
                    
                               
                                var new_frame_list_for_continuation = copy_frame_list_for_continuation(cdr(frame_list));   // 复制里面的array

                                var new_env = p.env.slice(0); // create new env for that lambda
                                var new_env_frame = [make_continuation(pc + 1, env.slice(0), /*cddr(frame_list)*/new_frame_list_for_continuation, functions_list, continuation_return_pc, continuation_env)]; // env, return-pc continuation...
                                new_env.push(new_env_frame);

                                // save pc to continuation_return_pc
                                // 不再把它存在current_frame_pointer里面了
                                continuation_return_pc = cons(pc + 1, continuation_return_pc);
                                // save old env to continuation_env
                                // 不再把它存在current_frame_pointer里面了
                                continuation_env = cons(env, continuation_env);

                                frame_list = cdr(frame_list); // pop top frame
                                current_frame_pointer = frame_list == GLOBAL_NULL ? GLOBAL_NULL : car(frame_list) // update frame_pointer

                                env = new_env;
                                pc = p.start_pc;
                                continue;
                            default:
                                console.log("Invalid Function");
                                return;
                        }
                    }
                }
            case NEWFRAME:
                // get function
                switch (accumulator.type){
                    case TYPE_LAMBDA:
                        var new_frame = [];
                        current_frame_pointer = new_frame;
                        frame_list = cons(new_frame, frame_list);
                        functions_list = cons(accumulator, functions_list);
                        pc = pc + 1;
                        continue;
                    case TYPE_BUILTIN_LAMBDA: case TYPE_VECTOR: case TYPE_OBJECT: case TYPE_CONTINUATION:
                        current_frame_pointer = BUILTIN_PRIMITIVE_PROCEDURE_STACK; // env[env.length - 1]; // get top frame
                        frame_list = cons(current_frame_pointer, frame_list);
                        functions_list = cons(accumulator, functions_list);
                        pc = pc + 1;
                        continue;
                    default: // apply function
                        current_frame_pointer = BUILTIN_PRIMITIVE_PROCEDURE_STACK;// env[env.length - 1]; // get top frame
                        frame_list = cons(current_frame_pointer, frame_list);
                        functions_list = cons(accumulator, functions_list);
                        pc = pc + 1;
                        continue;
                }
                // create new frame
                { // create new frame
                    var new_frame = [];
                    frame_list = cons(new_frame, frame_list);
                    current_frame_pointer = new_frame;
                    functions_list = cons(accumulator, functions_list);
                    pc = pc + 1;
                    continue;
                }
            case RETURN:
                // return
                { // return
                    // restore pc and env
                    // env = env[env.length - 1][0];
                    // pc = env[env.length - 1][1];

                    // 开始使用 continuation_env 来恢复 env
                    env = car(continuation_env);
                    continuation_env = cdr(continuation_env);
                    // 开始使用 continuation_return_pc 来恢复 return_pc
                    pc = car(continuation_return_pc);
                    continuation_return_pc = cdr(continuation_return_pc);

                    /*
                    if(continuation_return_pc !== GLOBAL_NULL){
                        pc = car(continuation_return_pc);
                        continuation_return_pc = cdr(continuation_return_pc);
                    }
                    else{
                        pc = env[env.length - 1][1];
                    }*/
                    continue;
                }
            case PUSH:
                // push to top frame
                env[env.length - 1].push(accumulator);
                pc++;
                continue;
            default:
                console.log("ERROR: Invalid opcode");
                return GLOBAL_NULL;
            }
        }
        //console.log("Finishing running VM");
        //console.log(accumulator);
        return accumulator;
    }
    // var l = lexer("((lambda [x] (+ x 1)) 12)")
    // var l = lexer("(def (test x) (def a 12) (+ x a)) (test 14)")
    // var l = lexer("(def (x a b . c) (+ a (+ b (car (cdr c))))) (x 3 4 5 6)")
    // var l = lexer("(def (test a) (cons 'b a)) (test 'c)")
    // var l = lexer("(def (x) (def a 12) (lambda (msg) (if (eq? msg 'a) a (set! a 10) ))) (def b (x)) (b 'b) (b 'a)")
    // var l = lexer(' (def (f n) (if (= n 0) 1 (* n (f (- n 1))))) (f 20)');
    // var l = lexer(' (def (f a . b) (+ a (car b))) (f 30 25 40)');
    // var l = lexer("(def (test a) a) (test 12)")
    // var l = lexer("(begin (def x 12) (def y 15) x)")
    // var l = lexer("(def (test) (def a 12) (lambda [msg] (if (= msg 0) a (set! a 15)))) (def a (test)) (a 1)(a 0)")
    // var l = lexer("(def (append x y) (if (null? x) y (cons (car x) (append (cdr x) y))))")
    // var l = lexer("(def x #[1,2,3]) (vector-slice x 1 2)")
    // var l = lexer("(let [x 0 y 2 x (+ y 1)] (+ x y)) ")
    // var l = lexer("(cond (() 2) (() 4) (else 5) )")
    // var l = lexer("(defmacro square ([x] [* ~x ~x])) (square 12) (macroexpand-1 (square 15))");
    // var l = lexer("(if () 2 (if 3 4 5))")
    // var l = lexer("(pow 2 3)");
    // var l = lexer("(defmacro test ([a . b] [quote ~b])) (macroexpand-1 (test 3 4 5))")
    // var l = lexer("(quote (a (b c) d))")
    // var l = lexer("(def (f n result) (if (= n 0) result (f (- n 1) (* n result)))) (f 100 1)")
    // var l = lexer("(if () 2 3)")
    // var l = lexer("(defmacro defm ([macro_name p b] [defmacro ~macro_name (~p ~b)])) (defm square [x] [* ~x ~x]) (square 16)")
    // var l = lexer("(defmacro square ([x] [* ~x ~x])) (square 12)")
    //var l = lexer("(def (factorial n result) (if (= n 0) result (factorial (- n 1) (* n result)) )) (factorial 150 1)");
    //var l = lexer("(def (test n) (test n)")
    //var l = lexer("(cons 1 ())")
    //var l = lexer("(def x 12)");
    //console.log(l);
    //var o = parser(l);
    //console.log(o);
    //console.log(car(cdr(cdr(car(o)))))
    //var p = compiler_begin(o, VARIABLE_TABLE, MACROS, null, null);
    //console.log(VARIABLE_TABLE);
    //printInstructions(INSTRUCTIONS);
    //console.log(VM(INSTRUCTIONS, ENVIRONMENT))
    //console.log(ENVIRONMENT);
    // var p = compiler_begin(o, Variable_Table);
    // console.log(p);
    //console.log(vm(p, Environment, null));
    // exports to Nodejs
    /*
    var l = new_lexer("(defmacro square ([x] `(* ~x ~x))) (square 12)"); // (defmacro square ([x] `(* ~x ~x))) (square 12)
    var o = new_parser(l);
    var p = compiler_begin(o, VARIABLE_TABLE, MACROS, null, null, ENVIRONMENT);
*/
    /*
var l = lexer("(def f (lambda [n] (if (= n 0) 1 (* n (f (- n 1)))))) (f 100)");
var o = parser(l);
compiler_begin(o, VARIABLE_TABLE, MACROS, null, null);
printInstructions(INSTRUCTIONS);
VM(INSTRUCTIONS,ENVIRONMENT);
*/


// test new lexer
/*
var v = "(def x '(1 2 ~x))"
var l = new_lexer(v);
console.log(l);
var p = new_parser(l)
console.log(p);
console.log(new_parser_debug(p));
*/
/*
var v = "(def x '(1 2 3)) (a:b:c)"
var l = new_lexer(v);
var p = new_parser(l);
console.log(new_parser_debug(p));
*/

if (typeof(module) != "undefined") {
    module.exports.vm_lexer = new_lexer;
    module.exports.vm_parser = new_parser;
    module.exports.vm_compiler_begin = compiler_begin;
    module.exports.vm = VM;
    module.exports.vm_env = ENVIRONMENT;
    module.exports.vm_vt = VARIABLE_TABLE;
    module.exports.vm_insts = INSTRUCTIONS;
}
