// helper file, load walley_wore.wa
char WALLEY_CORE_CONTENT[] = "(def walley-core-author (quote Yiyi-Wang ) )(def walley-core-version \"0.00034\" )(def walley-language-version \"0.3.8496\" )(def walley (Table \"author\" (quote Yiyi-Wang ) \"core-author\" (quote Yiyi-Wang ) \"core-version\" walley-core-version \"language-version\" walley-language-version ) )(defmacro println ((. args ) (quasiquote (print (unquote-splice args ) \"\n\" ) ) ) )(def break (quote break ) )(def continue (quote continue ) )(def int? (lambda (v ) (eq? (typeof v ) (quote integer ) ) ) )(def integer? int? )(def float? (lambda (v ) (eq? (typeof v ) (quote float ) ) ) )(def pair? (lambda (v ) (eq? (typeof v ) (quote pair ) ) ) )(def list? pair? )(def ratio? (v ) (eq? (typeof v ) (quote ratio ) ) )(def vector? (lambda (v ) (eq? (typeof v ) (quote vector ) ) ) )(def lambda? (lambda (v ) (eq? (typeof v ) (quote lambda ) ) ) )(def table? (lambda (v ) (eq? (typeof v ) (quote table ) ) ) )(def string? (lambda (v ) (eq? (typeof v ) (quote string ) ) ) )(def fn? lambda? )(def object? (v ) (eq? (typeof v ) (quote object ) ) )(def not (fn (a ) (if a () 1 ) ) )(def append (lambda (a b ) (if (null? a ) b (cons (car a ) (append (cdr a ) b ) ) ) ) )(def list-reverse (fn (a ) (def reverse_iter (fn (a result ) (if (null? a ) result (reverse_iter (cdr a ) (cons (car a ) result ) ) ) ) ) (reverse_iter a () ) ) )(def error (lambda (s ) (print \"ERROR: \" ) (print s ) ) )(defmacro defn ((func_name args . body ) (quasiquote (def (unquote func_name ) (lambda (unquote args ) (unquote-splice body ) ) ) ) ) )(def List (fn (. args ) args ) )(defmacro cond (() () ) ((#else body ) body ) ((#else body . rest ) (print \"ERROR: cond invalid statements.\n\" ) ) ((test body ) (quasiquote (if (unquote test ) (unquote body ) (quote () ) ) ) ) ((test body . rest ) (quasiquote (if (unquote test ) (unquote body ) (cond (unquote-splice rest ) ) ) ) ) )(defmacro and (() (quote true ) ) ((a ) a ) ((a . b ) (quasiquote (if (unquote a ) (and (unquote-splice b ) ) () ) ) ) )(defmacro or (() () ) ((a ) a ) ((a . b ) (quasiquote (if (unquote a ) (unquote a ) (or (unquote-splice b ) ) ) ) ) )(defmacro defm ((macro-name args pattern ) (quasiquote (defmacro (unquote macro-name ) ((unquote args ) (unquote pattern ) ) ) ) ) )(def list-length (lambda (n ) (def list-length-iter (lambda (n r ) (if (null? n ) r (list-length-iter (cdr n ) (+ r 1 ) ) ) ) ) (list-length-iter n 0 ) ) )(def list-slice-helper (l end ) (if (= end 0 ) () (cons (car l ) (list-slice-helper (cdr l ) (- end 1 ) ) ) ) )(def list-slice-remove-head (l start ) (if (= start 0 ) l (list-slice-remove-head (cdr l ) (- start 1 ) ) ) )(def list-slice (l start end ) (set! end (or end (list-length l ) ) ) (list-slice-helper (list-slice-remove-head l start ) (- end start ) ) )(def str (. b ) (def str-iter (b ) (if (null? b ) \"\" (string-append (car b ) (str-iter (cdr b ) ) ) ) ) (str-iter b ) )(def len (b ) (if (null? b ) 0 (if (string? b ) (string-length b ) (if (pair? b ) (list-length b ) (if (vector? b ) (vector-length b ) (error \"Function len invalid data type\" ) ) ) ) ) )(def string=? (a b ) (= (strcmp a b ) 0 ) )(def string>? (a b ) (> (strcmp a b ) 0 ) )(def string<? (a b ) (< (strcmp a b ) 0 ) )(def string>=? (a b ) (>= (strcmp a b ) 0 ) )(def string<=? (a b ) (<= (strcmp a b ) 0 ) )(def ** math-pow )(def caar (x ) (car (car x ) ) )(def cadr (x ) (car (cdr x ) ) )(def cddr (x ) (cdr (cdr x ) ) )(def caddr (x ) (car (cdr (cdr x ) ) ) )(def cdddr (x ) (cdr (cdr (cdr x ) ) ) )(def cadddr (x ) (car (cdr (cdr (cdr x ) ) ) ) )(def list-ref (l i ) (if (= i 0 ) (car l ) (list-ref (cdr l ) (- i 1 ) ) ) )(def list-append (l0 l1 ) (def list-append-iter0 (l0 ) (if (null? l0 ) l1 (cons (car l0 ) (list-append-iter0 (cdr l0 ) ) ) ) ) (if (pair? l1 ) (list-append-iter0 l0 ) (begin (set! l1 (cons l1 (quote () ) ) ) (list-append-iter0 l0 ) ) ) )(def list->vector (l ) (def list->vector-iter (l output ) (if (null? l ) output (list->vector-iter (cdr l ) (vector-push! output (car l ) ) ) ) ) (list->vector-iter l (Vector! ) ) )(def list-foreach (l f ) (if (null? l ) () (if (eq? (f (car l ) ) (quote break ) ) () (list-foreach (cdr l ) f ) ) ) )(def list-assoc (fn (l key ) (if (null? l ) () (if (eq? key (car (car l ) ) ) (cdr (car l ) ) (list-assoc (cdr l ) key ) ) ) ) )(def vector->list (v ) (def length (vector-length v ) ) (def vector->list-iter (v i ) (if (= i length ) (quote () ) (cons (v i ) (vector->list-iter v (+ i 1 ) ) ) ) ) (vector->list-iter v 0 ) )(def vector-find (v o start-index ) (set! start-index (or start-index 0 ) ) (def length (vector-length v ) ) (def vector-find-iter (i ) (if (= i length ) -1 (if (eq? (v i ) o ) i (vector-find-iter (+ i 1 ) ) ) ) ) (vector-find-iter start-index ) )(def vector-foreach (v f ) (def v-length (vector-length v ) ) (def vector-foreach-iter (count ) (if (= count v-length ) () (if (eq? (f count (v count ) ) (quote break ) ) () (vector-foreach-iter (+ count 1 ) ) ) ) ) (vector-foreach-iter 0 ) )(def table-foreach (t f ) (def keys (table-keys t ) ) (def table-foreach-iter (keys ) (if (null? keys ) () (if (eq? (f (car keys ) (t (car keys ) ) ) (quote break ) ) () (table-foreach-iter (cdr keys ) ) ) ) ) (table-foreach-iter keys ) )(def foreach (value func ) (def t (typeof value ) ) (cond (eq? t (quote pair ) ) (list-foreach value func ) (eq? t (quote vector ) ) (vector-foreach value func ) (eq? t (quote table ) ) (table-foreach value func ) else (print \"ERROR: Function foreach invalid param type\" ) ) )(def slice (v start end ) (def t (typeof v ) ) (set! end (or end (cond (eq? t (quote pair ) ) (list-length v ) (eq? t (quote string ) ) (string-length v ) (eq? t (quote vector ) ) (vector-length v ) ) ) ) (cond (eq? t (quote pair ) ) (list-slice v start end ) (eq? t (quote string ) ) (string-slice v start end ) (eq? t (quote vector ) ) (vector-slice v start end ) else () ) )(def times (n f ) (def times-iter (c ) (if (= c n ) () (begin (f c ) (times-iter (+ c 1 ) ) ) ) ) (times-iter 0 ) )(def for_fn (start end step f ) (def judge-sign (if (> step 0 ) >= <= ) ) (def for-iter (i ) (if (judge-sign i end ) () (if (eq? (f i ) (quote break ) ) () (for-iter (+ i step ) ) ) ) ) (for-iter start ) )(defmacro for ((start end step fn ) (quasiquote (for_fn (unquote start ) (unquote end ) (unquote step ) (unquote fn ) ) ) ) ((start end fn ) (quasiquote (for_fn (unquote start ) (unquote end ) 1 (unquote fn ) ) ) ) ((end fn ) (quasiquote (for_fn 0 (unquote end ) 1 (unquote fn ) ) ) ) )(def list-map1 (f p ) (if (null? p ) (quote () ) (cons (f (car p ) ) (list-map1 f (cdr p ) ) ) ) )(def list-map (f p0 . p ) (if (null? p0 ) (quote () ) (cons (apply f (list-map1 car (cons p0 p ) ) ) (apply list-map (cons f (list-map1 cdr (cons p0 p ) ) ) ) ) ) )(def list-zip (. args ) (apply list-map (cons List args ) ) )(def vector-map1 (f p count result length-of-vector ) (if (= count length-of-vector ) result (vector-map1 f p (+ count 1 ) (vector-push! result (f (p count ) ) ) length-of-vector ) ) )(def vector-get-nth-arguments (n args ) (if (null? args ) () (cons ((car args ) n ) (vector-get-nth-arguments n (cdr args ) ) ) ) )(def vector-map-helper (f count result length-of-1st-vector p ) (if (= count length-of-1st-vector ) result (begin (vector-push! result (apply f (vector-get-nth-arguments count p ) ) ) (vector-map-helper f (+ count 1 ) result length-of-1st-vector p ) ) ) )(def vector-map (f p . args ) (vector-map-helper f 0 (Vector! ) (vector-length p ) (cons p args ) ) )(def vector-zip (. args ) (apply vector-map (cons Vector! args ) ) )(def list-filter (fn (func l ) (if (null? l ) (quote () ) (if (func (car l ) ) (cons (car l ) (list-filter func (cdr l ) ) ) (list-filter func (cdr l ) ) ) ) ) )(def vector-filter (func v ) (def v-length (vector-length v ) ) (def helper (count result ) (if (= count v-length ) result (begin (if (func (v count ) ) (vector-push! result (v count ) ) ) (helper (+ count 1 ) result ) ) ) ) (helper 0 (Vector! ) ) )(def reduce (f l param-num ) (set! param-num (if param-num param-num 2 ) ) (def get-num-element-list (l num ) (if (= num 0 ) (quote () ) (if (null? l ) (print \"ERROR: reduce function invalid parameters list\" ) (cons (car l ) (get-num-element-list (cdr l ) (- num 1 ) ) ) ) ) ) (def cdrn (l num ) (if (= num 0 ) l (cdrn (cdr l ) (- num 1 ) ) ) ) (def reduce-iter (f l param-num-1 acc ) (if (null? l ) acc (reduce-iter f (cdrn l param-num-1 ) param-num-1 (apply f (cons acc (get-num-element-list l param-num-1 ) ) ) ) ) ) (if (null? l ) (quote () ) (reduce-iter f (cdr l ) (- param-num 1 ) (car l ) ) ) )(def curry (func . arg ) (fn (. another-arg ) (apply func (list-append arg another-arg ) ) ) )(def Object (Table \"type\" (quote Object ) \"clone\" (fn (self new-object ) (if new-object (begin (add-proto new-object self ) new-object ) (begin (def o (Table ) ) (add-proto o self ) o ) ) ) ) )(add-proto Object () )(def abs (x ) (if (> x 0 ) x (- x ) ) )(def chr (x ) (string-from-char-code x ) )(def ord (x ) (string-char-code-at x 0 ) )(def while-func (test body ) (if (test ) (if (eq? (body ) (quote break ) ) () (while-func test body ) ) () ) )(defm while (test . body ) (quasiquote (while-func (fn () (unquote test ) ) (fn () (unquote-splice body ) ) ) ) )(def multi-table (Table ) )(def multi (judge-fn dispatch-object ) (if (null? dispatch-object ) (set! dispatch-object (Table ) ) ) (def r (fn (. args ) (apply (dispatch-object (apply judge-fn args ) ) args ) ) ) (multi-table r dispatch-object ) r )(def defmethod (var judge func ) ((multi-table var ) judge func ) )(def map (multi (fn (f x . y ) (typeof x ) ) (Table \"vector\" vector-map \"pair\" list-map ) ) )(def filter (multi (fn (f x ) (typeof x ) ) ) )(defmethod filter (quote vector ) vector-filter )(defmethod filter (quote pair ) list-filter )(def vector (Table \"length\" vector-length \"push!\" vector-push! \"pop!\" vector-pop! \"slice\" vector-slice \"->list\" vector->list \"find\" vector-find \"foreach\" vector-foreach \"map\" vector-map \"filter\" vector-filter \"zip\" vector-zip ) )(def vec vector )(def list (Table \"length\" list-length \"slice\" list-slice \"reverse\" list-reverse \"ref\" list-ref \"append\" list-append \"->vector\" list->vector \"foreach\" list-foreach \"assoc\" list-assoc \"map\" list-map \"filter\" list-filter \"zip\" list-zip ) )(def table (Table \"keys\" table-keys \"delete!\" table-delete \"foreach\" table-foreach \"length\" table-length ) )(def string (Table \"slice\" string-slice \"length\" string-length \"append\" string-append \"->int\" string->int \"->float\" string->float \"find\" string-find \"replace\" string-replace \"char-code-at\" string-char-code-at \"from-char-code\" string-from-char-code ) )(def math (Table \"cos\" math-cos \"sin\" math-sin \"tan\" math-tan \"acos\" math-acos \"asin\" math-asin \"atan\" math-atan \"cosh\" math-cosh \"sinh\" math-sinh \"tanh\" math-tanh \"log\" math-log \"exp\" math-exp \"log10\" math-log10 \"pow\" math-pow \"sqrt\" math-sqrt \"ceil\" math-ceil \"floor\" math-floor ) )(def int (Table \"->string\" int->string ) )(def float (Table \"->string\" float->string \"->int64\" float->int64 ) )(def ratio (Table \"numer\" ratio-numer \"denom\" ratio-denom ) )(def file (Table \"read\" file-read \"write\" file-write \"abs-path\" file-abs-path \"close\" file-close \"open\" File \"readlines\" file-readlines \"seek\" file-seek \"SEEK_SET\" 1 \"SEEK_END\" 2 \"SEEK_CUR\" 3 ) )(def sys (Table \"argv\" sys-argv \"cmd\" sys-cmd ) )(def os (Table \"fork\" os-fork \"getpid\" os-getpid \"getppid\" os-getppid \"waitpid\" os-waitpid \"getenv\" os-getenv ) )(def walley-modules (Table \"vector\" vector \"list\" list \"table\" table \"string\" string \"math\" math \"int\" int \"float\" float \"ratio\" ratio \"file\" file \"sys\" sys \"os\" os ) )";