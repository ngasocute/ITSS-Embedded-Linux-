all:
	rm -f *.o start shutdown mng ctr op1 opx
	gcc -c sigs.c	
	gcc -o mng liftMng.c sigs.o `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	gcc -o ctr liftCtrl.c sigs.o	
	gcc -o op1 opePanel1.c sigs.o `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	gcc -o opx opePanelX.c sigs.o `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	gcc -o start start.c sigs.o
	gcc -o shutdown shutdown.c sigs.o	
start: start.c
	rm -f start
	gcc -o start start.c sigs.o
shutdown: shutdown.c
	rm -f shutdown
	gcc -o shutdown shutdown.c sigs.o	
mng: liftMng.c
	rm -f mng
	gcc -o mng liftMng.c sigs.o `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
ctr: liftCtrl.c
	rm -f ctr
	gcc -o ctr liftCtrl.c sigs.o
op1: opePanel1.c
	rm -f op1
	gcc -o op1 opePanel1.c sigs.o `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
opx: opePanelX.c
	rm -f opx
	gcc -o opx opePanelX.c sigs.o `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
clean:
	rm -f *.o start shutdown mng ctr op1 opx
