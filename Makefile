all:
	g++ -o pdu pdu.c `pkg-config --libs --cflags libcurl` -O2
