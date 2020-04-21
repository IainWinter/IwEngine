#include <regex>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

enum state {
    NONE,
    LIBRARY_IMAGES,
    IMAGE
};

int main(unsigned count, char** args) {
    system("mkdir fixed");

    for (int i = 1; i < count; i++) {
        std::cout << args[i] << std::endl;

        std::stringstream outfile;
        outfile << "fixed/" << args[i];

        std::ifstream is(args[i]);
        if (!is)
            return -1;

        std::ofstream os(outfile.str());
        if (!os)
            return -2;


        std::string s;
        state state = NONE;

        while (std::getline(is, s)) { // read from is 
            switch (state) {
                case NONE: {
                    if (s.find("<library_images") != std::string::npos) {
                        state = LIBRARY_IMAGES;
                    }
                    break;
                }
                case LIBRARY_IMAGES: {
                    if (s.find("<image") != std::string::npos) {
                        state = IMAGE;
                    }
                    break;
                }
                case IMAGE: {
                    size_t pos = s.find("<init_from>");

                    if (   pos != std::string::npos
                        && s.find("textures/", pos + 11) == std::string::npos)
                    {
                        std::cout << s;

                        s.insert(pos + 11, "textures/");

                        std::cout << " -> " << s << std::endl;
                    }

                    break;
                }
            }
            
            os << s << std::endl;
        }

        return 0;

	}
}


