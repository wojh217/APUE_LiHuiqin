CFLAGS+=-pthread
LDFLAGS+=-pthread

all: mytb

mytbf:main.o mytbf.o
    gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
    rm -rf *.o mytbf