class Pixel {
private:
    unsigned char r, g, b;

public:
    Pixel(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);

    bool isBlackAndWhite();

    bool isBlack();

    bool isWhite();

    unsigned char getBrightness();

    void setBrightness(unsigned char brightness);
};
