/*
 * eth_socket_tsk.c
 *
 *  Created on: Aug 4, 2020
 *      Author: liwei
 */
#include "eth_tsk.h"
#include "../../../main/main.h"

#include "esp_gap_ble_api.h"

#define ETH_SERVER_SOCKET_PORT 502

static uint8_t mEthClientConnected = 0;
static uint8_t mEthServerConnected = 0;
static int mServerSocket = 0;
static int mClientSocket = 0;
static uint32_t mClientIP = 0;
static uint16_t mClientPort = 0;
static uint8_t mConnected = 0;
static int mAddrFamily;
static ModbusRtuDataTPDF *mModbusRtuDataSend;
static ModbusRtuDataTPDF *mModbusRtuDataRec;
static char *mDataSend;
static char *mDataRec;

static const char *TAG = "Eth_Socket";

static void vETH_SocketServer_Init(void)
{
	static struct sockaddr_in6 tDestAddr;
    static struct sockaddr_in *tDestAddrIp4 = (struct sockaddr_in *)&tDestAddr;
    vTaskDelay(50);
    if(mAddrFamily == AF_INET)
    {
    	tDestAddrIp4->sin_addr.s_addr = htonl(0);
    	tDestAddrIp4->sin_family = AF_INET;
    	tDestAddrIp4->sin_port = htons(ETH_SERVER_SOCKET_PORT/*mPartitionTable.tEthernetConfig.EthernetPort*/);
    }
    else if (mAddrFamily == AF_INET6)
    {
    	bzero(&tDestAddr.sin6_addr.un, sizeof(tDestAddr.sin6_addr.un));
    	tDestAddr.sin6_family = AF_INET6;
    	tDestAddr.sin6_port = htons(mPartitionTable.tEthernetConfig.EthernetPort);
    }
    mServerSocket = socket(mAddrFamily, SOCK_STREAM, mAddrFamily == AF_INET6? IPPROTO_IPV6:IPPROTO_IP);
    if (mServerSocket < 0)
    {
    	ESP_LOGE(TAG, "Ethernet socket server creat fail!");
    	shutdown(mServerSocket, SHUT_RDWR);
    	close(mServerSocket);
    	return;
    }
    ESP_LOGI(TAG, "Ethernet socket server created!");
    int iErr = bind(mServerSocket, (struct sockaddr *)&tDestAddr, sizeof(tDestAddr));
    if (iErr != 0)
    {
    	ESP_LOGE(TAG, "Ethernet socket server bind creat fail!");
    	close(mServerSocket);
    	return;
    }
    ESP_LOGI(TAG, "Ethernet socket server bind created!");
    iErr = listen(mServerSocket, 1);
    if (iErr != 0)
    {
    	ESP_LOGE(TAG, "Ethernet socket server listened fail!");
    	shutdown(mServerSocket, SHUT_RDWR);
    	close(mServerSocket);
    	return;
    }
    ESP_LOGI(TAG, "Ethernet socket server listened!");
    mEthServerConnected = 1;
}

static void vETH_SocketClient_Init(void)
{
	static struct sockaddr_in tDestAddr;
	static struct sockaddr_in6 tDestAddr6 = { 0 };
	vTaskDelay(50);
	if(mAddrFamily == AF_INET)
	{
		tDestAddr.sin_addr.s_addr = mClientIP;//CONFIG_ETHERNET_SOCKET_CLIENT_IP;
		tDestAddr.sin_family = AF_INET;
		tDestAddr.sin_port = htons(mClientPort);//htons(CONFIG_ETHERNET_SOCKET_CLIENT_PORT);
	}
	else if(mAddrFamily == AF_INET6)
	{
		inet6_aton(mClientIP, &tDestAddr6.sin6_addr);
		tDestAddr6.sin6_family = AF_INET6;
		tDestAddr6.sin6_port = htons(mClientPort);
		//dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
	}
	mClientSocket=  socket(mAddrFamily, SOCK_STREAM, mAddrFamily == AF_INET6? IPPROTO_IPV6:IPPROTO_IP);
	if (mClientSocket < 0)
	{
		printf("Ethernet socket client creat fail!\n");
		shutdown(mClientSocket, SHUT_RDWR);
		close(mClientSocket);
		return;
	}
	printf("Ethernet socket client created!\n");
    int err = connect(mClientSocket, mAddrFamily == AF_INET6? (struct sockaddr *)&tDestAddr6: (struct sockaddr *)&tDestAddr,
					sizeof(struct sockaddr_in6));
    if (err != 0)
    {
    	printf("Ethernet socket client connect fail!\n");
    	shutdown(mClientSocket, SHUT_RDWR);
    	close(mClientSocket);
        return;
    }
    printf("Ethernet socket client connected!\n");
    mEthClientConnected = 1;
}

