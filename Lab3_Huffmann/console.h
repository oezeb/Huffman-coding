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
    Console(std::vector<std::string> args = std::vector<std::string>(), int count = 0)
        : z(false), u(false) {
        init(args, count);
    }

    //gets
    bool getz() const { return z; }
    bool getu() const { return u; }
    std::string getinputfile() const { return inputfile; }
    std::string getoutputfile() const { return outputFile; }

    void init(std::vector<std::string> args, int count = 0);
};

void Console::init(std::vector<std::string> args, int count) {
    //Basic Conditions
    if (args.empty())
        return;
    //Regular expressions
    std::regex uzreg("-[u,z]"); // match -u and -z
    std::regex namereg(".+[.]hc"); // match any string with .hc at the end
    
    if (std::regex_match(args[0], std::regex("-[u,z]"))) {
        //match -z or -u
        args.erase(args.begin());
        if (count == 0)
            init(args, count + 1);
    }//match -z or -u
    else if (std::regex_match(args[0], std::regex(".+[.]hc"))) {
        //match *.hc
        u = true;
        inputfile = args[0];
        outputFile = inputfile.substr(0, inputfile.find_last_of('.'));
    }//match *.hc
    else {
        z = true;
        inputfile = args[0];
        outputFile = inputfile + ".hc";
    }
}

#endif // CONSOLE_H
