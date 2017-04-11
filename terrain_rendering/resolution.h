#ifndef RESOLUTION
#define RESOLUTION

class Resolution {
public:
    Resolution(int width, int height): _width(width), _height(height) { }
    unsigned int width() { return _width; }
    unsigned int height() { return _height; }

private:
    unsigned int _width;
    unsigned int _height;
};

#endif // RESOLUTION
