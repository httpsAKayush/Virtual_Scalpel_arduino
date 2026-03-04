using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class UDPReceiver : MonoBehaviour {
    UdpClient udpClient;
    IPEndPoint remoteEndPoint;
    
    void Start() {
        udpClient = new UdpClient(1234); // Same port as ESP32
        remoteEndPoint = new IPEndPoint(IPAddress.Any, 1234);
        udpClient.BeginReceive(new AsyncCallback(ReceiveData), null);
    }

    void ReceiveData(IAsyncResult result) {
        byte[] receivedData = udpClient.EndReceive(result, ref remoteEndPoint);
        string dataStr = Encoding.ASCII.GetString(receivedData);
        Debug.Log("Received: " + dataStr);

        udpClient.BeginReceive(new AsyncCallback(ReceiveData), null); 
    }

    void OnApplicationQuit() {
        udpClient.Close();
    }
}
