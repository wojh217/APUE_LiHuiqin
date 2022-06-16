CFLAGS+=-pthread
LDFLAGS+=-pthread

all: mytbf

mytbf:main.o mytbf.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o mytbf