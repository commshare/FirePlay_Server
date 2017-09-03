using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using PacketInfo;
using UnityEngine;

// 비동기 콜백 TcpIp 통신을 목적으로 하는 클래스.
public class TcpNetwork
{
    private AsyncCallback _recvCallback;
    private AsyncCallback _sendCallback;
    private Socket        _socket;
    private Queue<Packet> _packetQueue;

    private bool      _isConnected = false;
    private string    _ipAddress   = "127.0.0.1";
    private int       _port        = 23452;

    public TcpNetwork(string ipAddr = "127.0.0.1", int port = 23452)
    {
        _ipAddress = ipAddr;
        _port      = port;

        try
        {
            // 소켓 생성.
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        }
        catch (Exception e)
        {
            Debug.LogError("Socket Create Failed Error : " + e.Message);
        }
    }

    // 소켓을 닫아주는 메소드.
    public void CloseNetwork()
    {
        _isConnected = false;
        _socket.Close();
        _socket = null;
    }

    // 게임서버와 접속을 시도하는 메소드.
    public void ConnectToServer()
    {
        try
        {
            // 게임 서버에 비동기 접속 요청 시작.
            // @ Param 3 : 비동기 접속이 끝났을 경우 호출될 콜백 메소드.
            _socket.BeginConnect(_ipAddress, _port, OnConnectSuccess, 0);
        }
        catch (SocketException e)
        {
            // TODO :: 재접속 시도.
            Debug.LogAssertion(e.ToString());
            _isConnected = false;
        }
    }

    // 비동기 접속이 끝났을 경우 호출되는 콜백 메소드.
    void OnConnectSuccess(IAsyncResult asyncResult)
    {
        try
        {
            // 접속이 완료되었으므로 Connect 요청을 그만둠. 
            _socket.EndConnect(asyncResult);
            _isConnected = true;
            Debug.LogFormat("Server Connect Success Ip {0}, Port : {1}", _ipAddress, _port);
        }
        catch (Exception e)
        {
            Debug.Log("Socket Connect Callback Function failed : " + e.Message);
            return;
        }
    }

    // 서버에 연결되어있는지를 반환하는 메소드.
    public bool IsConnected()
    {
        return _isConnected;
    }


}
