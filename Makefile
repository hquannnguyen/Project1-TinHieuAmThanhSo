CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET   = audio_demo
SRCS     = main.cpp
HEADERS  = AudioSignal.h AudioComponents.h AudioFile.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) -lm
	@echo "Build thanh cong: ./$(TARGET)"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.wav
	@echo "Da don sach."
