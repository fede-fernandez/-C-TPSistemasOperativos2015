H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')


install:
		cp -u Debug/libcommonsDeAsedio.so /usr/lib
		cp  --parents -u $(H_SRCS) /usr/include
