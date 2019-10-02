all:
	g++ watch.cpp -o watch -O2 -ltesseract -llept -lGdi32 -liconv