static void vTcp_Client_Task(void *pvParameters)
{
	char cRxBuffer[CONFIG_ETHERNET_SOCKET_FRAME_SIZE];
	while(1)
	{
		if(mConnected)
		{
			if(!mEthClientConnected)
			{
				vETH_SocketClient_Init();
			}
			else
			{
		        while (1)
		        {
		            int iLen = recv(mClientSocket, cRxBuffer, sizeof(cRxBuffer) - 1, 0);
		            // Error occurred during receiving
		            if (iLen < 0)
		            {
		            	printf("Ethernet socket client data rec fail!\n");
		                break;
		            }
		            else  // Data received
		            {
		            	cRxBuffer[iLen] = 0; // Null-terminate whatever we received and treat like a string
		    			MODB_vAnalysis_TCP((uint8_t *)cRxBuffer, iLen, mModbusRtuDataRec);
		    			mModbusRtuDataRec->tTag = (void *)mClientSocket;
		            }
		            vTaskDelay(2);
		        }
		        if (mClientSocket != -1)
		        {
		        	printf("Ethernet socket client disconnected!\n");
		            shutdown(mClientSocket, 0);
		            close(mClientSocket);
		        }
		        mEthClientConnected = 0;
			}
		}
		vTaskDelay(200);
	}
}

static void vTcp_Server_Task(void *pvParameters)
{
	int iLen;
	char cRxBuffer[CONFIG_ETHERNET_SOCKET_FRAME_SIZE];
	char *TCP_ServerLog = "eth_tcp_server";
	while(1)
	{
		ESP_LOGI(TCP_ServerLog, "mConnected: %d", mConnected);
		if(mConnected)
		{
			ESP_LOGI(TCP_ServerLog, "mEthServerConnected: %d", mEthServerConnected);
			if(!mEthServerConnected)
			{
				vETH_SocketServer_Init();
			}
			else
			{
		        struct sockaddr_in6 tSourceAddr; // Large enough for both IPv4 or IPv6
		        uint iAddrLen = sizeof(tSourceAddr);
		        int iClientSocket = accept(mServerSocket, (struct sockaddr *)&tSourceAddr, &iAddrLen);
		        if (iClientSocket < 0)
		        {
		        	ESP_LOGE(TAG, "Ethernet socket server accecpt fail!");
		        	mEthServerConnected = 0;
		        }
		        else
		        {
		        	char cAddrStr[128];
		        	// Convert ip address to string
		        	if (tSourceAddr.sin6_family == AF_INET)
		        	{
		            	inet_ntoa_r(((struct sockaddr_in *)&tSourceAddr)->sin_addr.s_addr, cAddrStr, sizeof(cAddrStr) - 1);
		        	}
		        	else if (tSourceAddr.sin6_family == AF_INET6)
		        	{
		            	inet6_ntoa_r(tSourceAddr.sin6_addr, cAddrStr, sizeof(cAddrStr) - 1);
		        	}
		        	ESP_LOGI(TAG, "Ethernet socket server client in, ip address: %s", cAddrStr);
		        	iLen = 1;
		        	while(iLen)
		        	{
		            	vTaskDelay(2);
		            	iLen = recv(iClientSocket, cRxBuffer, sizeof(cRxBuffer) - 1, 0);
		               	if (iLen < 0)
		               	{
		               		ESP_LOGE(TAG, "Ethernet socket server data rec fail!");
		               		break;
		               	}
		               	else if (iLen == 0)
		               	{
		               		ESP_LOGE(TAG, "Ethernet socket server client down!");
		               	}
		               	else
		               	{
		               		cRxBuffer[iLen] = 0;
		               		esp_log_buffer_hex(TCP_ServerLog, cRxBuffer, iLen);
		               		MODB_vAnalysis_TCP((uint8_t *)cRxBuffer, iLen, mModbusRtuDataRec);
		               		mModbusRtuDataRec->tTag = (void *)iClientSocket;
		               		ESP_LOGW(TAG, "Analysis mModbusRtuDataRec->tTag:%d", (int)mModbusRtuDataRec->tTag);
		               	}
		        	}
		        	shutdown(iClientSocket, 0);
		        	close(iClientSocket);
		        }
			}
		}
		vTaskDelay(200);
	}
}

