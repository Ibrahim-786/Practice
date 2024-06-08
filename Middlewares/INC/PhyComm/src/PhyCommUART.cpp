/**
 * @file    PhyCommUART.cpp
 * ----------------------------------------------------------
 * @author    ()
 * # Create On
 * @date    Wednesday, 05 June, 2024
 * -------------------------------------------------------------------------
 * #Description:
 * @details
 *
 * # TRACEABILITY :
 *  -sw Requirement Document(s) :
 *
 *  -sw Design Document (s)     :
 *
 *  -Repository Path        :
 *
 * - coding Standard (s)     :
 *  -
 * -
 * # DEVIATIONS FROM STANDARD :
 *
*/

/**********************************************************************************************
********************************************************************************************
* Standard Includes
 *******************************************************************************************/
#include<cerrno>
#include<cstdint>
#include<cstdlib>
#include<cstring>
#include<errno.h>
#include<fcntl.h>
#include<iostream>
#include<mutex>
#include<poll.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<termios.h>
#include<thread>
#include<unistd.h>
#include<vector>
 /******************************************************************************************
  * Other Includes
  ******************************************************************************************/
#include "common.hpp"
#include "log.hpp"
#include "return.hpp"
//PhyComm/public
#include "PhyComm.hpp"
#include "PhyCommUART.hpp"


  /****************************************************************************************
  * Using Directives
  ****************************************************************************************/
using namespace std;
  /******************************************************************************************
  * Forward Declaration
  ******************************************************************************************/

  /****************************************************************************************
  * Global Macros
  ****************************************************************************************/

  /****************************************************************************************
  * Global Constants
  ****************************************************************************************/

  /****************************************************************************************
  * Global Type defination
  ****************************************************************************************/

/****************************************************************************************
  * Local Variable declaration
  ****************************************************************************************/
static int64_t bytes_read = 0;
/****************************************************************************************
  * Local function declaration
  ****************************************************************************************/
void readReadyMonitor(int dev_fd,READ_READY_FN_CB fn_cb);

  /***************************************************************************************
   * Function definition
   * ***************************************************************************************/
CPhyCommUART::CPhyCommUART(string dev_name,int baud_rate):_dev_name(dev_name),_baud_rate(baud_rate){}
CPhyCommUART::~CPhyCommUART(){}
CMStatus CPhyCommUART::OpenDevice(){
CMStatus ret;
const char *device = _device_name.c_str();
_dev_fd = open(file:device,oflag:O_RDWR|O_NOCTTY|O_NONBLOCK);
	if(_dev_fd ==-1)
	{
		LOG_E("ERROR: open failed for %s| errno[%s]:%s",device,errno,strerror(errno));
		ret = OPEN_DEVICE_FAILED;
		return ret;
	}
	else
	{
		LOG_I("open success");
	}
	ret =ok;
	return ret;

}

CMStatus CphyCommUART::closeDevice(){
	
	CMStatus ret;

	int c_ret = close(fd: _dev_fd);
	if(c_ret ==0)
	{
		ret =ok;
	}
	else if(c_ret ==-1)
	{
		LOG_D("ERROR: errno[%d]:%s",errno,strerror(errno));
		ret = CLOSE_DEVICE_FAILED;
	}
	return ret;
}

