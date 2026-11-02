TARGETS = sum10 sum1000 sum1000000

all: $(TARGETS)

sum: sum.c
	gcc sum.c -o sum

$(TARGETS): sum%: sum random.sh
	bash random.sh $* | ./sum > $@

clean:
	rm -rf $(TARGETS)
