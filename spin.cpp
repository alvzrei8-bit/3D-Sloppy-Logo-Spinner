#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>
#include <algorithm>
#include <map>

// ANSI color codes
struct Color {
    static const std::string RED;
    static const std::string GREEN;
    static const std::string BLUE;
    static const std::string CYAN;
    static const std::string MAGENTA;
    static const std::string YELLOW;
    static const std::string WHITE;
    static const std::string BRIGHT_RED;
    static const std::string BRIGHT_GREEN;
    static const std::string BRIGHT_CYAN;
    static const std::string BRIGHT_MAGENTA;
    static const std::string RESET;
};

const std::string Color::RED = "\033[91m";
const std::string Color::GREEN = "\033[92m";
const std::string Color::BLUE = "\033[94m";
const std::string Color::CYAN = "\033[96m";
const std::string Color::MAGENTA = "\033[95m";
const std::string Color::YELLOW = "\033[93m";
const std::string Color::WHITE = "\033[97m";
const std::string Color::BRIGHT_RED = "\033[1;91m";
const std::string Color::BRIGHT_GREEN = "\033[1;92m";
const std::string Color::BRIGHT_CYAN = "\033[1;96m";
const std::string Color::BRIGHT_MAGENTA = "\033[1;95m";
const std::string Color::RESET = "\033[0m";

struct Point3D {
    float x, y, z;
};

struct CharPoint {
    Point3D pos;
    std::string ch;
    std::string color;
};

class LogoSpinner {
private:
    std::vector<CharPoint> characters;
    int width, height;
    float rotationY = 0.0f;

public:
    LogoSpinner() : width(0), height(0) {}

    bool isWhitespace(const std::string& str, size_t& pos) {
        if (pos >= str.length()) return false;
        unsigned char c = str[pos];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            pos++;
            return true;
        }
        return false;
    }

    std::string getUTF8Char(const std::string& str, size_t& pos) {
        if (pos >= str.length()) return "";
        
        unsigned char c = str[pos];
        std::string result;
        
        if ((c & 0x80) == 0) {
            result = str[pos];
            pos++;
        } else if ((c & 0xE0) == 0xC0) {
            if (pos + 1 < str.length()) {
                result = str.substr(pos, 2);
                pos += 2;
            } else {
                pos++;
            }
        } else if ((c & 0xF0) == 0xE0) {
            if (pos + 2 < str.length()) {
                result = str.substr(pos, 3);
                pos += 3;
            } else {
                pos++;
            }
        } else if ((c & 0xF8) == 0xF0) {
            if (pos + 3 < str.length()) {
                result = str.substr(pos, 4);
                pos += 4;
            } else {
                pos++;
            }
        } else {
            pos++;
        }
        
        return result;
    }

    bool loadLogo(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "'\n";
            return false;
        }

        std::vector<std::string> lines;
        std::string line;
        
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        if (lines.empty()) {
            std::cerr << "Error: Logo file is empty\n";
            return false;
        }

        height = lines.size();
        width = 0;
        for (const auto& l : lines) {
            int charCount = 0;
            for (size_t i = 0; i < l.length(); ) {
                getUTF8Char(l, i);
                charCount++;
            }
            width = std::max(width, charCount);
        }

        float centerX = width / 2.0f;
        float centerY = height / 2.0f;

        std::vector<std::string> colors = {
            Color::BRIGHT_CYAN, Color::BRIGHT_MAGENTA, Color::BRIGHT_GREEN,
            Color::CYAN, Color::MAGENTA, Color::GREEN, Color::BRIGHT_RED
        };

        int colorIdx = 0;
        for (int y = 0; y < height; y++) {
            const auto& l = lines[y];
            int x = 0;
            for (size_t i = 0; i < l.length(); ) {
                size_t oldI = i;
                if (!isWhitespace(l, i)) {
                    i = oldI;
                    std::string utf8Char = getUTF8Char(l, i);
                    if (!utf8Char.empty() && utf8Char != " ") {
                        CharPoint cp;
                        cp.pos.x = (x - centerX) * 0.5f;
                        cp.pos.y = (centerY - y) * 0.5f;
                        cp.pos.z = 0.0f;
                        cp.ch = utf8Char;
                        cp.color = colors[colorIdx % colors.size()];
                        characters.push_back(cp);
                        colorIdx++;
                    }
                }
                x++;
            }
        }

        return true;
    }

    Point3D rotatePoint(const Point3D& p) {
        float cosY = std::cos(rotationY);
        float sinY = std::sin(rotationY);
        Point3D p1;
        p1.x = p.x * cosY + p.z * sinY;
        p1.z = -p.x * sinY + p.z * cosY;
        p1.y = p.y;
        return p1;
    }

    std::string getColorByDepth(float z) {
        if (z > 0.8f) return Color::BRIGHT_RED;
        if (z > 0.5f) return Color::BRIGHT_CYAN;
        if (z > 0.2f) return Color::BRIGHT_MAGENTA;
        if (z > 0.0f) return Color::CYAN;
        if (z > -0.2f) return Color::MAGENTA;
        return Color::BLUE;
    }

    void render() {
        int screenWidth = 80;
        int screenHeight = 24;

        // Use a map for sparse rendering since UTF-8 chars can't go in fixed grid
        std::map<std::pair<int, int>, std::pair<std::string, float>> pixels;

        for (const auto& cp : characters) {
            Point3D rotated = rotatePoint(cp.pos);
            
            float perspective = 1.0f + rotated.z * 0.05f;
            int screenX = screenWidth / 2 + (int)(rotated.x * perspective * 8.0f);
            int screenY = screenHeight / 2 - (int)(rotated.y * perspective * 4.0f);

            if (screenX > 0 && screenX < screenWidth - 1 && screenY > 1 && screenY < screenHeight - 1) {
                auto key = std::make_pair(screenX, screenY);
                if (pixels.find(key) == pixels.end() || pixels[key].second > rotated.z) {
                    pixels[key] = std::make_pair(cp.ch, rotated.z);
                }
            }
        }

        // Render frame
        std::cout << "\033[2J\033[H";
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                auto key = std::make_pair(x, y);
                if (pixels.find(key) != pixels.end()) {
                    float z = pixels[key].second;
                    std::string color = getColorByDepth(z);
                    std::cout << color << pixels[key].first << Color::RESET;
                } else {
                    std::cout << ' ';
                }
            }
            std::cout << '\n';
        }
        std::cout.flush();
    }

    void updateRotation(float deltaTime) {
        rotationY += 2.5f * deltaTime;
        if (rotationY > 6.28f) rotationY -= 6.28f;
    }

    void spin(float duration = 0.0f) {
        auto startTime = std::chrono::high_resolution_clock::now();

        while (true) {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed = now - startTime;
            float elapsedSec = elapsed.count();

            updateRotation(0.016f);
            render();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));

            if (duration > 0.0f && elapsedSec > duration) {
                break;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <logo.txt>\n";
        std::cerr << "  logo.txt - ASCII art logo file\n";
        return 1;
    }

    LogoSpinner spinner;
    
    if (!spinner.loadLogo(argv[1])) {
        return 1;
    }

    std::cout << "Starting logo spinner. Press Ctrl+C to exit.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    spinner.spin();

    return 0;
}
