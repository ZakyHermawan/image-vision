main:
	g++ src/image.cpp \
	src/test.cpp \
	src/process_image.cpp \
	src/resize_image.cpp \
	src/filter_image.cpp -o image -Iinclude/
