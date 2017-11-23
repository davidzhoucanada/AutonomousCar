package com.autonomouscarapp.autonomouscar;

import android.support.v7.app.AppCompatActivity;

import android.os.Handler;
import android.os.Bundle;

import android.support.constraint.ConstraintLayout;
import android.widget.RelativeLayout;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import android.widget.Toast;

import android.bluetooth.BluetoothSocket;

import android.view.MotionEvent;

public class MotorControl extends AppCompatActivity {

    ImageView outerControlWheel;
    ImageView innerControlWheel;
    RelativeLayout outerControlWheelContainer;
    ConstraintLayout phoneScreen;
    Button userControlButton;
    Button autonomousModeButton;

    private BluetoothConnection myBluetoothConnection;
    BluetoothSocket myBluetoothSocket = null;
    String MACAddress;

    private float phoneScreenHeight;
    private float margin;
    private float outerControlWheelRadius;
    private float innerControlWheelRadius;
    private float innerWheelTopLeftDefaultX;
    private float innerWheelTopLeftDefaultY;
    private float innerWheelDefaultCenterX;
    private float innerWheelDefaultCenterY;
    float pointerX;
    float pointerY;
    double pointerRadius; // Distance of pointer from center of outer control wheel
    double maximumPointerRadius;

    int mode;
    int leftOrRight;
    int forwardOrBackward;
    String speedString;
    int speed;

    private static final String USER_CONTROL_MODE = "55555>";
    private static final String AUTONOMOUS_MODE = "66666>";

