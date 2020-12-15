#ifndef CONSOLE_H
#define CONSOLE_H


#include <regex>
#include <string>
#include <vector>

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2020/12/14 15:39
* @description: given environment variables(argc,argv) and determine what to do. see commands list
*/

/*
* --------Command list-------------------------
* ->myhuffman hello.txt
* compress hello.txt file into hello.txt.huff
*
* ->myhuffman -z hello.txt
* compress hello.txt file into hello.txt.huff
*
* ->myhuffman -u hello.txt.huff
* decompress hello.txt.huff file into hello.txt
*
* ->myhuffman -uz hello.txt
* report error and exit
*
* ->myhuffman -u hello.txt
* report error and exit
*
*/

class Console {
private:
    bool z; //compress
    bool u; //decompress
    std::string inputfile;
    std::string outputFile;

public:
    Console(std::vector<std::string> args = std::vector<std::string>());

    //gets
    bool getz() const { return z; }
    bool getu() const { return u; }
    std::string getinputfile() const { return inputfile; }
    std::string getoutputfile() const { return outputFile; }
};


Console::Console(std::vector<std::string> args) : z(false), u(false) {
    //Basic Conditions
    if (args.size() == 1 || args.size() == 2) {
        //check if 2nd arg match compressed file (".ch" file)
        inputfile = args.back();
        bool match_hc = std::regex_match(inputfile, std::regex(".+[.]hc"));
        args.pop_back(); //erase file name

        //checck if first arg match -u or -z
        bool match_u, match_z;
        if (match_hc && args.empty())
            match_u = true;
        else if (!match_hc && args.empty())
            match_z = true;
        else {
            match_u = std::regex_match(args.front(), std::regex("-u"));
            match_z = std::regex_match(args.front(), std::regex("-z"));
        }

        //
        if (match_hc && match_u) {
            //decompress
            u = true;
            outputFile = inputfile.substr(0, inputfile.find_last_of('.'));
        }
        else if (!match_hc && match_z) {
            //compress
            z = true;
            outputFile = inputfile + ".hc";
        }
        else
            exit(-1);
    }
}

#endif // CONSOLE_H
