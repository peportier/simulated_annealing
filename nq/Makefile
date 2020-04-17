# nq - simulated annealing applied to n-queens
# See LICENSE file for copyright and license details.

# CFLAGS = -O3  (for best performance)
# CFLAGS = -g   (for debugging)
CFLAGS = -O3 -Wall

LIBS = -lm

TARGET = nq

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
