CXX ?= g++
SRC = sim_test.cpp test_hal.cpp ../win_x86_qt_gui/plant.cpp \
      ../common/behavior.cpp ../common/chronometer.cpp \
      ../common/sm1.cpp ../common/sm2.cpp ../common/sm3.cpp ../common/sm4.cpp

sim_test: $(SRC)
	$(CXX) -std=c++17 -Wall -Wextra -o $@ $(SRC)

test: sim_test
	./sim_test

clean:
	rm -f sim_test
