#include <array>
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <string.h>

const int W = 888;
const int H = 888;

int xytoint(int x, int y) {
    return (W * (H - y) + x) * 4;
}

bool comparePoints(const std::array<double, 2>& arr1, const std::array<double, 2>& arr2){
    return (arr1[0] < arr2[0]);
}

struct Spline {
    double a, b, c, d, x;
};

std::vector<Spline> cubicSpline(const std::vector<double>& x, const std::vector<double>& y) {
    int n = x.size() - 1;
    std::vector<double> a(n + 1), b(n), d(n), h(n), alpha(n), c(n + 1), l(n + 1), mu(n + 1), z(n + 1);


    for (int i = 0; i <= n; ++i) a[i] = y[i];

    for (int i = 0; i < n; ++i) h[i] = x[i + 1] - x[i];

    for (int i = 1; i < n; ++i) alpha[i] = (3.0 / h[i]) * (a[i + 1] - a[i]) - (3.0 / h[i - 1]) * (a[i] - a[i - 1]);

    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;
    for (int i = 1; i < n; ++i) {
        l[i] = 2 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[n] = 1;
    z[n] = 0;
    c[n] = 0;
    for (int j = n - 1; j >= 0; --j) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = (a[j + 1] - a[j]) / h[j] - h[j] * (c[j + 1] + 2 * c[j]) / 3.0;
        d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]);
    }

    std::vector<Spline> splines(n);
    for (int i = 0; i < n; ++i) {
        splines[i] = {a[i], b[i], c[i], d[i], x[i]};
    }

    return splines;
}

double evaluateSpline(const std::vector<Spline>& splines, double x) {
    Spline s;
    for (const auto& spline : splines) {
        if (x >= spline.x) {
            s = spline;
        } else {
            break;
        }
    }
    double dx = x - s.x;
    return s.a + s.b * dx + s.c * dx * dx + s.d * dx * dx * dx;
}

double firstDerivative(const std::vector<Spline>& splines, double x) {
    Spline s;
    for (const auto& spline : splines) {
        if (x >= spline.x) {
            s = spline;
        } else {
            break;
        }
    }
    double dx = x - s.x;
    return s.b + s.c * dx * 2 + s.d * dx * dx * 3;
}

