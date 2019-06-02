#ifndef __GAME_EMULATOR_HPP__
#define __GAME_EMULATOR_HPP__

#include <optional>

#include "map.hpp"

struct monster
{
    point pt;
    size_t freeze = 0;
};

struct game_state
{
    using key_type = uint32_t;

    point man;
    std::vector<monster> monsters;
    key_type prev_state_key;
    bool has_prev = false;

    key_type get_key() const
    {
        key_type key = man.x | (man.y << 4);
        for (auto m : monsters)
            key = (key << 10) | (m.pt.x | (m.pt.y << 4) | (m.freeze << 8));
        return key;
    }
};

static std::ostream& operator<< (std::ostream& stream, const game_state& state)
{
    stream << "{man: (" << state.man.y << "," << state.man.x << ")";
    for (size_t i = 0; i < state.monsters.size(); ++i)
        stream << " monster" << i << ": (" << state.monsters[i].pt.y << "," << state.monsters[i].pt.x << ")";
    stream << "}";
    return stream;
}

class game_emulator
{
    map& map_;

public:
    game_emulator(map& map) : map_(map)
    {
    }

    game_state init_state() const
    {
        game_state init;
        init.man = map_.get_human_start_point();
        for (auto pt : map_.get_monster_start_points())
            init.monsters.push_back(monster {pt.y, pt.x, 0});
        init.prev_state_key = 0;
        init.has_prev = false;
        return init;
    }

    std::optional<game_state> next_state(const game_state& state, direction dir) const
    {
        game_state next(state);

        if (!map_.can_go(state.man, dir))
            return std::nullopt;

        switch (dir)
        {
        case LEFT:
            next.man.x--;
            break;
        case RIGHT:
            next.man.x++;
            break;
        case UP:
            next.man.y--;
            break;
        case DOWN:
            next.man.y++;
            break;
        }

        monsters_walk(next);

        if (!is_alive_state(next))
            return std::nullopt;

        next.prev_state_key = state.get_key();
        next.has_prev = true;

        return std::optional<game_state>(next);
    }

    bool is_alive_state(const game_state& state) const
    {
        if (map_.at(state.man).get(STAR))
            return false;

        for (auto monster : state.monsters)
        {
            if (monster.pt == state.man)
                return false;
        }
        return true;
    }

    bool is_final_state(const game_state& state) const
    {
        if (is_alive_state(state) && map_.at(state.man).get(EXIT))
            return true;
        return false;
    }

    static direction get_move_direction(const game_state& from, const game_state& to)
    {
        if (to.man.x == from.man.x + 1)
            return RIGHT;
        else if (to.man.x == from.man.x - 1)
            return LEFT;
        else if (to.man.y == from.man.y + 1)
            return DOWN;
        else if (to.man.y == from.man.y - 1)
            return UP;
        throw std::logic_error("Impossible path");
    }

private:
    void monsters_walk(game_state& state) const
    {
        for (auto& monster : state.monsters)
        {
            if (monster.freeze)
            {
                monster.freeze--;
                continue;
            }

            size_t count = 0;
            size_t prev_count = count;

            while (count < 2)
            {
                if (state.man.x < monster.pt.x && map_.can_go(monster.pt, LEFT))
                {
                    monster.pt.x --;
                    count++;
                }
                else if (monster.pt.x < state.man.x && map_.can_go(monster.pt, RIGHT))
                {
                    monster.pt.x ++;
                    count++;
                }
                else if (monster.pt.y < state.man.y && map_.can_go(monster.pt, DOWN))
                {
                    monster.pt.y ++;
                    count++;
                }
                else if (state.man.y < monster.pt.y && map_.can_go(monster.pt, UP))
                {
                    monster.pt.y --;
                    count++;
                }

                if (prev_count == count)
                    break;

                if (map_.at(monster.pt).get(STAR))
                {
                    monster.freeze = 3;
                    break;
                }

                prev_count = count;
            }
        }
    }
};

#endif
