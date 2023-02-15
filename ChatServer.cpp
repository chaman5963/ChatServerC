﻿//// ChatServer.cpp : アプリケーションのエントリ ポイントを定義します。
////
//#include "framework.h"
//#include "ChatServer.h"
//#include "resource.h"
//#include <string>
//#include <WinSock2.h>	// WinSock用
//#include <ws2tcpip.h>	// WinSock用
//
//#pragma comment( lib, "ws2_32.lib" )
//
//const unsigned short PORT = IDC_PORTEDIT;
//
//HINSTANCE hInst;
//std::wstring message; //チャット欄に
//
//HWND	hMessageEdit;		// チャット欄
//HWND	hSendMessageEdit;	// 送信メッセージ入力欄
//
//WCHAR buff[1024];	// マジックナンバーがとてもいや
//
//BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
//
//// このコード モジュールに含まれる関数の宣言を転送します:
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int  nCmdShow)
//{
//    hInst = hInstance;
//    DialogBox(hInstance, MAKEINTRESOURCE(IDD_CHATSERVERDIALOG), NULL, (DLGPROC)DlgProc);
//    return (int)0;
//
//    
//}
//
//BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
//{
//    HWND hMessageEdit = GetDlgItem(hDlg, IDC_MESSAGEEDIT);
//
//    u_long arg = 0x01;
//    SOCKADDR_IN bindAddr;
//    switch (msg) {
//
//    case WM_INITDIALOG:
//        hMessageEdit = GetDlgItem(hDlg, IDC_MESSAGEEDIT);
//        hSendMessageEdit = GetDlgItem(hDlg, IDC_EDIT2);
//        return  TRUE;
//
//        // WinSock初期化
//        WSADATA  wsaData;
//        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//        {
//            // 初期化失敗
//        }
//
//
//        // UDPソケットの作成	socket()
//        SOCKET  sock;
//
//        // UDPソケットの作成
//        if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
//        {
//            // エラー
//        }
//        
//        // ポート番号の割り当て	bind()
//        // ソケットアドレス構造体
//        SOCKADDR_IN  bindAddr;
//        memset(&bindAddr, 0, sizeof(bindAddr));		// 0クリアで初期化
//        bindAddr.sin_family = AF_INET;					// IPv4アドレス使用
//        bindAddr.sin_port = htons(PORT);			// ポート番号8080指定	マジックナンバーはNG
//        bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// すべての自分のNICが対象
//        
//        // ポート番号割り当て
//        if (bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
//        {
//            // エラー処理
//        }
//
//
//
//
//    case WM_COMMAND:
//
//        switch (LOWORD(wp)) {
//
//        case IDC_SENDBUTTON:
//
//            GetWindowText(hSendMessageEdit, (LPTSTR)buff, 1024);
//
//            // messageに↑で取得したbuffと改行コード追加
//            message.append(buff);
//            message.append(L"\r\n");
//            // チャット欄にmessageセット！
//            SetWindowText(hMessageEdit, message.c_str());
//            return TRUE;
//
//
//        case IDCANCEL:
//            EndDialog(hDlg, IDCANCEL);
//            return TRUE;
//        }
//        return FALSE;
//    }
//
//    return FALSE;
//}






#include <WinSock2.h>
#include <WS2tcpip.h>
#include "framework.h"
#include "ChatServer.h"
#include "resource.h"
#include <string>
#include <vector>

#pragma comment( lib, "ws2_32.lib" )


// グローバル変数
HINSTANCE hInst;
HWND hMessageEdit;
HWND hSendMessageEdit;
HWND hIpAddressEdit;
HWND hPortEdit;
const unsigned short SERVERPORT = 8080;
const UINT_PTR TIMERID = 1000;

std::string message;   // チャット欄にセットする文字列
SOCKET sock;

struct date
{
    SOCKADDR_IN addr;
};
std::vector<date> nameList;

// ダイアログプロシージャ
BOOL CALLBACK BindDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

