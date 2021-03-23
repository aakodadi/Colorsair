
all: *.cpp *.hpp
	g++ *.cpp -o colorsair -std=c++14 -lusb-1.0

clean:
	$(RM) colorsair

.PHONY: all clean