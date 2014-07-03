// helper file, load walley_wore.wa
char WALLEY_CORE_CONTENT[] = "(def walley-core-author (quote Yiyi-Wang ) )(def walley-core-version \"0.00029\" )(def walley-language-version \"0.3.8483\" )(def break (quote break ) )(def continue (quote continue ) )(def int? (lambda (v ) (eq? (typeof v ) (quote integer ) ) ) )(def integer? int? )(def float? (lambda (v ) (eq? (typeof v ) (quote float ) ) ) )(def pair? (lambda (v ) (eq? (typeof v ) (quote pair ) ) ) )(def list? pair? )(def null? (lambda (v ) (eq? v () ) ) )(def vector? (lambda (v ) (eq? (typeof v ) (quote vector ) ) ) )(def lambda? (lambda (v ) (eq? (typeof v ) (quote lambda ) ) ) )(def table? (lambda (v ) (eq? (typeof v ) (quote table ) ) ) )(def string? (lambda (v ) (eq? (typeof v ) (quote string ) ) ) )(def fn? lambda? )(def append (lambda (a b ) (if (null? a ) b (cons (car a ) (append (cdr a ) b ) ) ) ) )(def reverse (fn (a ) (def reverse_iter (fn (a result ) (if (null? a ) result (reverse_iter (cdr a ) (cons (car a ) result ) ) ) ) ) (reverse_iter a () ) ) )(def error (lambda (s ) (print \"ERROR: \" ) (print s ) ) )(defmacro defn ((func_name args . body ) (quasiquote (def (unquote func_name ) (lambda (unquote args ) (unquote-splice body ) ) ) ) ) )(def List (fn (. args ) args ) )(defmacro cond (() () ) ((#else body ) body ) ((#else body . rest ) (print \"ERROR: cond invalid statements.\n\" ) ) ((test body ) (quasiquote (if (unquote test ) (unquote body ) (quote () ) ) ) ) ((test body . rest ) (quasiquote (if (unquote test ) (unquote body ) (cond (unquote-splice rest ) ) ) ) ) )(defmacro and (() (quote true ) ) ((a ) a ) ((a . b ) (quasiquote (if (unquote a ) (and (unquote-splice b ) ) () ) ) ) )(defmacro or (() () ) ((a ) a ) ((a . b ) (quasiquote (if (unquote a ) (unquote a ) (or (unquote-splice b ) ) ) ) ) )(defmacro defm ((macro-name args pattern ) (quasiquote (defmacro (unquote macro-name ) ((unquote args ) (unquote pattern ) ) ) ) ) )(def list-length (lambda (n ) (def list-length-iter (lambda (n r ) (if (null? n ) r (list-length-iter (cdr n ) (+ r 1 ) ) ) ) ) (list-length-iter n 0 ) ) )(defn list-slice-helper (l end ) (if (= end 0 ) () (cons (car l ) (list-slice-helper (cdr l ) (- end 1 ) ) ) ) )(defn list-slice-remove-head (l start ) (if (= start 0 ) l (list-slice-remove-head (cdr l ) (- start 1 ) ) ) )(defn list-slice (l start end ) (set! end (or end (list-length l ) ) ) (list-slice-helper (list-slice-remove-head l start ) (- end start ) ) )(defn str (. b ) (defn str-iter (b ) (if (null? b ) \"\" (string-append (car b ) (str-iter (cdr b ) ) ) ) ) (str-iter b ) )(defn len (b ) (if (null? b ) 0 (if (string? b ) (string-length b ) (if (pair? b ) (list-length b ) (if (vector? b ) (vector-length b ) (error \"Function len invalid data type\" ) ) ) ) ) )(defn string=? (a b ) (= (strcmp a b ) 0 ) )(defn string>? (a b ) (> (strcmp a b ) 0 ) )(defn string<? (a b ) (< (strcmp a b ) 0 ) )(defn string>=? (a b ) (>= (strcmp a b ) 0 ) )(defn string<=? (a b ) (<= (strcmp a b ) 0 ) )(def ** math-pow )(defn caar (x ) (car (car x ) ) )(defn cadr (x ) (car (cdr x ) ) )(defn cddr (x ) (cdr (cdr x ) ) )(defn caddr (x ) (car (cdr (cdr x ) ) ) )(defn cdddr (x ) (cdr (cdr (cdr x ) ) ) )(defn cadddr (x ) (car (cdr (cdr (cdr x ) ) ) ) )(def list-reverse reverse )(defn list-ref (l i ) (if (= i 0 ) (car l ) (list-ref (cdr l ) (- i 1 ) ) ) )(defn list-append (l0 l1 ) (defn list-append-iter0 (l0 ) (if (null? l0 ) l1 (cons (car l0 ) (list-append-iter0 (cdr l0 ) ) ) ) ) (if (pair? l1 ) (list-append-iter0 l0 ) (begin (set! l1 (cons l1 (quote () ) ) ) (list-append-iter0 l0 ) ) ) )(defn list->vector (l ) (defn list->vector-iter (l output ) (if (null? l ) output (list->vector-iter (cdr l ) (vector-push! output (car l ) ) ) ) ) (list->vector-iter l (Vector! ) ) )(defn list-foreach (l f ) (if (null? l ) () (if (eq? (f (car l ) ) (quote break ) ) () (list-foreach (cdr l ) f ) ) ) )(defn vector->list (v ) (def length (vector-length v ) ) (defn vector->list-iter (v i ) (if (= i length ) (quote () ) (cons (v i ) (vector->list-iter v (+ i 1 ) ) ) ) ) (vector->list-iter v 0 ) )(defn vector-find (v o start-index ) (set! start-index (or start-index 0 ) ) (def length (vector-length v ) ) (defn vector-find-iter (i ) (if (= i length ) -1 (if (eq? (v i ) o ) i (vector-find-iter (+ i 1 ) ) ) ) ) (vector-find-iter start-index ) )(defn vector-foreach (v f ) (def v-length (vector-length v ) ) (defn vector-foreach-iter (count ) (if (= count v-length ) () (if (eq? (f count (v count ) ) (quote break ) ) () (vector-foreach-iter (+ count 1 ) ) ) ) ) (vector-foreach-iter 0 ) )(defn table-foreach (t f ) (def keys (table-keys t ) ) (defn table-foreach-iter (keys ) (if (null? keys ) () (if (eq? (f (car keys ) (t (car keys ) ) ) (quote break ) ) () (table-foreach-iter (cdr keys ) ) ) ) ) (table-foreach-iter keys ) )(defn foreach (value func ) (def t (typeof value ) ) (cond (eq? t (quote pair ) ) (list-foreach value func ) (eq? t (quote vector ) ) (vector-foreach value func ) (eq? t (quote table ) ) (table-foreach value func ) else (print \"ERROR: Function foreach invalid param type\" ) ) )(defn slice (v start end ) (def t (typeof v ) ) (set! end (or end (cond (eq? t (quote pair ) ) (list-length v ) (eq? t (quote string ) ) (string-length v ) (eq? t (quote vector ) ) (vector-length v ) ) ) ) (cond (eq? t (quote pair ) ) (list-slice v start end ) (eq? t (quote string ) ) (string-slice v start end ) (eq? t (quote vector ) ) (vector-slice v start end ) else () ) )(defn times (n f ) (defn times-iter (c ) (if (= c n ) () (begin (f c ) (times-iter (+ c 1 ) ) ) ) ) (times-iter 0 ) )(defn for_fn (start end step f ) (def judge-sign (if (> step 0 ) >= <= ) ) (defn for-iter (i ) (if (judge-sign i end ) () (if (eq? (f i ) (quote break ) ) () (for-iter (+ i step ) ) ) ) ) (for-iter start ) )(defmacro for ((start end step fn ) (quasiquote (for_fn (unquote start ) (unquote end ) (unquote step ) (unquote fn ) ) ) ) ((start end fn ) (quasiquote (for_fn (unquote start ) (unquote end ) 1 (unquote fn ) ) ) ) ((end fn ) (quasiquote (for_fn 0 (unquote end ) 1 (unquote fn ) ) ) ) )(defn map1 (f p ) (if (null? p ) (quote () ) (cons (f (car p ) ) (map1 f (cdr p ) ) ) ) )(defn map (f p0 . p ) (if (null? p0 ) (quote () ) (cons (apply f (map1 car (cons p0 p ) ) ) (apply map (cons f (map1 cdr (cons p0 p ) ) ) ) ) ) )(defn apply-test (f p0 . p ) (apply f (map1 car (cons p0 p ) ) ) )(def filter (fn (func l ) (if (null? l ) (quote () ) (if (func (car l ) ) (cons (car l ) (filter func (cdr l ) ) ) (filter func (cdr l ) ) ) ) ) )(defn reduce (f l param-num ) (set! param-num (if param-num param-num 2 ) ) (defn get-num-element-list (l num ) (if (= num 0 ) (quote () ) (if (null? l ) (print \"ERROR: reduce function invalid parameters list\" ) (cons (car l ) (get-num-element-list (cdr l ) (- num 1 ) ) ) ) ) ) (defn cdrn (l num ) (if (= num 0 ) l (cdrn (cdr l ) (- num 1 ) ) ) ) (defn reduce-iter (f l param-num-1 acc ) (if (null? l ) acc (reduce-iter f (cdrn l param-num-1 ) param-num-1 (apply f (cons acc (get-num-element-list l param-num-1 ) ) ) ) ) ) (if (null? l ) (quote () ) (reduce-iter f (cdr l ) (- param-num 1 ) (car l ) ) ) )(defn curry (func . arg ) (fn (. another-arg ) (apply func (list-append arg another-arg ) ) ) )(def Object (Table \"type\" (quote Object ) \"clone\" (fn (self ) (def new-object (Table ) ) (add-proto new-object self ) new-object ) ) )(add-proto Object () )(def vector (Table \"length\" vector-length \"push!\" vector-push! \"pop!\" vector-pop! \"slice\" vector-slice \"->list\" vector->list \"find\" vector-find \"foreach\" vector-foreach ) )(def list (Table \"length\" list-length \"slice\" list-slice \"reverse\" list-reverse \"ref\" list-ref \"append\" list-append \"->vector\" list->vector \"foreach\" list-foreach ) )(def table (Table \"keys\" table-keys \"delete\" table-delete \"foreach\" table-foreach \"length\" table-length ) )(def string (Table \"slice\" string-slice \"length\" string-length \"append\" string-append \"->int\" string->int \"->float\" string->float \"find\" string-find \"replace\" string-replace \"char-code-at\" string-char-code-at \"from-char-code\" string-from-char-code ) )(def math (Table \"cos\" math-cos \"sin\" math-sin \"tan\" math-tan \"acos\" math-acos \"asin\" math-asin \"atan\" math-atan \"cosh\" math-cosh \"sinh\" math-sinh \"tanh\" math-tanh \"log\" math-log \"exp\" math-exp \"log10\" math-log10 \"pow\" math-pow \"sqrt\" math-sqrt \"ceil\" math-ceil \"floor\" math-floor ) )(def int (Table \"->string\" int->string ) )(def float (Table \"->string\" float->string \"->int64\" float->int64 ) )(def ratio (Table \"numer\" ratio-numer \"denom\" ratio-denom ) )(def file (Table \"read\" file-read \"write\" file-write \"abs-path\" file-abs-path ) )(def sys (Table \"argv\" sys-argv \"cmd\" sys-cmd ) )";