// このコード モジュールに含まれる関数の宣言を転送します:
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int  nCmdShow)
{
    hInst = hInstance;

    //   if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_BINDDIALOG), NULL, (DLGPROC)BindDlgProc) == IDOK)
    {
        // ダイアログ
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_CHATSERVERDIALOG), NULL, (DLGPROC)DlgProc);
    }

    return (int)0;
}

BOOL CALLBACK BindDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
            // 送信ボタン押下時
        case IDOK:
            // 送信メッセージを取得

            EndDialog(hDlg, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }


        return FALSE;
    }

    return FALSE;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    char buff[1024];
    char ipAddr[256];
    int port;
    char portstr[256];
    u_long arg = 0x01;
    int ret;
    SOCKADDR_IN fromAddr;
    int fromlen = sizeof(fromAddr);

    SOCKADDR_IN toAddr;
    int tolen = sizeof(toAddr);
    bool is = false;

    switch (msg)
    {
    case WM_INITDIALOG:
        // 各コントロールのハンドル取得
        hMessageEdit = GetDlgItem(hDlg, IDC_MESSAGEEDIT);
        hSendMessageEdit = GetDlgItem(hDlg, IDC_EDIT2);
        hIpAddressEdit = GetDlgItem(hDlg, IDC_IPADDRESS1);
        hPortEdit = GetDlgItem(hDlg, IDC_PORTEDIT);


        // WinSock初期化
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // ソケット作成
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock > 0)
        {
            //  message.append("socket ok\r\n");

        }

        // ノンブロッキングソケットに設定
        ioctlsocket(sock, FIONBIO, &arg);


        // bind
        SOCKADDR_IN bindAddr;
        memset(&bindAddr, 0, sizeof(bindAddr));
        bindAddr.sin_family = AF_INET;
        bindAddr.sin_port = htons(SERVERPORT);
        bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// すべての自分のNICが対象
        if (ret = bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr)) == 0)
        {
            // message.append("bind ok\r\n");

        }



        // タイマーセット
        SetTimer(hDlg, TIMERID, 100, NULL);

        return TRUE;
    case WM_TIMER:
        // 受信
        ret = recvfrom(sock, (char*)buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, &fromlen);
        if (ret < 0)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                // エラー

            }
            return TRUE;
        }
        else
        {
            // 受信データがあれば、チャット欄に追加

            message.append((buff));
            message.append("-- from:");
            /*inet_ntop(AF_INET, &fromAddr.sin_addr, ipAddr, sizeof(ipAddr));
            message.append(ipAddr);
            message.append(":");
            sprintf_s(portstr, "%d", ntohs(fromAddr.sin_port));
            message.append(portstr);*/
            
            message.append("");
            message.append("\r\n");
            SetWindowTextA(hMessageEdit, message.c_str());
            
        }
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
            // 送信ボタン押下時
        case IDC_SENDBUTTON:
            // 送信メッセージを取得
            GetWindowTextA(hSendMessageEdit, buff, 1024);



            // 宛先IPアドレスの取得
            GetWindowTextA(hIpAddressEdit, ipAddr, 256);
            // 宛先のポート番号の取得
            port = GetDlgItemInt(hDlg, IDC_PORTEDIT, FALSE, FALSE);

            memset(&toAddr, 0, sizeof(toAddr));
            toAddr.sin_family = AF_INET;
            inet_pton(AF_INET, ipAddr, &toAddr.sin_addr.s_addr);
            toAddr.sin_port = htons(port);
            sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);


            // buffをチャット欄に追加
            message.append(buff);
            message.append("\r\n");

            // チャット欄に文字列セット
            //SetWindowTextA(hMessageEdit, message.c_str());



            // 送信メッセージ入力欄をクリア
            SetWindowTextA(hSendMessageEdit, "");

            return TRUE;
        case IDCANCEL:
            KillTimer(hDlg, TIMERID);
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        return FALSE;
    }

    return FALSE;
}
