APP		:= app
all: app

app: app.c
	arm-none-linux-gnueabi-gcc -static -o $(APP) $(APP).c

clean:
	rm -rf app
