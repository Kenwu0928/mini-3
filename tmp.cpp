#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
std::vector<std::tuple<int, int, int>> Point_list;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.emplace_back(Point{ x, y });
    }
}

const std::array<Point, 8> directions{ { Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                         Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                         Point(1, -1), Point(1, 0), Point(1, 1) } };

inline int get_next_player(int player) {
    return 3 - player;
}
inline bool is_spot_on_board(const Point& p) {    //有無超出邊界
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
inline int get_disc(Point p) {    //那格是黑或白或沒有棋子
    return board[p.x][p.y];
}
//檢查那一格是否是某種顏色
inline bool is_disc_at(const Point& p, int disc) {
    if (get_disc(p) != disc)
        return false;
    return true;
}

void put_disk(){
    std::array<std::array<int, SIZE>, SIZE> tmp_board = board;
}

void write_valid_spot(std::ofstream& fout) {
    for (const Point& center : next_valid_spots) {
        for (const Point& dir : directions) {
            int point_counter{};    //分數
            Point p = center + dir;
            while (true) {
                if (!is_spot_on_board(p) || get_disc(p) == player) {    //在棋盤外面或同顏色就停
                    Point_list.emplace_back(std::make_tuple(point_counter, center.x, center.y));
                    break;
                }
                ++point_counter;    //翻一個棋就加一分
                p = p + dir;    //繼續下一步

                if (center.x == 0 || center.x == 7 || center.y == 0 || center.y == 7)
                    point_counter += 5;    //邊邊角角的加分

                if (next_valid_spots.size() <= 4)
                    point_counter -= 3;
                else if (next_valid_spots.size() == 1)
                    point_counter += 5;
            }
        }
    }

    int tmp_point{}, ans_x{}, ans_y{}, ans_point{};    //最高分和相對的座標
    for (std::size_t i{}; i < Point_list.size(); ++i) {
        tmp_point = std::get<0>(Point_list[i]);
        if (tmp_point >= ans_point) {
            ans_x = std::get<1>(Point_list[i]);
            ans_y = std::get<2>(Point_list[i]);
        }
    }
    fout << ans_x << " " << ans_y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
