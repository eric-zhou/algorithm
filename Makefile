all:exp

exp:exp.cc
	g++ $^ -o $@

clean:
	rm -rf *.o exp 
