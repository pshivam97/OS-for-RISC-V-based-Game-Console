#include <stdint.h>

#define TEXTMODE 0x0
#define GRAPHICSMODE 0x1

uint32_t SystemCall(uint32_t* param);
uint32_t SystemCall2(uint32_t* param1, char* param2);

int main() {
    int X_Position = 12;
    uint32_t ControllerStatus;
    uint32_t InterruptPendingReg;
    uint32_t Mode;
    uint32_t CurrentTicks;
    uint32_t LastGlobalTicks = 0;

    /*  API Calls Parameters  */
    uint32_t TimerTicks_API_Parameters[] = {1};
    uint32_t Mode_API_Parameters[] = {2};
    uint32_t Controller_API_Parameters[] = {3};
    uint32_t InterruptPending_API_Parameters[] = {4};
    uint32_t TextMode_API_Parameters[] = {5,0}; // First element specifies the type of SystemCall
    uint32_t GraphicsMode_API_Parameters[] = {5,1}; // First element specifies the type of SystemCall
    uint32_t StartingMessagePosition_API_Parameters[] = {7,0}; // First element specifies the type of SystemCall
    uint32_t TimerMessage_API_Parameters[] = {7,64};

    char TimerMessage[] = "60 units elapsed!";
    char ClearTimerMessage[] = "                 ";
    char StartingMessage[] = "Hello World!X";
    char ReturnToGameMessage[] = "Welcome Back!";

    SystemCall2(StartingMessagePosition_API_Parameters,StartingMessage);

    while (1) {

        CurrentTicks = SystemCall(TimerTicks_API_Parameters); // "TimerTicks" is fetched by passing '1' as parameter to the SystemCall()
        if((CurrentTicks % 60) == 0) SystemCall2(TimerMessage_API_Parameters,TimerMessage);
        else if((CurrentTicks % 60) > 20) SystemCall2(TimerMessage_API_Parameters,ClearTimerMessage);

        if(CurrentTicks != LastGlobalTicks){

            Mode = SystemCall(Mode_API_Parameters); // "MODE CONTROL REGISTER" is fetched here !
            ControllerStatus = SystemCall(Controller_API_Parameters);  // "CONTROLLLER" status is fetched by passing '3' as parameter to the SystemCall()
            InterruptPendingReg = SystemCall(InterruptPending_API_Parameters); // "Interrupt Pending Regiser" status is fetched here !

            if(ControllerStatus){
                uint32_t ClearX_API_Parameters[] = {7,X_Position};
                char clear_X[] = " ";
                SystemCall2(ClearX_API_Parameters,clear_X);

                if(ControllerStatus & 0x1){ // 'd' -> LEFT
                    uint32_t MessageClearL_API_Parameters[] = {7,X_Position};
                    char messageClearL[] = "      ";
                    SystemCall2(MessageClearL_API_Parameters,messageClearL);

                    if(X_Position & 0x3F){
                        X_Position--;
                    }
                }
                if(ControllerStatus & 0x2){ // 'w' -> UP
                    uint32_t MessageClearU_API_Parameters[] = {7,X_Position};
                    char messageClearU[] = "      ";
                    SystemCall2(MessageClearU_API_Parameters,messageClearU);

                    if(X_Position >= 0x40){
                        X_Position -= 0x40;
                    }
                }
                if(ControllerStatus & 0x4){ // 'x' -> DOWN
                    uint32_t MessageClearD_API_Parameters[] = {7,X_Position};
                    char messageClearD[] = "      ";
                    SystemCall2(MessageClearD_API_Parameters,messageClearD);

                    if(X_Position < 0x8C0){
                        X_Position += 0x40;
                    }
                }
                if(ControllerStatus & 0x8){ // 'a' -> RIGHT
                    uint32_t MessageClearR_API_Parameters[] = {7,X_Position};
                    char messageClearR[] = "      ";
                    SystemCall2(MessageClearR_API_Parameters,messageClearR);

                    if((X_Position & 0x3F) != 0x3F){
                        X_Position++;
                    }
                }

                if((ControllerStatus & 0x20) && (Mode != TEXTMODE)){
                  // When 'i' is presssed, the MODE switches from "Graphics Mode" to "Text Mode"
                    uint32_t MessageClear_API_Parameters[] = {7,X_Position};
                    char messageClear[] = "      ";
                    SystemCall2(MessageClear_API_Parameters,messageClear);

                    SystemCall(TextMode_API_Parameters); // Sets MODE to TEXTMODE
                    Mode = SystemCall(Mode_API_Parameters);
                    SystemCall2(StartingMessagePosition_API_Parameters,ReturnToGameMessage); // Prints "Welcome Back!"
                }

                if((ControllerStatus & 0x10) && (Mode != GRAPHICSMODE)){
                  // When 'u' is presssed, the MODE switches from "Text Mode" to "Graphics Mode"
                    SystemCall(GraphicsMode_API_Parameters); // Sets MODE to GRAPHICSMODE
                    Mode = SystemCall(Mode_API_Parameters);

                    uint32_t Background_API_Parameters[] = {6,0,510,286,0,0,101,234,150,237};
                    SystemCall(Background_API_Parameters); // Sets Background

                    uint32_t SmallSprite1_API_Parameters[] = {8,5,100,100,15,15,0,0,179,200,20,210};
                    SystemCall(SmallSprite1_API_Parameters); // Draws a Small Sprite

                    uint32_t SmallSprite2_API_Parameters[] = {8,10,350,70,9,9,1,1,240,10,5,210};
                    SystemCall(SmallSprite2_API_Parameters); // Draws a Small Sprite
                }
                uint32_t CurrentX_API_Parameters[] = {7,X_Position};
                char current_X[] = "X";
                SystemCall2(CurrentX_API_Parameters,current_X);
            }

            if(InterruptPendingReg & 0x4) {
              // Check Command Interrupt - "PAUSED" message will be shown on the gaming console.
                uint32_t PauseMessage_API_Parameters[] = {7,X_Position};
                char Pause_message[] = "PAUSED";
                SystemCall2(PauseMessage_API_Parameters,Pause_message);
            }

            if((InterruptPendingReg & 0x2) && (Mode == GRAPHICSMODE)) {
              // Video Interrupt - "A Fast Blinking/Switching colours of a Large Sprite will be shown"
                while(1) {
                    CurrentTicks = SystemCall(TimerTicks_API_Parameters);

                    if((CurrentTicks % 6) < 3)  {
                        uint32_t LargeSprite_API_Parameters[] = {9,0,350,250,60,60,2,12,32,248,220};
                        SystemCall(LargeSprite_API_Parameters);
                    }
                    else  {
                        uint32_t LargeSprite_API_Parameters[] = {9,0,350,250,60,60,2,230,130,89,220};
                        SystemCall(LargeSprite_API_Parameters);
                    }
                }
            }
            LastGlobalTicks = CurrentTicks;
        }
    }
    return 0;
}
