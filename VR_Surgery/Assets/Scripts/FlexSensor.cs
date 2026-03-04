using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;

public class FlexSensor : MonoBehaviour
{
    private Thread receiveThread;
    private UdpClient client;
    public int port = 8052;
    
    private string receivedData = "";
    private readonly object lockObject = new object();
    [SerializeField] private int lastReceivedValue;

    void Start()
    {
        receiveThread = new Thread(new ThreadStart(ReceiveBroadcast))
        {
            IsBackground = true
        };
        receiveThread.Start();
    }

    private void ReceiveBroadcast()
    {
        try
        {
            client = new UdpClient(port);
            client.EnableBroadcast = true;
            
            Debug.Log($"Listening for broadcast on port {port}");

            while (true)
            {
                IPEndPoint anyIP = new IPEndPoint(IPAddress.Any, 0);
                byte[] data = client.Receive(ref anyIP);
                string message = Encoding.UTF8.GetString(data);
                
                if (message.StartsWith("FLEX:"))
                {
                    lock (lockObject)
                    {
                        receivedData = message.Substring(5);
                    }
                }
            }
        }
        catch (System.Exception e)
        {
            Debug.LogError($"Receive error: {e.Message}");
        }
    }

    void Update()
    {
        string newData;
        lock (lockObject)
        {
            newData = receivedData;
            receivedData = null;
        }

        if (!string.IsNullOrEmpty(newData) && int.TryParse(newData, out int value))
        {
            lastReceivedValue = value;
            Debug.Log($"Broadcast received: {value}");
        }
    }

    void OnDisable()
    {
        if (client != null) client.Close();
        if (receiveThread != null && receiveThread.IsAlive) receiveThread.Abort();
    }
}