void Eth_Socket_vTsk(uint8_t ucConnected, MyQueueDataTPDF tMyQueueData)
{
	uint16_t i;
	mConnected = ucConnected;
	if(xQueueReceive(mEthQueueSend, (void * )&tMyQueueData, (portTickType)0))
	{
		mModbusRtuDataSend->tFunction = tMyQueueData.tMyQueueCommand.tMyQueueCommandType;
		mModbusRtuDataSend->ucSlaveAddr = tMyQueueData.tMyQueueCommand.usSlaveAddr;
		mModbusRtuDataSend->usRegAddr = tMyQueueData.tMyQueueCommand.usRegAddr;
		mModbusRtuDataSend->usRegCount = tMyQueueData.tMyQueueCommand.usRegLen;
		mModbusRtuDataSend->usDataLen = tMyQueueData.tMyQueueCommand.usDatalen;
		memcpy(mModbusRtuDataSend->ucData,tMyQueueData.tMyQueueCommand.cData, tMyQueueData.tMyQueueCommand.usDatalen);
		mModbusRtuDataSend->usHead = tMyQueueData.Reserver[1];
		i = MODB_dBuild_TCP(mModbusRtuDataSend, (uint8_t *)mDataSend);
		ETH_vData_Send(tMyQueueData.Reserver[0], mDataSend, i);
	}
	if(mModbusRtuDataRec->ucEffect == MODBUS_RESULT_SUCCESS)
	{
		if(mModbusRtuDataRec->ucSlaveAddr == 0)
		{
			if(mModbusRtuDataRec->usRegAddr == 0x01fd && mModbusRtuDataRec->tFunction == WriteReg)
			{
				mModbusRtuDataRec->ucEffect = MODBUS_RESULT_FAIL;
				switch(mModbusRtuDataRec->ucData[1])
				{
					case 1:
						ESP_LOGI("ETH", "DeviceDiscoveryReStart");
						if(ble_task != NULL) esp_ble_gap_stop_scanning();
						while(ble_task != NULL) vTaskDelay(1);
						tMyQueueData.tMyQueueCommand.tMyQueueCommandType = DeviceDiscoveryReStart;
						ESP_ERROR_CHECK(esp_event_post_to(event_loop_task, TASK_EVENTS, CREATE_BLE_DISCOVERY_TASK, NULL, 0, portMAX_DELAY));
						while(ble_task == NULL) vTaskDelay(1);
						break;
					case 2:
						tMyQueueData.tMyQueueCommand.tMyQueueCommandType = DeviceDiscoveryContinue;
						break;
					default:
						tMyQueueData.tMyQueueCommand.tMyQueueCommandType = DeviceDiscoveryStop;
						break;
				}
				xQueueSend(mEthQueueRec, (void * )&tMyQueueData, (TickType_t)0);
				mModbusRtuDataSend->usHead = mModbusRtuDataRec->usHead;
				mModbusRtuDataSend->ucSlaveAddr = 0;
				mModbusRtuDataSend->tFunction = WriteReg;
				mModbusRtuDataSend->usRegAddr = 0x01fd;
				mModbusRtuDataSend->ucData[0] = mModbusRtuDataRec->ucData[0];
				mModbusRtuDataSend->ucData[1] = mModbusRtuDataRec->ucData[1];
				mModbusRtuDataSend->ucError = 0;
				i = MODB_dBuild_TCP(mModbusRtuDataSend, (uint8_t *)mDataSend);
				ETH_vData_Send((int)mModbusRtuDataRec->tTag, mDataSend, i);
			}
			else
			{
				CFG_dProcess_Protocol(mModbusRtuDataRec, mModbusRtuDataSend);
				if(mModbusRtuDataSend->ucEffect == MODBUS_RESULT_SUCCESS)
				{
					mModbusRtuDataSend->usHead = mModbusRtuDataRec->usHead;
					i = MODB_dBuild_TCP(mModbusRtuDataSend, (uint8_t *)mDataSend);
					ESP_LOGW(TAG, "before send() mModbusRtuDataRec->tTag:%d", (int)mModbusRtuDataRec->tTag);
					esp_log_buffer_hex(TAG, mDataSend, i);
					ETH_vData_Send((int)mModbusRtuDataRec->tTag, mDataSend, i);
				}
			}
		}
		else
		{
			tMyQueueData.tMyQueueCommand.tMyQueueCommandType = mModbusRtuDataRec->tFunction;
			tMyQueueData.tMyQueueCommand.usSlaveAddr = mModbusRtuDataRec->ucSlaveAddr;
			tMyQueueData.tMyQueueCommand.usRegAddr = mModbusRtuDataRec->usRegAddr;
			tMyQueueData.tMyQueueCommand.usRegLen = mModbusRtuDataRec->usRegCount;
			tMyQueueData.tMyQueueCommand.usDatalen = mModbusRtuDataRec->usDataLen;
			memcpy(tMyQueueData.tMyQueueCommand.cData, mModbusRtuDataRec->ucData, tMyQueueData.tMyQueueCommand.usDatalen);
			tMyQueueData.Reserver[0] = (uint32_t)mModbusRtuDataRec->tTag;
			tMyQueueData.Reserver[1] = mModbusRtuDataRec->usHead;
			xQueueSend(mEthQueueRec, (void * )&tMyQueueData, (TickType_t)0);
		}
		mModbusRtuDataRec->ucEffect = MODBUS_RESULT_FAIL;
	}
}

