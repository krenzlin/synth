.PHONY: profile test write perf

test:
	./tup bin/tests
	bin/tests

profile:
	./tup bin/profile
	rm -f callgrind.out.*
	valgrind --tool=callgrind --dump-instr=yes bin/profile
	kcachegrind callgrind.out.*
	rm -f callgrind.out.*

write:
	./tup bin/write_to_file
	bin/write_to_file

perf:
	./tup bin/profile
	perf record bin/profile
	perf report
