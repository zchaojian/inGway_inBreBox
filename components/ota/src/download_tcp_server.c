/* 1.download_tcp_server() function is a tcp server which is connected the PC sofrware
 * to download binary file. The binary file is a update package about ESP32 APP
 * function.
 */
#include "download_tcp_server.h"


#define CONFIG_EXAMPLE_IPV4
#define DOWNLOAD_PORT 22222
#define DATA_HEADER 6
#define CRC_LEN	2

static const char *TAG = "download_tcp_server";

static struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
char addr_str[128];

static unsigned int tcp_package_analyse(unsigned char *buf, int len)
{
	unsigned int image_data_len_16;
	unsigned char image_data_len[2];
/*	unsigned char transaction_ID[2];
	unsigned char protocol_ID[2];
	unsigned char recv_crc[2];
	unsigned char cal_crc[2];
	unsigned short cal_crc_16;
	unsigned char *temp_buf;*/

	/*//CRC check
	if(len > 2)
	{
		recv_crc[0] = buf[len-2];
		recv_crc[1] = buf[len-1];
		printf("recv_crc: %02X %02X\n",recv_crc[0], recv_crc[1]);

		cal_crc_16 = usMBCRC16(buf, len - 2);
		cal_crc[0] = (unsigned char)(cal_crc_16 & 0xFF);
		cal_crc[1] = (unsigned char)(cal_crc_16 >> 8);

		printf("cal_crc: %02X %02X\n",cal_crc[0], cal_crc[1]);

		if((cal_crc[0] == recv_crc[0]) && (cal_crc[1] == recv_crc[1]))
		{
			ESP_LOGI(TAG, "The CRC of Receive data check success!");
		}
		else
		{
			ESP_LOGE(TAG, "The CRC of Receive data check failed!");
			return ESP_FAIL;
		}
	}
	else
	{
		ESP_LOGE(TAG, "Read data too short!");
		return ESP_FAIL;
	}*/

	//Version check

	//data length combination
	image_data_len[0] = buf[4];
	image_data_len[1] = buf[5];
	image_data_len_16 = ((unsigned int)image_data_len[0] << 8U) | (unsigned int)image_data_len[1];
	ESP_LOGI(TAG, "image_data_len: 0x%X",image_data_len_16);

	return image_data_len_16;
}

int download_tcp_server(void)
{
    int addr_family;
    int ip_protocol;
    //char addr_str[128];
    //int clnt_sock = -1;

#ifdef CONFIG_EXAMPLE_IPV4
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(DOWNLOAD_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
        struct sockaddr_in6 dest_addr;
        bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(DOWNLOAD_PORT);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
        inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

		int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (listen_sock < 0) {
			ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
			//break;
			return ESP_FAIL;
		}
		ESP_LOGI(TAG, "Socket created");

		int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		if (err != 0) {
			ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
			//break;
			return ESP_FAIL;
		}
		ESP_LOGI(TAG, "Socket bound, port %d", DOWNLOAD_PORT);

		err = listen(listen_sock, 1);
		if (err != 0) {
			ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
			//break;
			return ESP_FAIL;
		}
		ESP_LOGI(TAG, "Socket listening");

		/*//struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
		uint addr_len = sizeof(source_addr);
		clnt_sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (clnt_sock < 0) {
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG, "Socket accepted");*/

		//Cleint_Socket = clnt_sock;

		/*
		if (clnt_sock != -1) {
			ESP_LOGE(TAG, "Shutting down socket and restarting...");
			shutdown(sock, 0);
			close(sock);
		}
		*/
		return listen_sock;
}

int tcp_client_accept(int listen_sock)
{
	int clnt_sock;

	//struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
	uint addr_len = sizeof(source_addr);
	clnt_sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
	if (clnt_sock < 0) {
	    ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
	    return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Socket accepted");
	return clnt_sock;
}


int tcp_server_read(int client_sock, unsigned char *buffer, int buf_len)
{
	int  rlen = ESP_FAIL;
	unsigned char ONOFF_transactionID[2] = {0xFF, 0xFF};
	unsigned char finish_sig[] = {0x00, 0x06, 0x00, 0x00, 0x00, OTA_UPDATE_FINISH};

    int len = recv(client_sock, buffer, buf_len, 0);
	// Error occurred during receiving
	if (len < 0)
	{
		ESP_LOGE(TAG, "recv failed: errno %d", errno);
		return ESP_FAIL;
	}
	// Connection closed
	else if (len == 0)
	{
		ESP_LOGI(TAG, "Connection closed");
		//break;
		return ESP_FAIL;
	}
	// Data received
	else
	{
		// Get the sender's ip address as string
		if (source_addr.sin6_family == PF_INET) {
			inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
		} else if (source_addr.sin6_family == PF_INET6) {
			inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
		}

		//buffer[len] = 0; // Null-terminate whatever we received and treat like a string
		ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
		//ESP_LOGI(TAG, "%s", buffer);

		//hex_diaplay(buffer, len);
		esp_log_buffer_hex(TAG,buffer,DATA_HEADER);
		//esp_log_buffer_hex(TAG,buffer + DATA_HEADER, len - DATA_HEADER);

		if(memcmp(ONOFF_transactionID, buffer, 2) == 0)
		{
			ESP_LOGI(TAG, "Receive OTA update finish signal, it will finish! ");
			if(memcmp(finish_sig, buffer + 6, 6) == 0)
			{
				ESP_LOGI(TAG, "Read data finish");
				return ESP_OK;
			}
			else
			{
				ESP_LOGE(TAG, "Receive OTA update signal: finish is WRONG");
				return ESP_FAIL;
			}
		}

		rlen = (int)tcp_package_analyse(buffer, len);
	}

	return rlen;
}

bool tcp_server_write(int client_sock, unsigned char *header_buffer, int header_buf_len, unsigned char response_state)
{
	unsigned char *write_data;
	/*int  response_crc_16;
	unsigned char response_crc[2];*/
	char read_ok = 0x01;
	char read_bad = 0x00;
	char state_data_len[2] = {0X00, 0x01};

	write_data = header_buffer;

	memcpy(write_data + header_buf_len - 2, state_data_len, 2);

	if( response_state == 0x01)
	{
		memcpy(write_data + DATA_HEADER, &read_ok, 1);
	}
	else if(response_state == 0x00)
	{
		memcpy(write_data + DATA_HEADER, &read_bad, 1);
	}

	/*response_crc_16 = usMBCRC16(write_data, header_buf_len + 1);
	response_crc[0] = (unsigned char)(response_crc_16 & 0xFF);
	response_crc[1] = (unsigned char)(response_crc_16 >> 8);

	//combine the header, ok code and crc
	memcpy(write_data + DATA_HEADER + 1, response_crc, CRC_LEN);
*/

	int err = send(client_sock, write_data, header_buf_len + 1, 0);
	//int err = send(client_sock, write_data, header_buf_len + 1 + CRC_LEN, 0);
	if (err < 0)
	{
		ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
		return ESP_FAIL;
	}
	ESP_LOGI(TAG, "Response data: ");
	//esp_log_buffer_hex(TAG,write_data,header_buf_len + 1 + CRC_LEN);
	esp_log_buffer_hex(TAG,write_data,header_buf_len + 1);
	return ESP_OK;
}
