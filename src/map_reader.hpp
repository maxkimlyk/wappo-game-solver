#ifndef __MAP_READER_HPP__
#define __MAP_READER_HPP__

#include <fstream>
#include <sstream>
#include <string>

#include "map.hpp"

class map_reader
{
    map map_;

public:
    map_reader(const std::string& filename)
    {
        std::ifstream file(filename);
        std::string line;

        std::getline(file, line);
        size_t width = get_value_from_string<size_t>(line, "width");

        std::getline(file, line);
        size_t height = get_value_from_string<size_t>(line, "height");

        map_ = map(height, width);

        size_t count = 0;
        while (count < width * height)
        {
            if (file.eof())
                throw std::runtime_error("End of file was reached before entire map was readed");

            std::getline(file, line);
            count += read_map_description_line(line, map_, count);
        }
    }

    map& get_map()
    {
        return map_;
    }

private:
    template<class Value>
    static Value get_value_from_string(const std::string& line, const std::string& expected_keyword)
    {
        std::istringstream stream(line);

        std::string keyword;
        std::string sign;
        Value value;

        stream >> keyword;
        if (keyword != expected_keyword)
            throw std::runtime_error(("Got " + keyword + " but expected " + expected_keyword).c_str());

        stream >> sign >> value;

        return value;
    }

    static void set_by_letter(map& map, size_t pos, char letter)
    {
        if (letter == 't')
            map.spawn_wall(pos, TWALL);
        else if (letter == 'b')
            map.spawn_wall(pos, BWALL);
        else if (letter == 'l')
            map.spawn_wall(pos, LWALL);
        else if (letter == 'r')
            map.spawn_wall(pos, RWALL);
        else if (letter == 'e')
            map.at(pos).set(EXIT);
        else if (letter == 's')
            map.at(pos).set(STAR);
        else if (letter == 'h')
            map.human_start_ = map.index_to_point(pos);
        else if (letter == 'm')
            map.monsters_start_.push_back(map.index_to_point(pos));
        else if (letter != ' ' && letter != '\n')
        {
            std::string what = std::string("Unknown specific letter `") + std::string(1, letter) + std::string("`");
            throw std::runtime_error(what.c_str());
        }
    }

    static size_t read_map_description_line(const std::string& line, map& map, size_t pos)
    {
        size_t count = 0;

        size_t end = 0;
        while (end != -1)
        {
            size_t start = line.find('[', end);
            end = line.find(']', start);

            if (start == -1 || end == -1)
                continue;

            for (size_t i = start + 1; i != end; ++i)
                set_by_letter(map, pos + count, line[i]);

            count++;
        }

        return count;
    }
};

#endif