CMStatus CPhyCommUART::InitDevice()
{
	CMStatus ret;
	struct termios tty;
	memset(s:&tty,c:0,n:sizeof(tty));
	if(tcgetattr(fd:_dev_fd,termios_p:&tty)){

		LOG_D("ERROR:tcgetattr failed | errno[%d]:%s",errno,strerror(errno));
		ret = INIT_DEVICE_FAILED;
		return ret;

	}
	else
	{
		LOG_I("tcgetattr success");

	}

	if(_baud_rate == 9600)
	{
		cfsetospeed(termios_p:&tty,speed:B9600);
		cfsetispeed(termios_p:&tty,speed:B9600);
	}

	else if(_baud_rate == 115200)
	{
		cfsetospeed(termios_p:&tty,speed:B115200);
		cfsetispeed(termios_p:&tty,speed:B115200);
	}
	else if(_baud_rate == 921600)
	{
		cfsetospeed(termios_p:&tty,speed:B921600);
		cfsetispeed(termios_p:&tty,speed:B921600);
	}		
	tty.c_cflag  &= ~PARENB; // No Parity bit
	tty.c_cflag  &= ~CSTOPB; // 1 stop bit
	tty.c_cflag  &= ~CSIZE;  // clear the mask
	tty.c_cflag  |=  CS8;    // 8 data bits
	tty.c_cflag  &= ~CRTSCTS;// No Hardware flow control

	tty.c_iflag  &= ~(IXON|IXOFF|IXANY); // No software flow control
	tty.c_lflag   = 0; // No signalling chars , no echo,

		           //no canonical processing
	tty.c_oflag   = 0; //No remapping ,No delays
	tty.c_cc[VMIN] = 0; //read dosent block
	tty.c_cc[VTIME] = 5 ; //0.5 seconds read timeout

	if(tcsetattr(fd:_dev_fd,optinal_actions:TCSANOW,termios_p:&tty)!=0)
	{
		LOG_D("ERROR:tcsetattr failed errno[%d]:%s",errno,strerror(errno));
		ret = INIT_DEVICE_FAILED;
		return ret;
	}
	else 
	{
		 LOG_I("tcsetattr success");
	}
	ret = ok;
//commenting to use direct read function call for now
//thread readReadyMonitorTask(&readReadyMonitor,_dev_fd,fn_cb);
//readReadyMonitorTask.join();
return ret;
}


CMStatus CPhyCommUART::ReadData(vector<uint8_t> &v_buff,int buff_size)
{
  CMStatus ret;
  //TODO:use buff_size for reading the number of bytes mentioned
  int num_bytes =0;
  struct pollfd fds;

  fds.fd = _dev_fd;
  fds.events = POLLIN;

  int result = poll(fds:&fds,nfds:1,timeout:20);

  if(result ==-1){
  LOG_E("ERROR:poll call failed | errno[%d]:%s",errno,strerror(errno));
  }
  else if(result == 0)
  {
   //LOG_D("poll call timeout");
  }
  else if(result == 1)
  {
   if(fds.revents & POLLIN)
   {
	  uint8_t buff;
	   num_bytes +=read(fd:_dev_fd,buf:&buff,nbytes:sizeof(buff));
	   v_buff.push_back(x:buff);
  
   }
  }

  if(num_bytes >0){
  	for(int i =0;i<num_bytes;i++){
	LOG_D("0x%02x",v_buff.at(n:0));
	}
	ret = ok;
  }else{
    ret = READ_DATA_FAILED;
  }
return ret;
}

CMStatus CPhyCommUART::writeData(const uint8_t &buff,int buff_size)
{
	CMStatus ret;

	int num_bytes = 0;
	ssize_t w_ret = write(fd:_dev_fd,buf:&buff,n:sizeof(buff));
	if(w_ret == buff_size)
	{
 		ret = ok;
	}
	else
	{
		ret = WRITE_DATA_FAILED;
		LOG_E("ERROR:errno[%d]:%s",errno,strerror(errno));
	}

return ret;
}

CMStatus CPhyCommUART::getDeviceType()
{
	CMStatus ret;
	ret ok;
return ret;
}

/********************************************************************************************
 * Local Function Definitions
 * *****************************************************************************************/
void readReadyMonitor(int dev_fd,READ_READY_FN_CB fn_cb)
{
	struct pollfd fds;
	LOG_I("dev_fd:%d",dev_fd);

	fds.fd = dev_fd;
	fds.events = POLLIN;

	while(1)
	{
	 int result = poll(fds:&fds,nfds:1,timeout:75);

	 if(result == -1)
	 {
	 	LOG_E("ERROR:poll call failed |errno[%d]:%s",errno,strerror(errno));
	 }
	 else if(result ==0){
	 
		//LOG_D("poll call timeout");
	 }
	 else if (result ==1){
	 	if(fds.revents & POLLIN)
		//LOG_D("poll call success");
		 fd_cb();
	 }
	
	}
}

  /*******************************************************************************************
  *  Revision History
  ********************************************************************************************/
