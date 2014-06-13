all:
	gcc main.c -o walley -lm
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
	gcc main.c -O3 -o walley -lm
test:
	gcc main.c -g -Wall -Werror -o walley -lm
clean:
	rm -rf *.o walley a.out
