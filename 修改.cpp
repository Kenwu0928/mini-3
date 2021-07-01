#include <array>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

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

const int SIZE = 8;

int node_value[SIZE][SIZE] = {
    66,  -6, 7, 4, 4, 7,  -6, 66,
    -6, -30, 3, 1, 1, 3, -30, -6,
    7, 3, 6, 2, 2, 6, 3, 7,
    4, 1, 2, 1, 1, 2, 1, 4,
    4, 1, 2, 1, 1, 2, 1, 4,
    7, 3, 6, 2, 2, 6, 3, 7,
    -6, -30, 3, 1, 1, 3, -30, -6,
    66, -6, 7, 4, 4, 7, -6, 66
};

int me;
int player;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
int now_score = 0;

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{ { Point(-1, -1), Point(-1, 0), Point(-1, 1),
                                             Point(0, -1), /*{0, 0}, */ Point(0, 1),
                                             Point(1, -1), Point(1, 0), Point(1, 1) } };
    std::array<std::array<int, SIZE>, SIZE> board;    //8*8���ѽL
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;

    OthelloBoard(const OthelloBoard& b) {
        cur_player = b.cur_player;
        board = b.board;
        next_valid_spots = b.next_valid_spots;
    }

    OthelloBoard& operator=(const OthelloBoard& b) {
        this->cur_player = b.cur_player;
        this->board = b.board;
        this->next_valid_spots = b.next_valid_spots;
        return *this;
    }

private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {    //���L�W�X���
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {    //����O�©ΥթΨS���Ѥl
        return board[p.x][p.y];
    }
    //�ˬd���@��O�_�O�Y���C��
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {    //�T�{�U�o�Ӧ�m�O�_�i��
        if (get_disc(center) != EMPTY)    //���Ѥl
            return false;
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            //�j�����Ѥl���O�O�P��N����U
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void set_disc(Point p, int disc) {    //��©Υմѩ�W�Y�@��
        board[p.x][p.y] = disc;
    }
    void flip_discs(Point center) {    //�⤤�������ܦ���ݴѤl���C��
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({ p });
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s : discs) {
                        set_disc(s, cur_player);    //88��
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();

                    //add
                    if (cur_player == me)
                        now_score = disc_count[cur_player]- disc_count[get_next_player(cur_player)];
                    else
                        now_score = disc_count[get_next_player(cur_player)]- disc_count[cur_player];
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }

public:
    // TODO 1
    int calc_value() {
        if (cur_player != me) {
            //���p�G�U�@�B�i�H�U�����ΨӨ�o�L�ѳ̫�@��ѡA�h�ڳo��ѻ��ȷ|���C
            //���p�G�i�U���I�p�󵥩�|�B�A�ڤU�o����ȴN����
            int ans = 0;
            ans = now_score;
            if (done) ans += 800;
            if (next_valid_spots.size() == 0) ans += 50;
            for (long unsigned i = 0; i < 50; i++) {
                if (next_valid_spots.size() == i) {
                    ans = ans - i*2;
                    break;
                }
            }

            for (Point i : next_valid_spots) {
                for (Point dir : directions) {
                    Point p = i + dir;
                    if (!is_disc_at(p, get_next_player(cur_player)) || !is_spot_on_board(p))
                        continue;
                    std::vector<Point> tmp_discs({ p });
                    p = p + dir;
                    while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                        if (is_disc_at(p, cur_player)) {
                            ans -= tmp_discs.size()/2;
                            break;
                        }
                        tmp_discs.push_back(p);
                        p = p + dir;
                    }
                }
            }
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++){
                    if (board[i][j] == me) ans += node_value[i][j];
                    else if (board[i][j] != me && board[i][j] != 0) ans -= node_value[i][j];
                }
            }

            if (next_valid_spots.size() == 1 && disc_count[EMPTY] == 1) ans -= 20;
            return ans;
        }
        else {
            int ans = 0;
            ans = now_score;
            if (done) ans -= 800;
            if (next_valid_spots.size() == 0) ans -= 50;
            for (long unsigned i = 0; i < 50; i++) {
                if (next_valid_spots.size() == i) {
                    ans = ans + i*2;
                    break;
                }
            }
            for (auto i : next_valid_spots) {
                for (Point dir : directions) {
                    Point p = i + dir;

                    if (!is_disc_at(p, get_next_player(cur_player)) || !is_spot_on_board(p))
                        continue;
                    std::vector<Point> tmp_discs({ p });
                    p = p + dir;
                    while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                        if (is_disc_at(p, cur_player)) {
                            ans += tmp_discs.size()/2;
                            break;
                        }
                        tmp_discs.push_back(p);
                        p = p + dir;
                    }
                }
            }
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if (board[i][j] == me) ans += node_value[i][j];
                    else if (board[i][j] != me && board[i][j] != 0) ans -= node_value[i][j];
                }
            }
            if (next_valid_spots.size() == 1 && disc_count[EMPTY] == 1) ans += 20;
            return ans;
        }
    }
    bool put_disc(Point p) {
        if (!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);    //�\�W�U���X�l
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);    //�B�z���ܦ⪺�Ѥl
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs)
                    winner = BLACK;
                else
                    winner = WHITE;
            }
        }
        return true;
    }
    OthelloBoard() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8 * 8 - 4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {    //�O�����@���I�i�H�U
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }

};

