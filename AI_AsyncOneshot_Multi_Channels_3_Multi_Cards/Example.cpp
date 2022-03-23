/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Copyright (c) 1995-2021, ADLink Technology Inc.  All rights reserved.  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

=============================================================================
REVISION HISTORY
Revision    :   1.0 
Date        :   11/03/2021     
-----------------------------------------------------------------------------
Log:  Inital version
-----------------------------------------------------------------------------
*/

/*
*
* Windows Example:
*    Example.cpp
*
* Example Category:
*    AI
*
* Description:
*    This Corelib sample program demonstrate how to operate the finite analog input and multi-channel acquisition.
*
* Instructions for Running:
*    1. Set the 'openDeviceByAlias' for opening the device.
*    2. Get the task object to config the task property, such as Channels, Trigger, Timing.
*    3. Set the 'Channel'property to decide which channel will be enable to scan analog samples.
*    4. Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
*    5. Set the 'Trigger' property to decide the acquisition operation will perform in which trigger condition.
*    6. Submit the device profile to device and prepare acquisition environment.
*    7. Start acquision and get acquisition data from channel buffer by channel index.

* I/O Connections Overview:
*    Please refer to your hardware reference manual.
*
******************************************************************************/

#include "Example.h"

#define SAMPLE_RATE         2000
#define SAMPLE_COUNT        2048

void AI_AsyncOneshot_Multi_Channels_dev0(I_AITask* aiTask);
void AI_AsyncOneshot_Multi_Channels_dev1(I_AITask* aiTask);
void AI_AsyncOneshot_Multi_Channels_dev2(I_AITask* aiTask);
void DataProcess(I_AITask* aiTask, int Device);
char filename[] = "Data.csv";
AI_TaskFinishEvent_dev0 taskFinish_dev0;
AI_TaskFinishEvent_dev1 taskFinish_dev1;
AI_TaskFinishEvent_dev2 taskFinish_dev2;
fstream fp[10];

