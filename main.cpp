#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <string>

template <class generic>
auto find (std::vector<generic>& vec, generic val) {
    return std::find(vec.begin(), vec.end(), val);
}

void get_trees(int cell1, int cell2, int* cell1_tree, int* cell2_tree, std::vector<std::vector<int>>& forest) {
    for (int i = 0; i < forest.size(); i++) {
        auto end = forest[i].end();
        if (find(forest[i], cell1) != end) {
            *cell1_tree = i;
        }
        if (find(forest[i], cell2) != end) {
            *cell2_tree = i;
        }
    }
}

void merge_trees(int index1, int index2, std::vector<std::vector<int>>& forest) {
    for (int i : forest[index2]) {
        forest[index1].push_back(i);
    }
    forest.erase(forest.begin() + index2);
}

bool check (int cell1, int cell2, std::vector<std::vector<int>>& forest) {
    int cell1_tree = -1, cell2_tree = -2;
    get_trees(cell1, cell2, &cell1_tree, &cell2_tree, forest);
    if (cell1_tree != -1) {
        if (cell2_tree == -2) {
            forest[cell1_tree].push_back(cell2);
        } else if (cell1_tree != cell2_tree) {
            merge_trees(cell1_tree, cell2_tree, forest);
        }
    } else if (cell2_tree != -2) {
        forest[cell2_tree].push_back(cell1);
    } else {
        std::vector<int> new_tree = {cell1, cell2};
        forest.push_back(new_tree);
    }
    return cell1_tree != cell2_tree;
}

int main() {
    int height, width;
    std::cout << "Enter a height: ";
    std::cin >> height;
    if (height < 1) {
        std::cout << "Invalid value.";
        abort();
    }
    std::cout << "Enter a width: ";
    std::cin >> width;
    if (width < 1) {
        std::cout << "Invalid value.";
        abort();
    }
    std::cout << "Generating a " << height << " by " << width << " maze" << std::endl;

    const int area = height * width;
    const int h_offset = height * (width - 1);
    std::vector<std::vector<int>> mst;
    std::vector<int> destroyed_walls;
    std::vector<int> walls(2 * area - height - width);
    std::iota(walls.begin(), walls.end(), 0);

    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine engine(seed);
    while(destroyed_walls.size() < height * width - 1) {
        std::uniform_int_distribution<int> distribution(0, walls.size()-1);
        int index = distribution(engine);
        int wall = walls[index];
        if (wall < h_offset) {
            int left = wall + wall / (width - 1);
            if (check(left, left + 1,mst)) {
                destroyed_walls.push_back(wall);
            }
        } else {
            int above = wall - h_offset;
            if(check(above, above + width, mst)) {
                destroyed_walls.push_back(wall);
            }
        }
        walls.erase(walls.begin() + index);
    }

    std::vector<std::string> maze;

    std::string horizontals;
    for (int i = 0; i < width; i++) {
        horizontals += "+---";
    }
    horizontals += "+\n";

    std::string verticals;
    for (int i = 0; i < width; i++) {
        verticals += "|   ";
    }
    verticals += "|\n";

    for (int i = 0; i < 2 * height + 1; i++) {
        if (i % 2 == 0) {
            maze.push_back(horizontals);
        } else {
            maze.push_back(verticals);
        }
    }
    
    for (int wall : destroyed_walls) {
        if (wall < h_offset) {
            int start = 4 + wall % (width - 1) * 4;
            maze[wall / (width - 1) * 2 + 1].replace(start, 1, " ");
        } else {
            int start = 1 + (wall - h_offset) % width * 4;
            maze[(wall - h_offset) / width * 2 + 2].replace(start, 3, "   ");
        }
    }

    maze[1].replace(0, 1, " ");
    maze[2 * height - 1].replace(4 * width, 1, " ");

    for (auto& row : maze) {
        std::cout << row;
    }
}