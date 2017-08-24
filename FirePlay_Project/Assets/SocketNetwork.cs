using System.Collections;
using System.Net.Sockets;
using System.Net;
using System.Text;
using UnityEngine;

public class SocketNetwork : MonoBehaviour
{
    private Socket _socket = null;
    public string _ipAddress = "127.0.0.1";
    public const int _port = 23452;

    private int _sendDataLength;
    private int _receiveDataLength;

    private byte[] _sendByte;
    private byte[] _receiveByte = new byte[2000];
    private string _receiveString;

    private void Awake()
    {
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
        }
        catch (SocketException e)
        {
            Debug.LogAssertion(e.ToString());
            return;
        }
    }

    void OnApplicationQuit()
    {
        _socket.Close();
        _socket = null;
    }
}
