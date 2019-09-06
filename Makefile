.PHONY: profile test write perf callgraph

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

callgraph:
	./tup bin/profile
	perf record --call-graph dwarf bin/profile
	perf report --call-graph --stdio

perf:
	./tup bin/profile
	perf record bin/profile
	perf report