double secondDerivative(const std::vector<Spline>& splines, double x) {
    Spline s;
    for (const auto& spline : splines) {
        if (x >= spline.x) {
            s = spline;
        } else {
            break;
        }
    }
    double dx = x - s.x;
    return s.c * 2 + s.d * dx * 6;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(W, H), "SFML works!");
    std::vector<std::array<double, 2>> pos;

    std::vector<sf::CircleShape> circles;

    std::vector<Spline> splines;

    uint8_t* pixels = new uint8_t[W*H*4];
    sf::Texture texture;
    texture.create(W, H);
    sf::Sprite sprite(texture);
    for(int i = 0; i < W*H*4; i += 4) {
        pixels[i] = 0;
        pixels[i+1] = 0;
        pixels[i+2] = 0;
        pixels[i+3] = 255;
    }

    texture.update(pixels);


    sf::Font font;
    if (!font.loadFromFile("../comic.ttf")) {
        std::cerr << "couldnt load font\n";
    }

    sf::Text textPosition;
    textPosition.setFont(font);
    textPosition.setCharacterSize(24);
    textPosition.setFillColor(sf::Color::White);
    textPosition.setPosition(0,0);

    sf::Text textInp;
    textInp.setFont(font);
    textInp.setCharacterSize(24);
    textInp.setFillColor(sf::Color::Green);
    textInp.setPosition(0,24);

    sf::Text textValue;
    textValue.setFont(font);
    textValue.setCharacterSize(24);
    textValue.setFillColor(sf::Color::Green);
    textValue.setPosition(0,48);

    sf::Text fDerivative;
    fDerivative.setFont(font);
    fDerivative.setCharacterSize(24);
    fDerivative.setFillColor(sf::Color::Green);
    fDerivative.setPosition(0,72);

    sf::Text sDerivative;
    sDerivative.setCharacterSize(24);
    sDerivative.setFillColor(sf::Color::Green);
    sDerivative.setFont(font);
    sDerivative.setPosition(0,96);

    sf::CircleShape pointer(6);
    pointer.setPosition(-6, -6);
    pointer.setFillColor(sf::Color::Green);

    std::string textInput = "";
    double checkValue = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                try {
                    checkValue = std::stod(textInput);
                    double resultOfCheck = evaluateSpline(splines, checkValue);
                    double fd = firstDerivative(splines, checkValue);
                    double sd = secondDerivative(splines, checkValue);

                    pointer.setPosition(checkValue - 6, H - resultOfCheck - 6);

                    textValue.setString("f(" + std::to_string(checkValue) + ") = " + std::to_string(resultOfCheck));
                    fDerivative.setString("f'(" + std::to_string(checkValue) + ") = " + std::to_string(fd));
                    sDerivative.setString("f''(" + std::to_string(checkValue) + ") = " + std::to_string(sd));

                    //std::cout << "Parsed value: " << evaluateSpline(splines, checkValue) << std::endl;
                    textInput.clear();
                } catch (const std::invalid_argument&) {
                    std::cerr << "";
                } catch (const std::out_of_range&) {
                    std::cerr << "";
                }
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !textInput.empty()) {
                    textInput.pop_back();
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    textInput += static_cast<char>(event.text.unicode);
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                double mouseX = double(sf::Mouse::getPosition(window).x);
                double mouseY = double(sf::Mouse::getPosition(window).y);

                bool removed = false;

                for(int i = 0; i < pos.size(); i++) {
                    if (pos[i][0] - 6 < mouseX && pos[i][0] + 6 > mouseX) {
                        removed = true;
                        pos.erase(pos.begin() + i);
                    }
                    if (circles[i].getPosition().x < mouseX && circles[i].getPosition().x + 12 > mouseX) {
                        circles.erase(circles.begin() + i);
                    }
                }

                if ( ! removed ) {
                    pos.push_back({mouseX, H - mouseY});
                    std::sort(pos.begin(), pos.end(), comparePoints);

                    sf::CircleShape circle(6);
                    circle.setPosition(mouseX-6, mouseY-6);
                    circle.setFillColor(sf::Color::White);

                    circles.push_back(circle);
                }

                for(int i = 0; i < W*H*4; i += 4) {
                    pixels[i] = 0;
                    pixels[i+1] = 0;
                    pixels[i+2] = 0;
                    pixels[i+3] = 255;
                }

                if(pos.size() >= 2) {
                    std::vector<double> x;
                    std::vector<double> y;

                    for(int i = 0; i < pos.size(); i++) {
                        x.push_back(pos[i][0]);
                        y.push_back(pos[i][1]);
                    }

                    splines = cubicSpline(x, y);

                    for(int i = 0; i < W; i++) {
                        int t = xytoint(i,evaluateSpline(splines, i));
                        if(t >= W*H*4 || t < 0) continue;
                        pixels[t] = 255;
                        pixels[t + 1] = 255;
                        pixels[t + 2] = 255;
                    }
                }
                texture.update(pixels);
            }
        }

        textPosition.setString("mouse position: " + std::to_string(sf::Mouse::getPosition(window).x) + ", " + std::to_string(H - sf::Mouse::getPosition(window).y));
        textInp.setString("x = " + textInput);

        window.clear();
        window.draw(sprite);

        for(int i = 0; i < circles.size(); ++i) {
            window.draw(circles[i]);
        }
        window.draw(pointer);
        window.draw(textPosition);
        window.draw(textInp);
        window.draw(textValue);
        window.draw(fDerivative);
        window.draw(sDerivative);

        window.display();
    }

    return 0;
}
