unbuf_chan_test: UnBufChanTest.cpp UnBufChan.hpp
	clang++ -std=c++23 -g -fsanitize=thread $< -o $@

disable-aslr:
	echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
