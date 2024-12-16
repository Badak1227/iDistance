CFLAGS = -std=c99 -g -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lm -pthread

# 빌드 타겟
iDistance.out: iDistance.o bpTree.o
	gcc $(CFLAGS) -o iDistance.out iDistance.o bpTree.o $(LDFLAGS)
	@rm -f *.o  # 빌드 후 목적 파일 삭제

# 개별 파일 컴파일
iDistance.o: bpTree.h iDistance.c
	gcc $(CFLAGS) -c -o iDistance.o iDistance.c

bpTree.o: bpTree.h bpTree.c
	gcc $(CFLAGS) -c -o bpTree.o bpTree.c

# clean 규칙
clean:
	rm -f iDistance.out
