#ifndef __BRUTEFORCER_HPP__
#define __BRUTEFORCER_HPP__

#include <algorithm>
#include <unordered_map>
#include <queue>

#include "game_emulator.hpp"
#include "update_string.hpp"

class bruteforcer
{
    game_emulator& game_;

    std::unordered_map<typename game_state::key_type, game_state> state_table_;
    std::queue<typename game_state::key_type> task_queue_;

    bool is_solved_;
    typename game_state::key_type key_solved_;

    update_string status_line_;

public:
    bruteforcer(game_emulator& game) : game_(game), is_solved_(false), status_line_(std::cout)
    {
        state_table_.reserve(4096);
    }

    ~bruteforcer()
    {
    }

    void process()
    {
        insert_state(game_.init_state());

        while (!task_queue_.empty() && !is_solved_)
        {
            auto key = task_queue_.front();
            task_queue_.pop();

            const auto& state = state_table_.at(key);

            for (auto dir : {LEFT, RIGHT, UP, DOWN})
            {
                if (auto next_state = game_.next_state(state, dir))
                {
                    // std::cout << "\n" << "{" << state.man.x + 1 << ", " << state.man.y + 1 << "; "
                    //           << state.monsters[0].pt.x + 1 << ", " << state.monsters[0].pt.y + 1 << "} ";
                    // std::cout << dir;
                    // std::cout << " {" << next_state.value().man.x + 1 << ", " << next_state.value().man.y + 1 << "; "
                    //           << next_state.value().monsters[0].pt.x + 1 << ", " << next_state.value().monsters[0].pt.y + 1 << "} ";
                    if (game_.is_final_state(next_state.value()))
                    {
                        is_solved_ = true;
                        key_solved_ = next_state.value().get_key();
                    }
                    insert_state(next_state.value());
                }
            }
        }

        std::cout << "\n";

        if (is_solved_)
        {
            auto path = build_path();
            write_path(std::cout, path);
            std::cout << "\n";
        }
        else
        {
            std::cout << "No solution found\n";
        }

    }

private:
    template<class State>
    void insert_state(State&& state)
    {
        auto key = state.get_key();

        if (state_table_.end() == state_table_.find(key))
        {
            state_table_.insert(std::make_pair(key, std::forward<State>(state)));
            task_queue_.push(key);
            status_line_ << reset() << "state_table size: " << state_table_.size();
            // std::cout << "\n" << state << "\n";
        }
    }

    std::vector<direction> build_path() const
    {
        std::vector<direction> path;
        auto state = state_table_.at(key_solved_);
        while (state.has_prev)
        {
            auto prev_state = state_table_.at(state.prev_state_key);
            path.push_back(game_.get_move_direction(prev_state, state));
            state = prev_state;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    static void write_path(std::ostream& stream, const std::vector<direction>& path)
    {
        for (size_t i = 0; i < path.size(); ++i)
            stream << (i == 0 ? "" : ", ") << path[i];
    }
};

#endif
