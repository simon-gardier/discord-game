#include "entrapment_bot.hpp"

/**
 * @brief main program
 * 
 * @param argc 
 * @param argv bot token
 * @return int 
 */
int main(int argc, char const *argv[]){
    if (argc != 2){
        std::cerr << "You must pass your bot token as parameter.\nExample : ./" << argv[0] << " <token>" << std::endl;
        return 1;
    }
    try{
        std::shared_ptr<EntrapmentBot> instance = EntrapmentBot::get_instance(argv[1]);
        instance->start();
    }
    catch(dpp::exception& e){
        std::cout << "Unable to create the EntrapmentBot instance :" << e.what() << std::endl;
        return 1;
    }
    return 0;
}
