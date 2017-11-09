package com.autonomouscarapp.autonomouscar;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import java.util.UUID;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by Michelle on 2017-10-23.
 */

public class BluetoothConnection {

    // Bluetooth Connection
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private BluetoothAdapter myBluetoothAdapter;
    private BluetoothSocket myBluetoothSocket;
    private BluetoothDevice myDevice;
    private boolean connectionStatus;

    // Input/Output
    private OutputStream myOutputStream;

    // Constructor for Bluetooth Connection class
    public BluetoothConnection(){
        myBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        connectionStatus = false;
    }

    // New thread for connection so main thread isn't blocked
    private class ConnectThread extends Thread {

        // Constructor for Connect Thread
        public ConnectThread(BluetoothDevice myDevice) {
            try {
                myBluetoothSocket = myDevice.createRfcommSocketToServiceRecord(MY_UUID);
            } catch (IOException e) {

            }
        }

        public void run() {

            // Tries to connect and create a new output stream
            try {
                myBluetoothSocket.connect();
                myOutputStream = myBluetoothSocket.getOutputStream();
                connectionStatus = true;

            } catch (IOException e) {

                //If connection doesn't work tries to close the stream
                try {
                    myBluetoothSocket.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        }
    }

    // Returns the Bluetooth device using its address
    public BluetoothDevice getDevice(String address){
        myDevice = myBluetoothAdapter.getRemoteDevice(address);
        return myDevice;
    }

    // Returns the Bluetooth socket used
    public BluetoothSocket getMyBluetoothSocket(){
        return myBluetoothSocket;
    }

    // Connects to device via Bluetooth
    public void connect(BluetoothDevice device) {
        new ConnectThread(device).start();
    }

    // Returns status of connection (connected/not connected)
    public boolean isConnected() {
        return connectionStatus;
    }

    // Sends a message
    public void send(String msg){
        try {
            myOutputStream.write(msg.getBytes());
        } catch (IOException e) {

        }
    }
}
