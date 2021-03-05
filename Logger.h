//
// Created by tharci on 2/9/21.
//

#ifndef LITBOARD_LOGGER_H
#define LITBOARD_LOGGER_H

#include <string>

namespace lbd {
    class Logger {
    public:
        void init(const std::string& fileP);
        void trace(const std::string& data);
        void warn(const std::string& data);
        void error(const std::string& data);

    private:
        void write(const std::string& data);
    };
}


#endif //LITBOARD_LOGGER_H