    private static final int FORWARD = 0;
    private static final int BACKWARD = 1;
    private static final int LEFT = 2;
    private static final int RIGHT = 3;
    private static final int STRAIGHT = 4;
    private static final int STOP = 9;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_motor_control);

        myBluetoothConnection = new BluetoothConnection();

        MACAddress = getIntent().getExtras().getString("device_address");

        message("Connecting...");
        myBluetoothConnection.connect(myBluetoothConnection.getBluetoothDevice(MACAddress));
        myBluetoothSocket = myBluetoothConnection.getMyBluetoothSocket();

        phoneScreen = (ConstraintLayout)findViewById(R.id.phoneScreen);
        outerControlWheelContainer = (RelativeLayout)findViewById(R.id.outerControlWheelContainer);
        outerControlWheel = (ImageView)findViewById(R.id.outerControlWheel);
        innerControlWheel = (ImageView)findViewById(R.id.innerControlWheel);
        userControlButton = (Button)findViewById(R.id.userControlButton);
        autonomousModeButton = (Button)findViewById(R.id.autonomousModeButton);

        innerControlWheel.setOnTouchListener(innerControlWheelTouchListener);
        userControlButton.setOnClickListener(userControlButtonClickListener);
        autonomousModeButton.setOnClickListener(autonomousModeButtonClickListener);

        // Sets constant values of control wheel after control wheel view has been set up
        outerControlWheelContainer.post(new Runnable() {
            @Override
            public void run() {
                phoneScreenHeight = phoneScreen.getHeight();
                outerControlWheelRadius = outerControlWheel.getWidth()/2;
                innerControlWheelRadius = innerControlWheel.getWidth()/2;
                margin = phoneScreenHeight - outerControlWheelContainer.getWidth();
                innerWheelTopLeftDefaultX = outerControlWheelRadius - innerControlWheelRadius;
                innerWheelTopLeftDefaultY = outerControlWheelRadius - innerControlWheelRadius;
                innerWheelDefaultCenterX = innerWheelTopLeftDefaultX + innerControlWheelRadius;
                innerWheelDefaultCenterY = innerWheelTopLeftDefaultY + innerControlWheelRadius;
                maximumPointerRadius = outerControlWheelRadius - innerControlWheelRadius;
            }
        });
    }

    private void message(String s)
    {
        final Toast toastMessage = Toast.makeText(getApplicationContext(),s,Toast.LENGTH_SHORT);
        toastMessage.show();

        // Shortens duration of Toast message
        // Taken from https://stackoverflow.com/questions/6094792/how-to-set-toast-display-time-less-than-toast-length-short, by user Erikas
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                toastMessage.cancel();
            }
        }, 500);
    }

    private View.OnClickListener userControlButtonClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View view) {
            if (myBluetoothConnection.isConnected() == true){
                myBluetoothConnection.sendCommand(USER_CONTROL_MODE);
            }
            innerControlWheel.setVisibility(View.VISIBLE);
            outerControlWheel.setVisibility(View.VISIBLE);
        }
    };

    private View.OnClickListener autonomousModeButtonClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View view) {
            if (myBluetoothConnection.isConnected() == true){
                myBluetoothConnection.sendCommand(AUTONOMOUS_MODE);
            }
            innerControlWheel.setVisibility(View.INVISIBLE);
            outerControlWheel.setVisibility(View.INVISIBLE);
        }
    };

    private View.OnTouchListener innerControlWheelTouchListener = new View.OnTouchListener() {

        public boolean onTouch(View v, MotionEvent touchEvent) {

            pointerX = touchEvent.getRawX();
            pointerY = touchEvent.getRawY();

            switch (touchEvent.getAction()) {

                // Moment inner control wheel is touched
                case MotionEvent.ACTION_DOWN:
                    synchronized (this) {
                    }
                    break;

                // Dragging inner control wheel around
                case MotionEvent.ACTION_MOVE:
                    synchronized (this) {

                        pointerY = pointerY - margin;

                        pointerRadius = Math.sqrt(Math.pow(pointerX - innerWheelDefaultCenterX, 2) + Math.pow(pointerY - innerWheelDefaultCenterY, 2));

                        if (pointerRadius <= maximumPointerRadius){
                            innerControlWheel.setX(pointerX - innerControlWheelRadius);
                            innerControlWheel.setY(pointerY - innerControlWheelRadius);
                        }

                        else {
                            pointerRadius = maximumPointerRadius;
                        }

                        speed = (int)((pointerRadius/(maximumPointerRadius))*255);

                        // Adding 1000 to get leading zeros if speed is 1 - e.g., if speed = 1, speed + 1000 = 1001, substring taken is 001
                        speedString = String.valueOf(speed+1000).substring(1);

                        if (pointerX - innerWheelDefaultCenterX > 25){
                            leftOrRight = RIGHT;
                        }

                        else if (pointerX - innerWheelDefaultCenterX < -25) {
                            leftOrRight = LEFT;
                        }

                        else {
                            leftOrRight = STRAIGHT;
                        }

                        // If control wheel is above center position
                        if (pointerY - innerWheelDefaultCenterY < 0) {
                            forwardOrBackward = FORWARD;
                        }

                        // If control wheel is below center position
                        else if (pointerY - innerWheelDefaultCenterY > 0){
                            forwardOrBackward = BACKWARD;
                        }

                        else {
                            forwardOrBackward = STOP;
                        }

                        // The ">" is the end message marker
                        if (myBluetoothConnection.isConnected() == true){
                            if (forwardOrBackward == STOP){
                                myBluetoothConnection.sendCommand("99999>");
                            }
                            else {
                                myBluetoothConnection.sendCommand(String.valueOf(leftOrRight)+String.valueOf(forwardOrBackward)+speedString+">");
                                message(String.valueOf(leftOrRight)+String.valueOf(forwardOrBackward)+speedString+">");
                            }
                        }
                    }
                    break;

                // Stop touching screen
                case MotionEvent.ACTION_UP:
                    synchronized (this) {

                        innerControlWheel.setX(innerWheelTopLeftDefaultX);
                        innerControlWheel.setY(innerWheelTopLeftDefaultY);

                        if (myBluetoothConnection.isConnected() == true)
                        {
                            myBluetoothConnection.sendCommand("99999>");
                        }
                    }
                    break;
            }
            return true;
        }
    };
}
