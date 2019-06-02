#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <cstddef>
#include <cstdint>
#include <vector>

enum cell_flag
{
    LWALL = 1,
    TWALL = 2,
    RWALL = 4,
    BWALL = 8,
    STAR = 16,
    EXIT = 32
};

class cell
{
    uint8_t flags_;

public:
    cell(uint8_t flags = 0) : flags_(flags) {}

    void set(cell_flag flag)
    {
        flags_ |= static_cast<uint8_t>(flag);
    }

    bool get(cell_flag flag) const
    {
        return static_cast<bool>(flags_ & flag);
    }
};

enum direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

std::ostream& operator<< (std::ostream& stream, direction dir)
{
    switch (dir)
    {
    case LEFT:
        stream << "LEFT";
        break;

    case RIGHT:
        stream << "RIGHT";
        break;

    case UP:
        stream << "UP";
        break;

    case DOWN:
        stream << "DOWN";
        break;
    }
    return stream;
}

struct point
{
    size_t y = 0;
    size_t x = 0;

    bool operator== (const point& other)
    {
        return x == other.x && y == other.y;
    }
};

class map
{
    friend class map_reader;

    size_t height_, width_;
    point human_start_;
    std::vector<point> monsters_start_;
    std::vector<cell> cells_;

    map() {};

public:
    map(size_t height, size_t width) : height_(height), width_(width), cells_(width * height)
    {
    }

    cell& at(size_t idx) { return cells_[idx]; }
    const cell& at(size_t idx) const { return cells_[idx]; }
    cell& at(size_t i, size_t j) { return cells_[i * width_ + j]; }
    const cell& at(size_t i, size_t j) const { return cells_[i * width_ + j]; }
    cell& at(point pt) { return cells_[pt.y * width_ + pt.x]; }
    const cell& at(point pt) const { return cells_[pt.y * width_ + pt.x]; }

    size_t get_width() const { return width_; }
    size_t get_height() const { return height_; }

    point get_human_start_point() const { return human_start_; }
    auto& get_monster_start_points() const { return monsters_start_; }

    point index_to_point(size_t pos) const
    {
        return point {pos / width_, pos % width_};
    }

    void spawn_wall(size_t idx, cell_flag flag)
    {
        point pt = index_to_point(idx);

        switch (flag)
        {
        case TWALL:
            if (pt.y != 0)
                at(pt.y - 1, pt.x).set(BWALL);
            break;
        case BWALL:
            if (pt.y != height_ - 1)
                at(pt.y + 1, pt.x).set(TWALL);
            break;
        case LWALL:
            if (pt.x != 0)
                at(pt.y, pt.x - 1).set(RWALL);
            break;
        case RWALL:
            if (pt.x != width_ - 1)
                at(pt.y, pt.x + 1).set(LWALL);
            break;
        }

        at(pt).set(flag);
    }

    bool can_go(point pt, direction dir) const
    {
        switch (dir)
        {
        case LEFT:
            if (pt.x == 0 || at(pt).get(LWALL))
                return false;
            break;

        case RIGHT:
            if (pt.x == width_ - 1 || at(pt).get(RWALL))
                return false;
            break;

        case UP:
            if (pt.y == 0 || at(pt).get(TWALL))
                return false;
            break;

        case DOWN:
            if (pt.y == height_ - 1 || at(pt).get(BWALL))
                return false;
            break;
        }
        return true;
    }
};

#endif
