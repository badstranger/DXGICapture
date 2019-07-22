#pragma once

#ifndef _MEDIA_HH
#include "Media.hh"
#endif

#include "Groupsock.hh"

class InputStreamSource :public Medium {
public:
	typedef void (InputStreamHandler)(void*, unsigned char*, int); //����������
	typedef bool (SendInputStreamHandler)(void*);
	static InputStreamSource* createNew(UsageEnvironment& env, int socketNum, netAddressBits address = 0, portNumBits port = 0, u_int32_t fOurSessionId = 0);

	Boolean sendStream(unsigned char* buffer, unsigned bufferSize);
	void setServerRequestAlternativeByteHandler(TaskScheduler::BackgroundHandlerProc* handler, void* clientData);
	static void SetRecInputStreamHandler(InputStreamHandler* recInputStreamHandler);

	void incrementReferenceCount() { ++m_fReferenceCountUdp; }
	void decrementReferenceCount() { if (m_fReferenceCountUdp > 0) --m_fReferenceCountUdp; }
	int getReferenceCount() const { return m_fReferenceCountUdp; }
protected:
	InputStreamSource(UsageEnvironment& env, int socketNum, netAddressBits address, portNumBits port, u_int32_t fOurSessionId);
	virtual ~InputStreamSource();

	virtual void inputStreamSourceHandler(unsigned char* buffer, int size);

	u_int32_t m_fOurSessionId;
private:
	//���մ���
	static void ReadTcpInputHandler(void* instance, int /*mask*/);
	static void ReadUdpInputHandler(void* instance, int /*mask*/);
	void incomingTcpInputHandler();
	void incomingUdpInputHandler();

	//���ʹ���
	static bool sendTcpStream(void* inputStreamSource);
	static bool sendUdpStream(void* inputStreamSource);
	bool sendTcpStream();
	bool sendUdpStream();

	Boolean sendData(unsigned char* buffer, unsigned bufferSize, netAddressBits address = 0, portNumBits portNum = 0);

private:
	int m_fReferenceCountUdp;
	int m_fSocketNum; // "TCP/UDP"
	netAddressBits m_destinationAddress; //UDPĿ�ĵ�ַ 
	portNumBits m_destinationPort; //UDPĿ�Ķ˿�
	struct sockaddr_in m_fromeAddr; //UDP��Դ��ַ
	Groupsock* m_inputGroupsock; //UDP�鲥

	TaskScheduler::BackgroundHandlerProc* m_serverRequestAlternativeByteHandler;
	void* m_serverRequestAlternativeByteHandlerClientData;
	static InputStreamHandler* m_recInputStreamHandler;

	unsigned char* m_fReceiveBuffer;
	int m_fReceiveBufferSize;

	unsigned char* m_buffer;
	int m_bufferSize;
	SendInputStreamHandler* m_sendInputStreamHandler;

	unsigned short m_fNextTCPReadSize;
	enum
	{
		AWAITING_DOLLAR,
		AWAITING_SIZE1,
		AWAITING_SIZE2,
		AWAITING_PACKET_DATA
	} fTCPReadingState;
};