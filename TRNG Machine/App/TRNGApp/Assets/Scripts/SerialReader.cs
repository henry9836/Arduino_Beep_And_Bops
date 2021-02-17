using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO.Ports;
using System.Threading;

public class SerialReader : MonoBehaviour
{
    public List<UInt64> trngNumbers = new List<UInt64>();
    public string COMPORT = "COM8";
    public int BANDRATE = 9600;

    private string pendingData;
    private SerialPort _serialPort;



    // Start is called before the first frame update
    void Start()
    {
        connectToSerial();
    }

    bool connectToSerial()
    {
        _serialPort = null;
        _serialPort = new SerialPort();
        _serialPort.PortName = COMPORT;
        _serialPort.BaudRate = BANDRATE;
        _serialPort.Open();
        return true;
    }

    void processSerial()
    {
        string newData = _serialPort.ReadExisting();

        //Got data
        if (newData.Length > 0)
        {
            Debug.Log($"PENDING: {newData}");
            newData.Replace("\n", String.Empty);
            newData.Replace("\r", String.Empty);
            newData.Replace("\t", String.Empty);
            pendingData += newData;
            Debug.Log($"PENDING: {pendingData}");
            //Take all seeds out
            while (pendingData.Contains("SEEDBEGIN:") && pendingData.Contains(":SEEDEND"))
            {
                //Convert to ulong
                string extractedBinary = pendingData.Substring(pendingData.IndexOf("SEEDBEGIN:") + 10, (pendingData.IndexOf(":SEEDEND") - (pendingData.IndexOf("SEEDBEGIN:") + 10)));
                UInt64 result = 0;

                for (int i = 0; i < extractedBinary.Length; i++)
                {
                    if (extractedBinary[i] == '1')
                    {
                        //Pow Function that doesn't panic at big numbers
                        UInt64 thebigone = 2;
                        for (int p = 0; p < i; p++)
                        {
                            thebigone *= (UInt64)2;
                        }
                        result += thebigone;
                    }
                }

                Debug.Log(extractedBinary);
                Debug.Log(result);
                trngNumbers.Add(result);
                pendingData = pendingData.Remove(pendingData.IndexOf("SEEDBEGIN:"), (pendingData.IndexOf(":SEEDEND") + 8));
                break;
            }

        }

    }

    // Update is called once per frame
    void Update()
    {
        processSerial();
    }
}
