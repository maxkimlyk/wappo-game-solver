#ifndef __GAME_EMULATOR_HPP__
#define __GAME_EMULATOR_HPP__

#include <optional>

#include "map.hpp"

struct monster
{
    point pt = {0, 0};
    size_t freeze = 0;
    bool is_boosted = false;
    bool is_hided = false;
};

struct game_state
{
    using key_type = uint64_t;

    point man;
    std::vector<monster> monsters;
    key_type prev_state_key;
    bool has_prev = false;

    key_type get_key() const
    {
        key_type key = man.x | (man.y << 4);
        for (auto m : monsters)
            key = (key << 12)
                | (m.pt.x | (m.pt.y << 4) | (m.freeze << 8) | (m.is_boosted << 10) | (m.is_hided << 11));
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
            init.monsters.push_back(monster {pt, 0, false, false});
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
        std::vector<size_t> steps_left(state.monsters.size());
        for (size_t i = 0; i < state.monsters.size(); ++i)
            steps_left[i] = state.monsters[i].is_hided ? 0 : (state.monsters[i].is_boosted ? 3 : 2);

        for (size_t step = 0; step < 3; ++step)
        {
            for (size_t i = 0; i < state.monsters.size(); ++i)
            {
                auto& monster = state.monsters[i];

                if (monster.freeze && !monster.is_boosted)
                {
                    steps_left[i] = 0;
                    if (step == 0)
                        monster.freeze--;
                    continue;
                }

                if (steps_left[i] == 0)
                    continue;

                steps_left[i]--;

                if (state.man.x < monster.pt.x && map_.can_go(monster.pt, LEFT))
                {
                    monster.pt.x--;
                }
                else if (monster.pt.x < state.man.x && map_.can_go(monster.pt, RIGHT))
                {
                    monster.pt.x++;
                }
                else if (monster.pt.y < state.man.y && map_.can_go(monster.pt, DOWN))
                {
                    monster.pt.y++;
                }
                else if (state.man.y < monster.pt.y && map_.can_go(monster.pt, UP))
                {
                    monster.pt.y--;
                }
                else
                {
                    steps_left[i] = 0;
                    continue;
                }

                if (map_.at(monster.pt).get(STAR))
                    monster.freeze = 3;
            }

            for (size_t i = 0; i < state.monsters.size(); ++i)
                for (size_t j = i + 1; j < state.monsters.size(); ++j)
                {
                    if (state.monsters[j].pt == state.monsters[i].pt && !state.monsters[j].is_hided && !state.monsters[i].is_hided)
                    {
                        state.monsters[j].is_boosted = true;
                        state.monsters[i].is_hided = true;
                        steps_left[j] = 0;
                        steps_left[i] = 0;
                    }
                }
        }
    }
};

#endif
