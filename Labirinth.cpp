#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <cstring>
#include <vector>

namespace {
    constexpr int DIRECTIONS = 4;
    const std::vector<int> dx = {0, 1, 0, -1};
    const std::vector<int> dy = {1, 0, -1, 0};

    constexpr char pathChar = '*';

    struct Point {
        int x;
        int y;

        Point() { 
            x = 0;
            y = 0; 
        }

        Point(int x_, int y_) : x(x_), y(y_) {}

        Point(const Point& other) = default;
        Point& operator=(const Point& other) = default;

        bool operator==(const Point& other) {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Point& other) {
            return x != other.x || y != other.y; 
        }
    };

    enum PointType {
        EMPTY = 1,
        WALL,
        START,
        FINISH,
        PATH
    };

    const std::map<char, PointType> symbol2PointType = {
        {'_', PointType::EMPTY},
        {'#', PointType::WALL},
        {'S', PointType::START},
        {'E', PointType::FINISH},
        {'*', PointType::PATH}
    };
} // namespace

class Labirinth {
private:
    std::vector<std::string> labirinth_;
    std::vector<std::vector<bool>> used_;
    std::vector<std::vector<Point>> parent_;
    Point start_;
    Point finish_;
    int xSize = 0;
    int ySize = 0;

    bool hasPath = false;
    int pathLength = -1;    

    std::queue<std::tuple<int, Point, Point>> queue_;

private:
    PointType getPointType(char charAtPoint) {
        return symbol2PointType.find(charAtPoint)->second;
    }

    PointType getPointType(const Point& point) {
        return symbol2PointType.find(labirinth_[point.x][point.y])->second;
    }

    bool isOutOfRange(const Point& point) {
        return point.x < 0 || point.x >= xSize || point.y < 0 || point.y >= ySize;
    }


    void readLabirinth(std::ifstream& in) {
        std::string line;
        while (getline(in, line)) {
            labirinth_.push_back(line);
        }
        
        xSize = labirinth_.size();
        if (xSize > 0) {
            ySize = labirinth_[0].size();
        }
        for (int i = 0; i < xSize; ++i) {
            for (int j = 0; j < ySize; ++j) {
                if (getPointType(labirinth_[i][j]) == PointType::START) {
                    start_ = Point(i, j);
                } else if (getPointType(labirinth_[i][j]) == PointType::FINISH) {
                    finish_ = Point(i, j);
                }
            }
        }

        used_.resize(xSize);
        parent_.resize(xSize);
        for (int i = 0; i < xSize; ++i) {
            used_[i].assign(ySize, false);
            parent_[i].assign(ySize, Point(-1, -1));
        }
    }

    void colorPath() {
        if (!hasPath) {
            return;
        }

        Point point = parent_[finish_.x][finish_.y];
        while (parent_[point.x][point.y] != point) {
            labirinth_[point.x][point.y] = pathChar;
            point = parent_[point.x][point.y];
        }
    }

public:
    Labirinth() = delete;
    Labirinth(const Labirinth& other) = delete;
    Labirinth& operator=(const Labirinth& other) = delete;

    Labirinth(std::ifstream& in) {
        readLabirinth(in);
    }

    bool isPathExists() {
        return hasPath;
    }

    int getPathLength() {
        return pathLength;
    }

    void printLabirinth(std::ofstream& out) {
        for (int i = 0; i < xSize; ++i) {
            out << labirinth_[i] << std::endl;
        }
    }

    void findPath() {
        queue_.push({0, start_, start_});
        while (!queue_.empty()) {
            int distance;
            Point tmp;
            Point parent;

            std::tie(distance, tmp, parent) = queue_.front();
            queue_.pop();

            if (used_[tmp.x][tmp.y]) {
                continue;
            }
            used_[tmp.x][tmp.y] = true;
            parent_[tmp.x][tmp.y] = parent;

            if (tmp == finish_) {
                hasPath = true;
                pathLength = distance;
                colorPath();
                return;
            }

            for (int i = 0; i < DIRECTIONS; ++i) {
                Point to = Point(tmp.x + dx[i], tmp.y + dy[i]);
                if (isOutOfRange(to) || getPointType(to) == PointType::WALL) {
                    continue;
                }
                
                queue_.push({distance + 1, to, tmp});
            }
        }
    }
};


int main(int argc, char* argv[]) {
    auto filename = argv[1];

    std::ifstream in(filename, std::ifstream::in);
    if (!in.is_open()) {
        std::cout << "Can't open filename for read" << std::endl;
        return 0;
    }

    Labirinth labirinth(in);
    labirinth.findPath();

    std::ofstream out(filename, std::ifstream::out);
    if (!in.is_open()) {
        std::cout << "Can't open filename for write" << std::endl;
        return 0;
    }

    labirinth.printLabirinth(out);

    if (labirinth.isPathExists()) {
        std::cout << "Path was finded! Length: " << labirinth.getPathLength() << std::endl;
    } else {
        std::cout << "There are no path from start to finish" << std::endl;
    }
    
    return 0;
}
