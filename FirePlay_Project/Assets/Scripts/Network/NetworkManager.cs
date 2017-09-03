using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    public HttpNetwork _httpNetwork = null;
    private Socket   _socket      = null;
    public bool      _isConnected = false;
    public string    _ipAddress   = "127.0.0.1";
    public const int _port        = 23452;

    // TODO :: Http 통신도 이쪽으로 옮길 수 있도록.

    private void Start()
    {
        // 씬 전환에도 소멸되지 않음.
        DontDestroyOnLoad(gameObject);

        // 소켓 생성.
        _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        // HttpNetwork 생성
        _httpNetwork = new HttpNetwork();

        ConnectToServer();
    }

    public void ConnectToServer()
    {
        try
        {
            IPAddress ipAddr = System.Net.IPAddress.Parse(_ipAddress);
            IPEndPoint iPEndPoint = new System.Net.IPEndPoint(ipAddr, _port);
            _socket.Connect(iPEndPoint);

            Debug.Log("Socket Connect to Server Success");
            _isConnected = true;
        }
        catch (SocketException e)
        {
            // TODO :: 재접속 시도.
            Debug.LogAssertion(e.ToString());
            _isConnected = false;
            return;
        }
    }

    void OnApplicationQuit()
    {
        _isConnected = true;
        _socket.Close();
        _socket = null;
    }
}
