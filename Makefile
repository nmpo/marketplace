marketplace: marketplace.cpp utils.h picosha2.h
	g++  marketplace.cpp -o marketplace

all: marketplace

clean:
	rm marketplace