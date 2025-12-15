/*
 * CommandStateMachine.h
 *
 *  Created on: Aug 21, 2025
 *      Author: Szymon-Aquilo
 */

#ifndef SRC_MODULES_QUECTELBG96_COMMANDSTATEMACHINE_H_
#define SRC_MODULES_QUECTELBG96_COMMANDSTATEMACHINE_H_

#include <string>
#include <string_view>
#include "Utils/Error.h"
#include "Device/Serial.h"
#include  "CommandListener.h"

namespace Modules
{

namespace QuectelBG96
{
    class CommandStateMachine
    {
        public:
    		static CommandStateMachine Create();
            CommandStateMachine& SendCommand(const char* command);
            CommandStateMachine& AwaitResponse(const char* command);
            CommandStateMachine& HandleError(const char* command, std::string& error_buffer);
            CommandStateMachine& HandleError(const char* handled_error);
            CommandStateMachine& CollectData(std::string& collect_buffer);
            CommandStateMachine& SetTimeout(uint32_t timeout_value);
            Error Execute();
        private:
            CommandStateMachine() = default;
            static constexpr uint32_t TEMP_BUFFER_SIZE = 100;
            std::array <uint8_t, TEMP_BUFFER_SIZE> temp_buffer;
            static constexpr uint32_t DEFAULT_TIMEOUT = 1000;
            uint32_t timeout_value = 0;
            //Todo Change strings to static memory allocation.
            const char* command_to_be_send{nullptr};
            const char* command_to_be_received{nullptr};
            const char* handled_error{nullptr};
            FreeRtos::Queues::SmallCommand received_command;
            std::string* all_data = nullptr;
            std::string* error_data = nullptr;
    };
}

}


#endif /* SRC_MODULES_QUECTELBG96_COMMANDSTATEMACHINE_H_ */
