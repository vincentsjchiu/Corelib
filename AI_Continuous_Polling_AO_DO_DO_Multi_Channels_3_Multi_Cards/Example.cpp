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

void AI_AO_DI_DO_Multi_Channels_dev0(I_AITask* aiTask, I_DITask* diTask, I_DOTask* doTask, I_AOTask* aoTask);
void AI_AO_DI_DO_Multi_Channels_dev1(I_AITask* aiTask, I_DITask* diTask, I_DOTask* doTask, I_AOTask* aoTask);
void AI_AO_DI_DO_Multi_Channels_dev2(I_AITask* aiTask, I_DITask* diTask, I_DOTask* doTask, I_AOTask* aoTask);
void DataProcess(I_AITask* aiTask, int Device);
void DO_Static(I_DOTask* doTask,uint32_t enable);
void DI_Static(I_DITask* diTask);
void AO_Static(I_AOTask* aoTask, double_t level);
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
        I_DITask* diTask0, * diTask1,* diTask2;
        I_DOTask* doTask0, * doTask1, * doTask2;
        I_Device* dev0 ,* dev1,* dev2; 
        I_AOTask* aoTask0, * aoTask1, * aoTask2;
        uint32_t enable=0x00;
        double_t level = 0;
        // Step 1: open a device by alias.
        // About the alias please to reference the application ACE in windows platform.
        dev0 = Corelib::getInstance()->openDeviceByAlias("USB-1903-4");//("USB-1901-0");
        dev1 = Corelib::getInstance()->openDeviceByAlias("USB-2405-2-0");
        dev2 = Corelib::getInstance()->openDeviceByAlias("USB-1901-0");
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
        diTask0 = dev0->getDITask();
        doTask0 = dev0->getDOTask();
        aoTask0 = dev0->getAOTask();
        if (aiTask0 != NULL&& diTask0 != NULL&& doTask0 != NULL&& aoTask0 != NULL)
            AI_AO_DI_DO_Multi_Channels_dev0(aiTask0,diTask0,doTask0, aoTask0);
        else
            cout << "The task is not support." << "\n";

        
        aiTask1 = dev1->getAITask();
        diTask1 = dev1->getDITask();
        doTask1 = dev1->getDOTask();
        aoTask1 = dev1->getAOTask();
        if (aiTask1 != NULL && diTask1 != NULL && doTask1 != NULL && aoTask1 != NULL)
            AI_AO_DI_DO_Multi_Channels_dev1(aiTask1, diTask1, doTask1, aoTask1);
        else
            cout << "The task is not support." << "\n";

        aiTask2 = dev2->getAITask();
        diTask2 = dev2->getDITask();
        doTask2 = dev2->getDOTask();
        aoTask2 = dev1->getAOTask();
        if (aiTask2 != NULL && diTask2 != NULL && doTask2 != NULL && aoTask2 != NULL)
            AI_AO_DI_DO_Multi_Channels_dev2(aiTask2, diTask2, doTask2, aoTask2);
        else
            cout << "The task is not support." << "\n";
         
        do {
            Sleep(1000);
            if (enable == 0x00)
            {
                enable =0x01;
            }
            else
            {
                enable = 0x00;
            }
            if (level == 0)
            {
                level = 3;
            }
            else
            {
                level = 0;
            }
            DO_Static(doTask0, enable);
            DI_Static(diTask0);
            AO_Static(aoTask0, level);
            cout << "Get Data ......" << "\n";
        } while (!_kbhit()); // Check task done and data is ready or abort task acquisition.

            if (_kbhit())
                cout << "Abort acquisition!!\n";

            // Stop acquisition task.
            aiTask0->asyncStop();
           /* aiTask1->asyncStop();
            aiTask2->asyncStop();*/

            fp[0].close();
            /*fp[1].close();
            fp[2].close();*/
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
        
    }
    catch (char* msg)
    {
        cout << msg << "\n";
    }
}
void AI_AO_DI_DO_Multi_Channels_dev0(I_AITask* aiTask, I_DITask* diTask, I_DOTask* doTask, I_AOTask* aoTask)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    aiTask->getChannelByIdx(0)->enableChannel();
    aiTask->getChannelByIdx(1)->enableChannel();
    aiTask->getChannelByIdx(2)->enableChannel();
    aiTask->getChannelByIdx(3)->enableChannel();
    diTask->getPortsByIdx(0)->enablePort();
    doTask->getPortsByIdx(0)->enablePort();
    aoTask->getChannelByIdx(0)->enableChannel();
    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(SAMPLE_RATE);
    timing->setSampleCountPerChannel(SAMPLE_COUNT);
    timing->setSampleMode(SampleModeEnum::CONTINUOUS);

    // Set Callback function
    //aiTask->addTaskFinishHandler((I_AICallback*)&taskFinish_dev0);
    aiTask->addDataReadyHandler((I_AICallback*)&taskFinish_dev0);
    // Step5  Set the 'Trigger' object to decide the acquisition operation will perform in which trigger condition.
    I_AITrigger* trigger = aiTask->getTrigger();

    // Set trigger source as No wait.
    trigger->setTriggerSource(TriggerSourceEnum::NOWAIT);

    // Set trigger mode as post trigger.
    trigger->setTriggerMode(TriggerModeEnum::POST_TRIGGER);

    // Step6. Submit the device profile to device and prepare acquisition environment.
    aiTask->downloadConfigurationToDevice();
    diTask->downloadConfigurationToDevice();
    doTask->downloadConfigurationToDevice();
    aoTask->downloadConfigurationToDevice();
    // Step7. Start acquision task.
    aiTask->asyncStart(10); // Timeout 10 Sec

   
}
void AI_AO_DI_DO_Multi_Channels_dev1(I_AITask* aiTask, I_DITask* diTask, I_DOTask* doTask, I_AOTask* aoTask)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    aiTask->getChannelByIdx(0)->enableChannel();
    aiTask->getChannelByIdx(1)->enableChannel();
    aiTask->getChannelByIdx(2)->enableChannel();
    aiTask->getChannelByIdx(3)->enableChannel();
    diTask->getPortsByIdx(0)->enablePort();
    doTask->getPortsByIdx(1)->enablePort();
    aoTask->getChannelByIdx(0)->enableChannel();
    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(SAMPLE_RATE);
    timing->setSampleCountPerChannel(SAMPLE_COUNT);
    timing->setSampleMode(SampleModeEnum::CONTINUOUS);

    // Set Callback function
    //aiTask->addTaskFinishHandler((I_AICallback*)&taskFinish_dev1);
    aiTask->addDataReadyHandler((I_AICallback*)&taskFinish_dev1);
    // Step5  Set the 'Trigger' object to decide the acquisition operation will perform in which trigger condition.
    I_AITrigger* trigger = aiTask->getTrigger();

    // Set trigger source as No wait.
    trigger->setTriggerSource(TriggerSourceEnum::NOWAIT);

    // Set trigger mode as post trigger.
    trigger->setTriggerMode(TriggerModeEnum::POST_TRIGGER);

    // Step6. Submit the device profile to device and prepare acquisition environment.
    aiTask->downloadConfigurationToDevice();
    diTask->downloadConfigurationToDevice();
    doTask->downloadConfigurationToDevice();
    aoTask->downloadConfigurationToDevice();
    // Step7. Start acquision task.
    aiTask->asyncStart(10); // Timeout 10 Sec

    
}
void AI_AO_DI_DO_Multi_Channels_dev2(I_AITask* aiTask, I_DITask* diTask, I_DOTask* doTask, I_AOTask* aoTask)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    aiTask->getChannelByIdx(0)->enableChannel();
    aiTask->getChannelByIdx(1)->enableChannel();
    aiTask->getChannelByIdx(2)->enableChannel();
    aiTask->getChannelByIdx(3)->enableChannel();
    diTask->getPortsByIdx(0)->enablePort();
    doTask->getPortsByIdx(1)->enablePort();
    aoTask->getChannelByIdx(0)->enableChannel();
    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(SAMPLE_RATE);
    timing->setSampleCountPerChannel(SAMPLE_COUNT);
    timing->setSampleMode(SampleModeEnum::CONTINUOUS);

    // Set Callback function
    //aiTask->addTaskFinishHandler((I_AICallback*)&taskFinish_dev2);
    aiTask->addDataReadyHandler((I_AICallback*)&taskFinish_dev2);
    // Step5  Set the 'Trigger' object to decide the acquisition operation will perform in which trigger condition.
    I_AITrigger* trigger = aiTask->getTrigger();

    // Set trigger source as No wait.
    trigger->setTriggerSource(TriggerSourceEnum::NOWAIT);

    // Set trigger mode as post trigger.
    trigger->setTriggerMode(TriggerModeEnum::POST_TRIGGER);

    // Step6. Submit the device profile to device and prepare acquisition environment.
    aiTask->downloadConfigurationToDevice();
    diTask->downloadConfigurationToDevice();
    doTask->downloadConfigurationToDevice();
    aoTask->downloadConfigurationToDevice();
    // Step7. Start acquision task.
    aiTask->asyncStart(10); // Timeout 10 Sec

  
}
void DO_Static(I_DOTask* doTask, uint32_t enable)
{
    try
    {
        
        int portCount = doTask->getPortSize();
        for (int i = 0; i < portCount; i++)
        {
            I_DOPort* port = doTask->getPortsByIdx(i);
            if (port->isEnablePort())
            {
                port->getUpdateSinglePointData()->setBinData(enable);
            }
        }
    }
    catch (const char* msg)
    {
        cout << msg << "\n";
    }
}
void DI_Static(I_DITask* diTask)
{
    try
    {
        
        int portCount = diTask->getPortSize();
        for (int i = 0; i < portCount; i++)
        {
            I_DIPort* port = diTask->getPortsByIdx(i);
            if (port->isEnablePort())
            {
                uint32_t scaledData = port->pollingSinglePointData()->getBinData();
                cout << scaledData << "\n";
            }
        }
    }
    catch (const char* msg)
    {
        cout << msg << "\n";
    }
}
void AO_Static(I_AOTask* aoTask,double_t level)
{
    try
    {
        aoTask->downloadConfigurationToDevice();
        int channelCount = aoTask->getChannelSize();
        for (int i = 0; i < channelCount; i++)
        {
            I_AOChannel* channel = aoTask->getChannelByIdx(i);
            if (channel->isChannelEnable())
            {
                channel->getSinglePointData()->setScaledData(level);
            }
        }
    }
    catch (const char* msg)
    {
        cout << msg;
    }
}