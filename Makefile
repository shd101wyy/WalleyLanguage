#install_walley_core:  # create walley_core.h from walley_core.wa
#	gcc helper.c -o helper
#	./helper      # this program will generate walley_core.h
#	rm -rf helper

all:
	$(CC) helper.c -o helper -lm;./helper;rm -rf helper
	$(CC) -O1 main.c -o walley -lm
install_walley_core:  # create walley_core.h from walley_core.wa
	$(CC) helper.c -o helper -lm
	./helper      # this program will generate walley_core.h
	rm -rf helper
emscripten:
	emcc main.c -O3 -o walley.js -s EXPORTED_FUNCTIONS="['_Walley_init_for_js', '_Walley_RunStringAndReturnString']" --memory-init-file 0
	echo "var Walley_init_for_js = Module.cwrap('Walley_init_for_js', '', []);" >> walley.js
	echo "var Walley_RunStringAndReturnString = Module.cwrap('Walley_RunStringAndReturnString', 'string', ['string']);" >> walley.js
	echo "var walley = {init: Walley_init_for_js, runStr: Walley_RunStringAndReturnString}" >> walley.js
install:
	rm -rf /usr/local/bin/walley
	rm -rf /usr/local/include/walley
	rm -rf /usr/local/lib/walley
	cp walley /usr/local/bin
	mkdir /usr/local/include/walley
	mkdir /usr/local/lib/walley
	cp *.wa /usr/local/lib/walley/
	cp *.h /usr/local/include/walley/
	rm -rf *.o walley a.out
uninstall:
	rm -rf /usr/local/bin/walley
	rm -rf /usr/local/include/walley
	rm -rf /usr/local/lib/walley
fast:
	$(CC) helper.c -o helper -lm;./helper;rm -rf helper
	$(CC) -O3 main.c -o walley -lm
test:
	$(CC) helper.c -o helper -lm;./helper;rm -rf helper
	$(CC) main.c -g -Wall -Werror -o walley -lm
clean:
	rm -rf *.o walley a.out walley_core.h
