CXXFLAGS ?= -std=c++17 -Dcimg_display=0 -Dcimg_use_png

LDLIBS = -lm -lpng16

HEADER_FILES = src
SOURCE_FILES = src/captcha.cpp

OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

priv/captcha: clean $(OBJECT_FILES)
	mkdir -p priv
	$(CXX) -I $(HEADER_FILES) -o $@ $(LDFLAGS) $(OBJECT_FILES) $(LDLIBS)

clean:
	rm -f priv/captcha $(OBJECT_FILES) $(BEAM_FILES)
