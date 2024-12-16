/*******************************************************
@company: Copyright (C) 2018, Leishen Intelligent System
@product: serial
@filename: lsiosr.cpp
@brief:
@version:       date:       author:     comments:
@v2.0           24-02-16    yn          recovery
*******************************************************/
#include "ls01b_v2/ls01b_node.h"
#include "ls01b_v2/lsiosr.h"
#include "ros/ros.h"

namespace ls {
int rc;
LSIOSR* LSIOSR::instance(std::string name, int speed, int fd)
{
  // static LSIOSR obj(name, speed, fd);
  return new LSIOSR(name, speed, fd);
}

LSIOSR::LSIOSR(std::string port, int baud_rate, int fd): port_(port), baud_rate_(baud_rate), fd_(fd)
{
  ROS_INFO("port = %s, baud_rate = %d", port.c_str(), baud_rate);
}

LSIOSR::~LSIOSR()
{
  close();
}
/* 직렬 포트 설정 함수 */
int LSIOSR::setOpt(int nBits, uint8_t nEvent, int nStop)
{
  struct termios newtio, oldtio;
  /*기존 직렬 포트 파라미터 설정을 저장하여 테스트하고 있으며, 여기서 직렬 번호 등의 오류가 발생하면 관련 오류 메시지가 나타납니다*/
  if (tcgetattr(fd_, &oldtio) != 0)
  {
    perror("SetupSerial 1");
    return -1;
  }
  bzero(&newtio, sizeof(newtio));
  /*Step1. 글자크기 설정*/
  newtio.c_cflag |= CLOCAL;   //만약 설정한다면，모뎀 제어선은 무시될 것입니다.설정이 되어 있지 않은 경우 open()함수는 반송파 감지선이 알려질 때까지 차단합니다. 모뎀 오프훅 상태가 될 때까지。
  newtio.c_cflag |= CREAD;    //입력 데이터를 포트에서 읽을 수 있도록 하기
  /*각 데이터의 자릿수 설정*/
  switch (nBits)
  {
  case 7:
    newtio.c_cflag |= CS7;
    break;
  case 8:
    newtio.c_cflag |= CS8;
    break;
  }
  /*패리티 비트 설정*/
  switch (nEvent)
  {
  case 'O': //홀수
    newtio.c_iflag |= (INPCK | ISTRIP);
    newtio.c_cflag |= PARENB;   //Enable check, PARODD를 설정하지 않으면 짝수 체크
    newtio.c_cflag |= PARODD;   //홀수 검사
    break;
  case 'E': //짝수
    newtio.c_iflag |= (INPCK | ISTRIP);
    newtio.c_cflag |= PARENB;
    newtio.c_cflag &= ~PARODD;
    break;
  case 'N':  //패리티 비트 없음
    newtio.c_cflag &= ~PARENB;
    break;
  }
  /*设置波特率*/
  switch (baud_rate_)
  {
  case 2400:
    cfsetispeed(&newtio, B2400);
    cfsetospeed(&newtio, B2400);
    break;
  case 4800:
    cfsetispeed(&newtio, B4800);
    cfsetospeed(&newtio, B4800);
    break;
  case 9600:
    cfsetispeed(&newtio, B9600);
    cfsetospeed(&newtio, B9600);
    break;
  case 115200:
    cfsetispeed(&newtio, B115200);
    cfsetospeed(&newtio, B115200);
    break;
  case 230400:
    cfsetispeed(&newtio, B230400);
    cfsetospeed(&newtio, B230400);
    break;
  case 460800:
    cfsetispeed(&newtio, B460800);
    cfsetospeed(&newtio, B460800);
    break;
  default:
    cfsetispeed(&newtio, B9600);
    cfsetospeed(&newtio, B9600);
    break;
  }
  /*
   * 정지 비트 설정
   * 정지 비트의 자릿수를 설정합니다. 설정하면 프레임마다 두 개의 정지 비트가 생성되고, 설정되지 않으면 한 개의 정지 비트가 생성됩니다
   * 스톱 비트. 일반적으로 한 자리 스톱 비트를 사용합니다.두 분의 정지 공간이 필요한 설비는 이미 구식입니다.
   * */
  if (nStop == 1)
  {
    newtio.c_cflag &= ~CSTOPB;
  }
  else if (nStop == 2)
  {
    newtio.c_cflag |= CSTOPB;
  }
  /*대기 시간과 최소 수신 문자 설정*/
  newtio.c_cc[VTIME] = 0;
  newtio.c_cc[VMIN] = 0;
  /*수신되지 않은 문자 처리*/
  tcflush(fd_, TCIFLUSH);
  /*새 설정 활성화*/
  if ((tcsetattr(fd_, TCSANOW, &newtio)) != 0)
  {
    perror("serial set error");
    return -1;
  }
  return 0;
}

/* 직렬 포트에서 데이터 읽기 */
int LSIOSR::read(char *buffer, int length, int timeout)
{
  memset(buffer, 0, length);

  int	totalBytesRead = 0;
  // int rc;
  char* pb = buffer;

  int reset_ck = 0;
  if (timeout > 0)
  {
    rc = waitReadable(timeout);
    if (rc <= 0)
    {
      // 재시작이 원래 있던 곳
      return (rc == 0) ? 0 : -1;
    }
    int	retry = 3;
    while (length > 0)
    {
      rc = ::read(fd_, pb, (size_t)length);

      if (rc > 0)
      {
        length -= rc;
        pb += rc;
        totalBytesRead += rc;

        if (length == 0)
        {
          break;
        }
      }
      else if (rc < 0)
      {
        ROS_ERROR("error rc %d", rc);
        retry--;
        if (retry <= 0) return -1;
      }
      else if (rc == 0)
      {
        ROS_ERROR("Port Disconnected");
        retry--;
        if (retry <= 0) return -1;
      }
      // else if (rc == 0)
      // {
      //   reset_ck = 1;
      //   break;
      // }
      rc = waitReadable(20);
      if (rc <= 0)
      {
        break;
      }
      // if (rc <= 0)
      // {
      //   if (rc == 0)
      //   {
      //     reset_ck = 1;
      //     break;
      //   }
      //   else
      //   {
      //     break;
      //   }
      // }
    }
    // if(reset_ck == 1)
    // {
    //   ROS_ERROR("Port Disconnected Port Initialization");
    //   LSIOSR::init();
    //   ls::LS01B_Node ls01b_node;
    //   ls01b_node.run();
    // }
  }
  else
  {
    rc = ::read(fd_, pb, (size_t)length);
    if (rc > 0)
    {
      totalBytesRead += rc;
    }
    else if ((rc < 0) && (errno != EINTR) && (errno != EAGAIN))
    {
      ROS_ERROR("read error");
      return -1;
    }
  }
  if(0)
  {
    ROS_ERROR("Serial Rx: ");
    for(int i = 0; i < totalBytesRead; i++)
    {
      ROS_ERROR("%02X ", (buffer[i]) & 0xFF);
    }
    ROS_ERROR("\n\n");
  }
  return totalBytesRead;
}

int LSIOSR::waitReadable(int millis)
{
  if (fd_ < 0)
  {
    return -1;
  }
  int serial = fd_;
  
  fd_set fdset;
  struct timeval tv;
  // int rc = 0;
  rc = 0;
  
  while (millis > 0)
  {
    if (millis < 5000)
    {
      tv.tv_usec = millis % 1000 * 1000;
      tv.tv_sec  = millis / 1000;

      millis = 0;
    }
    else
    {
      tv.tv_usec = 0;
      tv.tv_sec  = 5;

      millis -= 5000;
    }
    FD_ZERO(&fdset);
    FD_SET(serial, &fdset);
    
    rc = select(serial + 1, &fdset, NULL, NULL, &tv);
    if (rc > 0)
    {
      rc = (FD_ISSET(serial, &fdset)) ? 1 : -1;
      break;
    }
    else if (rc < 0)
    {
      rc = -1;
      break;
    }
  }
  return rc;
}


int LSIOSR::waitWritable(int millis)
{
  if (fd_ < 0)
  {
    return -1;
  }
  int serial = fd_;

  fd_set fdset;
  struct timeval tv;
  // int rc = 0;
  rc = 0;

  while (millis > 0)
  {
    if (millis < 5000)
    {
      tv.tv_usec = millis % 1000 * 1000;
      tv.tv_sec  = millis / 1000;

      millis = 0;
    }
    else
    {
      tv.tv_usec = 0;
      tv.tv_sec  = 5;

      millis -= 5000;
    }
    FD_ZERO(&fdset);
    FD_SET(serial, &fdset);

    rc = select(serial + 1, NULL, &fdset, NULL, &tv);
    if (rc > 0)
    {
      rc = (FD_ISSET(serial, &fdset)) ? 1 : -1;
      break;
    }
    else if (rc < 0)
    {
      rc = -1;
      break;
    }
  }
  return rc;
}

/* 직렬 포트로 데이터 보내기 */
int LSIOSR::send(const char* buffer, int length, int timeout)
{
  if (fd_ < 0)
  {
    return -1;
  }
  if ((buffer == 0) || (length <= 0))
  {
    return -1;
  }
  int	totalBytesWrite = 0;
  // int rc;
  char* pb = (char*)buffer;


  if (timeout > 0)
  {
    rc = waitWritable(timeout);
    if (rc <= 0)
    {
      return (rc == 0) ? 0 : -1;
    }
    int	retry = 3;
    while (length > 0)
    {
      rc = write(fd_, pb, (size_t)length);
      if (rc > 0)
      {
        length -= rc;
        pb += rc;
        totalBytesWrite += rc;

        if (length == 0)
        {
          break;
        }
      }
      else
      {
        retry--;
        if (retry <= 0)
        {
          break;
        }
      }
      rc = waitWritable(50);
      if (rc <= 0)
      {
        break;
      }
    }
  }
  else
  {
    rc = write(fd_, pb, (size_t)length);
    if (rc > 0)
    {
      totalBytesWrite += rc;
    }
    else if ((rc < 0) && (errno != EINTR) && (errno != EAGAIN))
    {
      return -1;
    }
  }
  if(0)
  {
    ROS_INFO("Serial Tx: ");
    for(int i = 0; i < totalBytesWrite; i++)
    {
      ROS_INFO("%02X ", (buffer[i]) & 0xFF);
    }
    ROS_INFO("\n\n");
  }
  return totalBytesWrite;
}

int LSIOSR::init()
{
  int error_code = 0;

  close();
  fd_ = open(port_.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);
  if (fd_ < 0)
  {
    error_code = -1;
    ROS_ERROR("open_port %s ERROR !", port_.c_str());
  }
  else
  {
    error_code = 0;
    setOpt(DATA_BIT_8, PARITY_NONE, STOP_BIT_1);//직렬 포트 매개 변수 설정
    ROS_INFO("open_port %s , fd %d OK !", port_.c_str(), fd_);
  }

  return error_code;
}

int LSIOSR::close()
{
  if(fd_ >= 0) ::close(fd_);
  fd_ = -1;
  return 1;
}

std::string LSIOSR::getPort()
{
  return port_;
}

int LSIOSR::setPort(std::string name)
{
  port_ = name;
  return 0;
}

}
