extern int PRINT_DEBUG;
#define DEBUG(text) if(PRINT_DEBUG){std::cout << "\033[34m" << text << "\033[0m" << std::endl;}

#define INFO(text) std::cout << "\033[32m" << text << "\033[0m" << std::endl;

#define WARNING(text) std::cout << "\033[35m" << text << "\033[0m" << std::endl;

#define ERROR(text) std::cout << "\033[36m" << text << "\033[0m" << std::endl;
