package com.autonomouscarapp.autonomouscar;

import android.app.Activity;

import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.ListView;

import android.widget.ArrayAdapter;
import java.util.ArrayList;
import java.util.Set;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import android.widget.Toast;

import android.content.Intent;
import android.os.Bundle;
import android.widget.AdapterView;

public class BluetoothDeviceList extends Activity  {

    Button listDevicesButton;
    ListView pairedDevicesList;
    TextView headerText;

    private BluetoothAdapter myBluetoothAdapter;
    private Set<BluetoothDevice>pairedDevicesSet;
    String EXTRA_ADDRESS = "device_address";

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_device_list);

        listDevicesButton = (Button)findViewById(R.id.listDevicesButton);
        headerText = (TextView)findViewById(R.id.headerText);
        pairedDevicesList = (ListView)findViewById(R.id.pairedDevicesList);

        myBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        headerText.setVisibility(View.INVISIBLE);
        pairedDevicesList.setVisibility(View.INVISIBLE);

        if (!myBluetoothAdapter.isEnabled()) {
            Intent turnOnBluetooth = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOnBluetooth, 0);
        }
    }

    public void showPairedDevices(View v){

        ArrayList pairedDevicesArrayList = new ArrayList();
        final ArrayAdapter listArrayAdapter = new  ArrayAdapter(this,android.R.layout.simple_list_item_1, pairedDevicesArrayList);

        listDevicesButton.setVisibility(View.INVISIBLE);
        headerText.setVisibility(View.VISIBLE);
        pairedDevicesList.setVisibility(View.VISIBLE);

        pairedDevicesSet = myBluetoothAdapter.getBondedDevices();

        for(BluetoothDevice bluetoothDevice : pairedDevicesSet) {
            pairedDevicesArrayList.add(bluetoothDevice.getName() + "\n" + bluetoothDevice.getAddress());
        }

        // Adds the paired devices to the list view
        pairedDevicesList.setAdapter(listArrayAdapter);

        pairedDevicesList.setOnItemClickListener(myListItemClickListener);
    }

    private AdapterView.OnItemClickListener myListItemClickListener = new AdapterView.OnItemClickListener()
    {
        @Override
        public void onItemClick(AdapterView av, View v, int position, long id) {

            String MACAddress = ((TextView) v).getText().toString();
            MACAddress = MACAddress.substring(MACAddress.length() - 17);

            Intent chooseModeActivity = new Intent(BluetoothDeviceList.this, ChooseMode.class);
            chooseModeActivity.putExtra(EXTRA_ADDRESS, MACAddress); // Sends device address to choose mode activity
            startActivity(chooseModeActivity);
        }

    };
}