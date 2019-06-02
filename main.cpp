#include <iostream>

#include "brutforcer.hpp"
#include "game_emulator.hpp"
#include "map.hpp"
#include "map_reader.hpp"

int main(int argc, char** argv)
{
    try
    {
        map_reader map_reader("example.map");
        map& map = map_reader.get_map();
        game_emulator game(map);
        brutforcer brutforcer(game);

        brutforcer.process();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}