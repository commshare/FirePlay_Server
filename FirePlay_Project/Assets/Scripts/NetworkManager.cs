using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    private Socket   _socket      = null;
    public bool      _isConnected = false;
    public string    _ipAddress   = "127.0.0.1";
    public const int _port        = 23452;

    private int _sendDataLength;
    private int _receiveDataLength;

    private byte[] _sendByte;
    private byte[] _receiveByte = new byte[2000];
    private string _receiveString;

    // TODO :: Http 통신도 이쪽으로 옮길 수 있도록.

    private void Awake()
    {
        // 씬 전환에도 소멸되지 않음.
        DontDestroyOnLoad(gameObject);

        // 소켓 생성.
        _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

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

    public Packet.LoginRes SendLoginRequest(Packet.LoginReq req)
    {
        var res = new Packet.LoginRes();

        var jsonReq = JsonUtility.ToJson(req);

        // Send
        _sendDataLength = Encoding.Default.GetByteCount(jsonReq);
        _sendByte = Encoding.Default.GetBytes(jsonReq);
        _socket.Send(_sendByte, _sendByte.Length, 0);
        Debug.Log("Send Packet " + jsonReq);

        // Receive
        //_socket.Receive(_receiveByte);

        return res;
    }

}
