compile_and_exec:
	@rm -f log.log
	@rm -f *.o
	@gcc -g -c *.c
	@gcc Util*.o Simulador*.o -lpthread -g -o Simulador
	@gcc Util*.o Monitor.o -lpthread -g -o Monitor
