all:
	@if [ ! -d /usr/include/rapidxml ];then \
		if [ ! -d rapidxml ];then \
		    if [ ! -f rapidxml-1.13.zip ];then \
			wget https://netcologne.dl.sourceforge.net/project/rapidxml/rapidxml/rapidxml%201.13/rapidxml-1.13.zip; \
		    fi; \
		    unzip rapidxml-1.13.zip; \
		    mv rapidxml-1.13 rapidxml; \
		fi \
	fi
	g++ -o pdu pdu.c `pkg-config --libs --cflags libcurl` -O2 -I.
