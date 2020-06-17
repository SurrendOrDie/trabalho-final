RUNMO = ./Monitor
RUNSI = ./Simulador

main: clean compile runm runs

clean:
	@rm -f log.log
	@rm -f estatisticas.est
	@rm -f *.o

compile:
	@gcc -g -c *.c
	@gcc Util*.o Simulador*.o -lpthread -g -o Simulador
	@gcc Util*.o Monitor.o -lpthread -g -o Monitor

runm:
			@x-terminal-emulator --geometry "5x5" -e "bash -c $(RUNMO);bash" > /dev/null;
runs:
			@x-terminal-emulator --geometry "5x5" -e "bash -c $(RUNSI);bash" > /dev/null;
