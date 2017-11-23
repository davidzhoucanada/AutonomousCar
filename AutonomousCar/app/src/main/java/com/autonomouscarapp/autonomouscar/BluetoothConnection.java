package com.autonomouscarapp.autonomouscar;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import java.util.UUID;
import java.io.IOException;
import java.io.OutputStream;

public class BluetoothConnection {

    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private BluetoothAdapter myBluetoothAdapter;
    private BluetoothSocket myBluetoothSocket;
    private BluetoothDevice myBluetoothDevice;
    private boolean connectionStatus;

    private OutputStream myOutputStream;

    public BluetoothConnection(){
        myBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        connectionStatus = false;
    }

    // Thread for connection to avoid blocking main thread
    private class ConnectThread extends Thread {

        public ConnectThread(BluetoothDevice myBluetoothDevice) {
            try {
                myBluetoothSocket = myBluetoothDevice.createRfcommSocketToServiceRecord(MY_UUID);
            } catch (IOException e) {

            }
        }

        public void run() {

            try {
                myBluetoothSocket.connect();
                myOutputStream = myBluetoothSocket.getOutputStream();
                connectionStatus = true;

            } catch (IOException e1) {

                try {
                    myBluetoothSocket.close();
                } catch (IOException e2) {

                }
            }
        }
    }

    public BluetoothDevice getBluetoothDevice(String MACAdress){
        myBluetoothDevice = myBluetoothAdapter.getRemoteDevice(MACAdress);
        return myBluetoothDevice;
    }

    public BluetoothSocket getMyBluetoothSocket(){
        return myBluetoothSocket;
    }

    public void connect(BluetoothDevice device) {
        new ConnectThread(device).start();
    }

    public boolean isConnected() {
        return connectionStatus;
    }

    public void sendCommand(String command){
        try {
            myOutputStream.write(command.getBytes());
        } catch (IOException e) {

        }
    }
}
