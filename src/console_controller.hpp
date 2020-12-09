#ifndef CONSOLE_CONTROLLER_H
#define CONSOLE_CONTROLLER_H

class Engine;

class ConsoleController{
    public:
        ConsoleController(Engine * engine);
        void Run();
    private:
        // Control from console
        void RunTransmitterMode();
        void RunReceiverMode();
        void RunViewMode();

        // Console Actions
        void ConsoleAddTransmitter();
        void ConsoleRemoveTransmitter();
        void ConsoleAddReceiver();
        void ConsoleRemoveReceiver();
        void ConsoleSelectTransmitter();
        void ConsoleSelectReceiver();
        void ConsoleConnectReceiverToTransmitter();
        void ConsoleDisconnectReceiverFromTransmitter();

        // Instructions
        void PrintInstructions();
        void PrintViewModeInstructions();
        void PrintTransmitterModeInstructions();
        void PrintReceiverModeInstructions();

        // Engine
        Engine * engine_;
};
#endif