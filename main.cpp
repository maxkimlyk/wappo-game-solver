#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "bruteforcer.hpp"
#include "game_emulator.hpp"
#include "map.hpp"
#include "map_reader.hpp"

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    std::string map_file;
    bool was_error = false;

    po::options_description cmd_opts("");

    po::options_description options("Options");
    options.add_options()
        ("help,h", "print this help message");
    cmd_opts.add(options);

    po::options_description positional("");
    positional.add_options()
        ("map_file", po::value<std::string>(&map_file)->required(), "file with map description");
    cmd_opts.add(positional);

    po::positional_options_description pos_desc;
    pos_desc.add("map_file", 1);

    po::variables_map params;
    try
    {
        po::store(
            po::command_line_parser(argc, argv)
                .options(cmd_opts)
                .positional(pos_desc)
                .style(po::command_line_style::unix_style)
                .run(), params);

        po::notify(params);
    }
    catch (po::error& poe)
    {
        std::cerr << poe.what() << "\n";
        was_error = true;
    }

    if (params.count("help") || map_file.empty())
    {
        std::cout << "USAGE:\n"
                  << argv[0] << " [OPTIONS] MAP_FILE\n"
                  << options << "\n";
        return -1 * was_error;
    }

    try
    {
        map_reader map_reader(map_file);
        map& map = map_reader.get_map();
        game_emulator game(map);
        bruteforcer bruteforcer(game);
        bruteforcer.process();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