struct Node {
    Point p;
    int score;
    Node(Point p, int score) : p(p), score(score) {}
    Node() : p(-5, -5) {
        score = 0;
    }
};

void read_board(std::ifstream& fin) {
    fin >> player;
    //me = player;
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
        next_valid_spots.push_back({ x, y });
    }
}

Node ab_pruning(OthelloBoard now_board, int num, bool ismax, int a, int b) {
    Node ans;
    if (num == 0 || now_board.next_valid_spots.empty()) {
        Node leaf;
        leaf.p = Point(-5, -5);
        leaf.score = now_board.calc_value();
        return leaf;
    }

    if (ismax) {
        Node tmp;
        int max_ans = INT_MIN;
        for (Point n : now_board.next_valid_spots) {
            OthelloBoard tmp_board = now_board;
            if (tmp_board.put_disc(n)) {
                tmp = ab_pruning(tmp_board, num - 1, false, a, b);
                max_ans = std::max(max_ans, tmp.score);
                if (max_ans > a){
                    a = max_ans;
                    ans.score = max_ans;
                    ans.p = n;
                }
                if (a >= b) break;
            }
        }
    }
    else {
        Node tmp_1;
        int min_ans = INT_MAX;

        for (Point p : now_board.next_valid_spots) {
            OthelloBoard tmp_board_1 = now_board;
            if (tmp_board_1.put_disc(p)) {
                tmp_1 = ab_pruning(tmp_board_1, num - 1, true, a, b);
                min_ans = std::min(min_ans, tmp_1.score);
                if (min_ans < b) {
                    b = min_ans;
                    ans.score = min_ans;
                    ans.p = p;
                }
                if (b <= a) break;
            }
        }
    }
    return ans;
}

/*Node minimax(OthelloBoard now_board, int num, bool ismax) {
    //OthelloBoard tmp_board = now_board;
    Node ans;
    if (num == 0 || now_board.next_valid_spots.empty()) {
        Node leaf;
        leaf.p = Point(-5, -5);
        leaf.score = now_board.calc_value();
        return leaf;
    }

    if (ismax) {
        Node tmp;
        int max_ans = INT_MIN;

        for (Point n : now_board.next_valid_spots) {
            OthelloBoard tmp_board = now_board;
            if (tmp_board.put_disc(n)) { //the opponent becomes cur_player, but I'm counting my value ,line 150
                tmp = minimax(tmp_board, num - 1, false);
                max_ans = std::max(max_ans,tmp.score);
                ans.score = max_ans;
                ans.p = n;
            }
        }
    }
    else {
        Node tmp_1;
        int min_ans = INT_MAX;

        for (Point p : now_board.next_valid_spots) {
            OthelloBoard tmp_board_1 = now_board;
            if (tmp_board_1.put_disc(p)) {
                tmp_1 = minimax(tmp_board_1, num - 1, true);
                min_ans = std::min(min_ans,tmp_1.score);
                ans.score = min_ans;
                ans.p = p;
            }
        }
    }
    return ans;
}*/

void write_valid_spot(std::ofstream& fout) {
    Node greatest;
    OthelloBoard now_board;
    now_board.board = board;
    now_board.cur_player = player;
    me = player;
    now_board.next_valid_spots = next_valid_spots;
    //greatest = minimax(now_board, 2, true);
    greatest = ab_pruning(now_board, 4, true, INT_MIN, INT_MAX);
    //std::cout << greatest.score << std::endl;
    fout << greatest.p.x << " " << greatest.p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    //�q�ɮ�Ū����� fin �O�@�Ӫ���
    std::ifstream fin(argv[1]);
    //�N��Ƽg�J�ɮ�
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