void Eth_vSocket_Close(void)
{
	if(mEthServerConnected)
	{
		close(mServerSocket);
	}
	if(mEthClientConnected)
	{
		close(mClientSocket);
	}
}

void Eth_Socket_vInit(/*uint32_t uiSocketIP, uint16_t usSocketPort, int iAddrFamily*/)
{
	free(mModbusRtuDataSend);
	free(mModbusRtuDataRec);
	free(mDataSend);
	free(mDataRec);
	mDataSend = malloc(CONFIG_ETHERNET_SOCKET_FRAME_SIZE);
	mDataRec = malloc(CONFIG_ETHERNET_SOCKET_FRAME_SIZE);
	/*mClientIP = uiSocketIP;
	mClientPort = usSocketPort;*/
	mAddrFamily = AF_INET;//iAddrFamily;
	mModbusRtuDataSend = malloc(sizeof(ModbusRtuDataTPDF));
	mModbusRtuDataRec = malloc(sizeof(ModbusRtuDataTPDF));
	mModbusRtuDataSend->tModbusMstSlvMode = MODBUS_MODE_SLAVE;
	mModbusRtuDataRec->tModbusMstSlvMode = MODBUS_MODE_SLAVE;
	mModbusRtuDataSend->ucEffect = MODBUS_RESULT_FAIL;
	mModbusRtuDataRec->ucEffect = MODBUS_RESULT_FAIL;
	mModbusRtuDataSend->ucError = 0;
	mModbusRtuDataRec->ucError = 0;
	//ESP_LOGE(TAG, "Client/Server: %d", mPartitionTable.tEthernetConfig.EthernetCliSerMode);
	/*if(mPartitionTable.tEthernetConfig.EthernetCliSerMode == ETHERNET_MODE_CLIENT)
	{
		xTaskCreate(vTcp_Client_Task, "socket client task", 4096, NULL, 4, NULL);
	}
	else if(mPartitionTable.tEthernetConfig.EthernetCliSerMode == ETHERNET_MODE_SERVER)
	{
		xTaskCreate(vTcp_Server_Task, "socket server task", 4096, NULL, 4, NULL);
	}*/
	// xTaskCreate(vTcp_Server_Task, "socket server task", 4096, NULL, 4, NULL);
	//Udp_Brdcast_Start();
}



