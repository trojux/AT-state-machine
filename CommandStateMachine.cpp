/*
 * CommandStateMachine.cpp
 *
 *  Created on: Aug 21, 2025
 *      Author: Szymon-Aquilo
 */

#include "CommandStateMachine.h"
#include "CommandListener.h"
#include "FreeRtos/Queues/List.h"

namespace Modules
{

namespace QuectelBG96
{

CommandStateMachine CommandStateMachine::Create()
{
    return CommandStateMachine{};
}

CommandStateMachine& CommandStateMachine::SendCommand(const char* command)
{
    this->command_to_be_send = command;
    return *this;
}

CommandStateMachine& CommandStateMachine::AwaitResponse(const char* command)
{
    this->command_to_be_received = command;
    return *this;
}

CommandStateMachine& CommandStateMachine::HandleError(const char* handled_error, std::string& error_buffer)
{
    this->handled_error = handled_error;
    this->error_data = &error_buffer;
    return *this;
}

CommandStateMachine& CommandStateMachine::HandleError(const char* handled_error)
{
    this->handled_error = handled_error;
    return *this;
}


CommandStateMachine& CommandStateMachine::CollectData(std::string& collect_buffer)
{
    this->all_data = &collect_buffer;
    return *this;
}

CommandStateMachine& CommandStateMachine::SetTimeout(uint32_t timeout_value)
{
    this->timeout_value = timeout_value;
    return *this;
}

Error CommandStateMachine::Execute()
{
    auto& modem = Device::Serial::Get().at_port;
    auto& logger = Device::Serial::Get().logger_port;

    FreeRtos::Queues::at_lines_q.Flush();
    if (this->command_to_be_send != nullptr)
    {
        modem.Transmit((uint8_t*)command_to_be_send, strlen(command_to_be_send));
        logger.Transmit((uint8_t*)command_to_be_send, strlen(command_to_be_send));
    }
    if (this->command_to_be_received != nullptr)
    {
        this->timeout_value = this->timeout_value == 0 ? CommandStateMachine::DEFAULT_TIMEOUT : this->timeout_value;
        TickType_t start_time = xTaskGetTickCount();

        while (1)
        {
            TickType_t current_time = xTaskGetTickCount();
            TickType_t time_passed = current_time - start_time;

            if (time_passed >= this->timeout_value)
            {
                return Error::TIMED_OUT;
            }
            else if (FreeRtos::Queues::at_lines_q.Receive(received_command, this->timeout_value - time_passed))
            {
              //  LOG_DEBUG("Debug", (const char*)received_command.message_buffer.data());
                if (this->all_data != nullptr)
                {
                    // Sending functionality made it as a C string
                    all_data->append((const char*)received_command.message_buffer.data());
                }
                if (this->handled_error != nullptr)
                {
                    if (received_command.Containes(this->handled_error))
                    {
                        if (error_data != nullptr)
                        {
                            error_data->append((const char*)received_command.message_buffer.data());
                        }
                        return Error::HANDLED_ERROR;
                    }
                }
                if (received_command.Containes(command_to_be_received))
                {
                    return Error::SUCCESS;
                }
            }
        }
    }
    return Error::SUCCESS;
}

}

}
