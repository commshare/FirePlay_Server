using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using UnityEngine;

public class TcpNetwork
{
    private Socket   _socket      = null;
    public bool      _isConnected = false;
    public string    _ipAddress   = "127.0.0.1";
    public const int _port        = 23452;

    public TcpNetwork()
    {
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

    public void CloseNetwork()
    {
        _isConnected = false;
        _socket.Close();
        _socket = null;
    }

    // 게임서버와 접속을 시도하는 메소드.
    public bool ConnectToServer()
    {
        try
        {
            IPAddress ipAddr = System.Net.IPAddress.Parse(_ipAddress);
            IPEndPoint iPEndPoint = new System.Net.IPEndPoint(ipAddr, _port);
            _socket.Connect(iPEndPoint);

            Debug.Log("Socket Connect to Server Success");
            _isConnected = true;
            return true;
        }
        catch (SocketException e)
        {
            // TODO :: 재접속 시도.
            Debug.LogAssertion(e.ToString());
            _isConnected = false;
            return false;
        }
    }

}
