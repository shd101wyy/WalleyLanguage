#install_walley_core:  # create walley_core.h from walley_core.wa
#	gcc helper.c -o helper
#	./helper      # this program will generate walley_core.h
#	rm -rf helper

all:
	gcc helper.c -o helper -lm;./helper;rm -rf helper
	gcc -O1 main.c -o walley -lm
install_walley_core:  # create walley_core.h from walley_core.wa
	gcc helper.c -o helper -lm
	./helper      # this program will generate walley_core.h
	rm -rf helper
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
	gcc helper.c -o helper -lm;./helper;rm -rf helper
	gcc -O3 main.c -o walley -lm
test:
	gcc helper.c -o helper -lm;./helper;rm -rf helper
	gcc main.c -g -Wall -Werror -o walley -lm
clean:
	rm -rf *.o walley a.out walley_core.h