int main(int argc, char* argv[])
{
    try
    {
        I_AITask* aiTask0,* aiTask1,*aiTask2;
        I_Device* dev0 ,* dev1,* dev2;        

        // Step 1: open a device by alias.
        // About the alias please to reference the application ACE in windows platform.
        dev0 = Corelib::getInstance()->openDeviceByAlias("USB-1901-0");
        dev1 = Corelib::getInstance()->openDeviceByAlias("USB-2405-2-0");
        dev2 = Corelib::getInstance()->openDeviceByAlias("USB-1210-0");
        if (dev0 == NULL)
            return 0;
        if (dev1 == NULL)
            return 0;
        if (dev2 == NULL)
            return 0;
        fp[0].open((std::string)"dev0_" + filename, ios::out); // append: ios::out|ios::app
        if (!fp) {//if open file is false:fp = 0; is successful:fp != 0
            cout << "Fail to open file: " << filename << endl;
            return 0;
        }
        fp[1].open((std::string)"dev1_" + filename, ios::out); // append: ios::out|ios::app
        if (!fp) {//if open file is false:fp = 0; is successful:fp != 0
            cout << "Fail to open file: " << filename << endl;
            return 0;
        }
        fp[2].open((std::string)"dev2_" + filename, ios::out); // append: ios::out|ios::app
        if (!fp) {//if open file is false:fp = 0; is successful:fp != 0
            cout << "Fail to open file: " << filename << endl;
            return 0;
        }
        // Step 2: Get the task object.
        // Get the AI Task to configure taks property and perform analog input acquisition.
        aiTask0 = dev0->getAITask();
        if (aiTask0 != NULL)
            AI_AsyncOneshot_Multi_Channels_dev0(aiTask0);
        else
            cout << "The task is not support." << "\n";

        
        aiTask1 = dev1->getAITask();
        if (aiTask1 != NULL)
            AI_AsyncOneshot_Multi_Channels_dev1(aiTask1);
        else
            cout << "The task is not support." << "\n";
        aiTask2 = dev2->getAITask();
        if (aiTask1 != NULL)
            AI_AsyncOneshot_Multi_Channels_dev2(aiTask2);
        else
            cout << "The task is not support." << "\n";
        
    }
    catch (const char* msg)
    {
        cout << msg << "\n";
    }

    return 0;
}
int AI_TaskFinishEvent_dev0::doEventProc(I_AITask* aiTask)
{
    DataProcess(aiTask, 0);
    return 0;
}
int AI_TaskFinishEvent_dev1::doEventProc(I_AITask* aiTask)
{
    DataProcess(aiTask, 1);

    return 0;
}
int AI_TaskFinishEvent_dev2::doEventProc(I_AITask* aiTask)
{
    DataProcess(aiTask, 2);

    return 0;
}
void DataProcess(I_AITask* aiTask,int Device)
{
    try
    {
        int channelCount = aiTask->getChannelSize();
        int enablechcount = 0;
        for (int i = 0; i < channelCount; i++)
        {
            I_AIChannel* channel = aiTask->getChannelByIdx(i);
            if (channel->isChannelEnable())
            {
                enablechcount++;
            }
        }

        double* scaledDataBuff = (double*)malloc(sizeof(double) * enablechcount * SAMPLE_COUNT);
        unsigned int chcount = 0;
        for (int i = 0; i < channelCount; i++)
        {
            I_AIChannel* channel = aiTask->getChannelByIdx(i);

            if (channel->isChannelEnable())
            {
                if (chcount == enablechcount - 1)
                {
                    fp[Device] << "CH-" << i << "\n";
                }
                else
                {
                    fp[Device] << "CH-" << i << ",";
                }
                double* chdata = channel->getBuffer()->getScaledDataBuffer();
                for (int point = 0; point < channel->getBuffer()->getBufferLength(); point++)
                {
                    *(scaledDataBuff + chcount * (channel->getBuffer()->getBufferLength()) + point) = chdata[point];
                }
                chcount++;
            }
        }
        for (int point = 0; point < SAMPLE_COUNT; point++)
        {
            for (int i = 0; i < enablechcount; i++)
            {
                if (i == enablechcount - 1)
                {
                    fp[Device] << *(scaledDataBuff + i * (SAMPLE_COUNT)+point) << "\n";
                }
                else
                {
                    fp[Device] << *(scaledDataBuff + i * (SAMPLE_COUNT)+point) << ",";
                }
            }
        }
        free(scaledDataBuff);
        fp[Device].close();
    }
    catch (char* msg)
    {
        cout << msg << "\n";
    }
}
void AI_AsyncOneshot_Multi_Channels_dev0(I_AITask* aiTask)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    aiTask->getChannelByIdx(0)->enableChannel();
     aiTask->getChannelByIdx(1)->enableChannel();
    aiTask->getChannelByIdx(2)->enableChannel();
    aiTask->getChannelByIdx(3)->enableChannel();

    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(SAMPLE_RATE);
    timing->setSampleCountPerChannel(SAMPLE_COUNT);
    timing->setSampleMode(SampleModeEnum::FINITE);

    // Set Callback function
    aiTask->addTaskFinishHandler((I_AICallback*)&taskFinish_dev0);

    // Step5  Set the 'Trigger' object to decide the acquisition operation will perform in which trigger condition.
    I_AITrigger* trigger = aiTask->getTrigger();

    // Set trigger source as No wait.
    trigger->setTriggerSource(TriggerSourceEnum::NOWAIT);

    // Set trigger mode as post trigger.
    trigger->setTriggerMode(TriggerModeEnum::POST_TRIGGER);

    // Step6. Submit the device profile to device and prepare acquisition environment.
    aiTask->downloadConfigurationToDevice();

    // Step7. Start acquision task.
    aiTask->asyncStart(10); // Timeout 10 Sec

    do {
        DELAY_10_MSEC
    } while ((!_kbhit()) && !aiTask->isTaskFinish()); // Check task done and data is ready or abort task acquisition.

        if (_kbhit())
            cout << "Abort acquisition!!\n";

        // Stop acquisition task.
        aiTask->asyncStop();
}
void AI_AsyncOneshot_Multi_Channels_dev1(I_AITask* aiTask)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    aiTask->getChannelByIdx(0)->enableChannel();
    aiTask->getChannelByIdx(1)->enableChannel();
    aiTask->getChannelByIdx(2)->enableChannel();
    aiTask->getChannelByIdx(3)->enableChannel();

    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(SAMPLE_RATE);
    timing->setSampleCountPerChannel(SAMPLE_COUNT);
    timing->setSampleMode(SampleModeEnum::FINITE);

    // Set Callback function
    aiTask->addTaskFinishHandler((I_AICallback*)&taskFinish_dev1);

    // Step5  Set the 'Trigger' object to decide the acquisition operation will perform in which trigger condition.
    I_AITrigger* trigger = aiTask->getTrigger();

    // Set trigger source as No wait.
    trigger->setTriggerSource(TriggerSourceEnum::NOWAIT);

    // Set trigger mode as post trigger.
    trigger->setTriggerMode(TriggerModeEnum::POST_TRIGGER);

    // Step6. Submit the device profile to device and prepare acquisition environment.
    aiTask->downloadConfigurationToDevice();

    // Step7. Start acquision task.
    aiTask->asyncStart(10); // Timeout 10 Sec

    do {
        DELAY_10_MSEC
    } while ((!_kbhit()) && !aiTask->isTaskFinish()); // Check task done and data is ready or abort task acquisition.

        if (_kbhit())
            cout << "Abort acquisition!!\n";

        // Stop acquisition task.
        aiTask->asyncStop();
}
void AI_AsyncOneshot_Multi_Channels_dev2(I_AITask* aiTask)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    aiTask->getChannelByIdx(0)->enableChannel();
    aiTask->getChannelByIdx(1)->enableChannel();
    aiTask->getChannelByIdx(2)->enableChannel();
    aiTask->getChannelByIdx(3)->enableChannel();

    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(SAMPLE_RATE);
    timing->setSampleCountPerChannel(SAMPLE_COUNT);
    timing->setSampleMode(SampleModeEnum::FINITE);

    // Set Callback function
    aiTask->addTaskFinishHandler((I_AICallback*)&taskFinish_dev2);

    // Step5  Set the 'Trigger' object to decide the acquisition operation will perform in which trigger condition.
    I_AITrigger* trigger = aiTask->getTrigger();

    // Set trigger source as No wait.
    trigger->setTriggerSource(TriggerSourceEnum::NOWAIT);

    // Set trigger mode as post trigger.
    trigger->setTriggerMode(TriggerModeEnum::POST_TRIGGER);

    // Step6. Submit the device profile to device and prepare acquisition environment.
    aiTask->downloadConfigurationToDevice();

    // Step7. Start acquision task.
    aiTask->asyncStart(10); // Timeout 10 Sec

    do {
        DELAY_10_MSEC
    } while ((!_kbhit()) && !aiTask->isTaskFinish()); // Check task done and data is ready or abort task acquisition.

        if (_kbhit())
            cout << "Abort acquisition!!\n";

        // Stop acquisition task.
        aiTask->asyncStop();
}