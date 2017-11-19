package com.example.miaomiaozhou.selfdrive;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import java.util.UUID;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by miaomiaozhou on 2017-11-16.
 */


public class BluetoothConnection {
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private BluetoothAdapter myBluetoothAdapter;
    private BluetoothSocket myBluetoothSocket;
    private BluetoothDevice myDevice;

    // Input/Output
    private OutputStream myOutputStream;

    // Constructor for Bluetooth Connection class
    public BluetoothConnection(){
        myBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

}

