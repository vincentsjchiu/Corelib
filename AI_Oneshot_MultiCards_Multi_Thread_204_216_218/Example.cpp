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
*    This Corelib sample program demonstrate how to operate the finite analog input acquisition and get raw data.
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

#define SAMPLE_RATE         1000
#define SAMPLE_COUNT        1024

#define DEVICE_MAX          10
#define FILE_NAME_MAX       128

void AI_AsyncOneshot_Raw_Data(I_AITask* aiTask, int devIndex,double samplerate,uint32_t samplecount);
void ThreadRoutine(int devIndex, const char* aliasName);
void DataProcess(I_AITask* aiTask, int Device);
char filename[DEVICE_MAX][FILE_NAME_MAX];
fstream fp[DEVICE_MAX];

std::thread th[DEVICE_MAX];


int main(int argc, char* argv[])
{
    try
    {
        const char* chDevice = "ADLINK MCM 204 Device";
        Corelib* equipment = Corelib::getInstance();
        vector<ST_DEV_LIST*> stDev = equipment->getDeviceList();  

        if (stDev.empty()) {
            cout << "There are no available device for manipulation." << endl;
            return -1;
        }

        int devIndex = 0;
        for (int i = 0; i < stDev.size(); i++)
        {
            if (i >= DEVICE_MAX) {
                break;
            }

            /*if (strcmp(stDev[i]->chModelName, chDevice) != 0) {
                continue;
            }*/

            sprintf_s(filename[devIndex], FILE_NAME_MAX, "Data_%s.txt", stDev[i]->chAliasName);

            cout << "(" << i << ")" << "aliasName: " << filename[devIndex] << endl;
            th[devIndex] = std::thread(ThreadRoutine, devIndex, stDev[i]->chAliasName);
            devIndex++;
            if (devIndex >= DEVICE_MAX) {
                cout << "Device queue is full" << endl;
                break;
            }
        }

        for (int i = 0; i < devIndex; i++) {
            if (i >= DEVICE_MAX) {
                break;
            }

            if (th[i].joinable()) {
                th[i].join();
            }
            cout << "Join: " << i << endl;
        }
    }
    catch (const char* msg)
    {
        cout << msg << "\n";
    }

    return 0;
}

void ThreadRoutine(int devIndex, const char* aliasName)
{
    I_AITask* aiTask;
    I_Device* dev;
    uint32_t samplecount=1024;
    double samplerate=1000;
    printf("[%d] %s\n", devIndex, aliasName);

    // Step 1: open a device byz alias.
    // About the alias please to reference the application ACE in windows platform.
    dev = Corelib::getInstance()->openDeviceByAlias(aliasName);
    if (dev == NULL) {
        cout << "dev == NULL" << endl;
        return;
    }
    if (strcmp(aliasName, "ADLINK MCM 204 Device") != 0) {
        samplerate = 128000;
        samplecount = samplerate / 2;
            }
    if (strcmp(aliasName, "ADLINK MCM 216 Device") != 0) {
        samplerate = 100000;
        samplecount = samplerate / 2;
    }
    if (strcmp(aliasName, "ADLINK MCM 218 Device") != 0) {
        samplerate = 100000;
        samplecount = samplerate / 2;
    }
    fp[devIndex].open(filename[devIndex], ios::out); // append: ios::out|ios::app
    if (!fp[devIndex]) {//if open file is false:fp = 0; is successful:fp != 0
        cout << "Fail to open file: " << filename[devIndex] << endl;
        Corelib::getInstance()->closeDevice(aliasName);
        return;
    }

    // Step 2: Get the task object.
    // Get the AI Task to configure taks property and perform analog input acquisition.
    aiTask = dev->getAITask();
    if (aiTask != NULL)
        AI_AsyncOneshot_Raw_Data(aiTask, devIndex, samplerate, samplecount);
    else
        cout << "The task is not support." << "\n";

    fp[devIndex].close();

    Corelib::getInstance()->closeDevice(aliasName);
}

void AI_AsyncOneshot_Raw_Data(I_AITask* aiTask, int devIndex, double samplerate, uint32_t samplecount)
{
    // Step3. Set the 'Channel' property to decide which channel will be enable to scan analog samples.
    int channelCount = aiTask->getChannelSize();
    for (int i = 0; i < channelCount; i++)
    {
        aiTask->getChannelByIdx(i)->enableChannel();
    }
    

    // Step4.  Set the 'Timing' property to decide analog acquisition will perform in which acquisition mode , rate and samples.
    I_AITiming* timing = aiTask->getTiming();
    timing->setSampleRatePerChannel(samplerate);
    timing->setSampleCountPerChannel(samplecount);
    timing->setSampleMode(SampleModeEnum::FINITE);

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
        
    printf("[%d] Check Ready...\n", devIndex);
    do {
        DELAY_10_MSEC
    } while ((!_kbhit()) && !aiTask->isTaskFinish()); // Check task done and data is ready or abort task acquisition.
    printf("[%d] Data Ready...\n", devIndex);

    // Stop acquisition task.
    aiTask->asyncStop(); 


    if (aiTask->isTaskFinish())
    {
        cout << "aiTask->isTaskFinish()" << endl;
        DataProcess(aiTask, devIndex);
    }
    else
    {
        cout << "Abort acquisition!!\n";
    }
}
void DataProcess(I_AITask* aiTask, int Device)
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