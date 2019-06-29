all: EVSystem

EVSystem: EVSystem.java
	javac EVSystem.java
	javah EVSystem
	sh execution.sh

clean:
	rm -rf *.class
	rm -rf *.h
	rm -rf *.so
