package com.autonomouscarapp.autonomouscar;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.view.View;
import android.widget.Button;

public class ChooseMode extends AppCompatActivity {

    Button controlMode;
    Button GPSMode;
    Button cameraMode;

    String MACAddress;
    String EXTRA_ADDRESS = "device_address";

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_choose_mode);

        MACAddress = getIntent().getExtras().getString("device_address");

        controlMode = (Button)findViewById(R.id.controlMode);
        GPSMode = (Button)findViewById(R.id.GPSMode);
        cameraMode = (Button)findViewById(R.id.cameraMode);

        controlMode.setOnClickListener(controlModeButtonClickListener);
        GPSMode.setOnClickListener(GPSModeButtonClickListener);
        cameraMode.setOnClickListener(cameraModeButtonClickListener);
    }

    private View.OnClickListener controlModeButtonClickListener = new View.OnClickListener(){

        @Override
        public void onClick(View v) {

            Intent motorControlActivity = new Intent(ChooseMode.this, MotorControl.class);
            motorControlActivity.putExtra(EXTRA_ADDRESS, MACAddress);
            startActivity(motorControlActivity);
        }
    };

    private View.OnClickListener GPSModeButtonClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            Intent GPSModeActivity = new Intent(ChooseMode.this, GPSMode.class);
            GPSModeActivity.putExtra(EXTRA_ADDRESS, MACAddress);
            startActivity(GPSModeActivity);
        }
    };

    private View.OnClickListener cameraModeButtonClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            Intent cameraModeActivity = new Intent(ChooseMode.this, CameraMode.class);
            cameraModeActivity.putExtra(EXTRA_ADDRESS, MACAddress);
            startActivity(cameraModeActivity);
        }
    };

}
