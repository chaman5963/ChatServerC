#include <iostream>		// ���o�͗p
#include <WinSock2.h>	// WinSock�p
#include <ws2tcpip.h>	// WinSock�p

#pragma comment( lib, "ws2_32.lib" )

const unsigned short PORT = 8080;	// �T�[�o�̃|�[�g�ԍ�

int main()
{
	std::cout << "Server\n";
	// WinSock�̏�����	WSAStartup()
	WSADATA  wsaData;

	// WinSock2.2 ����������
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		// �G���[����
	}

	// UDP�\�P�b�g�̍쐬	socket()
	SOCKET  sock;

	// UDP�\�P�b�g�̍쐬
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		// �G���[
	}

	// �|�[�g�ԍ��̊��蓖��	bind()
	// �\�P�b�g�A�h���X�\����
	SOCKADDR_IN  bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));		// 0�N���A�ŏ�����
	bindAddr.sin_family = AF_INET;					// IPv4�A�h���X�g�p
	bindAddr.sin_port = htons(PORT);			// �|�[�g�ԍ�8080�w��	�}�W�b�N�i���o�[��NG
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// ���ׂĂ̎�����NIC���Ώ�

	// �|�[�g�ԍ����蓖��
	if (bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
	{
		// �G���[����
	}

	// �펞�ғ��Ŗ������[�v
	while (true)
	{
		// �����l�̎�M	recvfrom()
		int recvCount;

		// ���M���̃A�h���X�����i�[����\�P�b�g�A�h���X�\����
		SOCKADDR_IN fromAddr;
		int fromlen = sizeof(fromAddr);

		// buff�Ɏ�M������������i�[
		int ret = recvfrom(sock, (char*)&recvCount, sizeof(recvCount), 0, (SOCKADDR*)&fromAddr, &fromlen);
		if (ret == SOCKET_ERROR)
		{
			// �G���[����
		}

		// �ƂĂ��厖�ȏ��� ... ���o�C�g�f�[�^������	???()
		int count = ntohl( recvCount );

		std::cout << "Receive count : " << count << std::endl;

		// �����l�̌�����x�𐶐�
		char  buff[1024];
		for (int i = 0; i < count; i++)
		{
			buff[i] = 'x';
		}
		buff[count] = '\0';
		std::cout << "Send message : " << buff << std::endl;

		// �o���オ����������𑗐M	sendto()
		ret = sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, fromlen);
	}

	// �����������[�v���I�������
	// UDP�\�P�b�g�j��		closesocket()
	if (closesocket(sock) == SOCKET_ERROR)
	{
		// �G���[
	}

	// WinSock�I������		WSACleanup()
	if (WSACleanup() != 0)
	{
		// �G���[����
	}

	return 0;	// ����I���ƐM���āI
}