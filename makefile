all:	test buddy

test: test.c
	gcc test.c -Wall -Wextra -pedantic -o test

buddy: buddy.c
	gcc buddy.c -Wall -Wextra -pedantic -o buddy

clean:
	rm -f test
	rm -f buddy

