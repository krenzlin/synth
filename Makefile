.PHONY: profile test

test:
	./tup bin/tests
	bin/tests

profile:
	./tup bin/profile
	rm -f callgrind.out.*
	valgrind --tool=callgrind --dump-instr=yes bin/profile
	kcachegrind callgrind.out.*
	rm -f callgrind.out.*
