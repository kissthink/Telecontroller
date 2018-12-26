
#include "driver/uart.h"  //����0��Ҫ��ͷ�ļ�
#include "osapi.h"  //����1��Ҫ��ͷ�ļ�
#include "user_interface.h" //WIFI������Ҫ��ͷ�ļ�
#include "espconn.h"//TCP������Ҫ��ͷ�ļ�
#include "mem.h" //ϵͳ������Ҫ��ͷ�ļ�
#include "gpio.h"  //�˿ڿ�����Ҫ��ͷ�ļ�
#include "my_json.h"

//#include "at_custom.h"
//#include "c_types.h"
//#include "eagle_soc.h"
//#include "espnow.h"
//#include "ets_sys.h"
//#include "ip_addr.h"
//#include "os_type.h"
//#include "ping.h"
//#include "pwm.h"
//#include "queue.h"
//#include "smartconfig.h"
#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
//#include "user_config.h"
#include "user_json.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "enddevice.h"

#define GPIO_EN_OUTPUT(gpio_no) 	gpio_output_set(0,0, 1<<gpio_no,0)

#define  N   5

os_timer_t checkTimer_wifistate;

LOCAL os_timer_t sntp_timer;

struct espconn user_tcp_conn;

int   begincnt=0;
int   cnt=0;

void ICACHE_FLASH_ATTR rgbGpioInit(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13);
	GPIO_OUTPUT_SET(15, 0);
	GPIO_OUTPUT_SET(12, 0);
	GPIO_OUTPUT_SET(13, 0);
}
void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg)  //����
{
	os_printf("�������ݳɹ���");
}
void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg)  //�Ͽ�
{
	os_printf("�Ͽ����ӳɹ���");
}
void ICACHE_FLASH_ATTR user_tcp_recv_cb(void *arg,  //����
		char *pdata,
		unsigned short len)
{

	os_printf("�յ����ݣ�%s\r\n",pdata);

	if(pdata[0]=='1')
	{
		GPIO_OUTPUT_SET(5, 1);
		os_printf("�̵�����\n");
	}
	else if(pdata[0]=='2')
	{
		GPIO_OUTPUT_SET(5, 0);
        os_printf("�̵�����\n");
	}
	else if(pdata[0]=='3')
	{
		GPIO_OUTPUT_SET(15, 1);
	}
	else if(pdata[0]=='4')
	{
		GPIO_OUTPUT_SET(15, 0);
	}
	else if(pdata[0]=='5')
	{
		GPIO_OUTPUT_SET(12, 1);
	}
	else if(pdata[0]=='6')
	{
		GPIO_OUTPUT_SET(12, 0);
	}
	else if(pdata[0]=='7')
	{
		GPIO_OUTPUT_SET(13, 1);
	}
	else if(pdata[0]=='8')
	{
		GPIO_OUTPUT_SET(13, 0);
	}
}
void ICACHE_FLASH_ATTR user_tcp_recon_cb(void *arg, sint8 err)//ע�� TCP ���ӷ����쳣�Ͽ�ʱ�Ļص������������ڻص������н�������
{
	os_printf("���Ӵ��󣬴������Ϊ%d\r\n",err);
	espconn_connect((struct espconn *)arg);
}
void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg)//ע�� TCP ���ӳɹ�������Ļص�����
{
	struct espconn *pespconn=arg;
	espconn_regist_recvcb(pespconn,user_tcp_recv_cb);//����
	espconn_regist_sentcb(pespconn,user_tcp_sent_cb);//����
	espconn_regist_disconcb(pespconn,user_tcp_discon_cb);//�Ͽ�

}

void ICACHE_FLASH_ATTR my_station_init(struct ip_addr *remote_ip,struct ip_addr *local_ip,int remote_port)
{
	user_tcp_conn.proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));//����ռ�
	user_tcp_conn.type=ESPCONN_TCP;//��������ΪTCPЭ��
	os_memcpy(user_tcp_conn.proto.tcp->local_ip,local_ip,4);
	os_memcpy(user_tcp_conn.proto.tcp->remote_ip,remote_ip,4);
	user_tcp_conn.proto.tcp->local_port=espconn_port();//���ض˿�
	user_tcp_conn.proto.tcp->remote_port=remote_port;//Ŀ��˿�
	//ע�����ӳɹ��ص��������������ӻص�����
	espconn_regist_connectcb(&user_tcp_conn,user_tcp_connect_cb);//ע�� TCP ���ӳɹ�������Ļص�����
	espconn_regist_reconcb(&user_tcp_conn,user_tcp_recon_cb);//ע�� TCP ���ӷ����쳣�Ͽ�ʱ�Ļص������������ڻص������н�������
	//��������
	espconn_connect(&user_tcp_conn);
}


void GetDataFunc(void)
{
	uint8 *RequestData = "0";
	espconn_sent(&user_tcp_conn,RequestData,strlen(RequestData));
	   //  os_printf("%s\n",RequestData);
}

void Check_WifiState(void)
{
	 uint8 getState;

		//struct espconn *pespconn;


	 getState = wifi_station_get_connect_status();

	 //��ѯ ESP8266 WiFi station �ӿ����� AP ��״̬
	 		  //����:
	 		  //enum{
	 		  //      STATION_IDLE = 0,    //δ�����ѱ����wifi
	 		  //      STATION_CONNECTING,  //��������·��
	 		  //      STATION_WRONG_PASSWORD, //�������
	 		  //      STATION_NO_AP_FOUND, //δ�����ѱ����wifi
	 		  //      STATION_CONNECT_FAIL, //����·��ʧ��
	 		  //      STATION_GOT_IP
	 		  //};
	 if(getState == STATION_GOT_IP)//���״̬��ȷ��֤���Ѿ�����
	  {
		 os_printf("WIFI���ӳɹ���");
		 os_timer_disarm(&checkTimer_wifistate);

        //183.230.40.33:80
		 struct ip_info info;
		 const char remote_ip[4]={106,12,132,115};//Ŀ��IP��ַ
		 wifi_get_ip_info(STATION_IF,&info);//��ѯ WiFiģ��� IP ��ַ
		 my_station_init((struct ip_addr *)remote_ip,&info.ip,10087);//���ӵ�Ŀ���������80�˿�

		  os_timer_setfn(&checkTimer_wifistate,(os_timer_func_t *)GetDataFunc,NULL);//���ö�ʱ���ص�����
		  os_timer_arm(&checkTimer_wifistate,2000,1);//������ʱ������λ������

	  }
}

void user_init() //��ʼ��
{
	uint8 opmode;

	uart_init(115200,115200);
	//uart_init(57600, 57600);//���ô���0�ʹ���1�Ĳ�����
	os_printf("enter here  2!\n");

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,FUNC_GPIO5);
	GPIO_OUTPUT_SET(5, 0);

	wifi_set_opmode(0x01);//����ΪSTATIONģʽ

	struct station_config stationConf;
	os_strcpy(stationConf.ssid,"Sakila");
	os_strcpy(stationConf.password,"12345679");

	wifi_station_set_config(&stationConf);//����WiFi station�ӿ����ã������浽 flash
	wifi_station_disconnect();//�Ͽ�·����
	wifi_station_connect();//����·����
	os_timer_disarm(&checkTimer_wifistate);//ȡ����ʱ����ʱ
	rgbGpioInit();
	os_timer_setfn(&checkTimer_wifistate,(os_timer_func_t *)Check_WifiState,NULL);//���ö�ʱ���ص�����
	os_timer_arm(&checkTimer_wifistate,500,1);//������ʱ������λ������
}

void user_rf_pre_init()
{

}

